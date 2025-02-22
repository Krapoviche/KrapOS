#ifndef __SHELL_H__
#define __SHELL_H__

#define MAX_COMMAND_LENGTH 150
#define MAX_COMMANDS 25

#include "stdbool.h"
#include "libc.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "commands.h"
#include "test.h"

int shell(bool sp);
void prompt(void);

extern bool show_prompt;

#endif