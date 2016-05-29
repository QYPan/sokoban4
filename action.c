#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include "action.h"
#include "face.h"
#include "ai.h"

char *player_menu[] = {
	"I  do",
	"He do",
	NULL
};

char GT[500][MAPSIZE][MAPSIZE];
char Num_Menu[50][50];
char BOX[MAPSIZE][MAPSIZE];
char NIL_BOX[MAPSIZE][MAPSIZE];

int load_map()
{
	int count = 0;
	int layers = 0;
	char s[MAPSIZE];
	FILE *fp;
	fp = fopen("g.txt", "rt");
	if(fp == NULL){
		perror("open map file failure!\n");
		return -1;
	}
	while(fgets(s, MAPSIZE, fp) != NULL){
		if(atoi(s)){
			GT[layers][count++][0] = 0;
			count = 0;
			layers++;
		}
		else{
			s[strlen(s)-1] = '\0';
			strcpy(GT[layers][count++], s);
		}
	}
	fclose(fp);
	return layers;
}

void fill_menu(char *menu[], int beg_r, int n)
{
	int i;
	int cur = beg_r;
	sprintf(Num_Menu[0], " map ");
	menu[0] = Num_Menu[0];
	for(i = 1; i <= n; i++){
		if(cur < 10)
			sprintf(Num_Menu[i], "  %d  ", cur);
		else if(cur < 100)
			sprintf(Num_Menu[i], "  %d ", cur);
		else if(cur < 1000)
			sprintf(Num_Menu[i], " %d ", cur);
		menu[i] = Num_Menu[i];
		cur++;
	}
	menu[i] = NULL;
}

void init_data(MAPELE *mele)
{
	memset(BOX, 0, sizeof(BOX));
	memset(NIL_BOX, 0, sizeof(NIL_BOX));
	(mele->way).m_count = 0;
	mele->row = 0;
	mele->col = 0;
	mele->box_count = 0;
	mele->cur_color = 11;
	mele->curin_color = 1;
	mele->nil_color = 1;
	mele->box_color = 8;
	mele->wall_color = 6;
	mele->full_color = 10;
	mele->empty_color = 1;

	mele->wall_p = ' ';
	mele->empty_p = ' ';
	mele->man_p = '*';
	mele->nil_box_p = 'O';
	mele->box_p = 'O';

	mele->wall_g = '#';
	mele->empty_g = ' ';
	mele->man_g = '*';
	mele->nil_box_g = 'O';
	mele->box_g = '@';

}

int judge_end(MAPELE *mele)
{
	int i, j;
	int count = 0;
	for(i = 0; i < mele->row; i++)
		for(j = 0; j < mele->col; j++)
			if(BOX[i][j] == '@' && NIL_BOX[i][j] == 'O')
				count++;
	if(count == mele->box_count)
		return 1;
	return 0;
}



void move_it(WINDOW *win_ptr, MAPELE *mele, char who, int r, int c, int d1, int d2)
{
	if('@' == who){
		if(NIL_BOX[r+d1][c+d2] == 'O'){
			chose_color(win_ptr, mele->full_color);
			mvwprintw(win_ptr, r+d1, c+d2, "%c", mele->box_p);
		}
		else{
			chose_color(win_ptr, mele->box_color);
			mvwprintw(win_ptr, r+d1, c+d2, "%c", mele->box_p);
		}
		BOX[r+d1][c+d2] = '@';
		BOX[r][c] = ' ';
	}
	else{
		BOX[r+d1][c+d2] = '*';
		BOX[r][c] = ' ';
		if(NIL_BOX[r+d1][c+d2] == 'O')
			chose_color(win_ptr, mele->curin_color);
		else
			chose_color(win_ptr, mele->cur_color);
		mvwprintw(win_ptr, r+d1, c+d2, "%c", mele->man_p);
		mele->curr = r + d1;
		mele->curc = c + d2;
	}
	if(NIL_BOX[r][c] == 'O'){
		chose_color(win_ptr, mele->nil_color);
		mvwprintw(win_ptr, r, c, "%c", mele->nil_box_p);
	}
	else{
		chose_color(win_ptr, mele->empty_color);
		mvwprintw(win_ptr, r, c, "%c", mele->empty_p);
	}
	wrefresh(win_ptr);
}

void go_back(WINDOW *win_ptr, MAPELE *mele)
{
	int m_count;
	int pr, pc, pd1, pd2;
	int br, bc, bd1, bd2;
	if((mele->way).m_count > 0){

		(mele->way).m_count--;
		m_count = (mele->way).m_count;
		pr = (mele->way).way_p[m_count].r;
		pc = (mele->way).way_p[m_count].c;
		pd1 = (mele->way).way_p[m_count].d1;
		pd2 = (mele->way).way_p[m_count].d2;
		br = (mele->way).way_b[m_count].r;
		bc = (mele->way).way_b[m_count].c;
		bd1 = (mele->way).way_b[m_count].d1;
		bd2 = (mele->way).way_b[m_count].d2;

		if((mele->way).way_p[m_count].flag)
			move_it(win_ptr, mele, '*', pr, pc, -pd1, -pd2); /* !!! */
		if((mele->way).way_b[m_count].flag)
			move_it(win_ptr, mele, '@', br, bc, -bd1, -bd2); /* !!! */

	}
}

