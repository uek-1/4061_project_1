#include "include/utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_status(){

     //TODO: write the status of each executable file to autograder.out. Your output should align with expected.out 
}


int main(int argc, char *argv[]) {


    if (argc < 2) {
        printf("Usage: %s <batch> <p1> <p2> ... <pn>\n", argv[0]);
        return 1;
    }

    int total_inputs = argc - 2;
    char* inputs[total_inputs];

    for (int i = 2; i < argc; i++) {
        inputs[i-2] = argv[i];
    }

    // Convert the first command-line argument to an integer to determine the batch size
    int batch_size = atoi(argv[1]);

    int solution_count = 0;

    // write the file paths from the "solutions" directory into the submissions.txt file
    write_filepath_to_submissions("solutions", "submissions.txt");
    // Open or create the output file
    FILE *file;
    int MAX_LINE_LENGTH = 256;
    char line[MAX_LINE_LENGTH];

    file = fopen("submissions.txt", "r");
    if (!file) {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }
    int i = 0;
    while (fgets(line, sizeof(line), file)){          
        solution_count += 1;
    }
    char solution_names[solution_count][MAX_LINE_LENGTH];
    rewind(file);
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        strncpy(solution_names[i], line, MAX_LINE_LENGTH - 1);
        solution_names[i][MAX_LINE_LENGTH - 1] = '\0';  
        i++;
    }


    //TODO: read the executable filename from submissions.txt

    int total_batches = solution_count / batch_size;
    if (solution_count % batch_size != 0) {
        total_batches += 1;
    }
    char* batches[total_batches][batch_size];

    int temp_batch_count = 0;
    for (int i = 0; i < solution_count; i++) {
        if (i != 0 && i % batch_size == 0) {
            temp_batch_count += 1;
        }
        batches[temp_batch_count][i % batch_size] = solution_names[i];
    }


    for (int input_number = 0; input_number < total_inputs; input_number++) {
        for (int batch_number = 0; batch_number < total_batches; batch_number++) {
            printf(" -- BATCH %d -- \n", batch_number);
            int real_batch_size = batch_size;
            if (batch_number == total_batches - 1 && solution_count % batch_size != 0) {
                real_batch_size = solution_count % batch_size;
            }
           for (int solution_idx = 0; solution_idx < real_batch_size; solution_idx++) {
                if (fork() == 0) {
                    printf("%d BATCH (%d): %s %s \n", getpid(), batch_number, batches[batch_number][solution_idx], inputs[input_number]);

                    execl(batches[batch_number][solution_idx], batches[batch_number][solution_idx], inputs[input_number],NULL);
                    return -1;
                }

                // int status;
                // waitpid(-1, &status, 0);
                // if (WIFEXITED(status) || WIFSIGNALED((status)) || WIFSTOPPED(status)) {
                //     printf("EXITED\n");
                // }
                
            } 
        } 
    }


    //TODO: For each parameter, run all executables in batch size chunks

    
    //TODO: Write the status of each executable file from "submissions.txt" to autograder.out. For testing purposes, you can compare the results with the provided expected.out file
    print_status();    


    return 0;
}     
