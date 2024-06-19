#include "shell.h"

bool show_prompt;

int shell(bool sp){
    void* tests[22] = {test1, test2, test3, test4, test5,
                       test6, test7, test8, test9, test10,
                       test11, test12, test13, test14, test15,
                       test16, test17, test18, test19, test20, test21, test22};

    show_prompt = sp;

    char command_line[MAX_COMMAND_LENGTH];
    // int command_size;
    char * chunk;
    cons_echo(1);
    
    while(1){
        if(sp) prompt();

        // Read next command
        command_line[cons_read(command_line, MAX_COMMAND_LENGTH)] = '\0';

        // Read each chunk
        chunk = strtok(command_line," ");

        // Deal with the command chunk
        if(strcmp(chunk, "echo") == 0){
            echo(command_line + 5);
            printf("\n");
        } else if (strcmp(chunk, "ps") == 0){
            ps();
        } else if (strcmp(chunk, "dump_stack") == 0){
            dump_stack();
        } else if (strcmp(chunk, "test") == 0){
            char ** end = NULL;
            chunk = strtok(NULL," "); 
            if (chunk == NULL){
                for(int i = 1 ; i <= 22 ; i++){
                    printf("Running test %d\n", i);
                    int pid = start(tests[i - 1], 8096, 128, "test", 0);
                    waitpid(pid, 0);
                    printf("\n");
                }
            } else {
                unsigned long numtest = strtol(chunk, end, 10);
                if((void*)chunk != (void*)end && numtest > 0 && numtest <= 22){
                    printf("Running test %ld\n", numtest);
                    int pid = start(tests[numtest - 1], 8096, 128, "test", 0);
                    waitpid(pid, 0);
                    printf("\n");
                } else {
                    printf("Wrong test number : %s\n", chunk);
                }
            }
        }
        else {
            printf("ERROR : No command %s\n", chunk);
        }
    }
}

void prompt(){
    printf("shell_1> ");
}