void go_move(WINDOW *win_ptr, MAPELE *mele, int d1, int d2)
{
	int curred = mele->curr + d1;
	int curced = mele->curc + d2;
	int m_c = (mele->way).m_count;
	int ok = 0;
	if(BOX[curred][curced] == '#')
		return;
	else if(BOX[curred][curced] == '@'){
		if(BOX[curred+d1][curced+d2] == '#' || BOX[curred+d1][curced+d2] == '@')
			return;
		move_it(win_ptr, mele, '@', curred, curced, d1, d2);
		ok = 1;
	}
	move_it(win_ptr, mele, '*', mele->curr, mele->curc, d1, d2);
	(mele->way).way_p[m_c].r = curred;
	(mele->way).way_p[m_c].c = curced;
	(mele->way).way_p[m_c].d1 = d1;
	(mele->way).way_p[m_c].d2 = d2;
	(mele->way).way_p[m_c].flag = 1;
	if(ok){
		(mele->way).way_b[m_c].r = curred + d1;
		(mele->way).way_b[m_c].c = curced + d2;
		(mele->way).way_b[m_c].d1 = d1;
		(mele->way).way_b[m_c].d2 = d2;
		(mele->way).way_b[m_c].flag = 1;
	}
	else 
		(mele->way).way_b[m_c].flag = 0;
	(mele->way).m_count++;
}

void you_play(WINDOW *win_ptr, MAPELE *mele)
{
	int key = -1;
	keypad(win_ptr, 1);
	while(!judge_end(mele)){
		key = wgetch(win_ptr);
		if(key == KEY_UP)
			go_move(win_ptr, mele, -1, 0);
		else if(key == KEY_DOWN)
			go_move(win_ptr, mele, 1, 0);
		else if(key == KEY_LEFT)
			go_move(win_ptr, mele, 0, -1);
		else if(key == KEY_RIGHT)
			go_move(win_ptr, mele, 0, 1);
		else if(key == 'w')
			go_back(win_ptr, mele);
		else if(key == 'q')
			break;
	}
	while(key != 'q' && key != KEY_ENTER && key != '\n')
		key = wgetch(win_ptr);
}

void go_play(WINDOW *win_ptr)
{
	char *G[MAPSIZE];
	WINDOW *sub_win;
	int chose;
	int layers;
	int player;
	int i;
	MAPELE mapele;
	init_data(&mapele);
	layers = load_map();
	if(layers < 0)
		return;
	chose = get_num_chose(layers);
	for(i = 0; i < MAPSIZE; i++)
		G[i] = GT[chose-1][i];
	player = get_select(player_menu);

	chose_color(win_ptr, 4);
	mvwprintw(win_ptr, 3, 23, " %d ", chose);
	wrefresh(win_ptr);

	sub_win = draw_game_map(G, &mapele);

	if(!player)
		you_play(sub_win, &mapele);
	else 
		computer_play(sub_win, &mapele);

	wclear(sub_win);
	wrefresh(sub_win);
	delwin(sub_win);

	chose_color(win_ptr, 1);
	wmove(win_ptr, 3, 23);
	mvwprintw(win_ptr, 3, 23, "     ");
	wrefresh(win_ptr);
}

void go_help(WINDOW *win_ptr)
{
	WINDOW *help_win;
	int key = -1;
	int r = 9;
	int c = 20;
	int tr, tc;
	int beg_r = (LINES - r) / 2;
	int beg_c = (COLS - c) / 2;
	help_win = subwin(win_ptr, r, c, beg_r, beg_c);
	touchwin(win_ptr);
	box(help_win, '|', '*');

	tr = 1;
	tc = 2;
	mvwprintw(help_win, tr++, tc, "up:      [UP]");
	mvwprintw(help_win, tr++, tc, "down:    [DOWN]");
	mvwprintw(help_win, tr++, tc, "left:    [LEFT]");
	mvwprintw(help_win, tr++, tc, "right:   [RIGHT]");
	mvwprintw(help_win, tr++, tc, "confirm: [ENTER]");
	mvwprintw(help_win, tr++, tc, "back:    [w]");
	mvwprintw(help_win, tr++, tc, "quit:    [q]");
	wrefresh(help_win);

	key = wgetch(help_win);
	while(key != KEY_ENTER && key != '\n' && key != 'q')
		key = wgetch(help_win);

	wclear(help_win);
	wrefresh(help_win);
	delwin(help_win);
}
