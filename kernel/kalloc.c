// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

// 这个宏将物理地址转换成页引用计数器的物理页索引
#define PA2PID(pa) (((uint64)(pa) - KERNBASE) / PGSIZE)

// 页引用计数器
struct {
    struct spinlock lock;
    int rcs[PA2PID(PHYSTOP) + 1];
} pagerc;
// 递增引用计数器，返回递增后的值
int REF_I(uint64 pa) {
    acquire(&pagerc.lock);
    int r = ++pagerc.rcs[PA2PID(pa)];
    release(&pagerc.lock);
    return r;
}
// 递减引用计数器，返回递减后的值
int REF_D(uint64 pa) {
    acquire(&pagerc.lock);
    int r = --pagerc.rcs[PA2PID(pa)];
    release(&pagerc.lock);
    return r;
}


void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  initlock(&pagerc.lock, "pagerc");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE) {
      // 先把引用计数器设置成1，然后调用kfree减1，页面的引用计数就成了0
      pagerc.rcs[PA2PID(p)] = 1;
      kfree(p);
  }
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // 递减引用计数器
  int rc = REF_D((uint64)pa);

  // 如果已经没有其它引用了
  if (rc == 0) {
      // Fill with junk to catch dangling refs.
      memset(pa, 1, PGSIZE);

      r = (struct run *) pa;

      acquire(&kmem.lock);
      r->next = kmem.freelist;
      kmem.freelist = r;
      release(&kmem.lock);
  }
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
    struct run *r;

    acquire(&kmem.lock);
    r = kmem.freelist;
    if(r) {
        kmem.freelist = r->next;
        // 设置引用计数器
        pagerc.rcs[PA2PID(r)] = 1;
    }
    release(&kmem.lock);

    if(r) {
        memset((char*)r, 5, PGSIZE); // fill with junk
    }
    return (void*)r;
}

