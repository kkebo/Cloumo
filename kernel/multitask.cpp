#include "../headers.h"

Task::Task(char *name, int level, int priority, void (*mainLoop)()) : name_(name), queue_(nullptr) {
	stack = (int)malloc4k(64 * 1024);
	tss_.esp = stack + 64 * 1024 - 12;
	tss_.eip = mainLoop;
	tss_.es = 1 * 8;
	tss_.cs = 2 * 8;
	tss_.ss = 1 * 8;
	tss_.ds = 1 * 8;
	tss_.fs = 1 * 8;
	tss_.gs = 1 * 8;
	run(level, priority);
}

Task::Task(char *name, int level, int priority, Queue<int> *queue, void (*mainLoop)()) : name_(name), queue_(queue) {
	queue_->task_ = this;
	stack = (int)malloc4k(64 * 1024);
	tss_.esp = stack + 64 * 1024 - 12;
	tss_.eip = mainLoop;
	tss_.es = 1 * 8;
	tss_.cs = 2 * 8;
	tss_.ss = 1 * 8;
	tss_.ds = 1 * 8;
	tss_.fs = 1 * 8;
	tss_.gs = 1 * 8;
	run(level, priority);
}

Task::~Task() {
	sleep();
	delete queue_;
	flags_ = TaskFlag::Free;
	free4k(stack);
}

static void *Task::operator new(size_t size) {
	for (int i = 0; i < MAX_TASKS; i++) {
		if (TaskController::tasks0_[i].flags_ ==  TaskFlag::Free) {
			Task *task = &TaskController::tasks0_[i];
			task->flags_ =  TaskFlag::Sleeping;
			task->tss_.eflags = 0x00000202;
			task->tss_.eax = 0;
			task->tss_.ecx = 0;
			task->tss_.edx = 0;
			task->tss_.ebx = 0;
			task->tss_.ebp = 0;
			task->tss_.esi = 0;
			task->tss_.edi = 0;
			task->tss_.es = 0;
			task->tss_.ds = 0;
			task->tss_.fs = 0;
			task->tss_.gs = 0;
			task->tss_.ldtr = 0;
			task->tss_.iomap = 0x40000000;
			task->tss_.ss0 = 0;
			task->fpu[0] = 0x037f; /* CW(control word) */
			task->fpu[1] = 0x0000; /* SW(status word)  */
			task->fpu[2] = 0xffff; /* TW(tag word)     */
			for (int i = 3; i < 108 / 4; i++) {
				task->fpu[i] = 0;
			}
			return task;
		}
	}
	// もうタスクは作れない
	return nullptr;
}

void Task::run(int level, int priority) {
	// level が負ならレベルを変更しない
	if (level < 0) level = level_;
	// 優先度は 1 以上
	if (priority > 0) priority_ = priority;

	if (flags_ ==  TaskFlag::Running && level_ != level) {
		TaskController::remove(this);
	}
	if (flags_ !=  TaskFlag::Running) {
		level_ = level;
		TaskController::add(this);
	}

	TaskController::lv_change_ = true;
}

void Task::sleep() {
	if (flags_ ==  TaskFlag::Running) {
		Task* now_task = TaskController::getNowTask();
		TaskController::remove(this);
		if (this == now_task) {
			TaskController::switchTaskSub();
			now_task = TaskController::getNowTask();
			FarJump(0, now_task->selector_);
		}
	}
}

Timer     *TaskController::timer_    = nullptr;
int       TaskController::now_lv_    = 0;
bool      TaskController::lv_change_ = false;
TaskLevel *TaskController::level_    = nullptr;
Task      *TaskController::tasks0_   = nullptr;
Task      *TaskController::task_fpu_ = nullptr;

Task *TaskController::init() {
	level_ = new TaskLevel[MAX_TASKLEVELS];
	tasks0_ = ::new Task[MAX_TASKS];
	for (int i = 0; i < MAX_TASKS; i++) {
		tasks0_[i].flags_ =  TaskFlag::Free;
		tasks0_[i].selector_ = (kTaskGdt0 + i) * 8;
		SetSegmentDescriptor((SegmentDescriptor*)kAdrGdt + kTaskGdt0 + i, 103, (int)&tasks0_[i].tss_, kArTss32);
	}
	for (int i = 0; i < MAX_TASKLEVELS; i++) {
		level_[i].running = 0;
		level_[i].now = 0;
	}

	/* メインタスク */
	Task *task = new Task();
	task->name_ = (char *)kMainTaskName;
	task->flags_ =  TaskFlag::Running;
	task->priority_ = 2; /* 0.02秒 */
	task->level_ = 0;
	add(task);
	switchTaskSub();
	LoadTr(task->selector_);
	timer_ = new Timer();
	timer_->set(task->priority_);

	return task;
}

void TaskController::switchTask() {
	TaskLevel *tl = &level_[now_lv_];
	Task *newTask;
	Task *nowTask = tl->tasks[tl->now];
	tl->now++;
	if (tl->now == tl->running) tl->now = 0;
	if (lv_change_) {
		switchTaskSub();
		tl = &level_[now_lv_];
	}
	newTask = tl->tasks[tl->now];
	timer_->set(newTask->priority_);
	if (newTask != nowTask) FarJump(0, newTask->selector_);
}

void TaskController::switchTaskSub() {
	int i;
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		if (level_[i].running > 0) break;
	}
	now_lv_ = i;
	lv_change_ = false;
}

Task *TaskController::getNowTask() {
	TaskLevel *tl = &level_[now_lv_];
	return tl->tasks[tl->now];
}

void TaskController::add(Task *task) {
	TaskLevel *tl = &level_[task->level_];
	if (tl->running < kMaxTasksLevel) {
		tl->tasks[tl->running] = task;
		tl->running++;
		task->flags_ = TaskFlag::Running;
	}
}

void TaskController::remove(Task *task) {
	int i;
	TaskLevel *tl = &level_[task->level_];

	// 現在のレベル内でのこのタスクのインデックス番号を取得
	for (i = 0; i < tl->running; i++)
		if (tl->tasks[i] == task)
			break;

	// 現在のレベルのタスク稼働数を1減らす
	tl->running--;
	
	// この瞬間に動作中のタスクのインデックス番号を修正
	if (i < tl->now) tl->now--;
	
	// この瞬間に動作中のタスクのインデックス番号が範囲を超えていたら
	if (tl->now >= tl->running) tl->now = 0;
	
	// フラグの書き換え
	task->flags_ =  TaskFlag::Sleeping;

	// このレベル内のタスクの配列をずらす
	for (; i < tl->running; i++) {
		tl->tasks[i] = tl->tasks[i + 1];
	}
}
