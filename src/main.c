#define BAD_PLLM (25)
#define BAD_PLLN (400)
#define BAD_PLLQ (10)
#define BAD_PLLP (PLLP4)

#define BAD_AHB_PRE     (HPRE_DIV_1)
#define BAD_APB1_PRE    (PPRE_DIV_2)
#define BAD_APB2_PRE    (PPRE_DIV_1)

#define BAD_RCC_IMPLEMENTATION
#define BAD_GPIO_IMPLEMENTATION
#define BAD_TIMER_IMPLEMENTATION
#define BAD_USART_IMPLEMENTATION
#define BAD_FLASH_IMPLEMENTATION
#define BAD_SYSTICK_IMPLEMETATION

#define BAD_HARDFAULT_IMPLEMENTATION
#define BAD_HARDFAULT_USE_UART
#include "badhal.h"

#define BAD_STRMAP_IMPLEMENTATION
#include "badstrmap.h"

#define UART_GPIO_PORT          (GPIOA)
#define UART1_TX_PIN            (9)
#define UART1_RX_PIN            (10)
#define UART1_TX_AF             (7)
#define UART1_RX_AF             (7)
    // HSE  = 25
    // PLLM = 25
    // PLLN = 400
    // PLLQ = 10
    // PLLP = 4
    // Sysclock = 100
#define BADHAL_PLLM (25)
#define BADHAL_PLLN (400)
#define BADHAL_PLLQ (10)
#define BADHAL_FLASH_LATENCY (FLASH_LATENCY_3ws)

#define BAD_UART_TEST_AHB1_PERIPEHRALS      (RCC_AHB1_GPIOA|RCC_AHB1_CRCEN)
#define BAD_UART_TEST_APB2_PERIPHERALS      (RCC_APB2_USART1)
#define BAD_UART_TEST_SETTINGS              (USART_FEATURE_TRANSMIT_EN|USART_FEATURE_RECIEVE_EN)

static inline void __main_clock_setup(){
    flash_acceleration_setup(BADHAL_FLASH_LATENCY, FLASH_DCACHE_ENABLE, FLASH_ICACHE_ENABLE);
    rcc_sysclock_setup();
}

static inline void __periph_setup(){
    rcc_set_ahb1_clocking(BAD_UART_TEST_AHB1_PERIPEHRALS);
    io_setup_pin(UART_GPIO_PORT, UART1_TX_PIN, MODER_af, UART1_TX_AF, OSPEEDR_high_speed, PUPDR_no_pull, OTYPR_push_pull);
    io_setup_pin(UART_GPIO_PORT, UART1_RX_PIN, MODER_af, UART1_RX_AF, OSPEEDR_high_speed, PUPDR_no_pull, OTYPR_push_pull);
    //Enable UART clocking
    rcc_set_apb2_clocking(BAD_UART_TEST_APB2_PERIPHERALS);
}

static inline void __uart_setup(){
    uart_setup(USART1, USART_BRR_115200, BAD_UART_TEST_SETTINGS, 0, 0);
    uart_enable(USART1);
}

static inline void bad_str_map_reset_crc(){
    crc_reset();
}

static inline uint32_t bad_str_map_get_crc(){
    return CRC->DR;
}

static inline void bad_str_map_update_crc(uint32_t val){
    CRC->DR = val;
}

int main(){
    __DISABLE_INTERUPTS;
    __main_clock_setup();
    __periph_setup();
    __uart_setup();   
   
    
    __ENABLE_INTERUPTS;
    char __attribute__((aligned(4))) *key ="Hello Hashmap";
    BAD_STR_MAP_CREATE_STATIC(map, 64, 256);
    bad_str_map_add_cstr(&map,key , (void *)123 );
    void *res = bad_str_map_lookup_cstr(&map, key);
    
    while(1){
        
    }
    return 0;
}
