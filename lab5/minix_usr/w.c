/* polecenie przyjmuje jako argument 1 albo 0 */

#include <stdlib.h>
#include <unistd.h>
#include <lib.h>

PUBLIC int worst_fit(int w) {
	message m;
	m.m1_i1 = w;
	_syscall(MM, WORST_FIT, &m);
}

int main(int argc, char *argv[]) {
	if (argc < 2) return 1;

	worst_fit(atoi(argv[1]));
	return 0;
}
