#pragma once
#ifndef BAD_HAL_H
#define BAD_HAL_H

#include <stdint.h>

//GLOBAL CONFIG
//Core
#define BAD_HAL_USE_SCB
#define BAD_HAL_USE_MPU
#define BAD_HAL_USE_FLASH
#define BAD_HAL_USE_RCC
#define BAD_HAL_USE_NVIC
#define BAD_HAL_USE_SYSTICK
//Peripherals
#define BAD_HAL_USE_USART
#define BAD_HAL_USE_GPIO
#define BAD_HAL_USE_SPI
#define BAD_HAL_USE_DMA
#define BAD_HAL_USE_EXTI
#define BAD_HAL_USE_SYSCFG
#define BAD_HAL_USE_BTIMER
#define BAD_HAL_USE_CRC
//common defines

#define __IO volatile

#define CLOCK_SPEED 100000000UL         //100MHZ
#define FALLBACK_CLOCK_SPEED 16000000UL //16MHZ
//hw interrupts (triggered by hardware and handled in drivers)
#define STRONG_ISR(x) void x(void)
#define WEAK_ISR(x) void x(void) __attribute__((weak, alias("default_isr")))
#define STRONG_USER_ISR(x,...) void x(__VA_ARGS__)
#define WEAK_USER_ISR(x,...) void x(__VA_ARGS__) __attribute__((weak, alias(#x"_default")))
#define DEFAULT_USER_ISR(x,...) void x##_default(__VA_ARGS__)
#define WEAK_PERIPH_USER_ISR(x,default_isr,...) void x(__VA_ARGS__) __attribute__((weak, alias(#default_isr"_default")))
#define ATTR_RAMFUNC __attribute__((section(".ramfunc")))
#define ALWAYS_INLINE static inline
#define UNUSED(x) (void)x

#define OPT_BARRIER asm volatile("": : :"memory")
#define DSB __asm volatile("dsb":::"memory")
#define DMB __asm volatile("dmb":::"memory")
#define ISB __asm volatile("isb":::"memory")
#define __ENABLE_INTERUPTS __asm volatile ("cpsie i":::"memory")
#define __DISABLE_INTERUPTS __asm volatile ("cpsid i":::"memory")

//Core


//SCB
#ifdef BAD_HAL_USE_SCB

typedef struct
{
  __IO  uint32_t CPUID;                  
  __IO  uint32_t ICSR;                   
  __IO  uint32_t VTOR;                   
  __IO  uint32_t AIRCR;                  
  __IO  uint32_t SCR;                    
  __IO  uint32_t CCR;                    
  __IO  uint8_t  SHP[12];               
  __IO  uint32_t SHCSR;                  
  __IO  uint32_t CFSR;                   
  __IO  uint32_t HFSR;                   
  __IO  uint32_t DFSR;                   
  __IO  uint32_t MMFAR;                  
  __IO  uint32_t BFAR;                   
  __IO  uint32_t AFSR;                   
  __IO  uint32_t PFR[2];                
  __IO  uint32_t DFR;                    
  __IO  uint32_t ADR;                    
  __IO  uint32_t MMFR[4];               
  __IO  uint32_t ISAR[5];               
        uint32_t RESERVED0[5];
  __IO  uint32_t CPACR;                  
} SCB_typedef_t;

typedef enum{
    SCB_PRIO_GROUP0 = 0,
    SCB_PRIO_GROUP1,
    SCB_PRIO_GROUP2,
    SCB_PRIO_GROUP3,
    SCB_PRIO_GROUP4
}SCB_prio_grouping_t;

typedef enum{
    SCB_MEMORY_MANAGEMENT_INTR = 0,
    SCB_BUS_FAULT_INTR=1,
    SCB_USAGE_FAULT_INTR=2,
    SCB_SVC_INTR = 7,
    SCB_DEBUG_MONITOR_INTR = 8,
    SCB_PENDSV_INTR = 10,
    SCB_SYSTICK_INTR = 11
}SCB_core_interrupt_t;

typedef enum {
    SCB_PRIO0 = 0,
    SCB_PRIO1,
    SCB_PRIO2,
    SCB_PRIO3,
    SCB_PRIO4,
    SCB_PRIO5,
    SCB_PRIO6,
    SCB_PRIO7,
    SCB_PRIO8,
    SCB_PRIO9,
    SCB_PRIO10,
    SCB_PRIO11,
    SCB_PRIO12,
    SCB_PRIO13,
    SCB_PRIO14,
    SCB_PRIO15

}SCB_interrupt_priority_t;

#define SCB ((SCB_typedef_t *) 0xE000ED00UL)


#define SCB_AIRCR_VECTKEY_SHIFT              16U                                            
#define SCB_AIRCR_VECTKEY_MASK               (0xFFFF << 16)            
#define SCB_ICSR_PENDSVSET                   (0x1 << 28 ) 
#define SCB_AIRCR_PRIGROUP_SHIFT              8                                            
#define SCB_AIRCR_PRIGROUP_MASK              (7 << SCB_AIRCR_PRIGROUP_SHIFT)                

ALWAYS_INLINE void SCB_trigger_pendsv(){
    SCB->ICSR = SCB_ICSR_PENDSVSET;
}

ALWAYS_INLINE void SCB_set_priority_grouping(SCB_prio_grouping_t prio){
    uint32_t reg_value  =  SCB->AIRCR;                                                
    reg_value &= ~(SCB_AIRCR_VECTKEY_MASK | SCB_AIRCR_PRIGROUP_MASK);  
    reg_value  =  (reg_value | (0x5FA << SCB_AIRCR_VECTKEY_SHIFT) | (prio << SCB_AIRCR_PRIGROUP_SHIFT));              
    SCB->AIRCR =  reg_value;
    DSB;
    OPT_BARRIER;
}

ALWAYS_INLINE void SCB_set_core_interrupt_priority(SCB_core_interrupt_t intr,SCB_interrupt_priority_t prio){
    SCB->SHP[intr] = prio << 4;
}
#endif // BAD_HAL_USE_SCB

//MPU
#ifdef BAD_HAL_USE_MPU

typedef struct {

    __IO uint32_t TYPE;                   
    __IO uint32_t CTRL;                   
    __IO uint32_t RNR;                    
    __IO uint32_t RBAR;                   
    __IO uint32_t RASR;                   
    __IO uint32_t RBAR_A1;                
    __IO uint32_t RASR_A1;                
    __IO uint32_t RBAR_A2;                
    __IO uint32_t RASR_A2;                
    __IO uint32_t RBAR_A3;                
    __IO uint32_t RASR_A3;                
} MPU_typedef_t;

typedef enum{
    MPU_TEXSCB_STRONGLY_ORDERED = 0,
    MPU_TEXSCB_SHARED_DEVICE = 0x10000,
    MPU_TEXSCB_NORMAL_NO_ALLOCATE_WRT = 0x20000,
    MPU_TEXSCB_NORMAL_NO_ALLOCATE_WRT_SHAREABLE = 0x60000,
    MPU_TEXSCB_NORMAL_NO_ALLOCATE_WRB = 0x30000,
    MPU_TEXSCB_NORMAL_NO_ALLOCATE_WRB_SHAREABLE = 0x70000,
    MPU_TEXSCB_NORMAL_NON_CACHEABLE = 0x80000,
    MPU_TEXSCB_NORMAL_NON_CACHEABLE_SHAREABLE = 0xC0000,
    MPU_TEXSCB_NORMAL_RW_ALLOCATE = 0xB0000,
    MPU_TEXSCB_NORMAL_RW_ALLOCATE_SHAREABLE = 0xF0000,
    MPU_TEXSCB_NON_SHAREABLE_DEVICE = 0x100000
}mpu_texscb_features_t;

typedef enum{
    MPU_AP_NO_ACCESS = 0,
    MPU_AP_PRIV_RW_UNPRIV_FAULT = 0x1000000,
    MPU_AP_PRIV_RW_UNPRIV_RO = 0x2000000,
    MPU_AP_FULL_ACCESS = 0x3000000,
    MPU_AP_PRIV_RO_UNPRIV_FAULT = 0x5000000,
    MPU_AP_PRIV_RO_UNPRIV_RO = 0x6000000
}mpu_permissions_t;

#define NEXT_POW2(x) ( \
    (x) <= 32 ? 32 :   \
    (x) <= 64 ? 64 :   \
    (x) <=128 ? 128:   \
    (x) <=256 ? 256:   \
    (x) <=512 ? 512:   \
    (x) <=1024 ? 1024: \
    (x) <=2048 ? 2048: \
    (x) <=4096 ? 4096: \
    (x) <=8192 ? 8912: \
    16384\
)

#define PREV_POW2(x)( \
    ((x) >= 32 && (x) < 64)  ? 32 :     \
    ((x) >= 64 && (x) < 128) ? 64 :     \
    ((x) >= 128 && (x) < 256) ? 128 :   \
    ((x) >= 256 && (x) < 512) ? 256 :   \
    ((x) >= 512 && (x) < 1024) ? 512 :  \
    ((x) >= 1024 && (x) < 2048) ? 1024 :\
    2048\
)

