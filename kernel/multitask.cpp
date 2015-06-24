#include "../headers.h"

TaskQueue::TaskQueue(int size, Task *task_) : Queue<int>(size), task(task_) {}

bool TaskQueue::push(int data) {
	if (Queue::push(data)) {
		// タスクが指定されていてかつスリープしていたら起こす
		if (task && !task->running) {
			task->run(-1, 0);
		}
		return true;
	}
	return false;
}

// メインタスク用の Constructor
Task::Task() {
	_running = true;
	
	// GDT に登録
	selector = kTaskGDT0 * 8;
	SetSegmentDescriptor((SegmentDescriptor *)kAdrGdt + kTaskGDT0, 103, (int)&tss, kArTss32);
	
	// Task State Segment の設定
	tss.eflags = 0x00000202;
	tss.eax = 0;
	tss.ecx = 0;
	tss.edx = 0;
	tss.ebx = 0;
	tss.ebp = 0;
	tss.esi = 0;
	tss.edi = 0;
	tss.es = 0;
	tss.ds = 0;
	tss.fs = 0;
	tss.gs = 0;
	tss.ldtr = 0;
	tss.iomap = 0x40000000;
	tss.ss0 = 0;
	
	// add this pointer to the list of tasks
	TaskSwitcher::_taskList->push_back(this);
}

Task::~Task() {
	TaskSwitcher::_taskList->remove(this);
	sleep();
	delete queue;
	free4k(reinterpret_cast<void *>(stack));
}

void Task::run(int newLevel, int newPriority) {
	// level が負ならレベルを変更しない
	if (newLevel < 0) newLevel = level;
	// 優先度は 1 以上
	if (newPriority > 0) _priority = newPriority;

	if (running && level != newLevel) {
		TaskSwitcher::remove(this);
	}
	if (!running) {
		_level = newLevel;
		TaskSwitcher::add(this);
	}

	TaskSwitcher::levelChanged = true;
}

void Task::sleep() {
	if (running) {
		Task *nowTask = TaskSwitcher::getNowTask();
		TaskSwitcher::remove(this);
		if (this == nowTask) {
			TaskSwitcher::switchTaskSub();
			nowTask = TaskSwitcher::getNowTask();
			FarJump(0, nowTask->selector);
		}
	}
}

int       TaskSwitcher::nowLevel     = 0;
bool      TaskSwitcher::levelChanged = false;
List<Task *> *TaskSwitcher::_taskList;
List<Task *> *const &TaskSwitcher::taskList = _taskList;
TaskLevel TaskSwitcher::_level[MAX_TASKLEVELS];
const TaskLevel (&TaskSwitcher::level)[MAX_TASKLEVELS] = _level;
Task      *TaskSwitcher::taskFPU     = nullptr;
Timer     *TaskSwitcher::timer;
int       TaskSwitcher::taskCount    = 1; // メインタスクの分をあらかじめ足しておく

Task *TaskSwitcher::init() {
	// タスクリストの初期化
	_taskList = new List<Task *>();
	
	// メインタスクの設定
	Task *task = new Task();
	task->_name = "メインタスク";
	task->_level = 0;
	task->_priority = 2;
	add(task);
	switchTaskSub();
	LoadTr(task->selector);
	
	// タスクスイッチ用タイマーの設定
	timer = new Timer(nullptr, -1);
	timer->set(task->priority);

	return task;
}

void TaskSwitcher::switchTask() {
	TaskLevel *tl = &_level[nowLevel];
	Task *newTask;
	Task *nowTask = tl->tasks[tl->now];
	++tl->now;
	if (tl->now == tl->running) tl->now = 0;
	if (levelChanged) {
		switchTaskSub();
		tl = &_level[nowLevel];
	}
	newTask = tl->tasks[tl->now];
	timer->set(newTask->priority);
	if (newTask != nowTask) FarJump(0, newTask->selector);
}

void TaskSwitcher::switchTaskSub() {
	for (int i = 0; i < MAX_TASKLEVELS; ++i) {
		// Idle Task がいるので絶対に途中で止まる
		if (level[i].running > 0) {
			nowLevel = i;
			levelChanged = false;
			return;
		}
	}
}

Task *TaskSwitcher::getNowTask() {
	TaskLevel *tl = &_level[nowLevel];
	return tl->tasks[tl->now];
}

void TaskSwitcher::add(Task *task) {
	TaskLevel *tl = &_level[task->level];
	if (tl->running < kMaxTasksLevel) {
		tl->tasks[tl->running] = task;
		++tl->running;
		task->_running = true;
	}
}

void TaskSwitcher::remove(Task *task) {
	int i;
	TaskLevel *tl = &_level[task->level];

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
	task->_running = false;

	// このレベル内のタスクの配列をずらす
	for (; i < tl->running; ++i) {
		tl->tasks[i] = tl->tasks[i + 1];
	}
}
