# I/O

系统调用和库函数的差别：系统调用通常提供一个最小接口，而库函数通常提供比较复杂的功能。

文件 I/O 的函数属于系统调用，在内核中进行的，所以是不带缓冲的 I/O 函数。

带有缓冲机制的函数是以内存空间作为缓冲区，避免不当操作降低多写性能。

## 文件 I/O

文件描述符是一个非负整数。文件描述符的变化范围是 0 ~ OPEN_MAX - 1。

| 文件描述符 | 用途 |  POSIX 名称  | stdio 流|
| --------   | -----:  | :----:  |:----:  |
| 0     | 标准输入 |   STDIN_FILENO    | stdin|
| 1        |   标准输出   |   STDOUT_FILENO   |stdout|
| 2        |    标准错误    |  STDERR_FILENO  |stderr|

```
& 表示等同于

2>&1   // 标准错误输出重定向等同于 标准输出
```

> 函数 open 和 openat

调用 open 或 openat 函数可以打开或创建一个文件。

```c
#include <fcntl.h>

int open(const char *path, int oflag, .../* mode_t mode */);

int openat(int fd, const char *path, int oflag, .../* mode_t mode */);
```

> 函数 create

创建一个新文件。

```c
#include <fcntl.h>

int creat(const char *path, mode_t mode);
```

> 函数 close

关闭一个打开文件。关闭一个文件时还会释放该进程加在该文件上的所有记录锁。

```c
#include <unistd.h>

int close(int fd);
```

> 函数 lseek

调用 lseek 显示地为一个打开文件设置偏移量。

```c
#include <unistd.h>

off_t lseek(int fd, off_t offset, int whence);
```

> 函数 read

打开文件中读数据

```c
#include <unistd.h>

ssize_t read(int fd, void *buf, size_t nbytes);
```

> 函数 write

向打开文件写数据。

```c
#include <unistd.h>

ssize_t write(int fd, const void *buf, size_t nbytes);
```

## 原子操作

```c
#include <unistd.h>

ssize_t pread(int fd, void *buf, size_t nbytes, off_t offset);

ssize_t pwrite(int fd, const void *buf, size_t nbytes, off_t offset); 
```

调用 pread 相当于调用 lseek 后调用 read；调用 pwrite 相当于调用 lseek 后调用 write。

原子操作指的是由多步组成的一个操作。如果该操作原子地执行，则要么执行完所有步骤，要么一步也不执行，不可能只执行所有步骤的一个子集。

## 函数 dup 和 dup2

都可以用来复制一个现有的文件描述符。

```c
#include <unistd.h>

int dup(int fd);
int dup2(int fd, int fd2);
```

## 函数 sync、fsync 和 fdatasync

```c
#include <unistd.h>

// 只对由文件描述符 fd 指定的一个文件起作用，并且等待写磁盘操作结束才返回
int fsync(int fd);

// 类似 fsync
int fdatasync(int fd); 

// 将所有修改过的块缓冲区排入写队列
void sync(void);
```
## 函数 fcntl

可以改变已经打开文件的属性。

```c
#include <fcntl.h>

int fcntl(int fd, int cmd, .../* int arg */);
```

## 函数 ioctl

```c
#include <unistd.h>

#include <sys/ioctl.h>

int ioctl(int fd, int request,...);
```






















