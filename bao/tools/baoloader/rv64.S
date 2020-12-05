#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

.section .nloader, "ax"
.global _start
_start:

    /* boot protocol */
    /* We are expecting hartid in a0 */
    la a1, __config_start
    
    /* jump to bao */
    la t0, __bao_start
    jalr t0
    j  .

.section .bao, "a"
    .incbin STRINGIFY(IMAGE)

.section .config, "a"
    .incbin STRINGIFY(CONFIG)
 
