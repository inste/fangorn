/*
 * fangorn.h
 *
 *  Created on: 08.12.2012
 *      Author: ilya
 */

#ifndef FANGORN_H_
#define FANGORN_H_

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define		GET_CURRENT_DOUBLE (num * exp(expo * log(10)) + mant / exp((mantsize - expo) * log(10)))

// Tokens in input file

enum Token {
	T_EOF = 0,
	T_NONE,
	T_UPCHAR, // X
	T_LOCHAR, // x
	T_PLUS,   // +
	T_MINUS,  // -
	T_LPAREN, // (
	T_RPAREN, // )
	T_COMMA,  // ,
	T_PUSH,   // [
	T_POP,    // ]
	T_COND,   // ?
	T_DDOTS,  // :
	T_SEMICL, // ;
	T_CURRENT,// $
	T_GREATER,// >
	T_SMALLER,// <
	T_EQUAL,  // =
	T_GREQUAL,// >=
	T_SMEQUAL,// <=
	T_AT,     // @
	T_NUMBER
};

// Drawing context

struct DrawCtx {
	double		width;
	double		length;
	double		r;
	double		g;
	double		b;
};

// Pencil position context

struct Context {
	struct DrawCtx	dctx;
	double			angle;
	double			posx;
	double			posy;
};

// Item of rewriting rule

struct Item {
	enum Token		type;
	int				ch;
	double			value;
	struct DrawCtx	dctx;
};

// Rewrite rule

struct Rule {
	int		ptrn;
	int		size;
	struct Item * rewrite;
};

// Definition of rewriting

struct LSystem {
	// Size of system variables
	int		size_v;
	// System variables list
	int *	vars;

	// Size of seed in items
	int		size_s;
	// Seed list
	struct Item *	seed;

	// Count of rewriting rules
	int		size_r;
	// Rules list
	struct Rule * rules;

	// Initial drawing context
	struct DrawCtx dctx;
};


// Completed array of items to draw

struct RunTime {
	int		size;
	struct Item * line;
};

#endif /* FANGORN_H_ */
