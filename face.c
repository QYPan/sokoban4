#include <sys/ioctl.h>
#include <curses.h>
#include <string.h>
#include <unistd.h>
#include "action.h"
#include "face.h"

int lines = 30;
int cols = 80;

int init_win()
{
#if 0
	struct winsize wsize;
	ioctl(STDIN_FILENO, TIOCGWINSZ, &wsize);
	LINES = 30;
	COLS = 90;
	wsize.ws_row = LINES;
	wsize.ws_col = COLS;
#endif
	char wsize[100];
	sprintf(wsize, "resize -c -s %d %d", lines, cols);
	system(wsize);
	curs_set(0);
	cbreak();
	noecho();
	if(!has_colors()){
		fprintf(stderr, "Error - no color support on this terminal\n");
		return -1;
	}
	if(start_color() != OK){
		fprintf(stderr, "Error - could not initialize colors\n");
		return -1;
	}
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_GREEN);
	init_pair(3, COLOR_GREEN, COLOR_RED);
	init_pair(4, COLOR_RED, COLOR_YELLOW);
	init_pair(5, COLOR_WHITE, COLOR_BLACK);
	init_pair(6, COLOR_MAGENTA, COLOR_BLUE);
	init_pair(7, COLOR_CYAN, COLOR_WHITE);
	init_pair(8, COLOR_YELLOW, COLOR_BLACK);
	init_pair(9, COLOR_BLUE, COLOR_BLACK);
	init_pair(10, COLOR_RED, COLOR_BLACK);
	init_pair(11, COLOR_CYAN, COLOR_BLACK);
	return 0;
}

void quit_win()
{
	system("clear");
	curs_set(1);
}

void chose_color(WINDOW *win_ptr, int c)
{
	wattron(win_ptr, COLOR_PAIR(c) | A_BOLD);
}

void draw_menu(WINDOW *win_ptr, char *menu[], int select_row, int cur, int oth)
{
	char **menu_prt = menu;
	int current_row = 1;
	int beg_col = 1;
	touchwin(win_ptr);
	while(*menu_prt){
		wmove(win_ptr, current_row, beg_col);
		if(current_row == select_row + 1){
			chose_color(win_ptr, cur);
			wprintw(win_ptr, "%s", *menu_prt);
		}
		else{
			chose_color(win_ptr, oth);
			wprintw(win_ptr, "%s", *menu_prt);
		}
		current_row++;
		menu_prt++;
	}
	wrefresh(win_ptr);
}

int get_select(char *menu[])
{
	WINDOW *sub_win;
	int sub_row = 0;
	int sub_col = 0;
	int sub_beg_row = 0;
	int sub_beg_col = 0;
	int select_row = 0;
	int selected = 0;
	int max_row = 0;
	int key = -1;
	char **menu_prt = menu;
	int sub_len = strlen(*menu_prt);
	while(*menu_prt){
		max_row++;
		menu_prt++;
	}
	sub_row = max_row + 2;
	sub_col = sub_len + 2;
	sub_beg_row = (LINES - sub_row) / 2;
	sub_beg_col = (COLS - sub_col) / 2;
	sub_win = newwin(sub_row, sub_col, sub_beg_row, sub_beg_col);
	chose_color(sub_win, 1);
	box(sub_win, '|', '*');
	wrefresh(sub_win);
	keypad(sub_win, true);
	while(key != KEY_ENTER && key != '\n' && key != 'q'){
		if(KEY_UP == key){
			select_row--;
			if(select_row < 0)
				select_row = max_row - 1;
		}
		if(KEY_DOWN == key){
			select_row++;
			if(select_row >= max_row)
				select_row = 0;
		}
		selected = select_row;
		draw_menu(sub_win, menu, select_row, 2, 1);
		key = wgetch(sub_win);
	}
	wclear(sub_win);
	wrefresh(sub_win);
	delwin(sub_win);
	if(key == 'q')
		selected = key;
	return selected;
}

