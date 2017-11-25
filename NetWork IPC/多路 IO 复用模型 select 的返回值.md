## 多路 IO 复用模型 select 的返回值

```c
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

void FD_CLR(int fd, fd_set *set);
int  FD_ISSET(int fd, fd_set *set);
void FD_SET(int fd, fd_set *set);
void FD_ZERO(fd_set *set);
```

### select 的参数

* nfds：表示所有文件描述符的最大值加 1
* readfs：监视文件描述符读的变化
* writefs：监视文件描述符写的变化
* exceptfs：监视文件错误异常
* timeout：select 的超时时间

### select 的返回值

* 超时，返回 0
* 错误，返回 -1

### timeout

* 当 timeout 为 NULL，不传入时间结构，此时 select 置于阻塞状态，一定等到监视文件描述符集合中某个文件描述符发生变化为止
* 当 timeout 为 0，变成非阻塞函数，不管文件描述符是否有变化，都立即返回继续执行
* 当 timeout 大于 0，就是等待超时时间

