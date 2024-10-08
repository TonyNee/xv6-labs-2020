#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(2, "Usage: sleep <time>\n");
        exit(1);
    }

    // 将输入字符串转换为整数
    int sleepNum = atoi(argv[1]);

    // 检查转换后的值是否合法
    if (sleepNum <= 0) {
        fprintf(2, "Error: argument must be a positive integer\n");
        exit(1);
    }

    // 打印提示信息，模拟“无事发生”
    printf("(nothing happens for a little while)\n");

    // 休眠指定的秒数
    sleep(sleepNum);

    // 正常退出
    exit(0);
}
