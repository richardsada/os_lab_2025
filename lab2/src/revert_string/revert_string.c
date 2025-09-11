#include "revert_string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void RevertString(char *str)
{
    char rev;
    for(int i = 0; i < strlen(str)/2; i++){
		rev = str[i];
		str[i] = str[strlen(str) - i - 1];
		str[strlen(str) - i - 1] = rev;
	}
}
