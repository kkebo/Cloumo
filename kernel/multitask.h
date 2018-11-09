/*
 * マルチタスク
 */

#pragma once

#include <stddef.h>
#include <pistring.h>
#include <Queue.h>
#include <List.h>

const int kTaskGDT0 = 3;
const int kMaxTasksLevel = 100;
const int MAX_TASKLEVELS = 10;

struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};

class Task;

struct TaskLevel {
	int running; // the number of running tasks on the level
	int now; // the index of the now running task
	Task *tasks[kMaxTasksLevel]; // tasks on the level
	
	TaskLevel() : running(0), now(0) {}
};

class Timer;

class TaskSwitcher {
private:
	static int nowLevel;
	static bool levelChanged; // 次回タスクスイッチ時にレベルも変えたほうがいいか
	static List<Task *> *_taskList;
	static TaskLevel _level[];
	static Task *taskFPU;
	static Timer *timer;
	static int taskCount;
	
	static void switchTask();
	static void switchTaskSub();
	static void add(Task *task);
	static void remove(Task *task);

public:
	static const TaskLevel (&level)[MAX_TASKLEVELS];
	static List<Task *> *const &taskList;

	friend class Task;
	friend void IntHandler07(int *esp); // FPU
	friend void IntHandler20(int *esp); // PIT割り込み
	static Task *init();
	static Task *getNowTask();
};


class TaskQueue : public Queue<int> {
private:
	Task *task;

public:
	friend class TimerController; // remove で使われている．代替案を思いつき次第削除すること
	TaskQueue(int size, Task *task_);
	bool push(int data);
};

class Task {
protected:
	string _name;
	bool _running = false;
	int _level, _priority;
	int selector;
	TSS32 tss;
	int fpu[108 / 4] = {
		0x037f, /* CW(control word) */
		0x0000, /* SW(status word)  */
		0xffff  /* TW(tag word)     */
	};
	uintptr_t stack;
	
	Task(); // メインタスク用

public:
	const string &name = _name;
	const bool &running = _running;
	const int &level = _level, &priority = _priority;
	TaskQueue *queue = nullptr;

	friend class TaskSwitcher;
	friend void IntHandler07(int *esp); // FPU
	template <typename ...Args>
	Task(const string &name_, int level_, int priority_, void (*mainLoop)(Args...), int args[] = {}) : _name(name_) {
		// GDT に登録
		selector = (kTaskGDT0 + TaskSwitcher::taskCount) * 8;
		SetSegmentDescriptor((SegmentDescriptor *)kAdrGdt + kTaskGDT0 + TaskSwitcher::taskCount, 103, (uintptr_t)&tss, kArTss32);
		++TaskSwitcher::taskCount;
		
		// 64KB のスタック確保
		stack = reinterpret_cast<uintptr_t>(malloc4k(64 * 1024));
		
		// Task State Segment の設定
		tss.eip = reinterpret_cast<uintptr_t>(mainLoop);
		tss.eflags = 0x00000202;
		tss.eax = 0;
		tss.ecx = 0;
		tss.edx = 0;
		tss.ebx = 0;
		tss.esp = stack + 64 * 1024 - (sizeof...(Args) + 1) * 4;
		tss.ebp = 0;
		tss.esi = 0;
		tss.edi = 0;
		tss.es = 1 * 8;//0
		tss.cs = 2 * 8;
		tss.ss = 1 * 8;
		tss.ds = 1 * 8;//0
		tss.fs = 1 * 8;//0
		tss.gs = 1 * 8;//0
		tss.ldtr = 0;
		tss.iomap = 0x40000000;
		tss.ss0 = 0;
		
		// 引数渡し
		for (size_t i = 0; i < sizeof...(Args); ++i) {
			*((int *)(tss.esp + 4 * (i + 1))) = args[i];
		}
		
		// add this pointer to the list of tasks
		TaskSwitcher::_taskList->push_back(this);
		
		// run
		run(level_, priority_);
	}
	template <typename ...Args>
	Task(const char *name_, int level_, int priority_, int queueSize, void (*mainLoop)(Args...), int args[] = {}) : Task(name_, level_, priority_, mainLoop, args) {
		queue = new TaskQueue(queueSize, this);
	}
	~Task();
	void run(int newLevel, int newPriority);
	void sleep();
};
