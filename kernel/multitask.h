/*
 * マルチタスク
 */

#ifndef _MULTITASK_H_
#define _MULTITASK_H_

#include <Queue.h>

const int MAX_TASKS = 1000;
const int kTaskGdt0 = 3;
const int kMaxTasksLevel = 100;
const int MAX_TASKLEVELS = 10;

struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};

enum class TaskFlag { Free, Sleeping, Running };

class Task;

class TaskQueue : public Queue<int> {
private:
	Task *task;

public:
	TaskQueue(int size, Task *task_);
	bool push(int data);
};

class Task {
private:
	Task() : flags(TaskFlag::Free), queue(nullptr) {}

public:
	char *name;
	int selector;
	TaskFlag flags;
	int level, priority;
	TaskQueue *queue;
	TSS32 tss;
	int fpu[108 / 4];
	int stack;

public:
	friend class TaskController;
	Task(char *name_, int level_, int priority_, void (*mainLoop)());
	Task(char *name_, int level_, int priority_, int queueSize, void (*mainLoop)());
	~Task();
	static void *operator new(size_t);
	static void operator delete(void *) {}
	void run(int newLevel, int newPriority);
	void sleep();
};

struct TaskLevel {
	int running; // the number of running tasks on the level
	int now; // the index of the now running task
	Task *tasks[kMaxTasksLevel]; // tasks on the level
	
	TaskLevel() : running(0), now(0) {}
};

class TaskController {
public:
	static int nowLevel;
	static bool levelChanged; // 次回タスクスイッチ時にレベルも変えたほうがいいか
	static TaskLevel level[];
	static Task tasks0[MAX_TASKS];

public:
	static Task *taskFPU;
	static Timer *timer;

public:
	static Task *init();
	static void switchTask();
	static void switchTaskSub();
	static Task *getNowTask();
	static void add(Task *task);
	static void remove(Task *task);
};

#endif