int get_num_chose(int max_num)
{
	int key = -1;
	int rows = 8;
	int cols = 5;
	int count = 1;
	int select_row = 1;
	int menu_beg_r = 1;
	int beg_r = (LINES - rows) / 2;
	int beg_c = (COLS - cols) / 2;
	char *menu[50];
	WINDOW *sub_win = newwin(rows+3, cols+2, beg_r, beg_c);
	chose_color(sub_win, 1);
	box(sub_win, '|', '*');
	wrefresh(sub_win);
	fill_menu(menu, menu_beg_r, rows);
	keypad(sub_win, true);
	while(key != KEY_ENTER && key != '\n'){
		if(KEY_UP == key){
			if(count > 1){
				select_row--;
				count--;
				if(select_row < 1){
					select_row++;
					menu_beg_r--;
					fill_menu(menu, menu_beg_r, rows);
				}
			}
		}
		if(KEY_DOWN == key){
			if(count < max_num){
				select_row++;
				count++;
				if(select_row > rows){
					select_row--;
					menu_beg_r++;
					fill_menu(menu, menu_beg_r, rows);
				}
			}
		}
		draw_menu(sub_win, menu, select_row, 2, 6);
		chose_color(sub_win, 4);
		mvwprintw(sub_win, 1, 1, " map ");
		wrefresh(sub_win);
		key = wgetch(sub_win);
	}
	wclear(sub_win);
	wrefresh(sub_win);
	delwin(sub_win);
	return count;
}

WINDOW *draw_game_map(char *g[], MAPELE *mele)
{
	WINDOW *map_win;
	int row;
	int col;
	int beg_row;
	int beg_col;
	while(g[mele->row][0]){
		int t_col = strlen(g[mele->row]);
		if(t_col > mele->col)
			mele->col = t_col;
		mele->row++;
	}
	beg_row = (LINES - mele->row) / 2;
	beg_col = (COLS - mele->col) / 2;
	map_win = newwin(mele->row, mele->col, beg_row, beg_col);
	row = 0;
	while(g[row][0]){
		for(col = 0; g[row][col]; col++){
			if(g[row][col] == '*'){
				BOX[row][col] = '*';
				NIL_BOX[row][col] = ' ';
				mele->curr = row;
				mele->curc = col;
				chose_color(map_win, mele->cur_color);
				mvwprintw(map_win, row, col, "%c", mele->man_p);
			}
			else if(g[row][col] == '%'){
				BOX[row][col] = '*';
				NIL_BOX[row][col] = 'O';
				mele->box_count++;
				mele->curr = row;
				mele->curc = col;
				chose_color(map_win, mele->curin_color);
				mvwprintw(map_win, row, col, "%c", mele->man_p);
			}
			else if(g[row][col] == 'O'){
				NIL_BOX[row][col] = 'O';
				BOX[row][col] = ' ';
				mele->box_count++;
				chose_color(map_win, mele->nil_color);
				mvwprintw(map_win, row, col, "%c", mele->nil_box_p);
			}
			else if(g[row][col] == '@'){
				BOX[row][col] = '@';
				NIL_BOX[row][col] = ' ';
				chose_color(map_win, mele->box_color);
				mvwprintw(map_win, row, col, "%c", mele->box_p);
			}
			else if(g[row][col] == '#'){
				BOX[row][col] = '#';
				NIL_BOX[row][col] = '#';
				chose_color(map_win, mele->wall_color);
				mvwprintw(map_win, row, col, "%c", mele->wall_p);
			}
			else if(g[row][col] == '$'){
				BOX[row][col] = '@';
				NIL_BOX[row][col] = 'O';
				mele->box_count++;
				chose_color(map_win, mele->full_color);
				mvwprintw(map_win, row, col, "%c", mele->nil_box_p);
			}
			else{ 
				BOX[row][col] = ' ';
				NIL_BOX[row][col] = ' ';
				chose_color(map_win, mele->empty_color);
				mvwprintw(map_win, row, col, "%c", mele->empty_p);
			}
		}
		row++;
	}
	wrefresh(map_win);
	return map_win;
}
