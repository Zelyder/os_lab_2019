#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define COMMAND "./sequential_min_max"

int main(int argc, char *argv[])
{
    int pid = fork(); // Создаем дочерний процесс
    if (pid == 0) // Если мы в дочернем
    {
        execv("./" COMMAND, argv); // Запускаем sequential
    }
    return 0;
}