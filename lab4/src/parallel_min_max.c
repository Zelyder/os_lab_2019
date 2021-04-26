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

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

pid_t* PID_arr; 
int pnum;
void child_killing(int sig)
{
  int i;
  for (i = 0; i < pnum; i++)
  {
    if (kill(PID_arr[i], SIGKILL) == 0)
    {
      printf("child process was killed\n"); 
    }
  }
}


int main(int argc, char **argv) {
  int seed = -1; // Число на основе которого генерируется массив 
  int array_size = -1;
  int timeout=-1;
  int pnum = -1;
  bool with_files = false;
  while (true) 
  {
    int current_optind = optind ? optind : 1;
    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"timeout",required_argument,0,0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};
    
    int option_index = 0;
    // -f - опция
    int c = getopt_long(argc, argv, "f", options, &option_index);
    if (c == -1) break;
    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            if(optarg<=0)
            {
                printf("seed  must be positive number\n");
                return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            if(optarg<=0)
            {
                printf("array_size  must be positive number\n");
                return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            if(optarg<=0)
            {
                printf("pnum  must be positive number\n");
                return 1;
            }
            break;
          case 3:
            timeout = atof(optarg);
            if (optarg <= 0) 
            {
              return 1;
            }
            break;
          case 4:
            with_files = true;
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }
  //Проверка на количество введеных аргументов которые не являются опцией
  if (optind < argc)
  {
    printf("Has at least one no option argument\n");
    return 1;
  }
  // Проверка количества введенных аргументов 
  if (seed == -1 || array_size == -1 || pnum == -1) 
  {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1; 
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  // Создаем переменную timeval и загоняем время нынешнее 
  struct timeval start_time;
  gettimeofday(&start_time, NULL);
  int n = array_size / pnum; // разбиение на отрезки
  int pipefd[2];
  pipe(pipefd); // создает pipe, однонаправленный канал данных, который можно использовать для межпроцессного взаимодействия.
  /* Данные, записанные в конец канала для записи, 
  буферизуются ядром до тех пор, пока не будут прочитаны из 
  конца канала для чтения  */
  PID_arr = malloc(sizeof(pid_t)*pnum);

  for (int i = 0; i < pnum; i++) {
    /*Процессы создаются системным вызовом fork (так что операция создания нового процесса иногда вызывает раздваивание процесса).
     Дочерний процесс, созданный fork - точный аналог первоначального родительского процесса, 
     за исключением того, что он имеет собственный ID. */
    pid_t child_pid = fork(); // Pid_t тип данных для ID процесса.
    /*Если операция является успешной, то и родительский и дочерний процессы видят что fork возвращается,
    но с различными значениями: она возвращает значение 0 в дочернем процессе
    и ID порожденного процесса (ребенка) в родительском процессе.
    Если создание процесса потерпело неудачу, fork возвращает значение -1 в родительском процессе. */
    if (child_pid >= 0) {
        PID_arr[i] = child_pid;
        active_child_processes += 1;
      if (child_pid == 0) {
        // child process
        // Получили min_max
        struct MinMax min_max;
        min_max = GetMinMax(array, (unsigned int)i * n, (unsigned int)(i + 1) * n);
        // parallel somehow
        // Запись в файл
        if (with_files)
        {
            FILE *fp;
            if (i == 0)
            {
            fp = fopen("output.txt", "w"); // записывает в первом цикле 
            }
            else
            {
            fp = fopen("output.txt", "a"); // добавляет в старый тхт
            }
          fprintf(fp, "%d %d\n", min_max.min, min_max.max);
          fclose(fp);
        } 
        else 
        {
            //0 - чтение 1 - запись
            // запись в пайп
            write(pipefd[1], &min_max, sizeof(struct MinMax));
          // use pipe here
        }
        return 0;
      }
    } 
    else 
    {
      printf("Fork failed!\n");
      return 1;
    }
  }
  if (timeout > 0)
  {
    alarm(timeout); 
    signal(SIGALRM, child_killing); 
    sleep(1);
  }

  while (active_child_processes > 0) {
    // your code here
    wait(NULL); // Ждем пока все доработают
    active_child_processes -= 1;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
        // из файла берем и сравниаваем
      struct MinMax min_max;
      FILE *fp;
      int j;
      fp = fopen("output.txt", "r");
      for (j = 0; j < i || (i == 0 && j <= i); j++)
      {
        fscanf(fp, "%d %d", &min_max.min, &min_max.max);
        if (min < min_max.min)
          min_max.min = min;
        if (max > min_max.max)
          min_max.max = max;
      }
      fclose(fp);
      min = min_max.min;
      max = min_max.max;
    } else {
      struct MinMax min_max;
      // читаем из pipe
      read(pipefd[0], &min_max, sizeof(struct MinMax));
      min = min_max.min;
      max = min_max.max;
      // read from pipes
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;
  free(array);
  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}