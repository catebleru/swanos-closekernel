#include <vmm.h>
#include <pmm.h>
#include <console.h>
#include <string.h>

page_directory *kernel_page_dir; // Pointer (physical) to kernel page dircetory structure

bool vmm_alloc_page(void *vaddr) {
    void *paddr = pmm_alloc_block();
    if (!paddr) {
        return false;
    }
    vmm_map_page(paddr, vaddr);
    return true;
}

bool vmm_alloc_page_with_userbit(void *vaddr) {
    void *paddr = pmm_alloc_block();
    if (!paddr) {
        return false;
    }
    vmm_map_page(paddr, vaddr);
    page_table_entry *pte = (void*)GET_PTE((uintptr_t)vaddr);
    page_table_entry_add_attrib(pte, I86_PTE_USER);
    return true;
}

void vmm_free_page(void *vaddr){
    page_table_entry *pte = (void*)GET_PTE((uintptr_t)vaddr);
    if(!page_table_entry_is_present(*pte)){
        return;
    }
    void *block = page_table_entry_frame(*pte);
    if(block){
        pmm_free_block(block);
    }
    page_table_entry_del_attrib(pte, I86_PTE_PRESENT);
}

void vmm_create_kernel_page_dir(){
    kernel_page_dir = (page_directory*)pmm_alloc_block();
    if(!kernel_page_dir){
        return;
    }

    page_directory *pd = kernel_page_dir;
    memset(pd, 0, sizeof(page_directory));

    for(size_t i = 0; i < PAGE_ENTRIES; i++){
        page_dir_entry *pde = (page_dir_entry*) &pd->entries[i];
        page_dir_entry_add_attrib(pde, I86_PTE_WRITABLE);
        page_dir_entry_del_attrib(pde, I86_PTE_PRESENT);

        if(i == PAGE_ENTRIES - 1){
            page_dir_entry_add_attrib(pde, I86_PTE_PRESENT);
            page_dir_entry_set_frame(pde, kernel_page_dir);
        }
    }
}

void vmm_map_page(void *paddr, void *vaddr){
    page_dir_entry *pde = (void*)GET_PDE((uintptr_t)vaddr);
    if(!page_dir_entry_is_present(*pde)){ 
        void *pt_p = pmm_alloc_block();
        if(!pt_p){
            return;
        }

        page_table *pt_v = (page_table*) vmm_temp_map_page(pt_p);
        memset(pt_v, 0, sizeof(page_table));
        page_dir_entry_add_attrib(pde, I86_PDE_PRESENT);
        page_dir_entry_add_attrib(pde, I86_PDE_WRITABLE);
        page_dir_entry_set_frame(pde, pt_p);
    }

    page_table_entry *pte = (void*)GET_PTE((uintptr_t)vaddr);
    page_table_entry_set_frame(pte, paddr);
    page_table_entry_add_attrib(pte, I86_PTE_PRESENT);
    page_table_entry_add_attrib(pte, I86_PTE_WRITABLE);
    flush_tlb_entry(vaddr);
}

void *vmm_temp_map_page(void *paddr){
    page_table_entry *pte = (void*)GET_PTE((uintptr_t)TEMP_PAGE_ADDR);
    page_table_entry_set_frame(pte, (void*)PAGE_ALIGN_DOWN((uintptr_t)paddr));
    page_table_entry_add_attrib(pte, I86_PTE_PRESENT);
    page_table_entry_add_attrib(pte, I86_PTE_WRITABLE);
    asm volatile("invlpg %0" :: "m" (*(uint32_t *) TEMP_PAGE_ADDR) : "memory" );

    return (void*)TEMP_PAGE_ADDR;
}

void vmm_switch_page_directory(page_directory *page_dir_phys_addr){
    asm volatile("mov %0, %%cr3" :: "r"((uint32_t) page_dir_phys_addr));
}

