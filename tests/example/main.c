#include <xtf/test.h>

void test_main(void)
{
    for ( ; ; )
        asm volatile ("rep; nop");
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
