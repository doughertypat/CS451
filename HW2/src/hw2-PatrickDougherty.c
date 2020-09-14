/************************************************
 * CS451 - HW2
 * Patrick Dougherty
 * patrick.r.dougherty@und.edu
 * 11Sep20
 * *********************************************/

#include <sys/types.h>
#include <unistd.h>

int main()
{
   pid_t pid = fork();
   if(pid==0)
   {
       char *args[]={"./a.out",NULL};
       execv(args[0], args);
   }
   system("xclock &");
   return 0;
}
