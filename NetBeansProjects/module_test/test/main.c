
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>

//0:error >0:number
#include<stdio.h>  
__attribute__((constructor)) void before_main()  
{  
   printf("before main/n");  
}  
  
__attribute__((destructor)) void after_main()  
{  
   printf("after main/n");  
}  
  
int main()  
{  
   printf("in main/n");  
   return 0;  
}  