#define FIND_SIZE(x)(\
    (x) == 32 ? (4<<1):   \
    (x) == 64 ? (5<<1):   \
    (x) == 128 ? (6<<1):  \
    (x) == 256 ? (7<<1):  \
    (x) == 512 ? (8<<1):  \
    (x) == 1024 ? (9<<1): \
    (x) == 2048 ? (10<<1):\
    (x) == 4096 ? (11<<1):\
    (x) == 8192 ? (12<<1):\
    16384\
)
#define MPU_RASR_XN (0x10000000)

#define MPU_CTRL_ENABLE (0x1)
#define MPU_RASR_ENABLE (0x1)
#define MPU_CTRL_DEFAULT_MAP (0x4)
#define MPU_BASE (0xE000ED90UL)

#define MPU ((MPU_typedef_t *)MPU_BASE)
ALWAYS_INLINE void mpu_enable_with_default_map(){
    MPU->CTRL = MPU_CTRL_ENABLE | MPU_CTRL_DEFAULT_MAP;
}

#endif // BAD_HAL_USE_MPU

//Flash
#ifdef BAD_HAL_USE_FLASH

#ifdef BAD_FLASH_IMPLEMENTATION

typedef struct flash_regs_t{
    __IO uint32_t ACR;
    __IO uint32_t KEYR;
    __IO uint32_t OPTKEYR;
    __IO uint32_t SR;
    __IO uint32_t CR;
    __IO uint32_t OPTCR;
}FLASH_regs_typedef_t;

typedef enum{
    FLASH_LATENCY_0ws   = 0x0,
    FLASH_LATENCY_1ws   = 0x1,
    FLASH_LATENCY_2ws   = 0x2,
    FLASH_LATENCY_3ws   = 0x3,
    FLASH_LATENCY_4ws   = 0x4,
    FLASH_LATENCY_5ws   = 0x5,
    FLASH_LATENCY_6ws   = 0x6,
    FLASH_LATENCY_7ws   = 0x7,
    FLASH_LATENCY_8ws   = 0x8,
    FLASH_LATENCY_9ws   = 0x9,
    FLASH_LATENCY_10ws  = 0xA,
    FLASH_LATENCY_11ws  = 0xB,
    FLASH_LATENCY_12ws  = 0xC,
    FLASH_LATENCY_13ws  = 0xD,
    FLASH_LATENCY_14ws  = 0xE,
    FLASH_LATENCY_15ws  = 0xF
}FLASH_latency_t;

typedef enum{
    FLASH_ICACHE_DISABLE    = 0x0, 
    FLASH_ICACHE_ENABLE     = 0x200
}FLASH_icache_state_t;

typedef enum{
    FLASH_DCACHE_DISABLE    = 0x0, 
    FLASH_DCACHE_ENABLE     = 0x400
}FLASH_dcache_state_t;

typedef enum{
    FLASH_PREFETCH_DISABLE    = 0x0, 
    FLASH_PREFETCH_ENABLE     = 0x100
}FLASH_prefetch_state_t;

#define FLASH_REG_BASE  (0x40023c00UL)
#define FLASH_REGS ((__IO FLASH_regs_typedef_t *)FLASH_REG_BASE)

extern inline void flash_acceleration_setup(FLASH_latency_t latency ,FLASH_dcache_state_t dcache, FLASH_icache_state_t icache){
    FLASH_REGS->ACR = latency | dcache | icache;
}
#endif

#endif // BAD_HAL_USE_FLASH

//RCC
#ifdef BAD_HAL_USE_RCC

extern void rcc_fallback_to_hsi();

#ifdef BAD_RCC_IMPLEMENTATION

typedef struct RCC_regs_t{
    __IO uint32_t CR;
    __IO uint32_t PLLCFGR;
    __IO uint32_t CFGR;
    __IO uint32_t CIR;
    __IO uint32_t AHB1RSTR;
    __IO uint32_t AHB2RSTR;
    uint32_t RESERVED0;
    uint32_t RESERVED1;
    __IO uint32_t APB1RSTR;
    __IO uint32_t APB2RSTR;
    uint32_t RESERVED3;
    uint32_t RESERVED4;
    __IO uint32_t AHB1ENR;
    __IO uint32_t AHB2ENR;
    uint32_t RESERVED5;
    uint32_t RESERVED6;
    __IO uint32_t APB1ENR;
    __IO uint32_t APB2ENR;
    uint32_t RESERVED7;
    uint32_t RESERVED8;
    __IO uint32_t AHB1LPENR;
    __IO uint32_t AHB2LPENR;
    uint32_t RESERVED9;
    uint32_t RESERVED10;
    __IO uint32_t APB1LPENR;
    __IO uint32_t APB2LPENR;
    uint32_t RESERVED11;
    uint32_t RESERVED12;
    __IO uint32_t BDCR;
    __IO uint32_t CSR;
    uint32_t RESERVED13;
    uint32_t RESERVED14;
    __IO uint32_t SSCGR;
    __IO uint32_t PLLI2SCFGR;
}RCC_typedef_t;

#define HSEON_MASK  (0x10000U)
#define HSERDY_MASK (0x20000U)
#define HSION_MASK  (0x1U)
#define HSIRDY_MASK (0x2U)
#define PLLON_MASK  (0x1U << 24)
#define PLLRDY_MASK (0x1U << 25)

typedef enum {
    PLL_SOURCE_HSI = 0x0,
    PLL_SOURCE_HSE = 0x400000
} PLL_source_t;

#define PLLSRC_MASK (0x1U << 22)
#define PLLN_SET(val)((val & 0x1FF) << 6)
#define PLLM_SET(val)((val & 0x3F))

typedef enum{
    PLLP2 = 0x0,
    PLLP4 = 0x10000,
    PLLP6 = 0x20000,
    PLLP8 = 0x40000
} PLLP_states_t;


#define PLLQ_SET(val)((val & 0xFU) << 24)


#define PPRE1_SET(val)((val & 0x7)<<10)
#define PPRE2_SET(val)((val & 0x7)<<13)
#define PPRE1_MASK (0x7<<10)
#define PPRE2_MASK (0x7<<13)
typedef enum{
    PPRE_DIV_1 = 0x0, //actually it is not DIVided if top(2) bit is not set
    PPRE_DIV_2 = 0b100,
    PPRE_DIV_4 = 0b101,
    PPRE_DIV_8 = 0b110,
    PPRE_DIV_16 = 0b111
} PPRE_state_t;

#define HPRE_SET(val)((val & 0xFU)<<4)
#define HPRE_MASK (0xF<<4)
typedef enum{
    HPRE_DIV_1 = 0x0, //actually it is not DIVided if top(15) bit is not set
    HPRE_DIV_2 = 0b1000,
    HPRE_DIV_4 = 0b1001,
    HPRE_DIV_8 = 0b1010,
    HPRE_DIV_16 = 0b1011,
    HPRE_DIV_64 = 0b1100,
    HPRE_DIV_128 = 0b1101,
    HPRE_DIV_256 = 0b1110,
    HPRE_DIV_512 = 0b1111
} HPRE_state_t;

typedef enum{
    SW_HSI = 0x0,
    SW_HSE = 0x1,
    SW_PLL = 0x2,
    SW_NOT_ALLOWED = 0x3
}SW_state_t;

typedef enum{
    RCC_AHB1_GPIOA = 0x1,
    RCC_AHB1_GPIOB = 0x2,
    RCC_AHB1_GPIOC = 0x4,
    RCC_AHB1_GPIOD = 0x8,
    RCC_AHB1_CRCEN = 0x1000,
    RCC_AHB1_DMA1  = 0x200000,
    RCC_AHB1_DMA2  = 0x400000  
}RCC_AHB1_peripherals_t;

typedef enum{
    RCC_AHB2_USB_OTG_FS = 0x80
}RCC_AHB2_peripherals_t;

typedef enum{
    RCC_APB1_TIM2   = 0x1,
    RCC_APB1_TIM3   = 0x2,
    RCC_APB1_TIM4   = 0x4,
    RCC_APB1_TIM5   = 0x8,
    RCC_APB1_WWDG   = 0x800,
    RCC_APB1_SPI2   = 0x4000,
    RCC_APB1_SPI3   = 0x8000,
    RCC_APB1_USART2 = 0x20000,
    RCC_APB1_I2C1   = 0x200000,
    RCC_APB1_I2C2   = 0x400000,
    RCC_APB1_I2C3   = 0x800000,
    RCC_APB1_PWR    = 0x1000000
}RCC_APB1_peripherals_t;


typedef enum{
    RCC_APB2_TIM1       = 0x1,
    RCC_APB2_USART1     = 0x10, 
    RCC_APB2_USART6     = 0x20,
    RCC_APB2_ADC1       = 0x100, 
    RCC_APB2_SDIO       = 0x800,
    RCC_APB2_SPI1       = 0x1000,
    RCC_APB2_SPI4       = 0x2000,
    RCC_APB2_SYSCFGEN   = 0x4000,
    RCC_APB2_TIM9       = 0x10000,
    RCC_APB2_TIM10      = 0x20000,
    RCC_APB2_TIM11      = 0x40000,
    RCC_APB2_SPI5       = 0x100000
}RCC_APB2_peripherals_t;

