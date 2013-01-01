/*
 * rewrite.h
 *
 *  Created on: 09.12.2012
 *      Author: ilya
 */

#ifndef REWRITE_H_
#define REWRITE_H_

#include "fangorn.h"
#include "stack.h"

void rewrite_init(struct LSystem * ls, struct RunTime * rt);
void rewrite_step(struct LSystem * ls, struct RunTime * rt);

#endif /* REWRITE_H_ */
