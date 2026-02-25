/* STARTUP */
#include "badhal.h"
extern unsigned int __estack;

extern unsigned int __rdata;
extern unsigned int __data;
extern unsigned int __edata;

extern unsigned int __bss;
extern unsigned int __ebss;

extern unsigned int __rramfunc;
extern unsigned int __ramfunc;
extern unsigned int __eramfunc;

extern unsigned int __rramfunc;
extern unsigned int __ramfunc;
extern unsigned int __eramfunc;

extern unsigned int __ram_ivt;
extern unsigned int __eram_ivt;

typedef void (*constructor_ptr)();

extern constructor_ptr __init_array[];
extern constructor_ptr __einit_array[];


extern int main();

void default_isr(){
    while(1);
}
WEAK_ISR(isr_hardfault);
WEAK_ISR(wwdg_isr);
WEAK_ISR(pvd_isr);
WEAK_ISR(tamp_stamp_isr);
WEAK_ISR(rtc_wkup_isr);
WEAK_ISR(flash_isr);
WEAK_ISR(rcc_isr);
WEAK_ISR(exti0_isr);
WEAK_ISR(exti1_isr);
WEAK_ISR(exti2_isr);
WEAK_ISR(exti3_isr);
WEAK_ISR(exti4_isr);
WEAK_ISR(dma1_stream0_isr);
WEAK_ISR(dma1_stream1_isr);
WEAK_ISR(dma1_stream2_isr);
WEAK_ISR(dma1_stream3_isr);
WEAK_ISR(dma1_stream4_isr);
WEAK_ISR(dma1_stream5_isr);
WEAK_ISR(dma1_stream6_isr);
WEAK_ISR(adc_isr);
WEAK_ISR(exti9_5_isr);
WEAK_ISR(tim1_brk_tim9_isr);
WEAK_ISR(tim1_up_tim10_isr);
WEAK_ISR(tim1_trg_com_tim11_isr);
WEAK_ISR(tim1_cc_isr);
WEAK_ISR(tim2_isr);
WEAK_ISR(tim3_isr);
WEAK_ISR(tim4_isr);
WEAK_ISR(i2c1_ev_isr);
WEAK_ISR(i2c1_er_isr);
WEAK_ISR(i2c2_ev_isr);
WEAK_ISR(i2c2_er_isr);
WEAK_ISR(spi1_isr);
WEAK_ISR(spi2_isr);
WEAK_ISR(usart1_isr);
WEAK_ISR(usart2_isr);
WEAK_ISR(exti15_10_isr);
WEAK_ISR(rtc_alarm_isr);
WEAK_ISR(otg_fs_wkup_isr);
WEAK_ISR(dma1_stream7_isr);
WEAK_ISR(sdio_isr);
WEAK_ISR(tim5_isr);
WEAK_ISR(spi3_isr);
WEAK_ISR(dma2_stream0_isr);
WEAK_ISR(dma2_stream1_isr);
WEAK_ISR(dma2_stream2_isr);
WEAK_ISR(dma2_stream3_isr);
WEAK_ISR(dma2_stream4_isr);
WEAK_ISR(otg_fs_isr);
WEAK_ISR(dma2_stream5_isr);
WEAK_ISR(dma2_stream6_isr);
WEAK_ISR(dma2_stream7_isr);
WEAK_ISR(usart6_isr);
WEAK_ISR(i2c3_ev_isr);
WEAK_ISR(i2c3_er_isr);
WEAK_ISR(fpu_isr);
WEAK_ISR(spi4_isr);
WEAK_ISR(spi5_isr);
WEAK_ISR(pendsv_isr);
WEAK_ISR(systick_isr);
WEAK_ISR(svc_isr);

static inline void data_init(){ 
    unsigned int *src = &__rdata;
    unsigned int *dest = &__data;
    while (dest<&__edata) {
        *dest++ = *src++;
    }
}

static inline void bss_init(){
    unsigned int *src = &__bss;
    while (src<&__ebss) {
        *src++ = 0; 
    }
}

static inline void ramfunc_init(){
    unsigned int *src = &__rramfunc;
    unsigned int *dest = &__ramfunc;
    while (dest<&__eramfunc) {
        *dest++ = *src++;
    }
}

static inline void constructors_init(){
    constructor_ptr* constructors = __init_array;
    while (constructors < __einit_array) {
        (*constructors)();  // call the constructor
        constructors++;
    }
}

static inline void copy_ivt_to_ram();


void __attribute__((noreturn)) isr_reset(){
    data_init();
    bss_init();
    ramfunc_init();
    copy_ivt_to_ram();
    constructors_init();
    main();
    while(1);
};
#define IVT_SIZE (102U)
typedef void (*isr_addr_t) (void);

const isr_addr_t ivt_table[IVT_SIZE] __attribute__((used,section(".ivt")))={ 
    (isr_addr_t)&__estack,
    isr_reset,
    0, //NMI
    isr_hardfault,
    isr_hardfault,
    isr_hardfault,
    isr_hardfault,
    0,
    0,
    0,
    0,
    svc_isr,
    isr_hardfault,
    0,
    pendsv_isr,
    systick_isr,
    wwdg_isr,
    pvd_isr,
    tamp_stamp_isr,
    rtc_wkup_isr,
    flash_isr,
    rcc_isr,
    exti0_isr,
    exti1_isr,
    exti2_isr,
    exti3_isr,
    exti4_isr,
    dma1_stream0_isr,
    dma1_stream1_isr,
    dma1_stream2_isr,
    dma1_stream3_isr,
    dma1_stream4_isr,
    dma1_stream5_isr,
    dma1_stream6_isr,
    adc_isr,
    0,
    0,
    0,
    0,
    exti9_5_isr,
    tim1_brk_tim9_isr,
    tim1_up_tim10_isr,
    tim1_trg_com_tim11_isr,
    tim1_cc_isr,
    tim2_isr,
    tim3_isr,
    tim4_isr,
    i2c1_ev_isr,
    i2c1_er_isr,
    i2c2_ev_isr,
    i2c2_er_isr,
    spi1_isr,
    spi2_isr,
    usart1_isr,
    usart2_isr,
    0,
    exti15_10_isr,
    rtc_alarm_isr,
    otg_fs_wkup_isr,
    0,
    0,
    0,
    0,
    dma1_stream7_isr,
    0,
    sdio_isr,
    tim5_isr,
    spi3_isr,
    0,
    0,
    0,
    0,
    dma2_stream0_isr,
    dma2_stream1_isr,
    dma2_stream2_isr,
    dma2_stream3_isr,
    dma2_stream4_isr,
    0,
    0,
    0,
    0,
    0,
    0,
    otg_fs_isr,
    dma2_stream5_isr,
    dma2_stream6_isr,
    dma2_stream7_isr,
    usart6_isr,
    i2c3_ev_isr,
    i2c3_er_isr,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    fpu_isr,
    0,
    0,
    spi4_isr,
    spi5_isr

};
static inline void copy_ivt_to_ram(){
    unsigned int *dest = &__ram_ivt;
    unsigned int *src = (unsigned int*)ivt_table; 
    while (dest<&__eram_ivt) {
        *dest++ = *src++; 
    }
}
