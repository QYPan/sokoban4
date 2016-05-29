#include "face.h"
#include "action.h"
#include "unistd.h"

char *Menu[] = {
	" Play ",
	" Help ",
	" Quit ",
	NULL
};

int menu_beg_row = 0;
int menu_beg_col = 0;
int menu_row = 25;
int menu_col = 50;

int main()
{
	WINDOW *menu_win;
	int select;
	initscr();
	init_win();
	menu_beg_row = (LINES - menu_row) / 2;
	menu_beg_col = (COLS - menu_col) / 2;
	menu_win = newwin(menu_row, menu_col, menu_beg_row, menu_beg_col);
	chose_color(menu_win, 1);
	box(menu_win, '|', '*');
	wrefresh(menu_win);
	do{
		select = get_select(Menu);
		switch(select){
			case 0 : go_play(menu_win); break;
			case 1 : go_help(menu_win); break;
		}
	}while(select != 2 && select != 'q');
	delwin(menu_win);
	quit_win();
	clear();
	refresh();
	endwin();
	return 0;
}


