#include "/usr/include/minix/callnr.h"
#include "/usr/include/lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define ROUND_ROBIN 0
#define AGING 1
#define SHORTEST_JOB_FIRST 2

void setpri(unsigned group_id, unsigned value){
	message m;
	int pid;
	pid = getpid();

	m.m1_i1 = pid;
	m.m1_i2 = group_id;
	m.m1_i3 = value;
	_syscall(MM, SETPRI, &m);
}

int main(int argc, char** argv) {
	int pid;
	int group_id;
	unsigned i,j;
	unsigned value;

	printf("\ntest1: parent has group with higher priority\n");
	printf("\ncomparing: child_group = 2, parent_group = 0\n"); 
	setpri(ROUND_ROBIN, 0);
	pid = fork();
	if (!pid) {
		setpri(SHORTEST_JOB_FIRST, 0);
		printf("child finished\n");
		return 0;
	} else {
		setpri(ROUND_ROBIN, 0);
		for (j = 0; j < 100000000; j++){}

		printf("parent finished\n");
		sleep(1); /* let the child run*/
		wait(NULL);
	}
	printf("\ncomparing: child_group = 1, parent_group = 0\n"); 
	setpri(ROUND_ROBIN, 0);
	pid = fork();
	if (!pid) {
		setpri(AGING, 0);
		printf("child finished\n");
		return 0;
	} else {
		setpri(ROUND_ROBIN, 0);
		for (j = 0; j < 100000000; j++){}

		printf("parent finished\n");
		sleep(1); /* let the child run*/
		wait(NULL);
	}
	printf("\ncomparing: child_group = 2, parent_group = 1\n"); 
	setpri(ROUND_ROBIN, 0);
	pid = fork();
	if (!pid) {
		setpri(SHORTEST_JOB_FIRST, 0);
		printf("child finished\n");
		return 0;
	} else {
		setpri(AGING, 0);
		for (j = 0; j < 100000000; j++){}

		printf("parent finished\n");
		sleep(1); /* let the child run*/
		wait(NULL);
	}


	printf("\ntest2: parent has group with lower priority\n");
	printf("\ncomparing: child_group = 0, parent_group = 1\n"); 
	setpri(ROUND_ROBIN, 0);
	pid = fork();
	if (!pid) {
		setpri(ROUND_ROBIN, 0);
		printf("child finished\n");
		return 0;
	} else {
		setpri(AGING, 0);
		for (j = 0; j < 100000000; j++){}

		printf("parent finished\n");
		sleep(1);
		wait(NULL);
	}
	printf("\ncomparing: child_group = 0, parent_group = 2\n"); 
	setpri(ROUND_ROBIN, 0);
	pid = fork();
	if (!pid) {
		setpri(ROUND_ROBIN, -1);
		printf("child finished\n");
		return 0;
	} else {
		setpri(SHORTEST_JOB_FIRST, 0);
		for (j = 0; j < 100000000; j++){}

		printf("parent finished\n");
		sleep(1); /* let the child run*/
		wait(NULL);
	}
	printf("\ncomparing: child_group = 1, parent_group = 2\n"); 
	setpri(ROUND_ROBIN, 0);
	pid = fork();
	if (!pid) {
		setpri(AGING, 0);
		printf("child finished\n");
		return 0;
	} else {
		setpri(SHORTEST_JOB_FIRST, 0);
		for (j = 0; j < 100000000; j++){}

		printf("parent finished\n");
		sleep(1);
		wait(NULL);
	}

	printf("\ntest3: same groups, but parent should be first\n");
	printf("\ncomparing: child_group = 2, parent_group = 2\n"); 
	setpri(ROUND_ROBIN, 0);
	pid = fork();
	if (!pid) {
		setpri(SHORTEST_JOB_FIRST, -1);
		printf("child finished\n");
		return 0;
	} else {
		setpri(SHORTEST_JOB_FIRST, 0);
		for (j = 0; j < 100000000; j++){}

		printf("parent finished\n");
		sleep(1);
		wait(NULL);
	}
	printf("\ncomparing: child_group = 1, parent_group = 1\n"); 
	setpri(ROUND_ROBIN, 0);
	pid = fork();
	if (!pid) {
		setpri(AGING, 0);
		printf("child finished\n");
		return 0;
	} else {
		setpri(AGING, -1);
		for (j = 0; j < 100000000; j++){}

		printf("parent finished\n");
		sleep(1); /* let the child run*/
		wait(NULL);
	}


	printf("\ntest4: same groups, but child should be first\n");
	printf("\ncomparing: child_group = 2, parent_group = 2\n"); 
	setpri(ROUND_ROBIN, 0);
	pid = fork();
	if (!pid) {
		setpri(SHORTEST_JOB_FIRST, 0);
		printf("child finished\n");
		return 0;
	} else {
		setpri(SHORTEST_JOB_FIRST, -1);
		for (j = 0; j < 100000000; j++){}

		printf("parent finished\n");
		sleep(1);
		wait(NULL);
	}
	printf("\ncomparing: child_group = 1, parent_group = 1\n"); 
	setpri(ROUND_ROBIN, 0);
	pid = fork();
	if (!pid) {
		setpri(AGING, -1);
		printf("child finished\n");
		return 0;
	} else {
		setpri(AGING, 0);
		for (j = 0; j < 100000000; j++){}

		printf("parent finished\n");
		sleep(1);
		wait(NULL);
	}
	return 0;
}
