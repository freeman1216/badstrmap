MEMORY
{
    ROM (rx) : ORIGIN = 0x8000000, LENGTH  = 512k
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH  = 128k
}
__eram = ORIGIN(RAM) + LENGTH(RAM);
__estack = __eram;

SECTIONS
{
    .text : ALIGN(4)
    {   __ivt_rom_start = .;
        KEEP(*(.ivt))
        __ivt_rom_end = .;
        *(.text)
        . = ALIGN(4);
        __etext = .;
    } > ROM

    __rdata = LOADADDR(.data); /* data placed in rom */

    
    .rivt : ALIGN(4)
    {
        __ram_ivt = .;
        . = . +( __ivt_rom_end - __ivt_rom_start); 
        . = ALIGN(4);
        __eram_ivt = .;
    } > RAM

    .data : ALIGN(4)
    {   
        __data = .;
        *(.data)
        . = ALIGN(4);
        __edata = .;
    } > RAM AT > ROM
    
    .bss : ALIGN(4)
    {
        __bss = .;
        *(.bss)
        . = ALIGN(4);
        __ebss = .;
    } > RAM 
    

    __rramfunc = LOADADDR(.ramfunc); /* data placed in rom */

    .ramfunc :ALIGN(4)
    {   
        __ramfunc = .;
        *(.ramfunc)
        *(.text.memmove)
        . = ALIGN(4);
        __eramfunc = .;
        
    } >RAM AT > ROM

    .init_array :ALIGN(4)
    {  
        __init_array = .;
       *(.init_array)
       . = ALIGN(4);
        __einit_array = .;
    } > ROM

    .rodata : ALIGN(4)
    {
        *(.rodata)
        . = ALIGN(4);
        __erodata = .;
    } > ROM
}
