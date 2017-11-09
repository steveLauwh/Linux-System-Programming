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


