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

int
make_and_run_coroutines(void) {
	void *stack;
	ucontext_t main, main2;
	const int STACK_SIZE = 4096;
	int finished = 0;

	for (int j = 0; j < MAX_COROUTINES; j++) {
	    getcontext(&coroutines[j]);

	    if ((stack = malloc(STACK_SIZE)) == NULL) {
	    	return 1;
	    }

	    /* Initialise the iterator context. uc_link points to main, the
	     * point to return to when the iterator finishes. */
	    coroutines[j].uc_link          = &main;
	    coroutines[j].uc_stack.ss_sp   = stack;
	    coroutines[j].uc_stack.ss_size = STACK_SIZE;

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

	return 0;
}

int
main(int argc, const char **argv) {
	return make_and_run_coroutines();
}
