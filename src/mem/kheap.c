#include <kheap.h>
#include <console.h>

kheap_item_t *kheap_begin, *kheap_end;
size_t kheap_memory_used, kheap_allocs_num;

void kheap_init(void){
    kheap_begin = (kheap_item_t*)KHEAP_START_VADDR;
    kheap_end = NULL;

    kheap_allocs_num = 0;
    kheap_memory_used = 0;
}

size_t kheap_increase(size_t size){
    size_t pages = (size / PAGE_SIZE) + (size % PAGE_SIZE != 0);
    if(kheap_end == NULL){
        kheap_end = kheap_begin;
    }

    for(size_t i = 0; i < pages; i++){

        vmm_alloc_page(kheap_end);
        memset((void*)kheap_end, 0, PAGE_SIZE);
        kheap_end = (kheap_item_t*)((uintptr_t)kheap_end + PAGE_SIZE);
    }
    return pages*PAGE_SIZE;
}

void kheap_merge_free_adjacent(void){
    for(kheap_item_t* item = (kheap_item_t*)kheap_begin; item != NULL; item = item->next){
        while(!item->used && item->next != NULL && !item->next->used){
            item->size += sizeof(kheap_item_t) + item->next->size;
            item->next = item->next->next;
        }
    }
}

void kfree(void* address){
    if(address == NULL){
        return;
    }
    for(kheap_item_t* item = kheap_begin; item != NULL; item = item->next){
        if(item == (kheap_item_t*)address - 1){
            item->used = 0;
            kheap_memory_used -= item->size;
            kheap_allocs_num--;
            kheap_merge_free_adjacent();
            break;
        }
    }
}

void* kmalloc(size_t size){
    if(size == 0){
        return NULL;
    }
    kheap_item_t *new_item = NULL, *tmp_item;
    size_t total_size = size + sizeof(kheap_item_t);
    kheap_item_t* last_item = kheap_begin;
    if(kheap_end != NULL){
        for(new_item = kheap_begin; new_item != NULL; new_item = new_item->next){
            if(new_item->next == NULL){
                last_item = new_item;
            }

            if(!new_item->used && (total_size <= new_item->size)){
                break;
            }
        }
    }
    
    if(new_item != NULL){
        if(new_item->size - total_size > 0){
            tmp_item = (kheap_item_t*)((uintptr_t)new_item + total_size);
            tmp_item->size = new_item->size - total_size;
            tmp_item->used = 0;
            tmp_item->next = new_item->next;
            new_item->next = tmp_item;
        }
    } else {
        new_item = (kheap_end == NULL) ? kheap_begin : kheap_end;
        size_t bytes_added = kheap_increase(total_size);

        if(bytes_added - total_size > sizeof(kheap_item_t)){
            tmp_item = (kheap_item_t*)((uintptr_t)new_item + total_size);
            tmp_item->size = bytes_added - total_size - sizeof(kheap_item_t);
            tmp_item->used = 0;
            tmp_item->next = NULL;
            new_item->next = tmp_item;
        } else {
            new_item->next = NULL;
        }
        if(new_item != kheap_begin){
            last_item->next = new_item;
        }
    }
    new_item->size = size;
    new_item->used = 1;
    
    kheap_allocs_num++;
    kheap_memory_used += total_size;

    return (void*)(new_item + 1);
}


void* krealloc(void *p, size_t newsz){
    void *p2 = kmalloc(newsz);
    memcpy(p2, p, *((uint8_t*)p - 1 - 4));
    kfree(p);
    return p2;
}


void kheap_test(void){
    uint32_t sz = 4096 - sizeof(kheap_item_t);
    uint8_t* mas = (uint8_t*)kmalloc(sz);
    memset(mas, 5, sz);

    int cnt = 12;
    int* arr = (int*)kmalloc(cnt * sizeof(int));

    int i;
    for(i = 0; i < cnt; i++){
        arr[i] = i * 2;
    }

    int* arr2 = (int*)kmalloc(24 * sizeof(int));
    for(i = 0; i < 24; i++){
        arr2[i] = i * 3;
    }
    kfree(arr2);

    int* arr3 = (int*)kmalloc(8200 * sizeof(int));
    for(i = 0; i < 8200; i++){
        arr3[i] = i * 2;
    }
    kfree(arr3);
}
