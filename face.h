#ifndef FACE_H
#define FACE_H

#include <curses.h>
#include "action.h"

extern int menu_row;
extern int menu_col;
extern int init_win();
extern int get_select(char *menu[]);
extern void quit_win();
extern void chose_color(WINDOW *win_ptr, int c);
extern int get_num_chose(int max_num);
extern WINDOW *draw_game_map(char *g[], MAPELE *mele);

#endif
