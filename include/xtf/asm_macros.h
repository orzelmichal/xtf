/**
 * @file include/xtf/asm_macros.h
 *
 * Macros for use in assembly files.
 */
#ifndef XTF_ASM_MACROS_H
#define XTF_ASM_MACROS_H

/**
 * Declare a global symbol.
 * @param name Symbol name.
 */
#define GLOBAL(name)                            \
    .globl name;                                \
name:

/**
 * Set the size of a named symbol.
 * @param name Symbol name.
 */
#define SIZE(name)                              \
    .size name, . - name;

/**
 * Create an ELF note entry.
 *
 * 'desc' may be an arbitrary asm construct.
 */
#define ELFNOTE(name, type, desc)                   \
    .pushsection .note.name                       ; \
    .align 4                                      ; \
    .long 2f - 1f         /* namesz */            ; \
    .long 4f - 3f         /* descsz */            ; \
    .long type            /* type   */            ; \
1:.asciz #name            /* name   */            ; \
2:.align 4                                        ; \
3:desc                    /* desc   */            ; \
4:.align 4                                        ; \
    .popsection

#endif /* XTF_ASM_MACROS_H */

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
