#include <lib.h>
#include <minix/callnr.h>
#include <minix/com.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


int getpidmch(void) {
  message m;
  int result;

  result = _syscall(MM, GETPIDMCH, &m);
  printf("Process with PID %d has the most children.\n", m.m1_i1);
  printf("Number of children: %d\n", m.m1_i2);
}

void test(int n_of_subpros)
{
  pid_t pid = 1;
  int i, j;
  
  for (i=0; (i < n_of_subpros) && pid; ++i) pid = fork();
 
 
  if (pid) {
  	getpidmch(); 
  	for (i = 0; i < n_of_subpros; ++i) wait(NULL);
  }
  else {
  	sleep(1);
  	_exit(0);

  }
}


int main(int argc, char **argv) {
  int n_of_subpros, i;
  pid_t res;

  if (argc != 2) return 1;
  n_of_subpros = atoi(argv[1]);

  test(n_of_subpros);
 

}
