## 文件 I/O

* [文件 I/O](https://github.com/steveLauwh/Linux-System-Programming/blob/master/File%20IO/%E6%96%87%E4%BB%B6%20IO.md)
* [文件和目录](https://github.com/steveLauwh/Linux-System-Programming/blob/master/File%20IO/%E6%96%87%E4%BB%B6%E5%92%8C%E7%9B%AE%E5%BD%95.md)
	
这些不带缓存的文件 I/O 函数都是内核提供的系统调用。它们是基于文件描述符的 I/O，对文件描述符进行操作。

系统最大打开文件描述符数：`cat /proc/sys/fs/file-max`

进程最大打开文件描述符数：`ulimit -n`
