/*
 * Copyright (C) 2005 Simon Nieuviarts
 *
 * Memory allocator in user space for user stacks. Used by the kernel.
	 */
#include "user_stack_mem.h"

#define mem_heap		user_stack_heap
#define mem_heap_end		user_end
#define mem_bug			user_stack_bug
#define mem_alloc		user_stack_alloc
#define mem_free		user_stack_free
#define mem_free_nolength	user_stack_free_nolength
#define sbrk			us_sbrk

#define USE_THIS_CUSTOM_PREFIX kus
#include "malloc.c.h"
