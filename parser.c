/*
 * parser.c
 *
 *  Created on: 08.12.2012
 *      Author: ilya
 */

#include "parser.h"

char *Tokens[] = {
	"T_EOF",
	"T_NONE",
	"T_UPCHAR", // X
	"T_LOCHAR", // x
	"T_PLUS",   // +
	"T_MINUS",  // -
	"T_LPAREN", // (
	"T_RPAREN", // )
	"T_COMMA",  // ,
	"T_PUSH",   // [
	"T_POP",    // ]
	"T_COND",   // ?
	"T_DDOTS",  // :
	"T_SEMICL", // ;
	"T_CURRENT",// $
	"T_GREATER",// >
	"T_SMALLER",// <
	"T_EQUAL",  // =
	"T_GREQUAL",// >=
	"T_SMEQUAL",// <=
	"T_AT",     // @
	"T_NUMBER"
};


// Global variables for parser
static int ch = ' ', tc;
static enum Token sym = T_NONE;
static int num, mant, mantsize, expo;
static char exposgn;

// System definition
struct LSystem LS;
// Ready-to-render array
struct RunTime RT;


void getsym(FILE * fd) {
	sym = T_NONE;
	while (ch != EOF && isspace(ch))
		ch = fgetc(fd);

	if (EOF == ch) {
		sym = T_EOF;
		return;
	}

	if (isalpha(ch)) {
		if (ch >= 'A' && ch <= 'Z')
			sym = T_UPCHAR;
		else
			sym = T_LOCHAR;
		tc = ch;
		ch = fgetc(fd);
	} else if (isdigit (ch)) {
		num = 0;
		expo = 0;
		mant = 0;
		mantsize = 0;
		do {
			num = num * 10 + (ch - '0');
			ch = fgetc(fd);
		} while (isdigit (ch));
		if (ch == '.') {
			ch = fgetc(fd);
			if (isdigit(ch)) {
				mant = 0;
				expo = 0;
				exposgn = '+';
				mantsize = 0;
				do {
					mant = mant * 10 + (ch - '0');
					++mantsize;
					ch = fgetc(fd);
				} while (isdigit(ch));

				if (ch == 'e' || ch == 'E') {
					ch = fgetc(fd);
					if (ch == '+' || ch == '-') {
						exposgn = ch;
						ch = fgetc(fd);
						if (isdigit(ch)) {
							do {
								expo = expo * 10 + (ch - '0');
								ch = fgetc(fd);
							} while (isdigit(ch));
						}
					} else if (isdigit(ch)) {
						if (isdigit(ch)) {
							do {
								expo = expo * 10 + (ch - '0');
								ch = fgetc(fd);
							} while (isdigit(ch));
						}
					}
				}
			}
		} else if (ch == 'e' || ch == 'E') {
					ch = fgetc(fd);
					if (ch == '+' || ch == '-') {
						exposgn = ch;
						ch = fgetc(fd);
						if (isdigit(ch)) {
							do {
								expo = expo * 10 + (ch - '0');
								ch = fgetc(fd);
							} while (isdigit(ch));
						}
						sym = fgetc(fd);
					} else if (isdigit(ch)) {
						if (isdigit(ch)) {
							do {
								expo = expo * 10 + (ch - '0');
								ch = fgetc(fd);
							} while (isdigit(ch));
						}
					}
		}
		sym = T_NUMBER;
	} else {
		switch (ch) {
			case '+' :
				sym = T_PLUS;
				ch = fgetc(fd);
				break;
			case '-' :
				sym = T_MINUS;
				ch = fgetc(fd);
				break;
			case '(' :
				sym = T_LPAREN;
				ch = fgetc(fd);
				break;
			case ')' :
				sym = T_RPAREN;
				ch = fgetc(fd);
				break;
			case ',' :
				sym = T_COMMA;
				ch = fgetc(fd);
				break;
			case '[' :
				sym = T_PUSH;
				ch = fgetc(fd);
				break;
			case ']' :
				sym = T_POP;
				ch = fgetc(fd);
				break;
			case '?' :
				sym = T_COND;
				ch = fgetc(fd);
				break;
			case ':' :
				sym = T_DDOTS;
				ch = fgetc(fd);
				break;
			case ';' :
				sym = T_SEMICL;
				ch = fgetc(fd);
				break;
			case '@' :
				sym = T_AT;
				ch = fgetc(fd);
				break;
			case '$' :
				sym = T_CURRENT;
				ch = fgetc(fd);
				break;
		}
	}
}



