#ifndef AI_H
#define AI_H

#include "action.h"
#include <curses.h>
#define HASH_SIZE 2379689
#define MAX(x, y) ((x)>(y)?(x):(y))
#define MIN(x, y) ((x)<(y)?(x):(y))
#define ABS(x) ((x)<0?(-(x)):(x))

typedef long long LL;

typedef struct{
	int r, c;
}Coor;

typedef struct HASHELE{
	LL key;
	int mr, mc;
	struct HASHELE *next;
}Hashele;

typedef struct STATE{
	LL mark_val;
	char m[MAPSIZE][MAPSIZE];
	Coor bcoor[100];
	int man_r, man_c;
	int d1, d2;
	int f, g, h, move; 
	struct STATE *fa;
}State;

typedef struct{
	WINDOW *win_ptr;
	State *beg_st;
	State *end_st;
}Thread_arg;

typedef struct{
	int state_count;
	int hit_count;
	int same_count;
	int push_box_count;
	int move_count;
	int sac;
}Count;

extern MAPELE *mele;
extern void computer_play(WINDOW *win_ptr, MAPELE *mele);

#endif
