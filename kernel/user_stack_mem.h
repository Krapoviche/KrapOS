/*
 * Copyright (C) 2004 Simon Nieuviarts
 *
 * Memory allocator in user space for user stacks. Used by the kernel.
 */
#ifndef __USER_STACK_MEM_H__
#define __USER_STACK_MEM_H__

void *user_stack_alloc(unsigned long length);
void user_stack_free(void *zone, unsigned long length);

#endif
