# I/O 多路复用

* 弄清楚机制
* 优缺点
* 使用场景
* 实现服务端和客户端通信

支持 I/O 多路复用的系统调用主要有 select、poll、epoll 函数，通过一种机制，一个进程可以监视多个描述符，一旦某个描述符就绪（一般是读就绪或者写就绪），能够通知程序进行相应的读写操作。

## select

不是传统方式那样阻塞在真正 I/O 读写的系统调用上，而是阻塞在 select 系统调用上，等待我们关注的描述符就绪。

select 系统调用是用来让我们的程序监视多个文件句柄的状态变化的。

```c
#include <sys/select.h>

struct timeval {
    long    tv_sec;         /* seconds */
    long    tv_usec;        /* microseconds */
};

void FD_CLR(int fd, fd_set *set);  // 将 fd 从 set 集合中删除
int  FD_ISSET(int fd, fd_set *set); // 检测 fd 是否在 set 集合中
void FD_SET(int fd, fd_set *set);  // 将 fd 加入 set 集合中
void FD_ZERO(fd_set *set);  // 将 set 集合清零，不包含任何 fd

// nfds：最大 fd 加 1
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
```

### select 机制

当一个客户端连接上服务器时，服务器就将其连接的 fd 加入到 fd_set 集合，然后轮询各个文件句柄，探测文件句柄的状态变化，等到这个连接准备好读或写的时候，就通知程序进行 I/O 操作，与客户端进行数据通信。

所以 select 本质是通过设置或者检查存放 fd_set 集合来做下一步处理。

### select 优缺点

> **select 优点**

只用单进程(线程)可以处理多个连接，占用资源少，在不消耗太多 CPU 情况下，为多个客户端提供服务。

> **select 缺点**

* 单个进程能够监视的文件描述符的数量存在最大限制，由 FD_SETSIZE 设置，32 位是 1024 个，64 位是 2048 个
* 对 socket 进行线性扫描，不管哪个 socket 活跃，都需要遍历一遍，即采用轮询的方法，效率较低
* 需要维护一个用来存放大量 fd 的数据结构，这样会使得用户空间和内核空间在传递该结构时复制开销大
* select 的触发方式是水平触发，如果用户进程不响应则会一直发送通知

### select 使用场景

在连接数少并且连接都十分活跃的情况。

### select 实现服务端和客户端通信


## poll

```c
#include <poll.h>

struct pollfd {
    int   fd;         /* file descriptor */
    short events;     /* requested events */
    short revents;    /* returned events */
};

// events：监视该文件描述符的事件掩码
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
```

### poll 机制

与 select 相似的处理流程，先创建描述符集合，设置关注事件，然后调用 poll，等待事件发生，轮询描述符集合，检查事件，处理事件。

但是细节有差别，select 需要为读、写、异常事件分别创建一个描述符集合，最后轮询的时候，需要分别轮询这三个集合。而 poll 只需要一个集合，在每个描述符对应的结构上可以设置读、写、异常事件(事件掩码)，最后轮询的时候，可以同时检查三种事件。

### poll 优缺点

> **poll 优点**

它没有最大连接数的限制，原因是基于链表存储的。

> **poll 缺点**

* 与 select 一样，需要维护存放大量 fd 的数据结构，在用户空间和内核空间传递
* poll 的触发方式也是水平触发，如果用户进程没有响应，下次 poll 会再次通知

### poll 使用场景

与 select 一样，适合连接数少的情况，当随着监视的描述符 fd 数量增长，其效率也会线性下降。

### poll 实现服务端和客户端通信


## epoll

```c
#include <sys/epoll.h>

struct epoll_event {
   uint32_t     events;    /* Epoll events */
   epoll_data_t data;      /* User data variable */
};

// 创建一个 epoll 的句柄，用来告诉内核要监听的数目
int epoll_create(int size);  

// epoll 的事件注册函数
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

// 等待内核通知事件发生
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
```

### epoll 机制

它把描述符列表交给内核，一旦有事件发生，内核把发生事件的描述符列表通知给进程(回调机制)，这样就避免了轮询整个描述符列表。

epoll 支持水平触发和边缘触发，最大的特点在于边缘触发，内核只告诉进程哪些 fd 刚刚变为就绪态，并且只会通知一次。

epoll 使用 “事件” 的就绪通知方式，通过 epoll_ctl 注册 fd，一旦该 fd 就绪，内核就会采用类似 callback 的回调机制来激活该 fd，epoll_wait 便可以收到通知。

### epoll 对文件描述符的操作两种模式

* LT（level trigger）：当 epoll_wait 检测到描述符事件发生并将此事件通知应用程序，应用程序可以不立即处理该事件。下次调用 epoll_wait 时，会再次响应应用程序并通知此事件。

* ET（edge trigger）：当 epoll_wait 检测到描述符事件发生并将此事件通知应用程序，应用程序必须立即处理该事件。如果不处理，下次调用 epoll_wait 时，不会再次响应应用程序并通知此事件。

ET 模式的效率比 LT 模式高。

### epoll 优缺点

> **epoll 优点**

* 没有最大并发连接的限制
* 效率提升，没有采用轮询的方式，只有活跃可用的 fd 才会调用 callback 函数
* epoll 使用 mmap 内存映射，将减少复制开销

> **epoll 缺点**

实现具有较好跨平台能力的服务器比较困难。可以用事件驱动库 libevent、libev 来解决。

### epoll 使用场景

适合大量并发连接。

### epoll 实现服务端和客户端通信

## 参考

* [聊聊IO多路复用之select、poll、epoll详解](https://my.oschina.net/xianggao/blog/663655)
* [Linux下I/O多路转接之select --fd_set](http://blog.csdn.net/li_ning_/article/details/52165993)
* [使用 libevent 和 libev 提高网络应用性能——I/O 模型演进变化史](http://blog.csdn.net/hguisu/article/details/38638183)
