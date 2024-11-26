struct vm_area {
    char* addr;                 // 起始地址
    uint64 length;              // 长度
    char prot;                  // 权限
    char flags;                 // 标志位
    struct file* file;         // 对应文件
};
