#include "ps.h"

void ps(){
    // Header
    printf("\t\t\t\tPROCESS STATUS\n");

    // Process running
    print_ps_process(process_table->running, "RUNNING");

    // Processes runnable
    ps_queue(process_table->runnable_queue, "RUNNABLE");

    // Processes sleeping
    ps_queue(process_table->sleeping_queue, "SLEEPING");

    // Processes dying
    ps_queue(process_table->dead_queue, "DYING");
    
    // Processes zombies
    ps_queue(process_table->zombie_queue, "ZOMBIE");
}

void permanent_ps(){
	while(1){
		ps();
		sleep(1);
		reset_screen();
		place_cursor(0, 0);
	}
}

void ps_queue(link* head, char* state){
    printf("%s\n",state);
    process_t* process;
    queue_for_each(process, head, process_t, queue_link){
        print_ps_process(process, state);
    }
}

void print_ps_process(process_t* process, char* state){
    printf("%d\t%s\t%s\n",process->pid,process->name,state);
}