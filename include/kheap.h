#ifndef _KHEAP_H_
#define _KHEAP_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <memory.h>
#include <string.h>

#include <vmm.h>

typedef struct kheap_item_t {
    struct kheap_item_t* next;
    size_t size;
    uint8_t used; // 0 - free, 1 - not free
} __attribute__((packed)) kheap_item_t;

extern kheap_item_t *kheap_begin, *kheap_end;
extern uint8_t *kheap_limit; 
extern size_t kheap_memory_used, kheap_allocs_num;

void kheap_init(void);
size_t kheap_increase(size_t size);
void kheap_merge_free_adjacent(void);
void kfree(void* address);
void* kmalloc(size_t size);
void* krealloc(void *p, size_t newsz);
void kheap_test(void);

#endif
