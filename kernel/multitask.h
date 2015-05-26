/*
 * マルチタスク
 */

#pragma once

#include <Queue.h>

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

class TaskQueue : public Queue<int> {
private:
	Task *task;

public:
	TaskQueue(int size, Task *task_);
	bool push(int data);
};

class Task {
protected:
	const char *_name;
	bool _running = false;
	int _level, _priority;
	int selector;
	TSS32 tss;
	int fpu[108 / 4] = {
		0x037f, /* CW(control word) */
		0x0000, /* SW(status word)  */
		0xffff  /* TW(tag word)     */
	};
	int stack;
	
	Task(); // メインタスク用

public:
	const char *const &name = _name;
	const bool &running = _running;
	const int &level = _level, &priority = _priority;
	TaskQueue *queue = nullptr;

	friend class TaskSwitcher;
	friend void IntHandler07(int *esp); // FPU
	Task(const char *name_, int level_, int priority_, void (*mainLoop)());
	Task(const char *name_, int level_, int priority_, int queueSize, void (*mainLoop)());
	~Task();
	void run(int newLevel, int newPriority);
	void sleep();
};

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

	friend class Task;
	friend void IntHandler07(int *esp); // FPU
	friend void IntHandler20(int *esp); // PIT割り込み
	static Task *init();
	static Task *getNowTask();
};
