## 守护进程

守护进程(daemon)是生存期长的一种进程。它是一直在后台运行的进程。

所有守护进程者都以超级用户（用户 ID 为 0）的优先权运行。

守护进程的父进程都是 init 进程。

它独立于控制终端并且周期性地执行某种任务或等待处理某些发生的事件。

### 守护进程的编程要点

* 创建子进程，父进程退出(调用 fork，然后让父进程退出 exit；子进程成为孤儿进程，将由 init 领养)
* 在子进程中创建新会话(调用 setsid 函数，使进程完全独立出来，从而摆脱其他进程的控制)
* 改变当前目录为根目录(让 “/” 作为守护进程的当前工作目录)
* 重设文件权限掩码(umask(0))
* 关闭文件描述符

守护进程的核心：创建子进程，父进程关闭，重置子进程，符合守护进程的特点。

### syslog 服务

由于守护进程完全脱离了控制终端，因此，不能像其他程序一样通过输出错误信息到控制台的方式通知我们。

通常使用 syslog 服务，将出错信息输入到 `/var/log/syslog` 系统日志文件中去。

syslog 是 Linux 中的系统日志管理服务通过守护进程 syslog 来维护。

## 参考

* [Linux 守护进程的启动方法](http://www.ruanyifeng.com/blog/2016/02/linux-daemon.html)