#define SWS_MASK (0x3 << 2)
#define SW_MASK SW_NOT_ALLOWED

#define RCC_BASE (0x40023800UL)
#define RCC ((__IO RCC_typedef_t *)RCC_BASE)

ALWAYS_INLINE void rcc_enable_hsi(void) {
    RCC->CR |= HSION_MASK;
    while (!(RCC->CR & HSIRDY_MASK));
}

extern void rcc_fallback_to_hsi(){
    rcc_enable_hsi();
    RCC->CFGR &= ~(SW_NOT_ALLOWED);
    RCC->CFGR |= SW_HSI;
    while ((RCC->CFGR & SWS_MASK)  != SW_HSI<<2);

}

ALWAYS_INLINE void rcc_enable_hse(void) {
    RCC->CR |= HSEON_MASK;
    while (!(RCC->CR & HSERDY_MASK));
}

ALWAYS_INLINE void rcc_set_ahb1_clocking(RCC_AHB1_peripherals_t ahb1_mask){
    RCC->AHB1ENR = ahb1_mask;
}

ALWAYS_INLINE void rcc_set_ahb2_clocking(RCC_AHB2_peripherals_t ahb2_mask){
    RCC->AHB2ENR = ahb2_mask;
}

ALWAYS_INLINE void rcc_set_apb1_clocking(RCC_APB1_peripherals_t apb1_mask){
    RCC->APB1ENR = apb1_mask;
}

ALWAYS_INLINE void rcc_set_apb2_clocking(RCC_APB2_peripherals_t apb2_mask){
    RCC->APB2ENR = apb2_mask;
}

ALWAYS_INLINE void rcc_enable_and_switch_to_pll(){
    RCC->CR |= PLLON_MASK;
    while (!(RCC->CR & PLLRDY_MASK));
    RCC->CFGR &= ~(SW_MASK);
    RCC->CFGR |= SW_PLL;
    while ((RCC->CFGR & SWS_MASK)  != SW_PLL<<2);// SWS repots same bits as SW just 2 bits farther
}

ALWAYS_INLINE void rcc_bus_prescalers_setup(HPRE_state_t ahb_prescaler,PPRE_state_t apb1_prescaler,
    PPRE_state_t apb2_prescaler)
{
    RCC->CFGR &= ~(PPRE1_MASK | PPRE2_MASK | HPRE_MASK);
    RCC->CFGR |= PPRE1_SET(apb1_prescaler) | PPRE2_SET(apb2_prescaler) | HPRE_SET(ahb_prescaler);
}

ALWAYS_INLINE void rcc_pll_setup(PLLP_states_t PLLP,uint8_t PLLM,uint16_t PLLN,uint8_t PLLQ, PLL_source_t source){
    
    if(!(RCC->CR & HSION_MASK) && source == PLL_SOURCE_HSI){
        rcc_enable_hsi();
    }

    if(RCC->CR & PLLON_MASK){
        RCC->CR &=~(PLLON_MASK); 
        while (RCC->CR & PLLRDY_MASK);
    }
    if(!(RCC->CR & HSEON_MASK) && source == PLL_SOURCE_HSE){
        rcc_enable_hse();
    }
    RCC->PLLCFGR = PLLP | PLLM_SET(PLLM) | PLLN_SET(PLLN) | PLLQ_SET(PLLQ) | source;  
    

}

extern void rcc_sysclock_setup(){
    rcc_enable_hse();
    rcc_pll_setup(BAD_PLLP, BAD_PLLM, BAD_PLLN, BAD_PLLQ, PLL_SOURCE_HSE);
    rcc_bus_prescalers_setup(BAD_AHB_PRE, BAD_APB1_PRE, BAD_APB2_PRE);
    rcc_enable_and_switch_to_pll();
}

#endif

#endif // BAD_HAL_USE_RCC

#ifdef BAD_HAL_USE_NVIC

typedef enum
{
    NVIC_WWDG_INTR                   = 0,
    NVIC_PVD_INTR                    = 1,
    NVIC_TAMP_STAMP_INTR             = 2,
    NVIC_RTC_WKUP_INTR               = 3,
    NVIC_FLASH_INTR                  = 4,
    NVIC_RCC_INTR                    = 5,
    NVIC_EXTI0_INTR                  = 6,
    NVIC_EXTI1_INTR                  = 7,
    NVIC_EXTI2_INTR                  = 8,
    NVIC_EXTI3_INTR                  = 9,
    NVIC_EXTI4_INTR                  = 10,
    NVIC_DMA1_STREAM0_INTR           = 11,
    NVIC_DMA1_STREAM1_INTR           = 12,
    NVIC_DMA1_STREAM2_INTR           = 13,
    NVIC_DMA1_STREAM3_INTR           = 14,
    NVIC_DMA1_STREAM4_INTR           = 15,
    NVIC_DMA1_STREAM5_INTR           = 16,
    NVIC_DMA1_STREAM6_INTR           = 17,
    NVIC_ADC_INTR                    = 18,
    NVIC_EXTI9_5_INTR                = 23,
    NVIC_TIM1_BRK_TIM9_INTR          = 24,
    NVIC_TIM1_UP_TIM10_INTR          = 25,
    NVIC_TIM1_TRG_COM_TIM11_INTR     = 26,
    NVIC_TIM1_CC_INTR                = 27,
    NVIC_TIM2_INTR                   = 28,
    NVIC_TIM3_INTR                   = 29,
    NVIC_TIM4_INTR                   = 30,
    NVIC_I2C1_EV_INTR                = 31,
    NVIC_I2C1_ER_INTR                = 32,
    NVIC_I2C2_EV_INTR                = 33,
    NVIC_I2C2_ER_INTR                = 34,
    NVIC_SPI1_INTR                   = 35,
    NVIC_SPI2_INTR                   = 36,
    NVIC_USART1_INTR                 = 37,
    NVIC_USART2_INTR                 = 38,
    NVIC_EXTI15_10_INTR              = 40,
    NVIC_RTC_ALARM_INTR              = 41,
    NVIC_OTG_FS_WKUP_INTR            = 42,
    NVIC_DMA1_STREAM7_INTR           = 47,
    NVIC_SDIO_INTR                   = 49,
    NVIC_TIM5_INTR                   = 50,
    NVIC_SPI3_INTR                   = 51,
    NVIC_DMA2_STREAM0_INTR           = 56,
    NVIC_DMA2_STREAM1_INTR           = 57,
    NVIC_DMA2_STREAM2_INTR           = 58,
    NVIC_DMA2_STREAM3_INTR           = 59,
    NVIC_DMA2_STREAM4_INTR           = 60,
    NVIC_OTG_FS_INTR                 = 67,
    NVIC_DMA2_STREAM5_INTR           = 68,
    NVIC_DMA2_STREAM6_INTR           = 69,
    NVIC_DMA2_STREAM7_INTR           = 70,
    NVIC_USART6_INTR                 = 71,
    NVIC_I2C3_EV_INTR                = 72,
    NVIC_I2C3_ER_INTR                = 73,
    NVIC_FPU_INTR                    = 81,
    NVIC_SPI4_INTR                   = 84,
    NVIC_SPI5_INTR                   = 85
} NVIC_programmable_intr_t;

typedef enum{
    NVIC_PRIO0 = 0,
    NVIC_PRIO1,
    NVIC_PRIO2,
    NVIC_PRIO3,
    NVIC_PRIO4,
    NVIC_PRIO5,
    NVIC_PRIO6,
    NVIC_PRIO7,
    NVIC_PRIO8,
    NVIC_PRIO9,
    NVIC_PRIO10,
    NVIC_PRIO11,
    NVIC_PRIO12,
    NVIC_PRIO13,
    NVIC_PRIO14,
    NVIC_PRIO15
} NVIC_prio_t;

typedef struct
{
    __IO  uint32_t ISER[8U];              
    uint32_t RESERVED0[24U];
    __IO  uint32_t ICER[8U];               
    uint32_t RESERVED1[24U];
    __IO  uint32_t ISPR[8U];               
    uint32_t RESERVED2[24U];
    __IO  uint32_t ICPR[8U];               
    uint32_t RESERVED3[24U];
    __IO  uint32_t IABR[8U];               
    uint32_t RESERVED4[56U];
    __IO  uint8_t  IP[240U];               
    uint32_t RESERVED5[644U];
    __IO  uint32_t STIR;                   
}  NVIC_typedef_t;


#define NVIC_BASE (0xE000E100UL)

#define NVIC ((NVIC_typedef_t*) NVIC_BASE)


