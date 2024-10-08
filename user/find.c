#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

/*
    将路径格式化为文件名
*/
char* fmt_name(char *path){
    char *p;
    
    // Find first character after last slash.
    for(p = path + strlen(path); p >= path && *p != '/'; p--);
    return p + 1;
}

/*
    在某路径中查找某文件
*/
void find(char *path, char *findName){
    int fd;
    struct stat st;
    
    // 打开目录
    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    
    // 获取文件状态
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    
    char buf[512], *p;
    struct dirent de;
    
    switch (st.type) {
        case T_FILE:
            // 如果是文件，则检查文件名是否匹配
            if (strcmp(fmt_name(path), findName) == 0) {
                printf("%s\n", path);
            }
            break;
            
        case T_DIR:
            // 处理目录
            if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
                printf("find: path too long\n");
                break;
            }
            
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';
            
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                    continue;
                    
                // 构建新的路径
                memmove(p, de.name, strlen(de.name));
                p[strlen(de.name)] = 0;
                
                // 递归查找
                find(buf, findName);
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[]){
    if (argc < 3) {
        printf("Usage: find <path> <fileName>\n");
        exit(0);
    }
    find(argv[1], argv[2]);
    exit(0);
}
