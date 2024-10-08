// mycopy.c: 将控制台输入内容输出到控制台

#include "kernel/types.h"
#include "user/user.h"

int main() {

	char buf[64];
	
	while(1) {

    	int n = read(0, buf, sizeof(buf));	//从console读取输入，通过system call的read函数实现
		//无输入结束程序
		if(n <= 0) {
			break;
		}
    	write(1, buf, n);	//将console输入输出到控制台，通过system call的write函数实现

  	}

	exit(0);
}