ALWAYS_INLINE void nvic_enable_interrupt(NVIC_programmable_intr_t intrnum)
{
    uint8_t ISER_idx = intrnum >> 5; //deside the register by dividing by 32 
    uint32_t ISER_intr_mask = 1 << (intrnum & 0x1F); // the remainder will be the bit number to which we should write
    OPT_BARRIER;
    NVIC->ISER[ISER_idx] = ISER_intr_mask;
    DSB;
    OPT_BARRIER;
   
}

ALWAYS_INLINE void nvic_disable_interrupt(NVIC_programmable_intr_t intrnum)
{
    uint8_t ICER_idx = intrnum >> 5;
    uint32_t ICER_intr_mask = 1 << (intrnum & 0x1F);
    OPT_BARRIER;
    NVIC->ICER[ICER_idx] = ICER_intr_mask;
    DSB;
    OPT_BARRIER;
} 

ALWAYS_INLINE void nvic_set_interrupt_priority(NVIC_programmable_intr_t intrnum, NVIC_prio_t prio){
    NVIC->IP[intrnum] = prio << 4;
    DSB;
    OPT_BARRIER;
}

#endif // BAD_HAL_USE_NVIC

//Systick
#ifdef BAD_HAL_USE_SYSTICK

typedef struct {
  __IO uint32_t CTRL;                   
  __IO uint32_t LOAD;                   
  __IO uint32_t VAL;                    
  __IO  uint32_t CALIB;                  
} Systick_typedef_t;

#define SYSTICK_BASE (0xE000E010UL)

#define SYSTICK ((Systick_typedef_t *)SYSTICK_BASE)


#define SysTick_CTRL_ENABLE 0x1                                           
typedef enum{
    SYSTICK_FEATURE_TICK_INTERRUPT = 0x2, 
    SYSTICK_FEATURE_CLOCK_SOURCE = 0x4,
}SYSTICK_features_t;

ALWAYS_INLINE void systick_setup(uint32_t reload_value,SYSTICK_features_t features){
    SYSTICK->LOAD = reload_value-1;
    SYSTICK->CTRL = features;
    SYSTICK->VAL = 0;
}

ALWAYS_INLINE void systick_enable(){
    SYSTICK->CTRL |= SysTick_CTRL_ENABLE;
}

ALWAYS_INLINE void systick_disable(){
    SYSTICK->CTRL &= ~(SysTick_CTRL_ENABLE);
}

#endif // BAD_HAL_USE_SYSTICK


//Peripherals
//USART
#ifdef BAD_HAL_USE_USART

#ifndef BAD_USART_DEF
#ifdef BAD_USART_STATIC
    #define BAD_USART_DEF ALWAYS_INLINE
#else
    #define BAD_USART_DEF extern
#endif
#endif



typedef struct USART_regs_t{
    __IO uint32_t SR;
    __IO uint32_t DR;
    __IO uint32_t BRR;
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t CR3;
    __IO uint32_t GTPR;
} USART_typedef_t;
// Compile time divisor calculation
#define USART1_BASE (0x40011000)

#define USART1      ((__IO USART_typedef_t *)USART1_BASE)


typedef enum{
    USART_IDLEIE = 0x10,
    USART_RXNEIE = 0x20,
    USART_TCIE = 0x40,
    USART_TXEIE = 0x80,
    USART_PEIE = 0x100
}USART_interrupt_flags_t;


#define USART_CALCULATE_BRR(baud,clock) \
    (((uint16_t)((float)clock/(16*baud)) << 4) | \
     (uint8_t)(((((float)clock/(16*baud)) - (uint16_t)((float)clock/(16*baud)))*16) + 0.5f))


#define USART_BRR_115200 USART_CALCULATE_BRR(115200UL,CLOCK_SPEED)
#define USART_BRR_9600 USART_CALCULATE_BRR(9600UL,CLOCK_SPEED)
#define USART_CR1_USART_ENABLE 0x2000
typedef enum{
    USART_SR_PE     = 0x1,
    USART_SR_FE     = 0x2,
    USART_SR_NF     = 0x4,
    USART_SR_ORE    = 0x8,
    USART_SR_IDLE   = 0x10,
    USART_SR_RXNE   = 0x20,
    USART_SR_TC     = 0x40,
    USART_SR_TXE    = 0x80,
    USART_SR_TBD    = 0x100,
    USART_SR_CTS    = 0x200
}USART_SR_flags_t;

typedef enum{
    USART_FEATURE_RECIEVE_EN = 0x4,
    USART_FEATURE_TRANSMIT_EN = 0x8,
    USART_FEATURE_PARITY_EVEN = 0x0,
    USART_FEATURE_PARITY_ODD = 0x200,
    USART_FEATURE_PARITY_OFF = 0x0,
    USART_FEATURE_PARITY_ON = 0x400,
    USART_FEATURE_WAKE_IDLE = 0x0,
    USART_FEATURE_WAKE_ADDR_MART =0x800,  
    USART_FEATURE_8BIT_WORD = 0x0,
    USART_FEATURE_9BIT_WORD = 0x1000,
    USART_FEATURE_OVERSAMPLING_16 =0x0, 
    USART_FEATURE_OVERSAMPLING_8 = 0x8000
}USART_feature_t;

typedef enum{
    USART_MISC_DMA_RECIEVE = 0x40, 
    USART_MISC_DMA_TRANSMIT = 0x80
}USART_misc_t;


ALWAYS_INLINE void uart_enable_misc(__IO USART_typedef_t * USART , USART_misc_t misc){
    USART->CR3 |= misc;
}

ALWAYS_INLINE void uart_disable_misc(__IO USART_typedef_t * USART ,USART_misc_t misc){
    USART->CR3 &= ~(misc);
}

ALWAYS_INLINE void uart_enable_interrupts(__IO USART_typedef_t * USART,USART_interrupt_flags_t interrupts){
    USART->CR1 |= interrupts;
}

ALWAYS_INLINE void uart_disable_interrupts(__IO USART_typedef_t * USART,USART_interrupt_flags_t interrupts){
    USART->CR1 &= ~(interrupts);
}
BAD_USART_DEF void uart_enable(__IO USART_typedef_t* USART);
BAD_USART_DEF void uart_disable(__IO USART_typedef_t * USART);
BAD_USART_DEF void uart_putchar_polling(__IO USART_typedef_t*,char);
BAD_USART_DEF char uart_getchar_polling(__IO USART_typedef_t*);
BAD_USART_DEF void uart_setup(__IO USART_typedef_t * USART,
    uint16_t BRR,
    USART_feature_t features,
    USART_misc_t misc,
    USART_interrupt_flags_t interrupt);
BAD_USART_DEF void uart_send_str_polling(__IO USART_typedef_t* USART ,const char* str);
BAD_USART_DEF void uart_send_hex_32bit(__IO USART_typedef_t* USART,uint32_t value);
BAD_USART_DEF void uart_send_dec_unsigned_32bit(__IO USART_typedef_t *USART ,uint32_t value);

#ifdef BAD_USART_IMPLEMENTATION

BAD_USART_DEF void uart_enable(__IO USART_typedef_t* USART){
    while (!(USART->SR & USART_SR_TC));
    USART->CR1 |= USART_CR1_USART_ENABLE;
}
BAD_USART_DEF void uart_disable(__IO USART_typedef_t * USART) {
    while (!(USART->SR & USART_SR_TC));
    USART->CR1 &= ~USART_CR1_USART_ENABLE;
}

BAD_USART_DEF void uart_putchar_polling(__IO USART_typedef_t* USART,char ch){
    while (!(USART->SR & USART_SR_TXE)); 
    USART->DR = ch;
}

BAD_USART_DEF char uart_getchar_polling(__IO USART_typedef_t* USART){
    while(!(USART->SR & USART_SR_RXNE));
    return (char)USART->DR;
}

BAD_USART_DEF void uart_setup(__IO USART_typedef_t * USART,
    uint16_t BRR,
    USART_feature_t features,
    USART_misc_t misc,
    USART_interrupt_flags_t interrupts)
{
    USART->CR1 = features| interrupts;
    USART->BRR = BRR;
    USART->CR2 = 0; //unsupported for now
    USART->CR3 = misc;
}
BAD_USART_DEF void uart_send_str_polling(__IO USART_typedef_t* USART ,const char* str){
    while(*str){
        uart_putchar_polling(USART,*str);
        str++;
    }   
}

BAD_USART_DEF void uart_send_hex_32bit(__IO USART_typedef_t* USART,uint32_t value){
    const char lookup[] ="0123456789ABCDEF";
    
    for (uint8_t i = 0; i < 8 ;i++){
        uint32_t idx = (value >>28) & 0xF ;
        char c = lookup[idx];
        uart_putchar_polling(USART,c);
        value <<= 4;
    }
    uart_send_str_polling(USART, "\r\n");
}

BAD_USART_DEF void uart_send_dec_unsigned_32bit(__IO USART_typedef_t *USART ,uint32_t value){
    char buff[11];
    char *write = buff+11;
    *--write = 0;

    do{ 
        *--write = (value%10)+'0';
        value/=10;
    }
    while (value!=0);

    uart_send_str_polling(USART, write);
    uart_send_str_polling(USART, "\r\n");
}
#endif

