/*
 * ld directives the for kernel
 */

FORCE_COMMON_ALLOCATION

SECTIONS {
	/* The kernel starts at 1M */
	. = 0x100000;

	_start = .;

	.text ALIGN(0x1000) : {
        */crt0.o(.text)
		*(.text)
		_etext = .;
	}

    .rodata ALIGN(0x1000) : {
        _rodata_start = .;
        *(.rodata)
        *(.rodata.str1.1)
        *(.rodata.str1.4)
        *(.rodata.str1.32)
        *(.rodata.cst8)
        _rodata_end = ALIGN(0x1000);
    }

	.data ALIGN(0x1000) : {
		__CTOR_LIST__ = .;
		LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
		*(.ctors)
		LONG(0)
		__CTOR_END__ = .;
		__DTOR_LIST__ = .;
		LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
		*(.dtors)
		LONG(0)
		__DTOR_END__ = .;

		*(.fini)
		*(.anno)
		*(__ex_table)
		*(.data)

		/* The user program is wrapped in as data */
		usercode_start = .;
		*(.user)
		usercode_end = .;

		task_dump_screen = .;
		*(.task_dump_screen)

		_edata  =  .;
	}

	.bss ALIGN(0x1000) : {
		__bss_start = .;
		*(.bss)
		*(COMMON)
		__bss_end = .;
	}

	_end = .;

	/DISCARD/ : {
		*(.comment)
	}

	/* The memory map */
	idt = 0x1000;
	gdt = 0x10000;
	tss = 0x20000;

	/* Beginning of kernel memory heap */
	mem_heap = _end;

	/* End of kernel memory heap */
	mem_heap_end = 0x1000000;

	/* Beginning of user space. User program is loaded here by the kernel */
	user_start = mem_heap_end;
	
	/* Heap for the kernel to allocate user stacks */
	user_stack_heap = 0x2800000;
	
	/* End of user space */
	user_end = 0x3000000;
}
