#include <setjmp.h>
#include <stdio.h>

struct jmp_pts {
	jmp_buf env;
	jmp_buf home;
	int exited;
};

#define MAX_COROUTINES 5
static struct jmp_pts coroutines[MAX_COROUTINES];
static int counts[MAX_COROUTINES];

void
coroutine(const int i) {
	int j;
	for (;;) {
		if (j = setjmp(coroutines[i].env)) {
			j--;
			printf("coroutine %d says %d\n", j, counts[j]);
			counts[j]++;
			if (counts[j] > 100) {
				longjmp(coroutines[j].home, 2);
			}
		}

		longjmp(coroutines[j].home, 1);
	}
}

void
make_and_run_coroutines(void) {
	jmp_buf home;
	int ret;

	for (int i = 0; i < MAX_COROUTINES; i++) {
		coroutines[i].exited = 0;
		if (setjmp(coroutines[i].home)) {
			continue;
		}

		coroutine(i);
	}

	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < MAX_COROUTINES; j++) {
			if (coroutines[j].exited == 1) continue;
			if (ret = setjmp(coroutines[j].home)) {
				if (ret == 2) {
					coroutines[j].exited = 1;
				}
				continue;
			}

			longjmp(coroutines[j].env, j+1);
		}
	}
}

int
main(int argc, const char **argv) {
	make_and_run_coroutines();

	return 0;
}
