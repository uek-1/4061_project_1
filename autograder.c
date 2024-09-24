#include "include/utility.h"

void print_status(){

     //TODO: write the status of each executable file to autograder.out. Your output should align with expected.out 
}


int main(int argc, char *argv[]) {


    if (argc < 2) {
        printf("Usage: %s <batch> <p1> <p2> ... <pn>\n", argv[0]);
        return 1;
    }

    // Convert the first command-line argument to an integer to determine the batch size
    int batch_size = atoi(argv[1]);


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
        int array_size = 0;
        int i = 0;
        while (fgets(line, sizeof(line), file)){          
            array_size += 1;
        }
        char sub_array[array_size][MAX_LINE_LENGTH];
        rewind(file);
        while (fgets(line, sizeof(line), file)) {
            line[strcspn(line, "\n")] = '\0';
            strncpy(sub_array[i], line, MAX_LINE_LENGTH - 1);
            sub_array[i][MAX_LINE_LENGTH - 1] = '\0';  
            i++;
        }


    //TODO: read the executable filename from submissions.txt



    //TODO: For each parameter, run all executables in batch size chunks

    
    //TODO: Write the status of each executable file from "submissions.txt" to autograder.out. For testing purposes, you can compare the results with the provided expected.out file
    print_status();    


    return 0;
}
