# I/O

系统调用和库函数的差别：系统调用通常提供一个最小接口，而库函数通常提供比较复杂的功能。

## 文件 I/O

文件描述符是一个非负整数。文件描述符的变化范围是 0 ~ OPEN_MAX - 1。

| 文件描述符 | 用途 |  POSIX 名称  | stdio 流|
| --------   | -----:  | :----:  |:----:  |
| 0     | 标准输入 |   STDIN_FILENO    | stdin|
| 1        |   标准输出   |   STDOUT_FILENO   |stdout|
| 2        |    标准错误    |  STDERR_FILENO  |stderr|

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

> 




















