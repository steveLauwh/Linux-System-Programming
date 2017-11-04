## 网络进程间通信(socket)

不同计算机(通过网络相连)上的进程相互通信的机制。

socket - create an endpoint for communication
 
应用程序用套接字描述符访问套接字。

网络中的进程通信，需要唯一标识一个进程(网络层的 IP 地址 + 传输层的 协议 + 端口)。
 
### 创建一个套接字

```c
#include <sys/socket.h>

// 成功，返回套接字描述符；失败，返回 -1
int socket(int domain, int type, int protocol);
```
* domain 参数确定通信的特性，protocol family；如 AF_INET(IPV4 因特网域)，AF_INET6(IPV6 因特网域)

* type 参数确定套接字的类型，如 SOCK_STREAM(有序的、可靠的、双向的、面向连接的字节流)，SOCK_DGRAM(无连接的、不可靠的报文传递)，
SOCK_SEQPACKET(固定长度的、有序的、可靠的、面向连接的报文传递)

* protocol 参数通常为 0，表示为给定的域和类型选择默认协议，如 SOCK_STREAM(TCP)，SOCK_DGRAM(UDP)

SCTP(流控制传输协议)提供了因特网域上的顺序数据包服务。

### 销毁一个套接字

```c
#include <unistd.h>

// 释放套接字
int close(int fd);
```

```c
#include <sys/socket.h>

// 套接字通信是双向的，禁止一个套接字的 I/O
// how 为 SHUT_RD 关闭读端，SHUT_WR 关闭写端，SHUT_RDWR 关闭读写端
int shutdown(int sockfd, int how);
```

### bind(将套接字与地址关联)

IP 地址 + 端口号

不同的主机之间的处理器架构不一样，导致字节序不一致；而 TCP/IP 协议栈使用大端字节序，那么应用程序都要转换大端字节序传递。

```c
#include <arpa/inet.h>

uint32_t htonl(uint32_t hostlong); // 主机字节序转换为网络字节序，表示 32 位整数

uint16_t htons(uint16_t hostshort); // 主机字节序转换为网络字节序，表示 16 位整数

uint32_t ntohl(uint32_t netlong); // 网络字节序转换为主机字节序，表示 32 位整数

uint16_t ntohs(uint16_t netshort); // 网络字节序转换为主机字节序，表示 16 位整数
```

通用的地址结构 sockaddr

```c
// IPv4
struct sockaddr {
   sa_family_t sa_family;
   char        sa_data[14];
}
```

```c
// IPv6
struct sockaddr_in6 {
   sa_family_t     sin6_family;   /* AF_INET6 */
   in_port_t       sin6_port;     /* port number */
   uint32_t        sin6_flowinfo; /* IPv6 flow information */
   struct in6_addr sin6_addr;     /* IPv6 address */
   uint32_t        sin6_scope_id; /* Scope ID (new in 2.4) */
};

struct in6_addr {
   unsigned char   s6_addr[16];   /* IPv6 address */
};
```

地址格式转换函数

```c
#include <arpa/inet.h>

// 将文本字符串格式转换成网络字节序的二进制地址
int inet_pton(int af, const char *src, void *dst);

// 将网络字节序的二进制地址转换成文本字符串格式
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
```

服务器保留一个地址并且注册在 /etc/services 或者某个名字服务中。

使用 bind 函数来关联地址和套接字。

```c
#include <sys/socket.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

### listen(监听)

服务器调用 listen 函数表示可以接受连接请求。

```c
#include <sys/socket.h>

// backlog 表示队列的最大连接数
int listen(int sockfd, int backlog);
```

### connect(客户端连接请求)

```c
#include <sys/socket.h>

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

### accept(服务端接受连接请求)

```c
#include <sys/socket.h>

// 返回已连接的 socket 描述符
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

### 数据传输

```
#include <sys/socket.h>

// 面向连接的套接字，通常使用 send 发送
ssize_t send(int sockfd, const void *buf, size_t len, int flags);

ssize_t recv(int sockfd, void *buf, size_t len, int flags);    
```

```c
#include <sys/socket.h>

// 发送报文，通常用于无连接的套接字
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
                      const struct sockaddr *dest_addr, socklen_t addrlen);
                      
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                        struct sockaddr *src_addr, socklen_t *addrlen);    
```

```c
#include <sys/socket.h>

// 带有缓冲区
ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
```

### 套接字选项

* SO_REUSEADDR：可以让端口释放后立即使用
* TCP_NODELAY：不使用 Nagle 算法，不会将小包进行拼接成打包再发送
* TCP_CORK：每次发送最大的数据量
* SO_LINGER：延迟 socket 的 close 行为
* TCP_DEFER_ACCEPT：推迟接收
* SO_KEEPALIVE：用于保持连接检测对方主机是否崩溃，避免服务器永远阻塞于 TCP 连接的输入
* SO_SNDTIMEO：设置 socket 的发送超时时间
* SO_RCVTIMEO：设置 socket 的接收超时时间

```c
#include <sys/socket.h>

// optval 参数
int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```
