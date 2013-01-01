/*
 * stack.h
 *
 *  Created on: 09.12.2012
 *      Author: ilya
 */

#ifndef STACK_H_
#define STACK_H_

#include "fangorn.h"

#define STACK_FRAME		512

void init_stack();
void push(struct Context ctx);
struct Context pop();


#endif /* STACK_H_ */
