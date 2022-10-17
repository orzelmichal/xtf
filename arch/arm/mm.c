/**
 * @file arch/arm/mm.c
 *
 * Memory management on arm.
 */

#include <xtf/lib.h>
#include <arch/mm.h>
#include <arch/barrier.h>

paddr_t phys_offset;

#ifdef CONFIG_MMU
/*
 * Static boot page tables used before BSS is zeroed.
 * Make boot page tables part of the loaded image by putting them inside
 * ".data.page_aligned" so that they are zeroed when loading image into memory.
 */
lpae_t __page_aligned_data l1_bpgtable[TABLE_ENTRIES];
lpae_t __page_aligned_data l2_bpgtable[TABLE_ENTRIES];
lpae_t __page_aligned_data l1_idmap[TABLE_ENTRIES];
lpae_t __page_aligned_data fix_pgtable[TABLE_ENTRIES];
#endif

void store_pgt_entry(lpae_t *addr, lpae_t val)
{
    *addr = val;
    dsb(ishst);
    isb();
}

/* Map a page in a fixmap entry */
void *set_fixmap(uint8_t slot, paddr_t pa, uint64_t flags)
{
#ifdef CONFIG_MMU
    unsigned int index;

    index = L3_TABLE_INDEX(FIXMAP_ADDR(slot));
    store_pgt_entry(&fix_pgtable[index], ((pa & ~(L3_TABLE_SIZE - 1)) | flags));

    return (void *)(FIXMAP_ADDR(slot) + (pa & PAGE_OFFSET));
#else
    return (void *)(vaddr_t)pa;
#endif
}

void setup_mm(paddr_t boot_phys_offset)
{
    phys_offset = boot_phys_offset;
}

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
