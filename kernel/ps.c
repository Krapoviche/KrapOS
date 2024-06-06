#include "ps.h"
#include "cpu.h"

void ps(){
    char* state[9] = {"RUNNING", "RUNNABLE", "DYING", "LOCKED_MESS", "LOCKED_SEM", "LOCKED_IO", "LOCKED_CHILD", "SLEEPING", "ZOMBIE"};

    // Header
    printf("\t\t\t\tPROCESS STATUS\n\n");
    printf("\t\t\tPID\tNAME\tSTATUS\n");
    for(int i = 0 ; i < NBPROC ; i++){
        process_t* process = get_process(i);
        if(process){
            print_ps_process(process, state[process->state]);
        }
    }
}

void permanent_ps(){
	while(1){
        cli();
		ps();
        sti();
		sleep(1);
		reset_screen();
		place_cursor(0, 0);
	}
}

void print_ps_process(process_t* process, char* state){
    printf("\t\t\t%d\t\t%s\t\t%s\n",process->pid,process->name,state);
}