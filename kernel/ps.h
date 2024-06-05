#ifndef __PS_H__
#define __PS_H__

#include "process.h"
#include "stdio.h"
#include "screen.h"

void ps();
void permanent_ps();
void ps_queue(link* head, char* state);
void print_ps_process(process_t* process, char* state);

#endif