#endif // BAD_HAL_USE_USART

#ifdef BAD_HAL_USE_GPIO

typedef struct{
    __IO uint32_t MODER;
    __IO uint32_t OTYPER;
    __IO uint32_t OSPEEDR;
    __IO uint32_t PUPDR;
    __IO uint32_t IDR;
    __IO uint32_t ODR;
    __IO uint32_t BSRR;
    __IO uint32_t LCKR;
    __IO uint32_t AFRL;
    __IO uint32_t AFRH;
}GPIO_typedef_t;

#define GPIOA_BASE      (0x40020000UL)
#define GPIOA ((__IO GPIO_typedef_t*)GPIOA_BASE)

#define GPIOB_BASE      (0x40020400UL)
#define GPIOB ((__IO GPIO_typedef_t*)GPIOB_BASE)

#define GPIOC_BASE      (0x40020800UL)
#define GPIOC ((__IO GPIO_typedef_t*)GPIOC_BASE)

#define BSSRx_BR(x)     (1<<(x + 16))
#define BSSRx_BS(x)     (1 << x)

ALWAYS_INLINE void io_pin_set(volatile GPIO_typedef_t *GPIO, uint8_t pin_num){
    GPIO->BSRR = BSSRx_BS(pin_num);
}

ALWAYS_INLINE void io_pin_reset(volatile GPIO_typedef_t *GPIO, uint8_t pin_num){
    GPIO->BSRR = BSSRx_BR(pin_num);
}

#ifdef BAD_GPIO_IMPLEMENTATION
#define MODERx_MASK(x)  (0x3<<(x * 2))
#define MODERx_SET(val,x)((val & 0x3) <<(x * 2))
#define OSPEEDRx_SET(val,x) ((val & 0x3)<<(x * 2))
#define OSPEEDRx_MASK(x) (0x3<<(x * 2))
#define PUPDRx_SET(val,x) ((val & 0x3)<<(x * 2))
#define PUPDRx_MASK(x) (0x3<<(x * 2))
#define OTx_MASK(x) (1 << x )//1 bit field 0-push pull 1-open drain
#define ODRx_MASK(x)    (1 << x )
#define OTYPERx_SET(val,x) (val << x)

typedef enum {
    MODER_reset_input = 0b00,
    MODER_output = 0b01,
    MODER_af = 0b10,
    MODER_analog = 0b11
}MODERx_states_t;


typedef enum {
    OSPEEDR_low_speed = 0b00,
    OSPEEDR_medium_speed = 0b01,
    OSPEEDR_fast_speed = 0b10,
    OSPEEDR_high_speed = 0b11
}OSPEEDRx_states_t;

#define OSPEEDRx_SET(val,x) ((val & 0x3)<<(x * 2))
#define OSPEEDRx_MASK(x) (0x3<<(x * 2))

typedef enum {
    PUPDR_no_pull = 0b00,
    PUPDR_pullup = 0b01,
    PUPDR_pulldown = 0b10,
    PUPDR_reserved = 0b11
}PUPDRx_states_t;


typedef enum {
    OTYPR_push_pull = 0,
    OTYPR_open_drain = 1
}OTYPRx_state_t;



extern inline void io_setup_pin(__IO GPIO_typedef_t *GPIO, uint8_t pin_num, MODERx_states_t mode, uint8_t af, OSPEEDRx_states_t speed, PUPDRx_states_t pull, OTYPRx_state_t type){
    GPIO->MODER &=~(MODERx_MASK(pin_num));
    GPIO->MODER |= MODERx_SET(mode, pin_num);
    if (mode == MODER_af) {
        if(pin_num >=8 && pin_num <=15){
            GPIO->AFRH &= ~(0xFU << (((pin_num) - 8) * 4));                 
            GPIO->AFRH |=  (af & 0xF) << (((pin_num) - 8) * 4);       
        }else if (pin_num<=7) {
            GPIO->AFRL &= ~(0xFU << (pin_num) * 4);                 
            GPIO->AFRL |=  (af & 0xF) << (pin_num  * 4);  
        }
    }
    GPIO->OTYPER &= ~(OTx_MASK(pin_num));
    GPIO->OTYPER |= OTYPERx_SET(type, pin_num);
    GPIO->OSPEEDR &= ~(OSPEEDRx_MASK(pin_num));
    GPIO->OSPEEDR |= OSPEEDRx_SET(speed,pin_num);
    GPIO->PUPDR &= ~PUPDRx_MASK(pin_num);
    GPIO->PUPDR |= PUPDRx_SET(pull,pin_num);
}

#endif

#endif //BAD_HAL_USE_GPIO

//SPI
#ifdef BAD_HAL_USE_SPI

#ifdef BAD_SPI_STATIC
    #define BAD_SPI_DEF ALWAYS_INLINE
#else
    #define BAD_SPI_DEF extern
#endif

typedef struct{
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t SR;
    __IO uint32_t DR;
    __IO uint32_t CRCPR;
    __IO uint32_t RXCRCR;
    __IO uint32_t TXCRCR;
    __IO uint32_t I2SCFGR;
    __IO uint32_t I2SPR;
}SPI_typedef_t;

#define SPI1_BASE   (0x40013000)
#define SPI1        ((__IO SPI_typedef_t*)SPI1_BASE)

typedef enum {
    SPI_FEATURE_CPHA0   = 0x0,
    SPI_FEATURE_CPHA1   = 0x1,
    SPI_FEATURE_CPOL0   = 0x0,
    SPI_FEATURE_CPOL1   = 0x2, 
    SPI_FEATURE_MASTER  = 0x4,
    SPI_FEATURE_SLAVE   = 0x0,
    SPI_FEATURE_PRECALER_div_2 = 0x0,
    SPI_FEATURE_PRECALER_div_4 = 0x8,
    SPI_FEATURE_PRECALER_div_8 = 0x10,
    SPI_FEATURE_PRECALER_div_16 = 0x18,
    SPI_FEATURE_PRECALER_div_32 = 0x20,
    SPI_FEATURE_PRECALER_div_64 = 0x28,
    SPI_FEATURE_PRECALER_div_128 = 0x30,
    SPI_FEATURE_PRECALER_div_256 = 0x38,
    SPI_FEATURE_FORMAT_MSB = 0x0,
    SPI_FEATURE_FORMAT_LSB = 0x80,
    SPI_FEATURE_HARDWARE_CS = 0x200,
    SPI_FEATURE_SOFTWARE_CS = 0x300,
    SPI_FEATURE_RX_ONLY = 0x400,
    SPI_FEATURE_FRAME_FORMAT_8bit = 0x0,
    SPI_FEATURE_FRAME_FORMAT_16bit = 0x800,
    SPI_FEATURE_ENABLE_CRC = 0x2000,
    SPI_FEATURE_BIDIOE_RX_ONLY = 0,
    SPI_FEATURE_BIDIOE_TX_ONLY = 0x4000,
    SPI_FEATURE_BIDIRECTIONAL = 0x8000 
}SPI_feature_t;

typedef enum{
    SPI_MISC_ENABLE_DMA_RX = 0x1,
    SPI_MISC_ENABLE_DMA_TX = 0x2,
    SPI_MISC_ENABLE_HARDWARE_NSS = 0x4,
    SPI_MISC_MOTOROLA_FORMAT = 0x0,
    SPI_MISC_TI_FORMAT = 0x10,
}SPI_misc_t;

typedef enum{
    SPI_INTERRUPT_ERROR_INTR = 0x20,
    SPI_INTERRUPT_RX_NOT_EMPTY_INTR = 0x40,
    SPI_INTERRUPT_TX_EMPTY_INTR = 0x80
}SPI_interrupt_t;

#define SPI_CR1_SPIEN_MASK  (0x40)
#define SPI_SR_BSY_MASK     (0x80)
#define SPI_SR_RXNE_MASK    (0x1)
#define SPI_SR_TXE_MASK     (0x2)

ALWAYS_INLINE void spi_enable_interrupts(__IO SPI_typedef_t* SPI,SPI_interrupt_t interrupts){
    SPI->CR2 |= interrupts;
}

ALWAYS_INLINE void spi_disable_interrupts(__IO SPI_typedef_t* SPI,SPI_interrupt_t interrupts){
    SPI->CR2 &= ~(interrupts);
}

ALWAYS_INLINE void spi_enable_misc(__IO SPI_typedef_t* SPI, SPI_misc_t misc){ //call this only when spi is disabled
    SPI->CR2 |= misc;
}

ALWAYS_INLINE void spi_disable_misc(__IO SPI_typedef_t* SPI, SPI_misc_t misc){ //call this only when spi is disabled
    SPI->CR2 &= ~misc;
}

ALWAYS_INLINE void spi_setup(__IO SPI_typedef_t* SPI, SPI_feature_t features,SPI_misc_t misc, SPI_interrupt_t interrupts){
    SPI->CR1 = features;
    SPI->CR2 = misc | interrupts;
}

