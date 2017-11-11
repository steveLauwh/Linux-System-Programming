## 线程控制

### 线程状态转换

通过 pthread_create() 创建一个线程，进入 Runnale 就绪态

线程处于就绪态，当获得 CPU 后，进入 Running 运行态

线程处于运行态，运行结束或异常退出，进入 Dead 态

线程处于运行态，调用 sleep() 或 pthread_join()，进入 Blocked 态

线程处于运行态，调用互斥锁或读写锁，进入 "锁定Blocked" 态

线程处于运行态，调用 pthread_cond_wait() 进入 "等待Blocked" 态

线程处于 "等待Blocked" 态，调用 pthread_cond_signal() 进入 "锁定Blocked" 态

### 线程属性和线程优先级

线程属性结构体：`pthread_attr_t`

```c
#include <pthread.h>

// 线程属性的初始化和销毁
int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);
```

```c
#include <pthread.h>

// 设置和获取分离属性
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
int pthread_attr_getdetachstate(pthread_attr_t *attr, int *detachstate);
```

* 以默认方式启动的线程，在线程结束后不会自动释放占用的系统资源，要在主控线程中调用 pthread_join() 后才会释放。
* 以分离状态启动的线程，在线程结束后会自动释放所占有的系统资源

```c
#include <pthread.h>

// 获取和设置优先级
int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param);
int pthread_attr_getschedparam(pthread_attr_t *attr, struct sched_param *param);
```

```c
#include <pthread.h>

// 设置调度策略和获取调度策略
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);
int pthread_attr_getschedpolicy(pthread_attr_t *attr, int *policy);
```

* SCHED_OTHER：分时调度策略，不支持优先级使用（默认调度策略）
* SCHED_FIFO：实时调度策略，一旦占用 CPU 则一直运行，直到有更高优先级到达或自己放弃
* SCHED_RR：实时调度策略，时间片轮转，当进程的时间片用完，系统将重新分配时间片，并置于就绪队列尾

### 线程私有数据

* 线程私有数据是存储和查询与某个线程相关的数据的一种机制，它希望每个线程可以独立的访问数据副本，而不需要担心与其他线程的同步访问问题
* 进程中的所有线程都可以访问进程的整个地址空间（如全局变量），从而造成线程间使用数据的混乱，而线程私有数据可以维护基于每个线程的数据，提高数据的独立性

```c
#include <pthread.h>

// 创建与线程私有数据关联的键
int pthread_key_create(pthread_key_t *key, void (*destructor)(void*));

// 取消键与私有数据的关联
int pthread_key_delete(pthread_key_t key);
```

```c
#include <pthread.h>

// 设置和获取键关联的私有数据
void *pthread_getspecific(pthread_key_t key);
int pthread_setspecific(pthread_key_t key, const void *value);
```

### 线程和信号

在 Linux 的多线程中使用信号机制，与在进程中使用信号机制有着根本的区别。

在进程环境中，对信号的处理是，先注册信号处理函数，当信号异步发生时，调用处理函数来处理信号。它完全是异步的。
因此，信号处理函数里有时要考虑某些函数可重入或被异步信号中断后的处理。

多线程中处理信号的原则却完全不同，它的基本原则是：将对信号的异步处理，转换成同步处理，也就是说用一个线程专门的来“同步等待”信号的到来，而其它的线程可以完全不被该信号中断/打断(interrupt)。

```c
#include <signal.h>

// 等待一个或者多个指定信号发生
int sigwait(const sigset_t *set, int *sig);
```

```c
#include <signal.h>

// 线程的信号屏蔽
int pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset);
```

```c
#include <signal.h>

// 向指定的线程发送信号
int pthread_kill(pthread_t thread, int sig);
```

异步处理的方式：利用信号处理函数（由 sigaction 注册的函数）

同步处理的方式：sigwait 将异步信号以同步的方式处理

* 主线程设置信号屏蔽字，阻塞希望同步处理的信号
* 主线程创建一个信号处理线程，该线程将希望同步处理的信号集作为 sigwait() 的参数
* 主线程做其他工作，如创建其它工作线程。（注意主线程的信号屏蔽字会被其创建的新线程继承，因此这些线程没调用 sigwait，所以将不会收到信号）

### 线程和fork

* 在多线程执行的情况下调用 fork 函数，仅会将发起调用的线程复制到子进程中去（线程ID与父进程发起fork调用的线程ID一样）。也就是说，不能同时创建出与父进程一样多的子线程

* 其他线程均在子进程中立即停止并消失，并且不会为这些线程调用清理函数以及针对线程局部存储变量的析构函数，这可能会造成子进程的内存泄漏

* 虽然只有 fork 调用线程被复制到子进程，但子进程继承整个地址空间的副本，也从父进程那里继承了所有互斥量、读写锁和条件变量的状态。这意味着，如果某个线程锁定了某个互斥锁，那么在子进程中这个互斥锁可能因得不到释放而造成死锁现象的发生
