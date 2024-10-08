#include "kernel/types.h"
#include "user/user.h"

#define BUF_SIZE 512
#define ARG_SIZE 32

int main(int argc, char *argv[]) {
    char block[BUF_SIZE];
    char buf[BUF_SIZE];
    char *p = buf;
    char *lineSplit[ARG_SIZE];
    int i, j = 0, m = 0;
    
    // 将命令行参数添加到 lineSplit 中
    for (i = 1; i < argc; i++) {
        lineSplit[j++] = argv[i];
    }
    
    int n;
    while ((n = read(0, block, sizeof(block))) > 0) {
        for (int l = 0; l < n; l++) {
            if (block[l] == '\n') {
                buf[m] = 0;  // 结束当前行
                m = 0;
                lineSplit[j++] = p;  // 把分割好的字符串放入 lineSplit
                p = buf;
                lineSplit[j] = 0;  // exec 需要以 null 结束的数组
                j = argc - 1;  // 重置 j 为初始命令参数数量
                
                if (fork() == 0) {
                    // 子进程执行命令
                    if (exec(argv[1], lineSplit) < 0) {
                        fprintf(2, "exec %s failed\n", argv[1]);
                        exit(1);
                    }
                }
                wait(0);  // 等待子进程结束
            } else if (block[l] == ' ') {
                buf[m++] = 0;  // 用空字符分割参数
                lineSplit[j++] = p;  // 保存参数
                p = &buf[m];  // 指向下一个参数开始的位置
            } else {
                buf[m++] = block[l];  // 将输入字符保存到 buf 中
            }
        }
    }

    exit(0);
}
