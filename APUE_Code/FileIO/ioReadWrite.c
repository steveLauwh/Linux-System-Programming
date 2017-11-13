#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

#define BUFFERSIZE 1024

// 复制功能
void copy(int fin, int fout)
{
	char buffer[BUFFERSIZE];

	ssize_t size;

	lseek(fin, 0L, SEEK_SET);
	lseek(fout, 0L, SEEK_SET);

	// 将源文件的内容读到 buffer, 注意 buffer 长度只有 1024
	while ((size = read(fin, buffer, BUFFERSIZE)) > 0)
	{
		// 将 buffer 内容读到目标文件中
		if (write(fout, buffer, size) != size)
		{
			fprintf(stderr, "write error : %s\n", strerror(errno));
			
			exit(1);
		}
	}

	if (size < 0)
	{
		fprintf(stderr, "read error : %s\n", strerror(errno));

		exit(1);
	}

}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "usage: %s srcfile destfile\n", argv[0]);
		
		exit(1);
	}

	int fin, fout;

	// 打开一个已有文件
	fin = open(argv[1], O_RDONLY);

	if (fin < 0)
	{
		fprintf(stderr, "open error : %s\n", strerror(errno));
		
		exit(1);
	}
	else
	{
		printf("open file : %d\n", fin);			
	}

	// 调用 lseek 系统调用，求出文件的长度
	printf("source file length : %ld\n", lseek(fin, 0L, SEEK_END));

	// 打开一个新文件，如果没有则创建，文件的权限为 0777
	fout = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0777);

	if (fout < 0)
	{
		fprintf(stderr, "open error : %s\n", strerror(errno));

		exit(1);
	}
	else
	{
		printf("open file : %d\n", fout);
	}

	// 将源文件的内容复制到新文件
	copy(fin, fout);

	// 新文件的长度
	printf("destination file length : %ld\n", lseek(fout, 0L, SEEK_END));

	close(fin);
	
	close(fout);

	return 0;
}
























