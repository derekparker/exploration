#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>

#define MAX_COROUTINES 4
static ucontext_t coroutines[MAX_COROUTINES];

void
coroutine(ucontext_t *main2, ucontext_t *me, int j) {
	for (int i = 0; i < 100; i++) {
		printf("coroutine %d says %d\n", j, i);
		swapcontext(me, main2);
	}
}

void
make_and_run_coroutines(void) {
	ucontext_t main, main2;
	int *f;

    // Flag indicating that the iterator has completed.
    int finished = 0;

	for (int j = 0; j < MAX_COROUTINES; j++) {
	    getcontext(&coroutines[j]);

	    /* Initialise the iterator context. uc_link points to main, the
	     * point to return to when the iterator finishes. */
	    coroutines[j].uc_link          = &main;
	    coroutines[j].uc_stack.ss_sp   = malloc(SIGSTKSZ);
	    coroutines[j].uc_stack.ss_size = SIGSTKSZ;

	    /* Fill in the coroutine context, this will make it start at the
	     * coroutine function with it's own stack. */
	    makecontext(&coroutines[j], (void (*)(void)) coroutine,
	    	        3, &main2, &coroutines[j], j);
	}

	// Set main context here for when our coroutines return.
	getcontext(&main);

    if (!finished) {
    	finished = 1;
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < MAX_COROUTINES; j++) {
				swapcontext(&main2, &coroutines[j]);
			}
		}
	}
}

int
main(int argc, const char **argv) {
	make_and_run_coroutines();

	return 0;
}
