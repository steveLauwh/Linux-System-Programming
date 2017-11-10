## 信号

信号本质上是在软件层次上对中断机制的一种模拟，是一种软件中断。

信号（signal）机制是 Linux 系统中最为古老的进程之间的通信机制，解决进程在正常运行过程中被中断的问题，导致进程的处理流程会发生变化。

信号是异步的，进程不可能一直等待信号的到来，也不知道信号会到来。

### 信号的处理机制

信号的接收不是在用户进程完成的，而是由内核代理。

用户进程由于中断或者 system call 陷入内核之后，将断点信息都保存到了堆栈中，在内核返回用户态时，如果存在被触发的 signal，
那么直接将待执行的 signal 处理函数 push 到堆栈中，在 CPU 切换到用户模式之后，
直接 pop 堆栈就可以执行 signal 处理函数并且返回到用户进程了。signal 处理函数应用了进程上下文，并且应用实际的中断模拟了进程的软中断过程。

### 信号的发送

```c
#include <signal.h>

typedef void (*sighandler_t)(int);

// 向内核登记信号处理函数，signum：要登记的信号值（如SIGCHLD）
sighandler_t signal(int signum, sighandler_t handler);
```

产生 SIGCHLD 信号：子进程结束时，父进程会收到该信号。如果父进程先终止，由子进程的终止由 init 进程接管。

当父进程捕获到 SIGCHLD 信号后要调用 wait 来回收子进程，否则，子进程会成为僵尸进程。

> 信号发送函数 kill 和 raise

```c
#include <sys/types.h>
#include <signal.h>

// 向指向进程发送一个信号
int kill(pid_t pid, int sig);

// 向进程本身发送一个信号
int raise(int sig);
```

### 设置定时器

```c
#include <unistd.h>

// 当定时器超时，产生 SIGALRM 信号
// seconds 为 0 表示取消以前设置的定时器
unsigned int alarm(unsigned int seconds);
```

```c
#include <unistd.h>

// 使调用进程挂起直到捕捉到一个信号
int pause(void);
```

```c
#include <sys/time.h>

// 获取定时器状态
int getitimer(int which, struct itimerval *curr_value);

// 设置定时器
int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value);
```

### 信号的可靠性

不可靠：信号可能会丢失，一个信号发生了，但进程却可能一直不知道这一点。

信号可靠性的机制：

* 信号在处理过程中 blocked 字段相应的标志位被置为1，处理完毕解除屏蔽（置0）
* 现在 Linux 信号在内核层面上都是可靠的。进程在处理信号期间，若发生了同类型的信号不会丢失（相同类型只保留一次，信号未决字相应的标志位被置 1），但不会被立即处理，而是延迟处理
* 若发生不同类型的信号则会被保留并直接处理，处理完后再处理原有信号

### 中断的系统调用

> 慢系统调用

* 可能会永远阻塞的系统调用
* pause 和 wait 系统调用
* 从终端设备、管道或网络设备上的文件读取
* 向上述文件写入
* 某些设备上的文件打开
* 一些设备的 ioctl 操作
* 一些进程间的通信函数

慢系统调用被信号中断后，当信号处理完毕后，该系统调用会被重新从头开始执行，而不是从中断的地方继续执行。

在信号发生时，慢系统调用可以被中断并在信号处理后系统调用会被重启。

在信号发生时，用户函数可以被中断但不能被重启，沿着中断点继续执行（在用户函数中要保证数据一致性，即可重入性不要去访问全局变量和静态变量，堆中的变量若在用户函数内部分配没有关系，否则会出现不可重入性）。

### 可重入函数和不可重入函数

* 在调用某个函数过程中出现信号，且该信号处理函数中再次调用该函数，就可能产生可重放入性问题
* 访问全局或静态变量的函数是不可重入函数

### 信号集和信号屏蔽

信号集函数：信号集为一个或多个信号的集合，主要用在信号屏蔽函数中。

```c
#include <signal.h>

// set 为信号集，将信号集清空，对应将所有信号屏蔽字置 0
int sigemptyset(sigset_t *set);

// 将所有信号加入到信号集中，对应将所有信号的屏蔽字置 1
int sigfillset(sigset_t *set);

// 将某个信号加入到信号集中，对应将的信号屏蔽字某位置 1
int sigaddset(sigset_t *set, int signum);

// 将某个信号从信号集中删除，对应将的信号屏蔽字某位置 0
int sigdelset(sigset_t *set, int signum);

// 测试信号集中是否包含有某个信号，对应判断信号屏蔽字某位是否置 1
int sigismember(const sigset_t *set, int signum);
```

