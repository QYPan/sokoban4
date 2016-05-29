#ifndef ACTION_H
#define ACTION_H

#include <curses.h>
#define MAPSIZE 40

typedef struct{
	int r, c;
	int d1, d2;
	int flag;
}Point;

typedef struct{
	Point way_p[5000];
	Point way_b[5000];
	int m_count;
}WAYS;

typedef struct{
	WAYS way;
	int row, col;
	int box_count;
	int curr, curc;

	int cur_color;
	int curin_color;
	int nil_color;
	int box_color;
	int wall_color;
	int full_color;
	int empty_color;

	char wall_p;
	char empty_p;
	char man_p;
	char nil_box_p;
	char box_p;
}MAPELE;

extern char GT[500][MAPSIZE][MAPSIZE];
extern int load_map();
extern char BOX[MAPSIZE][MAPSIZE];
extern char NIL_BOX[MAPSIZE][MAPSIZE];

extern void fill_menu(char *menu[], int beg_r, int n);
extern void go_play(WINDOW *win_ptr);
extern void go_help(WINDOW *win_ptr);
extern void move_it(WINDOW *win_ptr, MAPELE *mele, char who, int r, int c, int d1, int d2);

#endif
