#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <common.h>

#ifndef PGSIZE
#define PGSIZE 4096 //1=1 bit, 4096=4096 bit=4 kb
#endif

#define PG_ALIGN __attribute((aligned(PGSIZE)))

void* new_page(size_t);
int mm_brk(uintptr_t brk);


#endif
