/*
 * stack.c
 *
 *  Created on: 09.12.2012
 *      Author: ilya
 */


#include "stack.h"

static struct Context * stack = NULL;
static int sp = 0;
static int allocated = 0;

void init_stack() {
	sp = 0;
}

void push(struct Context ctx) {
	if (allocated == sp) {
		stack = (struct Context *)realloc(stack, (allocated += STACK_FRAME) * sizeof(struct Context));
	}
	stack[sp++] = ctx;
}

struct Context pop() {
	if (allocated > sp + STACK_FRAME) {
		stack = (struct Context *)realloc(stack, (allocated -= STACK_FRAME) * sizeof(struct Context));
	}
	return stack[--sp];
}

