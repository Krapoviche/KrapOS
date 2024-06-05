#include "ps.h"

void ps(){
    // Header
    printf("\t\t\t\tPROCESS STATUS\n");

    // Process running
    printf("\t\t\t\tRUNNING\n");
    print_ps_process(process_table->running, "RUNNING");

    // Processes runnable
    printf("\t\t\t\tRUNNABLE\n");
    ps_queue(process_table->runnable_queue, "RUNNABLE");

    // Processes sleeping
    printf("\t\t\t\tSLEEPING\n");
    ps_queue(process_table->sleeping_queue, "SlEEPING");

    // Processes dying
    printf("\t\t\t\tDYING\n");
    ps_queue(process_table->dead_queue, "DYING");
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
    process_t* process;
    queue_for_each(process, head, process_t, queue_link){
        print_ps_process(process, state);
    }
}

void print_ps_process(process_t* process, char* state){
    printf("%d\t%s\t%s\n",process->pid,process->name,state);
}