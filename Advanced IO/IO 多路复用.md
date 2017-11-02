## I/O 多路复用

### select

```c
#include <sys/select.h>

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

void FD_CLR(int fd, fd_set *set);
int  FD_ISSET(int fd, fd_set *set);
void FD_SET(int fd, fd_set *set);
void FD_ZERO(fd_set *set);
```

### poll

```c
#include <poll.h>

struct pollfd {
    int   fd;         /* file descriptor */
    short events;     /* requested events */
    short revents;    /* returned events */
};

int poll(struct pollfd *fds, nfds_t nfds, int timeout);
```

### epoll

```c
#include <sys/epoll.h>

int epoll_create(int size);
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
```

## 参考

* [聊聊IO多路复用之select、poll、epoll详解](https://my.oschina.net/xianggao/blog/663655)
* [使用 libevent 和 libev 提高网络应用性能——I/O 模型演进变化史](http://blog.csdn.net/hguisu/article/details/38638183)
