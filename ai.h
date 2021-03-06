#ifndef AI_H
#define AI_H

#include "action.h"
#include <curses.h>
#define HASH_SIZE (1<<23)
#define BOXCOUNT 20
#define MAX(x, y) ((x)>(y)?(x):(y))
#define MIN(x, y) ((x)<(y)?(x):(y))
#define ABS(x) ((x)<0?(-(x)):(x))

typedef long long LL;

typedef struct{
	int r, c;
}Coor;

typedef struct HASHELE{
	LL key;
	int vis[MAPSIZE];
	int step;
	int g;
	int mr, mc;
	struct HASHELE *next;
}Hashele;

typedef struct STATE{
	LL mark_val;
	char m[MAPSIZE][MAPSIZE];
	Coor bcoor[BOXCOUNT];
	int man_r, man_c;
	int d1, d2;
	int mlen;
	int f, g, h, move;
	int next_count;
	struct STATE *next[BOXCOUNT<<2];
	struct STATE *fa;
}State;

typedef struct{
	WINDOW *win_ptr;
	State *beg_st;
	State *end_st;
}Thread_arg;

typedef struct{
	int state_limit;
	int state_count;
	int hit_count;
	int hash_count;
	int same_count;
	int move_count;
	int depth;
	int sac;
}Count;

extern MAPELE *mele;
extern int gr[];
extern int gc[];
extern void computer_play(WINDOW *win_ptr, MAPELE *mele);
extern int kill(char g[][MAPSIZE], int br, int bc, int d1, int d2);

#endif
