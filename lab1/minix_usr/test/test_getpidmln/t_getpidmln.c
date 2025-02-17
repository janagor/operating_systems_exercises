#include <lib.h>
#include <minix/callnr.h>
#include <minix/com.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


int getpidmln(int excl_pid, int print) {
  message m;
  int result;
  m.m1_i1 = excl_pid;

  result = _syscall(MM, GETPIDMLN, &m);
  if (print) {
  	printf("Process with PID %d has the longest path of descendants.\n", m.m1_i1);
  	printf("Length of the path: %d\n", m.m1_i2);
  }
  return m.m1_i1;
}

void test_hierarchy(int level, int max_level) {
  int pid;
  if (level > max_level) return;

  pid = fork();
  if (!pid) {
  	test_hierarchy(level + 1, max_level);
  	sleep(2);
  	_exit(0);
  }
  wait(NULL);
}

int main(int argc, char*argv[]) {
  int i;
  int max;
  int pid;
  int cpid = getppid();

  if (argc !=2) return -1;
  max = atoi(argv[1]);

  pid = fork();
  if (pid) {
  	for (i = 1; i < max && pid; ++i) {
  		pid = fork();
  		if (!pid) test_hierarchy(1, i);
  	}
  	for (i = 0; i < max; ++i)
	wait(NULL);
  }
  else {
  	sleep(1);
  	getpidmln(cpid, 1);
  	_exit(0);
  }
  return 0;

}
