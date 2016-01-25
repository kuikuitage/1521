/************************************************************************
* ckcore/Linkcmd.ld
*
* (C) Copyright 2005 to 2007 ImageSilicon Tech.,ltd
*
* The linker ld command script file
* Platform: Target Arch = CKCore, Host Arch = x86 Linux
*
* Create:  Fqh <feng_qinghua@dahuatech.com> 2006-11-20  Version: v0.1
* Modify:                                               Version:
*
*************************************************************************/

ENTRY (_start)

/* Search directory for input files */
SEARCH_DIR(.)

/* No dynamic libraries */
__DYNAMIC  =  0;

/*
 * Setup the memory map of the evaluation board, stack grows down from high memory.
 * The memory map look like this In High down to Low Memory View:
 *
 * +--------------------+
 * |        __bss_end   | start of heap, used by sbrk()
 * | .bss               |
 * |        __bss_start | start of bss, cleared by crt0
 * +--------------------+
 * |        _edata      |
 * | .rodata            |
 * | .data              | initialized data goes here
 * |        _data       |
 * +--------------------+
 * |        _etext      |
 * | .text              |
 * |        _text_start |
 * +--------------------+ <- low memory
 * Please Refer to sysdram_cfg.h: PROGMEM_SIZE/PROGMEM_START to decide LENGTH = ?M
 */
MEMORY
{
  /* ram : ORIGIN = 0, LENGTH = 0x380000 */
    ram : ORIGIN = 0, LENGTH = 8M
}


/*
 * stick everything in ram (of course)
 */
SECTIONS
{
  .text :
  {
    _text_start		= .;
    *(.text)
    *(.text*)              /* -----Ìí¼ÓÕâ¸ö*/
    . = ALIGN(0x4);
    *(.eh_fram)
    . = ALIGN(0x4);
    _etext		= .;
    *(.lit)
  } > ram

  .data BLOCK (0x4) :
  {
    _data = .;
    *(.shdata)
    *(.data)
    *(.rodata)
    *(.rodata.*)
    *(.gcc_except_table) 
    *(.gcc_exc)
    _edata		= .;
  } > ram

  .bss BLOCK (0x4) :
  {
    __bss_start		= . ;
    *(.shbss)
    *(.bss)
    *(COMMON)
    /* __bss_end		=  ALIGN (0x4); */
    __bss_end		=  ALIGN (0x8);    
  } > ram

  .stab 0 (NOLOAD) :
  {
    *(.stab)
  }

  .stabstr 0 (NOLOAD) :
  {
    *(.stabstr)
  }
}