void vmm_init(){
    vmm_create_kernel_page_dir();

    page_table *table1 = (page_table*) pmm_alloc_block();
    page_table *table2 = (page_table*) pmm_alloc_block();

    memset((void*) table1, 0, sizeof(page_table));
    memset((void*) table2, 0, sizeof(page_table));

    uint8_t *frame, *virt;
    for(frame = (uint8_t*)0x0, virt = (uint8_t*)0xC0000000;
         frame < (uint8_t*)0x100000;
         frame += PAGE_SIZE, virt += PAGE_SIZE){
        page_table_entry page = 0;
        page_table_entry_add_attrib(&page, I86_PTE_PRESENT);
        page_table_entry_set_frame(&page, frame);
        table1->entries[PAGE_TABLE_INDEX((uintptr_t)virt)] = page;
    }

    for(frame = (uint8_t*)KERNEL_START_PADDR, virt = (uint8_t*)KERNEL_START_VADDR;
         frame < (uint8_t*)KERNEL_PHYS_MAP_END;
         frame += PAGE_SIZE, virt += PAGE_SIZE){
        page_table_entry page = 0;
        page_table_entry_add_attrib(&page, I86_PTE_PRESENT);
        page_table_entry_set_frame(&page, frame);
        table2->entries[PAGE_TABLE_INDEX((uintptr_t)virt)] = page;
    }

    page_dir_entry *pde1 = &kernel_page_dir->entries[PAGE_DIRECTORY_INDEX(0x00000000)];
    page_dir_entry_add_attrib(pde1, I86_PDE_PRESENT);
    page_dir_entry_add_attrib(pde1, I86_PDE_WRITABLE);
    page_dir_entry_set_frame(pde1, table1);

    page_dir_entry *pde2 = &kernel_page_dir->entries[PAGE_DIRECTORY_INDEX(0xC0100000)];
    page_dir_entry_add_attrib(pde2, I86_PDE_PRESENT);
    page_dir_entry_add_attrib(pde2, I86_PDE_WRITABLE);
    page_dir_entry_set_frame(pde2, table2);

    update_phys_memory_bitmap_addr((void *)KERNEL_END_VADDR);

    enable_paging(kernel_page_dir);
}

void vmm_test(){

    void *padr1 = (void *)0xC0500000;
    void *vadr1 = vmm_temp_map_page(padr1);
    *(uint8_t*) vadr1 = 77;

    int eip;
    asm volatile("1: lea 1b, %0;": "=a"(eip));
}

void page_table_entry_add_attrib(page_table_entry *entry, uint32_t attrib){
    *entry |= attrib;
}

void page_table_entry_del_attrib(page_table_entry *entry, uint32_t attrib){
    *entry &= ~attrib;
}

void page_table_entry_set_frame(page_table_entry *entry, void *addr){
    *entry = (*entry & ~I86_PTE_FRAME) | (uintptr_t)addr;
}

bool page_table_entry_is_present(page_table_entry entry){
    return entry & I86_PTE_PRESENT;
}

bool page_table_entry_is_writable(page_table_entry entry){
    return entry & I86_PTE_WRITABLE;
}

void *page_table_entry_frame(page_table_entry entry) {
    return (void *)(entry & I86_PTE_FRAME);
}

void page_dir_entry_add_attrib(page_dir_entry *entry, uint32_t attrib) {
    *entry |= attrib;
}

void page_dir_entry_del_attrib(page_dir_entry *entry, uint32_t attrib) {
    *entry &= ~attrib; //old: was without ~ !!
}

void page_dir_entry_set_frame(page_dir_entry *entry, void *addr) {
    *entry = (*entry & ~I86_PDE_FRAME) | (uintptr_t)addr;
}

bool page_dir_entry_is_present(page_dir_entry entry) {
    return entry & I86_PDE_PRESENT;
}

bool page_dir_entry_is_user(page_dir_entry entry) {
    return entry & I86_PDE_USER;
}

bool page_dir_entry_is_4mb(page_dir_entry entry) {
    return entry & I86_PDE_4MB;
}

bool page_dir_entry_is_writable(page_dir_entry entry) {
    return entry & I86_PDE_WRITABLE;
}

void *page_dir_entry_frame(page_dir_entry entry) {
    return (void *)(entry & I86_PDE_FRAME);
}

void flush_tlb_entry(void *addr) {
    asm volatile("invlpg (%0)" : : "b"(addr) : "memory");
}