BAD_SPI_DEF void spi_enable(__IO SPI_typedef_t* SPI);
BAD_SPI_DEF void spi_disable(__IO SPI_typedef_t* SPI);
BAD_SPI_DEF uint8_t spi_transmit_recieve(__IO SPI_typedef_t *SPI, uint8_t data);
BAD_SPI_DEF void spi_transmit_only(__IO SPI_typedef_t *SPI, uint8_t data);

#ifdef BAD_SPI_IMPLEMENTATION


BAD_SPI_DEF void spi_enable(__IO SPI_typedef_t* SPI){
    SPI->CR1 |= SPI_CR1_SPIEN_MASK;
    while (!(SPI->CR1 & SPI_CR1_SPIEN_MASK));
    while (SPI->SR & SPI_SR_BSY_MASK); 
}

BAD_SPI_DEF void spi_disable(__IO SPI_typedef_t* SPI){
    while (SPI->SR & SPI_SR_BSY_MASK); 
    SPI->CR1 &= ~(SPI_CR1_SPIEN_MASK);
    while (SPI->CR1 & SPI_CR1_SPIEN_MASK); 
}


BAD_SPI_DEF uint8_t spi_transmit_recieve(__IO SPI_typedef_t *SPI, uint8_t data){
    SPI->DR = data;
    while (!(SPI->SR & SPI_SR_RXNE_MASK));
    return SPI->DR;
} 

BAD_SPI_DEF void spi_transmit_only(__IO SPI_typedef_t *SPI, uint8_t data){;     
    SPI->DR = data;
    while (!(SPI->SR & SPI_SR_BSY_MASK));
    while (SPI->SR & SPI_SR_BSY_MASK); 

}
#endif

#endif //BAD_HAL_USE_SPI

//DMA
#ifdef BAD_HAL_USE_DMA

#ifdef BAD_DMA_STATIC
    #define BAD_DMA_DEF static inline
#else
    #define BAD_DMA_DEF extern
#endif

typedef struct{
    __IO uint32_t CR;
    __IO uint32_t NDTR;
    __IO uint32_t PAR;
    __IO uint32_t M0AR;
    __IO uint32_t M1AR;
    __IO uint32_t FCR;
}DMA_stream_typedef_t;

typedef struct {
    __IO uint32_t LISR;
    __IO uint32_t HISR;
    __IO uint32_t LIFCR;
    __IO uint32_t HIFCR;
    __IO DMA_stream_typedef_t streams[8];
} DMA_typedef_t ;

typedef enum{
    DMA_STREAM0 = 0,
    DMA_STREAM1, 
    DMA_STREAM2,  
    DMA_STREAM3,
    DMA_STREAM4,
    DMA_STREAM5,
    DMA_STREAM6,
    DMA_STREAM7  
} DMA_stream_num_t;

typedef enum {
    DMA_enable_DME = 0x2,
    DMA_enable_TE  = 0x4,
    DMA_enable_HT  = 0x8,
    DMA_enable_TC  = 0x10
} DMA_interrupts_t;

typedef enum{
    DMA_clear_FE  = 0x1,
    DMA_clear_DME = 0x4,
    DMA_clear_TE  = 0x8,
    DMA_clear_HT  = 0x10,
    DMA_clear_TC  = 0x20,
    DMA_clear_all = DMA_clear_DME | DMA_clear_FE | DMA_clear_HT | DMA_clear_TE | DMA_clear_TC
} DMA_clear_interrupts_t;

typedef enum{
    DMA_feature_PFCTRL = 0x20,
    DMA_feature_DIR_periph_to_mem = 0x0,
    DMA_feature_DIR_mem_to_periph = 0x40,
    DMA_feature_DIR_mem_to_mem = 0x80,
    DMA_feature_CIRC = 0x100,
    DMA_feature_PINC = 0x200,
    DMA_feature_MINC = 0x400,
    DMA_feature_PSIZE_byte = 0x0,
    DMA_feature_PSIZE_half_word = 0x800,
    DMA_feature_PSIZE_word = 0x1000,
    DMA_feature_MSIZE_byte = 0x0,
    DMA_feature_MSIZE_half_word = 0x2000,
    DMA_feature_MSIZE_word = 0x4000,
    DMA_feature_PINCOS_pinc = 0x0,
    DMA_feature_PINCOS_word = 0x8000,
    DMA_feature_PL_low_prio = 0x0,
    DMA_feature_PL_medium_prio = 0x10000,
    DMA_feature_PL_high_prio = 0x20000,
    DMA_feature_PL_very_high_prio = 0x30000,
    DMA_feature_DBM = 0x40000,
    DMA_feature_CT = 0x80000,
    DMA_feature_PBURST_no_burst = 0x0,
    DMA_feature_PBURST_incr4 = 0x200000,
    DMA_feature_PBURST_incr8 = 0x400000,
    DMA_feature_PBURST_incr16 = 0x600000,
    DMA_feature_MBURST_no_burst = 0x0,
    DMA_feature_MBURST_incr4 = 0x800000,
    DMA_feature_MBURST_incr8 = 0x1000000,
    DMA_feature_MBURST_incr16 = 0x2000000,
}DMA_features_t;

typedef enum {
    DMA_channel0 = 0,
    DMA_channel1 = (1<<25),
    DMA_channel2 = (2<<25),
    DMA_channel3 = (3<<25),
    DMA_channel4 = (4<<25),
    DMA_channel5 = (5<<25),
    DMA_channel6 = (6<<25),
    DMA_channel7 = (7<<25)
}DMA_channel_num_t;

typedef enum{
    DMA_FIFO_THRESHOLD_1_out_4 = 0x0,
    DMA_FIFO_THRESHOLD_2_out_4 = 0x1,
    DMA_FIFO_THRESHOLD_3_out_4 = 0x2,
    DMA_FIFO_THRESHOLD_4_out_4 = 0x3,
    DMA_FIFO_ENABLE_FIFO = 0x4,
    DMA_FIFO_ENABLE_ERROR_INTERRUPT = 0x80
}DMA_fifo_settings_t;
#define DMA1_BASE   (0x40026000)
#define DMA2_BASE   (0x40026400)

#define DMA1 ((__IO DMA_typedef_t *) DMA1_BASE)
#define DMA2 ((__IO DMA_typedef_t *) DMA2_BASE)

#define CR_EN_MASK  (0x1)

ALWAYS_INLINE void dma_clear_interrupts(__IO DMA_typedef_t * DMA,DMA_stream_num_t stream, DMA_clear_interrupts_t interrupts){
    static const uint32_t shift[4] = {
        0,
        6,
        16,
        22
    };

    if (stream <= 3){
        DMA->LIFCR = interrupts << shift[stream];
    }
    else{
        DMA->HIFCR = interrupts << shift[stream - 4];
    }
}

ALWAYS_INLINE uint8_t dma_stream_n_poll_ready(__IO DMA_typedef_t * DMA,DMA_stream_num_t stream){
    return DMA->streams[stream].NDTR == 0;
}

ALWAYS_INLINE void dma_enable_interrupts(__IO DMA_typedef_t * DMA,DMA_stream_num_t stream,DMA_interrupts_t interrupts){
    DMA->streams[stream].CR |= interrupts;
}
ALWAYS_INLINE void dma_disable_interrupts(__IO DMA_typedef_t * DMA,DMA_stream_num_t stream, DMA_interrupts_t interrupts){
    DMA->streams[stream].CR &= ~(interrupts);
}

ALWAYS_INLINE void dma_start_transfer(__IO DMA_typedef_t * DMA, DMA_stream_num_t stream){
    DMA->streams[stream].CR |= CR_EN_MASK;
}

BAD_DMA_DEF void dma_setup_transfer(__IO DMA_typedef_t * DMA, 
    DMA_stream_num_t stream,
    DMA_channel_num_t channel,volatile uint32_t mem,
    uint16_t bufflen,
    uint32_t periph, 
    DMA_interrupts_t interrupts, 
    DMA_features_t features,
    DMA_fifo_settings_t fifo_settings);



#ifdef BAD_DMA_IMPLEMENTATION

BAD_DMA_DEF void dma_setup_transfer(__IO DMA_typedef_t * DMA, DMA_stream_num_t stream,DMA_channel_num_t channel,volatile uint32_t mem,uint16_t bufflen,uint32_t periph, DMA_interrupts_t interrupts, DMA_features_t features,DMA_fifo_settings_t fifo_settings){
    DMA->streams[stream].CR &= ~(CR_EN_MASK);
    while(DMA->streams[stream].CR & CR_EN_MASK);
    dma_clear_interrupts(DMA, stream, DMA_clear_all);
    DMA->streams[stream].PAR = (uint32_t)periph;
    DMA->streams[stream].M0AR = (uint32_t)mem;
    DMA->streams[stream].NDTR = bufflen;
    DMA->streams[stream].CR = interrupts | features | channel;
    DMA->streams[stream].FCR = fifo_settings;
}

#endif
//MPU
#endif // BAD_HAL_USE_DMA


//EXTI
#ifdef BAD_HAL_USE_EXTI