void print_ls() {
	int i, j;

	printf("Size of vars: %d\nVars: ", LS.size_v);
	for (i = 0; i < LS.size_v; ++i)
		printf("'%c' ", LS.vars[i]);
	printf("\nSize of seed:%d\nSeed:", LS.size_s);
	for (i = 0; i < LS.size_s; ++i)
		printf("Token: %s, symbol: %c, value: %.3f", Tokens[LS.seed[i].type], LS.seed[i].ch, LS.seed[i].value);
	printf("\nSize of rules: %d\n", LS.size_r);
	for (i = 0; i < LS.size_r; ++i) {
		printf("Rule %d, pattern: '%c', size of rewrite:%d\nPatterns:\n", i + 1, LS.rules[i].ptrn, LS.rules[i].size);
		for (j = 0; j < LS.rules[i].size; ++j) {
			printf("Token: %s, symbol: %c, value: %.3f\n", Tokens[LS.rules[i].rewrite[j].type], LS.rules[i].rewrite[j].ch, LS.rules[i].rewrite[j].value);
		}
	}

}


void parse(char * filename) {
	int repeat = 0, i;

	FILE * fd = fopen(filename, "r");

	LS.size_v = 0;
	LS.vars = NULL;

	LS.size_s = 0;
	LS.seed = NULL;

	LS.size_r = 0;
	LS.rules = NULL;

	getsym(fd);

		// Filling list of variables
		while (T_SEMICL != sym) {
			LS.size_v++;
			LS.vars = (int *)realloc(LS.vars, LS.size_v * sizeof(int));
			LS.vars[LS.size_v - 1] = tc;

			getsym(fd);
		}

		getsym(fd);
		// Reading seed

		while (T_SEMICL != sym) {

			if (T_NUMBER == sym) {
				LS.seed[LS.size_s - 1].value = GET_CURRENT_DOUBLE;
				getsym(fd);
				continue;
			}
			LS.size_s++;
			LS.seed = (struct Item *)realloc(LS.seed, LS.size_s * sizeof(struct Item));
			LS.seed[LS.size_s - 1].type = sym;
			LS.seed[LS.size_s - 1].ch = tc;
			LS.seed[LS.size_s - 1].dctx.length = 1.0;
			LS.seed[LS.size_s - 1].dctx.width = 1.0;
			LS.seed[LS.size_s - 1].dctx.r = 1.0;
			LS.seed[LS.size_s - 1].dctx.g = 1.0;
			LS.seed[LS.size_s - 1].dctx.b = 1.0;
			getsym(fd);
		}

		// Initial parameters of line
		// Length
		getsym(fd);

		LS.dctx.length = GET_CURRENT_DOUBLE;

		getsym(fd);

		// Width
		LS.dctx.width = GET_CURRENT_DOUBLE;

		getsym(fd);

		// Red colour step multiplier
		LS.dctx.r = GET_CURRENT_DOUBLE;

		getsym(fd);

		// Green colour step multiplier
		LS.dctx.g = GET_CURRENT_DOUBLE;

		getsym(fd);

		// Blue colour step multiplier
		LS.dctx.b = GET_CURRENT_DOUBLE;

		getsym(fd);

		// Times to repeat rewriting
		repeat = num;

		getsym(fd);

		if (T_SEMICL != sym)
			printf("No semicolon found\n");

		getsym(fd);

		// Reading rules
		while (T_EOF != sym) {
			// Got new rule
			LS.size_r++;
			LS.rules = (struct Rule *)realloc(LS.rules, LS.size_r * sizeof(struct Rule));

			if (T_UPCHAR != sym && T_LOCHAR != sym)
				printf("Error\n");

			LS.rules[LS.size_r - 1].ptrn = tc;
			LS.rules[LS.size_r - 1].size = 0;

			getsym(fd);

			if (T_DDOTS != sym)
				printf("Error2");

			getsym(fd);


			while (T_SEMICL != sym) {

				LS.rules[LS.size_r - 1].size++;
				LS.rules[LS.size_r - 1].rewrite = (struct Item *)realloc(LS.rules[LS.size_r - 1].rewrite,
						LS.rules[LS.size_r - 1].size * sizeof(struct Item));

				LS.rules[LS.size_r - 1].rewrite[LS.rules[LS.size_r - 1].size - 1].type = sym;
				LS.rules[LS.size_r - 1].rewrite[LS.rules[LS.size_r - 1].size - 1].ch = ' ';

				// Default value, means that we have no configuration of draw context at all
				LS.rules[LS.size_r - 1].rewrite[LS.rules[LS.size_r - 1].size - 1].dctx.length = 1.0;
				LS.rules[LS.size_r - 1].rewrite[LS.rules[LS.size_r - 1].size - 1].dctx.width = 1.0;
				LS.rules[LS.size_r - 1].rewrite[LS.rules[LS.size_r - 1].size - 1].dctx.r = 1.0;
				LS.rules[LS.size_r - 1].rewrite[LS.rules[LS.size_r - 1].size - 1].dctx.g = 1.0;
				LS.rules[LS.size_r - 1].rewrite[LS.rules[LS.size_r - 1].size - 1].dctx.b = 1.0;

				if (T_UPCHAR == sym || T_LOCHAR == sym)
					LS.rules[LS.size_r - 1].rewrite[LS.rules[LS.size_r - 1].size - 1].ch = tc;


				if (T_PUSH == sym) {
					getsym(fd);

					if (T_AT == sym) {
						getsym(fd);

						if (T_NUMBER == sym) {
							LS.rules[LS.size_r - 1].rewrite[LS.rules[LS.size_r - 1].size - 1].dctx.length =
									GET_CURRENT_DOUBLE;
						}

						getsym(fd);

						if (T_SEMICL != sym)
							printf("Error3");

						getsym(fd);

						if (T_NUMBER == sym) {
							LS.rules[LS.size_r - 1].rewrite[LS.rules[LS.size_r - 1].size - 1].dctx.width =
									GET_CURRENT_DOUBLE;
						}

						getsym(fd);

						if (T_SEMICL != sym)
							printf("Error4");

						getsym(fd);

						if (T_NUMBER == sym) {
							LS.rules[LS.size_r - 1].rewrite[LS.rules[LS.size_r - 1].size - 1].dctx.r =
									GET_CURRENT_DOUBLE;
						}

						getsym(fd);

						if (T_SEMICL != sym)
							printf("Error5");

						getsym(fd);

						if (T_NUMBER == sym) {
							LS.rules[LS.size_r - 1].rewrite[LS.rules[LS.size_r - 1].size - 1].dctx.g =
									GET_CURRENT_DOUBLE;
						}

						getsym(fd);

						if (T_SEMICL != sym)
							printf("Error6");

						getsym(fd);

						if (T_NUMBER == sym) {
							LS.rules[LS.size_r - 1].rewrite[LS.rules[LS.size_r - 1].size - 1].dctx.b =
									GET_CURRENT_DOUBLE;
						}

						getsym(fd);

						if (T_SEMICL != sym)
							printf("Error7");

						getsym(fd);
						goto out;

					}
					goto out;
				}

				if (T_PLUS == sym || T_MINUS == sym) {
					getsym(fd);

					if (T_NUMBER == sym) {
						LS.rules[LS.size_r - 1].rewrite[LS.rules[LS.size_r - 1].size - 1].value =
									GET_CURRENT_DOUBLE;
						getsym(fd);
					}
				} else
					getsym(fd);

out:		;
			}

			getsym(fd);
		}

		// Initial fill of rewrite array with seed

		rewrite_init(&LS, &RT);

		// Rewriting

		for (i = 0; i < repeat; ++i)
			rewrite_step(&LS, &RT);


	fclose(fd);
}
