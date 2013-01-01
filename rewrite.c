/*
 * rewrite.c
 *
 *  Created on: 08.12.2012
 *      Author: ilya
 */

#include "rewrite.h"

extern char *Tokens[];

void print_line(struct RunTime * rt) {
	int i;

	printf("Size of line: %d\n", rt->size);
	for (i = 0; i < rt->size; ++i) {
		printf("Token: %s, symbol: %c, value: %.3f, width: %.3f\n",
				Tokens[rt->line[i].type], rt->line[i].ch, rt->line[i].value,
				rt->line[i].dctx.width);
	}
}

void rewrite_init(struct LSystem * ls, struct RunTime * rt) {
	rt->size = ls->size_s;
	rt->line = (struct Item *)malloc(rt->size * sizeof(struct Item));
	memcpy(rt->line, ls->seed, rt->size * sizeof(struct Item));
}

void rewrite_step(struct LSystem * ls, struct RunTime * rt) {
	int i, j, k, m, old_size = rt->size, l = 0;
	struct Item * new_line = NULL;
	struct Context ctx;

	init_stack();

	ctx.dctx = ls->dctx;

	for (i = 0; i < old_size; ++i) {

		// It is the upper char in seed, and it must be rewritten
		if (T_UPCHAR == rt->line[i].type) {
			k = -1;
			for (j = 0; j < ls->size_r; ++j) {
				if (ls->rules[j].ptrn == rt->line[i].ch) {
					k = j;
					break;
				}
			}

			if (k > -1) {
				new_line = (struct Item *)realloc(new_line, (l + ls->rules[k].size) * sizeof(struct Item));
				for (m = 0; m < ls->rules[k].size; ++m) {
					new_line[l + m].ch = ls->rules[k].rewrite[m].ch;
					new_line[l + m].type = ls->rules[k].rewrite[m].type;
					new_line[l + m].value = ls->rules[k].rewrite[m].value;

					if (T_PUSH == ls->rules[k].rewrite[m].type) {
						push(ctx);
						ctx.dctx.length *= ls->rules[k].rewrite[m].dctx.length;
						ctx.dctx.width *= ls->rules[k].rewrite[m].dctx.width;
						ctx.dctx.r *= ls->rules[k].rewrite[m].dctx.r;
						ctx.dctx.g *= ls->rules[k].rewrite[m].dctx.g;
						ctx.dctx.b *= ls->rules[k].rewrite[m].dctx.b;
					}

					if (T_POP == ls->rules[k].rewrite[m].type)
						ctx = pop();

					new_line[l + m].dctx = ctx.dctx;
				}
				l += ls->rules[k].size;
			}

		}

		if (T_PUSH == rt->line[i].type) {
			push(ctx);
			ctx.dctx.length *= rt->line[i].dctx.length;
			ctx.dctx.width *= rt->line[i].dctx.width;
			ctx.dctx.r *= rt->line[i].dctx.r;
			ctx.dctx.g *= rt->line[i].dctx.g;
			ctx.dctx.b *= rt->line[i].dctx.b;
		}

		if (T_POP == rt->line[i].type)
			ctx = pop();

		// It is the lower char or any other in seed, should be just copied
		if (T_LOCHAR == rt->line[i].type || T_PLUS == rt->line[i].type || T_MINUS == rt->line[i].type || T_PUSH == rt->line[i].type || T_POP == rt->line[i].type) {
			new_line = (struct Item *)realloc(new_line, (l + 1) * sizeof(struct Item));
			new_line[l].ch = rt->line[i].ch;
			new_line[l].type = rt->line[i].type;
			new_line[l].value = rt->line[i].value;
			if (T_LOCHAR == rt->line[i].type)
				new_line[l].dctx = ctx.dctx;
			else
				new_line[l].dctx = rt->line[i].dctx;
			++l;
		}

	}

	free(rt->line);
	rt->size = l;
	rt->line = new_line;

}


