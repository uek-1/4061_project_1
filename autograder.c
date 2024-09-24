#include "include/utility.h"
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

    for (int i = 3; i < argc; i++) {
        inputs[i - 3] = argv[i];
    }

    // Convert the first command-line argument to an integer to determine the batch size
    int batch_size = atoi(argv[1]);


    // write the file paths from the "solutions" directory into the submissions.txt file
    write_filepath_to_submissions("solutions", "submissions.txt");


    //TODO: read the executable filename from submissions.txt
    
    FILE* submissions_file = fopen("submissions.txt", "r");
    if (submissions_file == NULL) {
        perror("Failed to open submission file");
        exit(EXIT_FAILURE);
    }

    int solution_count = 0;
    char* solution_names[0];
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
           for (int solution_idx = 0; solution_idx < batch_size; solution_idx++) {
                if (fork() == 0) {
                    printf("%d : %s %s", getpid(), batches[batch_number][solution_idx], inputs[input_number]);
                    execv(batches[batch_number][solution_idx], &inputs[input_number]);
                }
            } 
           for (int solution_idx = 0; solution_idx < batch_size; solution_idx++) {
                int status;
                waitpid(-1, &status, WNOHANG);
            } 
        } 
    }


    //TODO: For each parameter, run all executables in batch size chunks

    
    //TODO: Write the status of each executable file from "submissions.txt" to autograder.out. For testing purposes, you can compare the results with the provided expected.out file
    print_status();    


    return 0;
}     

    
