#include "kernel/types.h"
#include "user/user.h"

/*
 * Chapter 01: 使用 Fork 和 Pipes 实现埃拉托斯特尼筛法
 * 这个程序通过多个进程和管道实现了埃拉托斯特尼筛法，用于找出素数。
 */

void sieve(int *input, int count) {
    if(count == 0){
        return;
    }
    if(count == 1){
        printf("prime %d\n", input[0]);
        return;
    }

    int p[2];
    if(pipe(p) < 0){
        fprintf(2, "sieve: 创建管道失败\n");
        exit(1);
    }

    int prime = input[0];
    printf("prime %d\n", prime);

    if(fork() == 0){
        // 子进程：筛选出不是 prime 倍数的数，并写入管道
        close(p[0]); // 关闭未使用的读端

        for(int i = 1; i < count; i++){
            if(input[i] % prime != 0){
                if(write(p[1], &(input[i]), sizeof(int)) != sizeof(int)){
                    fprintf(2, "sieve: 向管道写入数据失败\n");
                    exit(1);
                }
            }
        }
        close(p[1]); // 写入完成后关闭写端
        exit(0);
    }

    // 父进程：读取子进程筛选后的数据
    close(p[1]); // 关闭未使用的写端

    int filtered[count -1]; // 存储筛选后的数，最大可能为 count-1
    int num_filtered = 0;
    int temp;

    while(read(p[0], &temp, sizeof(int)) == sizeof(int)){
        filtered[num_filtered++] = temp;
    }
    close(p[0]); // 读取完成后关闭读端

    wait(0); // 等待子进程结束

    // 递归调用筛选函数
    sieve(filtered, num_filtered);
}

int main(int argc, char *argv[]) {
    int input[34];
    for(int i = 0; i < 34; i++){
        input[i] = i + 2; // 初始化数值，从2到35
    }

    sieve(input, 34);

    exit(0);
}
