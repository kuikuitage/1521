/*****************************************************************************/
/* Copyright (c) 2008 Montage Tech - All Rights Reserved                     */
/*****************************************************************************/
/*!
  \file link_mips.ld

  This file is linker scriptor which links all objects after compiling. All sections are defined here.
  */


OUTPUT_FORMAT("elf32-littlemips", "elf32-tradbigmips","elf32-littlemips")
OUTPUT_ARCH(mips)

/**** Start point ****/
EXTERN(_start)
ENTRY(_start)			/* Entry point of application		*/

MEMORY
{
   ram (rwx) : ORIGIN = 0x00000000, LENGTH = 2M
   rom (rwx) : ORIGIN = 0x08000000, LENGTH = 512K
}

__RAM_BASE = 0x00180000;
__ROM_BASE = 0x1800040C; /*reserve 0x40C Bytes for bootloader header*/


SECTIONS
{
   /**** boot up code ****/
     .boot __ROM_BASE : AT(__ROM_BASE) {
      *(.init_boot)
      *(.init_ram)
      . = ALIGN(8);
   }
   /**** Code and read-only data ****/
	__TEXT_RAM_ADDR = __RAM_BASE;
	__TEXT_ROM_ADDR = __ROM_BASE + SIZEOF(.boot);
	.text __TEXT_RAM_ADDR : AT(__TEXT_ROM_ADDR) {
   	 _ftext = ABSOLUTE(.) ;	/* Start of code and read-only data	*/
   	. = . + 36;
    *(.interrupt)
    *(.blentry)
    *(.cache)
    *(.text .text.*)
    *(.gnu.linkonce.t*)
    *(.mips16.fn.*)
    *(.mips16.call.*)
	_ecode = ABSOLUTE(.) ;	/* End of code				*/

	*(.rodata*)
	*(.rel.dyn)

	. = ALIGN(8);
	_etext = ABSOLUTE(.);	/* End of code and read-only data	*/
  } > ram

  /**** Initialised data ****/
   __DATA_RAM_ADDR = __TEXT_RAM_ADDR + SIZEOF(.text);
   __DATA_ROM_ADDR = __TEXT_ROM_ADDR + SIZEOF(.text);
   
  .data __DATA_RAM_ADDR : AT(__DATA_ROM_ADDR)  {
    /* *(.data) */   

	*(.got.plt) *(.got)
	*(.data .data.* .gnu.linkonce.d.*)
	*(.rel.dyn)
	
    . = ALIGN(8);
    _gp = ABSOLUTE(. + 0x7ff0); /* Base of small data			*/


    _fdata = ABSOLUTE(.);	/* Start of initialised data		*/
	
    *(.lit8) 
    *(.lit4) 
    *(.sdata) 
    *(.sdata .sdata.* .gnu.linkonce.s.*)

    . = ALIGN(8);

    _edata  = ABSOLUTE(.);	/* End of initialised data		*/
  } > ram

  /**** Uninitialised data ****/
   __SBSS_RAM_ADDR = __DATA_RAM_ADDR + SIZEOF(.data);

  _fbss = .;			/* Start of uninitialised data		*/

  .sbss __SBSS_RAM_ADDR : AT(__SBSS_RAM_ADDR) { 
    *(.sbss .sbss.* .gnu.linkonce.sb.*) 
    *(.scommon)
  } > ram
  
  __BSS_RAM_ADDR = __SBSS_RAM_ADDR + SIZEOF(.sbss);
  .bss __BSS_RAM_ADDR : AT(__BSS_RAM_ADDR) {
    *(.bss .bss.* .gnu.linkonce.b.*)
    *(COMMON)

    /* Allocate room for stack */
    .   =  ALIGN(8) ;
    _freemem = .;
  } > ram
  
  _fbss_end = .;
  _end = . ;			/* End of unitialised data		*/

  .gptab.sdata : { *(.gptab.data) *(.gptab.sdata) }
  .gptab.sbss : { *(.gptab.bss) *(.gptab.sbss) }

  /DISCARD/ :
  {
    *(.reginfo) *(.pdr)  *(.comment) *(.note.GNU-stack)
  }
  
   .reginfo 0 : { *(.reginfo) }
   .mdebug 0 : { *(.mdebug) }
   
  PROVIDE(etext = _etext);
  PROVIDE (edata = .);
  PROVIDE (end = .);
}



