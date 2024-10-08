#include <kernel/types.h>
#include <user/user.h>

int main() {
    int p1[2], p2[2];
    char buffer[] = {'X'};
    long length = sizeof(buffer);

    if (pipe(p1) < 0 || pipe(p2) < 0) {
        printf("pipe creation error\n");
        exit(1);
    }

    // pipe1(p1)：父进程写入，子进程读取
    // pipe2(p2)；子进程写入，父进程读取
    if (fork() == 0) {
        // 子进程：从 p1 读取，向 p2 写入
        close(p1[1]); // 关闭写端
        close(p2[0]); // 关闭读端
        
        if (read(p1[0], buffer, length) != length) {
            printf("child: read error from p1\n");
            exit(1);
        }
        printf("%d: received ping\n", getpid());
        
        if (write(p2[1], buffer, length) != length) {
            printf("child: write error to p2\n");
            exit(1);
        }

        close(p1[0]); // 关闭读端
        close(p2[1]); // 关闭写端
        exit(0);
    } else {
        // 父进程：向 p1 写入，从 p2 读取
        close(p1[0]); // 关闭读端
        close(p2[1]); // 关闭写端
        
        if (write(p1[1], buffer, length) != length) {
            printf("parent: write error to p1\n");
            exit(1);
        }
        
        if (read(p2[0], buffer, length) != length) {
            printf("parent: read error from p2\n");
            exit(1);
        }
        printf("%d: received pong\n", getpid());

        close(p1[1]); // 关闭写端
        close(p2[0]); // 关闭读端
        wait(0); // 等待子进程结束
    }

    exit(0);
}
