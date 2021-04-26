#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <pthread.h>
#include "utils.h"
#include "Summary.h"

struct SumArgs {
  int *array;
  int begin;
  int end;
};

void *tsum(void *args) {
  struct SumArgs *ptr = (struct SumArgs *)args;
  return (void *)(size_t)Summary(ptr->array,ptr->begin,ptr->end);
}

int main(int argc, char **argv) {
  /*
   *  TODO:
   *  threads_num by command line arguments
   *  array_size by command line arguments
   *	seed by command line arguments
   */

  uint32_t threads_num = 0;
  uint32_t array_size = 0;
  uint32_t seed = 0;
  

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"threads_num", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;
    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
       
		if(seed<=0){
		   printf("seed can't be negative \n");		
		   return -1;
		}
            // error handling
            break;
          case 1:
            array_size = atoi(optarg);
     
		if(array_size<=0){
		   printf("size can't be negative \n");		
		   return -1;
		}
            // error handling
            break;
          case 2:
           threads_num = atoi(optarg);
           
		   if(threads_num<=0){
		   printf("amount of threads can't be negative\n");		
		   return -1;
		}
            break;

          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }
	

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || threads_num == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }
  
  if (threads_num > array_size) {
  printf("The number of threads cant be greater than size of array \n");
   return 1; 
}

  /*
   * TODO:
   * your code here
   * Generate array here
   */
   pthread_t threads[threads_num];

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int size_thread= threads_num<=array_size ? (array_size/threads_num) : 1;
  struct SumArgs args[threads_num];
  struct timeval s_time;
  gettimeofday(&s_time, NULL);
  
  for (uint32_t i = 0; i < threads_num; i++) {

    args[i].begin=size_thread*i;
    if(i==threads_num-1) 
    {
        args[i].end=array_size-1;
    }

    else  args[i].end=args[i].begin+size_thread-1;
    args[i].array=array;

    if (pthread_create(&threads[i], NULL, tsum, (void *)&args[i])) 
    {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  int total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    int sum = 0;
    pthread_join(threads[i], (void **)&sum);
    total_sum += sum;
  }

  struct timeval e_time;
  gettimeofday(&e_time, NULL);
  double elapsed_time = (e_time.tv_sec - s_time.tv_sec) * 1000.0;
  elapsed_time += (e_time.tv_usec - s_time.tv_usec) / 1000.0;

  free(array);
  printf("Total: %d\n", total_sum);
  printf("Elapsed time: %fms\n", elapsed_time);
  //fflush(NULL);
  return 0;
}