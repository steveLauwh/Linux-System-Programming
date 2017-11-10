## 线程

进程是资源管理的最小单位，线程是程序执行的最小单位。

### 什么是线程

线程是进程的一条执行路径，每个进程由自己的代码段、数据段和堆栈段；线程包含独立的栈和CPU寄存器状态。

每个线程共享其所附属进程的所有资源。

在操作系统设计上，从进程演化出线程，最主要的目的就是更好的支持多处理器，并且减少进程上下文切换的开销。

线程按其调度者可分为用户级线程和内核级线程。

当进程退出时，该进程所产生的线程都会被强制退出并清除。

### 线程 API

每个进程内部的不同线程都有自己唯一标识，线程标识是 pthread_t 数据类型。

详细看：[线程 API](https://github.com/steveLauwh/Network-Programming/blob/master/Multithreading/Basic%20API%20of%20Multi-threading.md)

* 创建线程：`pthread_create`
* 主动终止：线程的执行函数中调用 return 语句或 pthread_exit()
* 被动终止：线程可以被同一进程的其它线程取消，调用pthread_cancel()
* 等待线程结束：`pthread_join`
* 线程清理函数：线程在退出时需要调用的函数

```c
#include <pthread.h>

// 线程清理函数
void pthread_cleanup_push(void (*routine)(void *), void *arg);
void pthread_cleanup_pop(int execute);
```

> 线程调用清理函数的时机

* 调用 pthread_exit;（注意，线程函数中调用 return 返回时，不会执行清理函数！）
* 响应 pthead_cancel 的取消请求
* 调用 thread_cleanup_pop，并传入非0的 execute 参数。

### 线程同步机制

详细看：[线程同步机制](https://github.com/steveLauwh/Network-Programming)

* 线程同步：是一个宏观概念，在微观上包含线程的相互排斥和线程的先后执行的约束问题。解决同步方式一般采用条件变量和信号量
* 线程互斥：线程执行的相互排斥（注意，它不关心线程间执行的先后顺序！）。解决互斥一般使用互斥锁、读写锁和信号量

> **互斥锁**

互斥锁数据类型：`pthread_mutex_t`

互斥锁（mutex）是用一种简单的加锁方法来控制对共享资源的访问。在同一时刻只能有一个线程拥有某个互斥锁，拥有上锁状态的线程能够对共享资源进行访问。
若其他线程希望上锁一个己经被上了互斥锁的资源，则该线程挂起，直到上锁的线程释放互斥锁为止。

```c
#include <pthread.h>

// 互斥锁属性的创建和销毁
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
int pthread_mutexattr_init(pthread_mutexattr_t *attr);
```

```c
 #include <pthread.h>

// 互斥锁的进程共享属性
int pthread_mutexattr_getpshared(const pthread_mutexattr_t *restrict attr, int *restrict pshared);
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared);
```

```c
#include <pthread.h>

// 互斥锁的类型：标准互斥锁、递归互斥锁、检错互斥锁、默认互斥锁
int pthread_mutexattr_gettype(const pthread_mutexattr_t *restrict attr, int *restrict type);
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);
```

> **读写锁**

读写锁的数据类型：`pthread_rwlock_t`

线程使用互斥锁缺乏并发性。

当读操作较多，写操作较少时，可使用读写锁提高线程并发性。

> **条件变量**

条件变量的数据类型：`pthread_cond_t`

条件变量通过允许线程阻塞和等待另一个线程发送信号的方法弥补了互斥锁的不足。

条件变量内部是一个等待队列，放置等待的线程，线程在条件变量上等待和通知。

**pthread_cond_wait 内部实现原理**

* 将线程自己插入到条件变量的等待队列中
* 释放 mutext 锁，允许其它线程对 "测试条件" 的改变
* 调用线程阻塞，等待其它线程通知唤醒
* 唤醒后，将自己从等待队列中删除
* 调用 lock 重新加 mutex 锁

```c
/************pthread_cond_wait()的使用方法**********/

pthread_mutex_t qlock;
pthread_cond_t  qready；

pthread_mutex_lock(&qlock);    /*lock*/

//等待某资源，并以 qready 作为条件通知我们
while (condition == FALSE){
    pthread_cond_wait(&qready, &qlock); /*enqueue-->unlock-->wait() -->lock*/
}

//do something

pthread_mutex_unlock(&qlock); /*unlock*/
```

> **信号量**

信号量数据类型：`sem_t`

信号量（semaphore）从本质上是一个非负整数计数器，是共享资源的数目，通常被用来控制对共享资源的访问。

通过 sem_post() 和 sem_wait() 函数对信号量进行加减操作，从而解决线程的同步和互斥。

信号量可以实现线程的同步和互斥。

> **自旋锁**

自旋锁是专为防止多处理器并发（实现保护共享资源）而引入的一种锁机制。

自旋锁不会引起调用者睡眠，如果自旋锁已经被别的执行单元保持，调用者就一直循环在那里看是否该自旋锁的保持者已经释放了锁，“自旋”一词就是因此而得名。

自旋锁只有在内核可抢占或 SMP（多处理器）的情况下才真正需要，在单 CPU 且不可抢占的内核下，自旋锁的所有操作都是空操作。

自旋锁不应该被持有时间过长。

自旋锁的缺点：

* 死锁：试图递归地获得自旋锁必然会引起死锁
* 过多占有 CPU 资源

```c
#include <pthread.h>

// 循环等待直到自旋锁解锁（置为1），然后将自旋锁锁上（置为0）
int pthread_spin_lock(pthread_spinlock_t *lock);   

int pthread_spin_trylock(pthread_spinlock_t *lock);
  
int pthread_spin_unlock(pthread_spinlock_t *lock);	 // 将自旋锁解锁（置为1）
```

### 死锁

死锁：两个线程试图同时占用两个资源，并按不同的次序锁定相应的共享资源。

避免死锁方法：

* 按相同的次序锁定相应的共享资源
* 使用 pthread_mutex_trylock()，它是 pthread_mutex_lock() 函数的非阻塞版
