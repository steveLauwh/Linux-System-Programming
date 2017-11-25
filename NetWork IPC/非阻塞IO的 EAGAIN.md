## 非阻塞 IO 的 EAGAIN

在非阻塞式 IO 中，用户进程其实需要不断地主动询问 kernel 数据是否准备好。

在 Linux 进行非阻塞的 socket 接收数据时经常出现 Resource temporarily unavailable，errno 代码为 11(EAGAIN)。

在非阻塞状态下，recv() 接口在被调用后立即返回，返回值代表不同的含义：

* recv() 返回值大于 0，表示接收数据完毕，返回值即是接收到的字节数
* recv() 返回 0，表示连接已经正常断开
* recv() 返回 -1，且 errno 等于 EAGAIN，表示 recv 操作还没执行完成
* recv() 返回 -1，且 errno 不等于 EAGAIN，表示 recv 操作遇到系统错误 errno
