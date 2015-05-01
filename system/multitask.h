/*
 * マルチタスク
 */

#ifndef _MULTITASK_H_
#define _MULTITASK_H_

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

enum TaskFlag { TASKFLAG_FREE, TASKFLAG_SLEEPING, TASKFLAG_RUNNING };

class Task {
public:
	char *name_;
	int selector_;
	TaskFlag flags_;
	int level_, priority_;
	Queue *queue_;
	TSS32 tss_;
	//int fpu[108 / 4];
	//int stack;

public:
	Task(char *name, int level, int priority, void (*mainLoop)(), Queue *queue = nullptr);
	~Task() {
		if (queue_) delete queue_;
	};
	static void *operator new(size_t size);
	static void operator delete(void *) {}; // タスクの削除を行うべき
	void run(int, int);
	void sleep();
};

struct TaskLevel {
	int running; // the number of running tasks
	int now; // the index of a runnning task
	Task *tasks[kMaxTasksLevel]; // tasks on this level
};

class TaskController {
public:
	static int now_lv_; // the index of a running level
	static bool lv_change_; // 次回タスクスイッチ時にレベルも変えたほうがいいか
	static TaskLevel *level_;
	static Task *tasks0_;

public:
	//static Task *task_fpu_;
	static Timer *timer_;

public:
	static Task *init();
	static Task *alloc();
	static void switchTask();
	static void switchTaskSub();
	static Task *getNowTask();
	static void add(Task*);
	static void remove(Task*);
};

#endif
