ENTRY(_start)

SECTIONS {
  . = _pmem_start + 0x100000; /*the section start from 0x100000*/
  .text : {
    *(entry) /*first is entry section defined in start.S,in start.S:set _stack_pointer */
    *(.text*)/*then is some section of other .o,for example ,dummy.o and trm.o */
  }
  etext = .;
  _etext = .;
  .rodata : {
    *(.rodata*)
  }
  .data : {  /*declare the memory district for storing the data */
    *(.data)
  }
  edata = .;
  _data = .;
  .bss : {   /*the buffer area*/
	_bss_start = .;
    *(.bss*)
    *(.sbss*)
    *(.scommon)
  }
  _stack_top = ALIGN(0x1000);
  . = _stack_top + 0x8000;
  _stack_pointer = .;
  end = .;
  _end = .;
  _heap_start = ALIGN(0x1000);
}
