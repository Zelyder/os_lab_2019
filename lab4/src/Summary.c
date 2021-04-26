#include <stdio.h>
#include <stdlib.h>
#include "Summary.h"

int Summary(int* array,int begin,int end) {
  int sum = 0;
  int i;
  printf("summary open\n");
  for(i=begin; i<=end;i++)
  {
      
	sum+=array[i];
        
  }
  return sum;
}