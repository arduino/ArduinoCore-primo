#ifndef _NRF_DELAY_H
#define _NRF_DELAY_H


/*lint --e{438, 522} "Variable not used" "Function lacks side-effects" */
#if defined ( __CC_ARM   )
static __asm void __INLINE nrf_delay_us(uint32_t volatile number_of_us)
{
loop
        SUBS    R0, R0, #1
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        BNE    loop
        BX     LR
}
#elif defined ( __ICCARM__ )
static void __inline nrf_delay_us(uint32_t volatile number_of_us)
{
__asm (
"loop:\n\t"
       " SUBS R0, R0, #1\n\t"
       " NOP\n\t"
       " NOP\n\t"
       " NOP\n\t"
       " NOP\n\t"
       " NOP\n\t"
       " NOP\n\t"
       " NOP\n\t"
       " NOP\n\t"
       " NOP\n\t"
       " NOP\n\t"
       " NOP\n\t"
       " NOP\n\t"
       " BNE loop\n\t");
}
#elif defined   (  __GNUC__  )
__inline static void nrf_delay_us(uint32_t volatile number_of_us)
{
    do 
    {
    __asm volatile (
        "NOP\n\t"
        "NOP\n\t"
        "NOP\n\t"
        "NOP\n\t"
        "NOP\n\t"
        "NOP\n\t"
        "NOP\n\t"
        "NOP\n\t"
        "NOP\n\t"
        "NOP\n\t"
        "NOP\n\t"
        "NOP\n\t"
        "NOP\n\t"
        "NOP\n\t"
    );
    } while (--number_of_us);
}
#endif

void nrf_delay_ms(uint32_t volatile number_of_ms);

#endif
