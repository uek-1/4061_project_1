#include "include/utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const int MAX_LINE_LENGTH = 256;

void print_status(int solution_count, int input_count,
                  int results[solution_count][input_count],
                  char names[solution_count][MAX_LINE_LENGTH],
                  char *inputs[input_count]) {
  for (int i = 0; i < solution_count; i++) {
    printf("%s", names[i]);
    for (int j = 0; j < input_count; j++) {
      if (results[i][j] == 1) {
        printf(" %s(correct)", inputs[j]);
      } else if (results[i][j] == 0) {
        printf(" %s(incorrect)", inputs[j]);
      } else if (results[i][j] == -1) {
        printf(" %s(crashed)", inputs[j]);
      }
    }
    printf("\n");
  }

  // TODO: write the status of each executable file to autograder.out. Your
  // output should align with expected.out
}

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("Usage: %s <batch> <p1> <p2> ... <pn>\n", argv[0]);
    return 1;
  }

  int total_inputs = argc - 2;
  char *inputs[total_inputs];

  for (int i = 2; i < argc; i++) {
    inputs[i - 2] = argv[i];
  }

  // Convert the first command-line argument to an integer to determine the
  // batch size
  int batch_size = atoi(argv[1]);

  int solution_count = 0;

  // write the file paths from the "solutions" directory into the
  // submissions.txt file
  write_filepath_to_submissions("solutions", "submissions.txt");
  // Open or create the output file
  FILE *submissions_file;
  char line[MAX_LINE_LENGTH];

  submissions_file = fopen("submissions.txt", "r");
  if (!submissions_file) {
    perror("Failed to open output file");
    exit(EXIT_FAILURE);
  }
  int solution_count_temp = 0;
  while (fgets(line, sizeof(line), submissions_file)) {
    solution_count += 1;
  }
  char solution_names[solution_count][MAX_LINE_LENGTH];
  rewind(submissions_file);
  while (fgets(line, sizeof(line), submissions_file)) {
    line[strcspn(line, "\n")] = '\0';
    strncpy(solution_names[solution_count_temp], line, MAX_LINE_LENGTH - 1);
    solution_names[solution_count_temp][MAX_LINE_LENGTH - 1] = '\0';
    solution_count_temp++;
  }

  // TODO: read the executable filename from submissions.txt

  int total_batches = solution_count / batch_size;
  if (solution_count % batch_size != 0) {
    total_batches += 1;
  }
  char *batches[total_batches][batch_size];

  int temp_batch_count = 0;
  for (int i = 0; i < solution_count; i++) {
    if (i != 0 && i % batch_size == 0) {
      temp_batch_count += 1;
    }
    batches[temp_batch_count][i % batch_size] = solution_names[i];
  }

  for (int i = 0; i < total_batches; i++) {
    printf(" -- BATCH %d -- \n", i);

    int real_batch_size = batch_size;
    if (i == total_batches - 1 && solution_count % batch_size != 0) {
      real_batch_size = solution_count % batch_size;
    }
    for (int j = 0; j < real_batch_size; j++) {
      printf("%s \n", batches[i][j]);
    }
  }

  int solution_results[solution_count][total_inputs];

  for (int i = 0; i < solution_count; i++) {
    for (int j = 0; j < solution_count; j++) {
      solution_results[i][j] = 0;
    }
  }

  for (int input_number = 0; input_number < total_inputs; input_number++) {
    for (int batch_number = 0; batch_number < total_batches; batch_number++) {
      printf(" -- BATCH %d -- \n", batch_number);

      // Last batch might have less than batch_size files
      int real_batch_size = batch_size;
      if (batch_number == total_batches - 1 &&
          solution_count % batch_size != 0) {
        real_batch_size = solution_count % batch_size;
      }

      pid_t batch_pids[real_batch_size];
      memset(batch_pids, -1, sizeof(batch_pids[0]) * real_batch_size);

      // Spawn processes for solutions
      for (int solution_idx = 0; solution_idx < real_batch_size;
           solution_idx++) {
        pid_t current_pid = fork();
        if (current_pid == 0) {
          printf("%d BATCH (%d): %s %s \n", getpid(), batch_number,
                 batches[batch_number][solution_idx], inputs[input_number]);

          if (execl(batches[batch_number][solution_idx],
                    batches[batch_number][solution_idx], inputs[input_number],
                    NULL) == -1) {
            printf("FAILURE! %d BATCH (%d) (%d): %s %s \n", getpid(),
                   batch_number, solution_idx,
                   batches[batch_number][solution_idx], inputs[input_number]);
          }
        } else if (current_pid == -1) {
          printf("ERROR FORKING!");
        } else {
          batch_pids[solution_idx] = getpid();
        }
      }

      // Wait on solution processes to exit
      int batch_statuses[real_batch_size];
      int finished_processes = 0;

      while (finished_processes < real_batch_size) {
        // Get all statuses
        for (int i = 0; i < real_batch_size; i++) {
          if (batch_pids[i] != -1) {
            waitpid(batch_pids[i], &batch_statuses[i], WNOHANG);
          }
        }

        // Check statuses to see if all exited
        for (int i = 0; i < real_batch_size; i++) {
          if (batch_pids[i] != -1) {
            if (WIFEXITED(batch_statuses[i])) {
              solution_results[batch_size * batch_number + i][input_number] =
                  WEXITSTATUS(batch_statuses[i]);
              finished_processes += 1;
              batch_pids[i] = -1;
            } else if (WIFSIGNALED(batch_statuses[i])) {
              solution_results[batch_size * batch_number + i][input_number] =
                  -1;
              finished_processes += 1;
              batch_pids[i] = -1;
            } else if (WIFSTOPPED(batch_statuses[i]) &&
                       WSTOPSIG(batch_statuses[i]) == SIGSEGV) {
              solution_results[batch_size * batch_number + i][input_number] =
                  -1;
              finished_processes += 1;
              batch_pids[i] = -1;
            }
          }
        }
        sleep(1);
      }
    }
  }

  print_status(solution_count, total_inputs, solution_results, solution_names,
               inputs);

  return 0;
}
