#include "../headers.h"

Timer     *TaskController::timer       = nullptr;
int       TaskController::nowLevel     = 0;
bool      TaskController::levelChanged = false;
TaskLevel TaskController::level[MAX_TASKLEVELS];
Task      TaskController::tasks0[MAX_TASKS];
Task      *TaskController::taskFPU     = nullptr;

TaskQueue::TaskQueue(int size, Task *task_) : Queue<int>(size), task(task_) {}

bool TaskQueue::push(int data) {
	if (Queue::push(data)) {
		if (task && task->flags != TaskFlag::Running) {
			task->run(-1, 0);
		}
		return true;
	}
	return false;
}

Task::Task(char *name_, int level_, int priority_, void (*mainLoop)()) : name(name_), queue(nullptr) {
	stack = (int)malloc4k(64 * 1024);
	tss.esp = stack + 64 * 1024 - 12;
	tss.eip = mainLoop;
	tss.es = 1 * 8;
	tss.cs = 2 * 8;
	tss.ss = 1 * 8;
	tss.ds = 1 * 8;
	tss.fs = 1 * 8;
	tss.gs = 1 * 8;
	run(level_, priority_);
}

Task::Task(char *name_, int level_, int priority_, int queueSize, void (*mainLoop)()) : Task(name_, level_, priority_, mainLoop) {
	queue = new TaskQueue(queueSize, this);
}

Task::~Task() {
	sleep();
	delete queue;
	flags = TaskFlag::Free;
	free4k(stack);
}

static void *Task::operator new(size_t) {
	for (auto &&task : TaskController::tasks0) {
		if (task.flags == TaskFlag::Free) {
			task.flags = TaskFlag::Sleeping;
			task.tss.eflags = 0x00000202;
			task.tss.eax = 0;
			task.tss.ecx = 0;
			task.tss.edx = 0;
			task.tss.ebx = 0;
			task.tss.ebp = 0;
			task.tss.esi = 0;
			task.tss.edi = 0;
			task.tss.es = 0;
			task.tss.ds = 0;
			task.tss.fs = 0;
			task.tss.gs = 0;
			task.tss.ldtr = 0;
			task.tss.iomap = 0x40000000;
			task.tss.ss0 = 0;
			task.fpu[0] = 0x037f; /* CW(control word) */
			task.fpu[1] = 0x0000; /* SW(status word)  */
			task.fpu[2] = 0xffff; /* TW(tag word)     */
			for (int i = 3; i < 108 / 4; ++i) {
				task.fpu[i] = 0;
			}
			return &task;
		}
	}
	// もうタスクは作れない
	return nullptr;
}

void Task::run(int newLevel, int newPriority) {
	// level が負ならレベルを変更しない
	if (newLevel < 0) newLevel = level;
	// 優先度は 1 以上
	if (newPriority > 0) priority = newPriority;

	if (flags ==  TaskFlag::Running && level != newLevel) {
		TaskController::remove(this);
	}
	if (flags !=  TaskFlag::Running) {
		level = newLevel;
		TaskController::add(this);
	}

	TaskController::levelChanged = true;
}

void Task::sleep() {
	if (flags == TaskFlag::Running) {
		Task* nowTask = TaskController::getNowTask();
		TaskController::remove(this);
		if (this == nowTask) {
			TaskController::switchTaskSub();
			nowTask = TaskController::getNowTask();
			FarJump(0, nowTask->selector);
		}
	}
}

Task *TaskController::init() {
	for (int i = 0; i < MAX_TASKS; ++i) {
		tasks0[i].selector = (kTaskGdt0 + i) * 8;
		SetSegmentDescriptor((SegmentDescriptor*)kAdrGdt + kTaskGdt0 + i, 103, (int)&tasks0[i].tss, kArTss32);
	}

	/* メインタスク */
	Task *task = new Task();
	task->name = (char *)kMainTaskName;
	task->flags =  TaskFlag::Running;
	task->priority = 2; /* 0.02秒 */
	task->level = 0;
	add(task);
	switchTaskSub();
	LoadTr(task->selector);
	timer = &TimerController::timers0[1];
	timer->flags = TimerFlag::Reserved;
	timer->set(task->priority);

	return task;
}

void TaskController::switchTask() {
	TaskLevel *tl = &level[nowLevel];
	Task *newTask;
	Task *nowTask = tl->tasks[tl->now];
	++tl->now;
	if (tl->now == tl->running) tl->now = 0;
	if (levelChanged) {
		switchTaskSub();
		tl = &level[nowLevel];
	}
	newTask = tl->tasks[tl->now];
	timer->set(newTask->priority);
	if (newTask != nowTask) FarJump(0, newTask->selector);
}

void TaskController::switchTaskSub() {
	int i;
	for (i = 0; i < MAX_TASKLEVELS; ++i) {
		if (level[i].running > 0) break;
	}
	nowLevel = i;
	levelChanged = false;
}

Task *TaskController::getNowTask() {
	TaskLevel *tl = &level[nowLevel];
	return tl->tasks[tl->now];
}

void TaskController::add(Task *task) {
	TaskLevel *tl = &level[task->level];
	if (tl->running < kMaxTasksLevel) {
		tl->tasks[tl->running] = task;
		++tl->running;
		task->flags = TaskFlag::Running;
	}
}

void TaskController::remove(Task *task) {
	int i;
	TaskLevel *tl = &level[task->level];

	// 現在のレベル内でのこのタスクのインデックス番号を取得
	for (i = 0; i < tl->running; ++i)
		if (tl->tasks[i] == task)
			break;

	// 現在のレベルのタスク稼働数を1減らす
	--tl->running;
	
	// この瞬間に動作中のタスクのインデックス番号を修正
	if (i < tl->now) --tl->now;
	
	// この瞬間に動作中のタスクのインデックス番号が範囲を超えていたら
	if (tl->now >= tl->running) tl->now = 0;
	
	// フラグの書き換え
	task->flags =  TaskFlag::Sleeping;

	// このレベル内のタスクの配列をずらす
	for (; i < tl->running; ++i) {
		tl->tasks[i] = tl->tasks[i + 1];
	}
}
