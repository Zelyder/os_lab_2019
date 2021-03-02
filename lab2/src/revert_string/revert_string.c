#include "revert_string.h"


void RevertString(char *str)
{
  int i, size = strlen(str);
  for (i = 0; i < size/2;i++){
      char temp = str[i];
      str[i] = str[size-i-1];
      str[size-i-1] = temp;
  }
  
}