#ifdef BAD_EXTI_STATIC
    #define BAD_EXTI_DEF static inline
#else
    #define BAD_EXTI_DEF extern
#endif

typedef struct{
    __IO uint32_t IMR;
    __IO uint32_t EMR;
    __IO uint32_t RTSR;
    __IO uint32_t FTSR;
    __IO uint32_t SWIER;
    __IO uint32_t PR;
} EXTI_typedef_t;

typedef enum {
    EXTI_TRIGGER_RISING  = 1,
    EXTI_TRIGGER_FALLING = 2,
    EXTI_TRIGGER_BOTH = EXTI_TRIGGER_RISING | EXTI_TRIGGER_FALLING, // 0x3
} EXTI_trigger_t;

#define EXTI_BASE   (0x40013C00UL)
#define EXTI        ((__IO EXTI_typedef_t *)EXTI_BASE)
BAD_EXTI_DEF void exti_configure_line(uint8_t line, EXTI_trigger_t trigger);

#ifdef BAD_EXTI_IMPLEMENTATION
BAD_EXTI_DEF void exti_configure_line(uint8_t line, EXTI_trigger_t trigger)
{
    
    EXTI->RTSR &= ~(1 << line);
    EXTI->FTSR &= ~(1 << line);

    
    if (trigger & EXTI_TRIGGER_RISING)
        EXTI->RTSR |= (1 << line);
    if (trigger & EXTI_TRIGGER_FALLING)
        EXTI->FTSR |= (1 << line);

    // Unmask the interrupt
    EXTI->PR  |= (1 << line);
    EXTI->IMR |= (1 << line);
}
#endif

#endif // BAD_HAL_USE_EXTI

//Syscfg
#ifdef BAD_HAL_USE_SYSCFG

typedef struct {
    __IO uint32_t MEMRM;
    __IO uint32_t PMC;
    __IO uint32_t EXTICR[4];
    __IO uint32_t CMPCR;
} SYSCFG_typedef_t;

typedef enum{
    SYSCFG_PAx = 0,
    SYSCFG_PBx = 1,
    SYSCFG_PCx = 2,
    SYSCFG_PDx = 3,
    SYSCFG_PEx = 4,
    SYSCFG_PHx = 7, 
}SYSCFG_EXTI_port_t;

#define SYSCFG_BASE (0x40013800UL)

#define SYSCFG ((__IO SYSCFG_typedef_t *)SYSCFG_BASE)

ALWAYS_INLINE void syscfg_set_exti_pin(SYSCFG_EXTI_port_t port, uint8_t pin){
    uint8_t crnum = pin >> 2;
    uint8_t shift = (pin & 0x3) << 2;

    SYSCFG->EXTICR[crnum] &= ~(0xF << shift);

    SYSCFG->EXTICR[crnum] |= port << shift;
}

#endif // BAD_HAL_USE_SYSCFG

#ifdef BAD_HAL_USE_BTIMER

#ifndef BAD_TIMER_DEF
#ifdef BAD_TIMER_STATIC
    #define BAD_TIMER_DEF ALWAYS_INLINE
#else
    #define BAD_TIMER_DEF extern
#endif
#endif

typedef struct {
    __IO uint32_t CR1;
    uint32_t PADDING0[2];
    __IO uint32_t DIER;
    __IO uint32_t SR;
    __IO uint32_t EGR;
    __IO uint32_t CCMR1;
    __IO uint32_t CCER;
    uint32_t PADDING1;
    __IO uint32_t CNT;
    __IO uint32_t PSC;
    __IO uint32_t ARR;
    uint32_t PADDING2;
    __IO uint32_t CCR1;
} BTIMER_typedef_t;

typedef enum{
    BTIMER_UPDATE = 0x1,
    BTIMER_CC = 0x2
}BTIMER_interrupts_t;

#define BTIM10_BASE 0x40014400UL

#define BTIM10 ((__IO BTIMER_typedef_t *)BTIM10_BASE)

#define TIM_CR_CEN 0x1

ALWAYS_INLINE void tim_enable(__IO BTIMER_typedef_t* TIM){
    TIM->CR1 |= TIM_CR_CEN; 
}

ALWAYS_INLINE void tim_disable(__IO BTIMER_typedef_t* TIM){
    TIM->CR1 &= ~TIM_CR_CEN;
}

BAD_TIMER_DEF void basic_timer_setup(__IO BTIMER_typedef_t* TIM,uint16_t barr,uint16_t bpsc, BTIMER_interrupts_t intr);
#ifdef BAD_TIMER_IMPLEMENTATION
BAD_TIMER_DEF void basic_timer_setup(__IO BTIMER_typedef_t* TIM,uint16_t barr,uint16_t bpsc, BTIMER_interrupts_t intr){
    TIM->ARR = barr;
    TIM->PSC = bpsc; 
    TIM->EGR = 1;
    TIM->DIER = intr;
}
#endif

#endif // BAD_HAL_USE_BTIMER

#ifdef BAD_HAL_USE_CRC

typedef struct {
    __IO uint32_t DR;
    __IO uint32_t IDR;
    __IO uint32_t CR;
}CRC_typedef_t;

#define CRC_BASE 0x40023000

#define CRC ((__IO CRC_typedef_t *)CRC_BASE)

#define CRC_CR_RESET 0x1

ALWAYS_INLINE void crc_reset(){
    CRC->CR = CRC_CR_RESET;
}

#endif

//Interrupts
//Hardfault interrupt
//HardFault handler with optional UART logging.
#ifdef BAD_HARDFAULT_ISR_IMPLEMENTATION

#ifdef BAD_HARDFAULT_USE_UART

#ifndef FAULT_LOG_UART
#define FAULT_LOG_UART USART1
#endif

#define FAULT_LOG_UART_SETTINGS (USART_FEATURE_TRANSMIT_EN)

#endif

void __attribute__((naked)) isr_hardfault(){ 
    __asm volatile(
        "cpsid i        \n"
        "tst lr,#4      \n"
        "ite eq         \n"
        "mrseq r0,msp   \n"
        "mrsne r0,psp   \n"
        "b hardfault_c  \n"
    );
}

void __attribute__((used)) hardfault_c(uint32_t* stack){
    volatile uint32_t r0  = stack[0];
    volatile uint32_t r1  = stack[1];
    volatile uint32_t r2  = stack[2];
    volatile uint32_t r3  = stack[3];
    volatile uint32_t r12 = stack[4];
    volatile uint32_t lr  = stack[5]; 
    volatile uint32_t pc  = stack[6];  
    volatile uint32_t psr = stack[7];
    volatile uint32_t hfsr = SCB->HFSR;
    volatile uint32_t cfsr = SCB->CFSR;
    volatile uint32_t mmfar = SCB->MMFAR;
    volatile uint32_t bfar = SCB->BFAR;
    volatile uint32_t afsr = SCB->AFSR;
    volatile uint32_t dfsr = SCB->DFSR;
#ifdef BAD_HARDFAULT_USE_UART
    uart_disable(FAULT_LOG_UART);
    uart_setup(FAULT_LOG_UART,USART_BRR_9600,FAULT_LOG_UART_SETTINGS,0,0);
    uart_enable(FAULT_LOG_UART);
    uart_send_str_polling(FAULT_LOG_UART,"HARDFAULT\r\n");
    uart_send_str_polling(FAULT_LOG_UART, "R0 = ");
    uart_send_hex_32bit(FAULT_LOG_UART, r0);
 
    uart_send_str_polling(FAULT_LOG_UART, "R1 = ");
    uart_send_hex_32bit(FAULT_LOG_UART, r1);

    uart_send_str_polling(FAULT_LOG_UART, "R2 = ");
    uart_send_hex_32bit(FAULT_LOG_UART, r2);

    uart_send_str_polling(FAULT_LOG_UART, "R3 = ");
    uart_send_hex_32bit(FAULT_LOG_UART, r3);

    uart_send_str_polling(FAULT_LOG_UART, "R12 = ");
    uart_send_hex_32bit(FAULT_LOG_UART, r12);

    uart_send_str_polling(FAULT_LOG_UART, "LR = ");
    uart_send_hex_32bit(FAULT_LOG_UART, lr);

    uart_send_str_polling(FAULT_LOG_UART, "!!PC = ");
    uart_send_hex_32bit(FAULT_LOG_UART, pc&~(0x1));

    uart_send_str_polling(FAULT_LOG_UART, "xPSR =  ");
    uart_send_hex_32bit(FAULT_LOG_UART, psr);

    uart_send_str_polling(FAULT_LOG_UART, "SP = ");
    uart_send_hex_32bit(FAULT_LOG_UART, (uint32_t)stack);



    uart_send_str_polling(FAULT_LOG_UART, "CFSR = ");
    uart_send_hex_32bit(FAULT_LOG_UART, cfsr);

    uart_send_str_polling(FAULT_LOG_UART, "HFSR = ");
    uart_send_hex_32bit(FAULT_LOG_UART, hfsr);

    uart_send_str_polling(FAULT_LOG_UART, "DFSR = ");
    uart_send_hex_32bit(FAULT_LOG_UART, dfsr);

    uart_send_str_polling(FAULT_LOG_UART, "MMFAR = ");
    uart_send_hex_32bit(FAULT_LOG_UART, mmfar);

    uart_send_str_polling(FAULT_LOG_UART,"BFAR = " );
    uart_send_hex_32bit(FAULT_LOG_UART, bfar);

    uart_send_str_polling(FAULT_LOG_UART, "AFSR = ");
    uart_send_hex_32bit(FAULT_LOG_UART,afsr );
#endif
    while (1) {
    
    }
}


