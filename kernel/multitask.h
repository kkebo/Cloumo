/*
 * マルチタスク
 */

#ifndef _MULTITASK_H_
#define _MULTITASK_H_

#include "Queue.h"

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
public:
	Task *task_;
	
	TaskQueue(int size, Task *task);
	bool push(int data);
};

class Task {
public:
	char *name_;
	int selector_;
	TaskFlag flags_;
	int level_, priority_;
	TaskQueue *queue_;
	TSS32 tss_;
	int fpu[108 / 4];
	int stack;

public:
	Task() {}
	Task(char *name, int level, int priority, void (*mainLoop)());
	Task(char *name, int level, int priority, int queueSize, void (*mainLoop)());
	~Task();
	static void *operator new(size_t size);
	static void operator delete(void *) {}
	void run(int, int);
	void sleep();
};

struct TaskLevel {
	int running; // the number of running tasks on the level
	int now; // the index of the now running task
	Task *tasks[kMaxTasksLevel]; // tasks on the level
};

class TaskController {
public:
	static int now_lv_;
	static bool lv_change_; // 次回タスクスイッチ時にレベルも変えたほうがいいか
	static TaskLevel *level_;
	static Task *tasks0_;

public:
	static Task *task_fpu_;
	static Timer *timer_;

public:
	static Task *init();
	static void switchTask();
	static void switchTaskSub();
	static Task *getNowTask();
	static void add(Task*);
	static void remove(Task*);
};

#endif
