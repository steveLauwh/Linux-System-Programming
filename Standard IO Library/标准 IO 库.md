## 标准 I/O 库

对于标准 I/O 库，围绕 流(stream) 进行的。

### 设置流的定向
```c
#include <wchar.h>

int fwide(FILE *stream, int mode);
```

### 缓冲

标准 I/O 库提供缓冲的目的是尽可能减少使用 read 和 write 调用的次数。

标准 I/O 提供 3 种类型的缓冲

* 全缓冲：当填满标准 I/O 缓存后才进行实际的 I/O 操作
* 行缓冲：当输入或输出中遇到新行符时，标准 I/O 库执行 I/O 操作
* 不带缓冲：标准 I/O 库不对字符进行缓冲存储

```c
#include <stdio.h>

// 冲洗一个流，使流所有未写的数据都被传送到内核
int fflush(FILE *stream);
```

### 打开一个文件

```c
#include <stdio.h>

FILE *fopen(const char *path, const char *mode);

FILE *fdopen(int fd, const char *mode);

FILE *freopen(const char *path, const char *mode, FILE *stream);
```

### 关闭文件

```c
#include <stdio.h>

int fclose(FILE *fp);
```

### 读一个字符

```c
#include <stdio.h>

int fgetc(FILE *stream);

int getc(FILE *stream);

int getchar(void);
```

### 输出一个字符

```c
#include <stdio.h>

int fputc(int c, FILE *stream);

int putc(int c, FILE *stream);

int putchar(int c);
```

### 输入一行

```c
char *fgets(char *s, int size, FILE *stream);

char *gets(char *s);
```

### 输出一行

```c
// 遇到 null 字节时就停止
int fputs(const char *s, FILE *stream);

int puts(const char *s);
```

### 判断文件结尾

```c
int feof(FILE *stream);
```

### 读和写(二进制流)

```c
#include <stdio.h>

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
```

### 定位流

```c
#include <stdio.h>

// The whence argument to fseek() was not SEEK_SET, SEEK_END, or SEEK_CUR.
int fseek(FILE *stream, long offset, int whence);
```

### 格式化输出

```c
#include <stdio.h>

int printf(const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);
```

### 格式化输入

```c
#include <stdio.h>

int scanf(const char *format, ...);
int fscanf(FILE *stream, const char *format, ...);
int sscanf(const char *str, const char *format, ...);
```

## 参考

* [C语言详解 FILE文件操作](http://blog.csdn.net/yong_sun/article/details/8921061)