#endif

//Systick
#ifdef  BAD_SYSTICK_SYSTICK_ISR_IMPLEMENTATION

void systick_usr();

STRONG_ISR(systick_isr){
    systick_usr();
}

#endif

//USART interrupts
#ifdef BAD_USART_USART1_ISR_IMPLEMENTATION
#ifdef BAD_USART_USART1_USE_RXNE
void usart1_rx_isr(char);
#endif

STRONG_ISR(usart1_isr){
    if(USART1->SR & RXNE_MASK){
#ifdef BAD_USART_USART1_USE_RXNE
        usart1_rx_isr(USART1->DR);
#endif
    }
}
#endif
//

//SPI interrupts
#ifdef BAD_SPI_SPI1_ISR_IMPLEMENTATION

#ifdef BAD_SPI_SPI1_USE_RXNE
void spi1_rx_isr(uint16_t data);
#endif

STRONG_ISR(spi1_isr){
    if(SPI1->SR & SPI_SR_RXNE_MASK){
#ifdef BAD_SPI_SPI1_USE_RXNE
        spi1_rx_isr(SPI->DR);
#endif
    }
}

#endif
//

//DMA interrupts
#ifdef BAD_DMA_DMA2_STREAM2_ISR_IMPLEMENTATION

#ifdef BAD_DMA_DMA2_STREAM2_USE_FE
void dma2_stream2_fe(uint16_t offset);
#endif

#ifdef BAD_DMA_DMA2_STREAM2_USE_DME
void dma2_stream2_dme(uint16_t offset);
#endif

#ifdef BAD_DMA_DMA2_STREAM2_USE_TE
void dma2_stream2_te(uint16_t offset);
#endif

#ifdef BAD_DMA_DMA2_STREAM2_USE_TC
void dma2_stream2_tc(uint16_t offset);
#endif

#ifdef BAD_DMA_DMA2_STREAM2_USE_HT
void dma2_stream2_ht(uint16_t offset);
#endif


STRONG_ISR(dma2_stream2_isr){
    enum DMA_Stream2_flag_LISR {
        DMA_Stream2_frame_error  = (1UL << 16UL),
        DMA_Stream2_direct_mode_error= (1UL << 18UL),
        DMA_Stream2_transfer_error  = (1UL << 19UL),
        DMA_Stream2_half_transfer  = (1UL << 20UL),
        DMA_Stream2_transfer_complete  = (1UL << 21UL)
    };
    
    if(DMA2->LISR & DMA_Stream2_frame_error){
        
        DMA2->LIFCR |= DMA_Stream2_frame_error;
#ifdef BAD_DMA_DMA2_STREAM2_USE_FE
        dma2_stream2_fe(DMA2->streams[2].NDTR);
#endif
    }

    if(DMA2->LISR & DMA_Stream2_direct_mode_error){
        DMA2->LIFCR |= DMA_Stream2_direct_mode_error;
#ifdef BAD_DMA_DMA2_STREAM2_USE_DME
        dma2_stream2_dme(DMA2->streams[2].NDTR);
#endif
    }

    if(DMA2->LISR & DMA_Stream2_transfer_error){
        DMA2->LIFCR |= DMA_Stream2_transfer_error;
#ifdef BAD_DMA_DMA2_STREAM2_USE_TE
        dma2_stream2_te(DMA2->streams[2].NDTR);
#endif
    }

    if(DMA2->LISR & DMA_Stream2_transfer_complete){
        
        DMA2->LIFCR|= DMA_Stream2_transfer_complete;
#ifdef BAD_DMA_DMA2_STREAM2_USE_TC
        dma2_stream2_tc(DMA2->streams[2].NDTR);
#endif
    }

    if(DMA2->LISR & DMA_Stream2_half_transfer){
        DMA2->LIFCR |= DMA_Stream2_half_transfer;
#ifdef BAD_DMA_DMA2_STREAM2_USE_HT
        dma2_stream2_ht(DMA2->streams[2].NDTR);
#endif
    }
}
#endif
//

//EXTI interrupts
#ifdef BAD_EXTI_EXTI0_ISR_IMPLEMENTATION

void exti0_usr();

#define EXTI_PR_EXTI0 (0x1)

STRONG_ISR(exti0_isr){
    EXTI->PR = EXTI_PR_EXTI0;
    exti0_usr();
}

#endif

#ifdef BAD_EXTI_EXTI1_ISR_IMPLEMENTATION

void exti1_usr();

#define EXTI_PR_EXTI1 (0x2)

STRONG_ISR(exti1_isr){
    EXTI->PR = EXTI_PR_EXTI1;
    exti1_usr();
}

#endif

#ifdef BAD_EXTI_EXTI2_ISR_IMPLEMENTATION

void exti2_usr();

#define EXTI_PR_EXTI2 (0x4)

STRONG_ISR(exti2_isr){
    EXTI->PR = EXTI_PR_EXTI2;
    exti2_usr();
}

#endif

#ifdef BAD_EXTI_EXTI3_ISR_IMPLEMENTATION

void exti3_usr();

#define EXTI_PR_EXTI3 (0x8)

STRONG_ISR(exti3_isr){
    EXTI->PR = EXTI_PR_EXTI3;
    exti3_usr();
}

#endif

#ifdef BAD_EXTI_EXTI4_ISR_IMPLEMENTATION

void exti4_usr();

#define EXTI_PR_EXTI4 (0x10)

STRONG_ISR(exti4_isr){
    EXTI->PR = EXTI_PR_EXTI4;
    exti4_usr();
}

#endif
#ifdef BAD_EXTI_EXTI9_5_ISR_IMPLEMENTATION

#ifdef BAD_EXTI_USE_EXTI_5_USER_ISR
void exti5_usr();
#endif
#ifdef BAD_EXTI_USE_EXTI_6_USER_ISR
void exti6_usr();
#endif
#ifdef BAD_EXTI_USE_EXTI_7_USER_ISR
void exti7_usr();
#endif
#ifdef BAD_EXTI_USE_EXTI_8_USER_ISR
void exti8_usr();
#endif
#ifdef BAD_EXTI_USE_EXTI_9_USER_ISR
void exti9_usr();
#endif

#define EXTI_PR_EXTI9_5_mask (0x3E0)

STRONG_ISR(exti9_5_isr){
    enum EXTI9_5_masks{
        EXTI_PR_EXTI5 = 0x20,
        EXTI_PR_EXTI6 = 0x40,
        EXTI_PR_EXTI7 = 0x80,
        EXTI_PR_EXTI8 = 0x100,
        EXTI_PR_EXTI9 = 0x200
    };

    uint32_t pending = EXTI->PR & (EXTI_PR_EXTI9_5_mask);

    if(pending & EXTI_PR_EXTI5){
        EXTI->PR = EXTI_PR_EXTI5;
#ifdef BAD_EXTI_USE_EXTI_5_USER_ISR        
        exti5_usr();
#endif
    }

    if(pending & EXTI_PR_EXTI6){
        EXTI->PR = EXTI_PR_EXTI6;
#ifdef BAD_EXTI_USE_EXTI_6_USER_ISR
        exti6_usr();
#endif
    }

    if(pending & EXTI_PR_EXTI7){
        EXTI->PR = EXTI_PR_EXTI7;
#ifdef BAD_EXTI_USE_EXTI_7_USER_ISR
        exti7_usr();
#endif
    }

    if(pending & EXTI_PR_EXTI8){
        EXTI->PR = EXTI_PR_EXTI8;
#ifdef BAD_EXTI_USE_EXTI_8_USER_ISR
        exti8_usr();
#endif
    }

    if(pending & EXTI_PR_EXTI9){
        EXTI->PR = EXTI_PR_EXTI9;
#ifdef BAD_EXTI_USE_EXTI_9_USER_ISR
        exti9_usr();
#endif
    }
}
#endif
//

//Timer interrupts
#ifdef BTIMER_TIM1_UP_TIM10_ISR_IMPLEMENTATION

#ifdef BTIMER_USE_TIM10_USR
void tim10_usr();
#endif
#define TIM_SR_UIF 0x1
STRONG_ISR(tim1_up_tim10_isr){
    if(BTIM10->SR & TIM_SR_UIF ){
      BTIM10->SR &= ~TIM_SR_UIF;
#ifdef BTIMER_USE_TIM10_USR
      tim10_usr();
#endif
    } 
}

#endif

#endif // !BAD_HAL_H
