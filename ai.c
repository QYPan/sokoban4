#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "ai.h"
#include "face.h"

const int inf = (1<<30);

Hashele *hashtable[HASH_SIZE];
LL mark[2][MAPSIZE][MAPSIZE];
int gr[] = {-1, 1, 0, 0};
int gc[] = {0, 0, -1, 1};

MAPELE *mele;
WINDOW *cur_win;
double use_time;
Count co;

void init_time()
{
	static int flag = 0;
	if(!flag){
		srand((unsigned int)time(NULL));
		flag = 1;
	}
}

LL rand64()
{
	double val = rand() / (RAND_MAX+1.0);
	return (LL)(val * 10e14);
}

void build_zobrist_board()
{
	int i, j, k;
	for(k = 0; k < 2; k++){
		for(i = 0; i < MAPSIZE; i++){
			for(j = 0; j < MAPSIZE; j++){
				mark[k][i][j] = rand64();
			}
		}
	}
}

int H(State *st)
{
	int cnt;
	int r, c;
	int total_dis = 0;
	int vis[MAPSIZE][MAPSIZE];
	for(cnt = 0; cnt < mele->box_count; cnt++){
		Coor tco;
		int dis = inf;
		for(r = 0; r < mele->row; r++){
			for(c = 0; c < mele->col; c++){
				if(NIL_BOX[r][c] == mele->nil_box_g && !vis[r][c]){
					int br = st->bcoor[cnt].r;
					int bc = st->bcoor[cnt].c;
					int d = ABS(br-r)+ABS(bc-c); 
					if(d < dis){
						dis = d;
						vis[r][c] = 1;
						vis[tco.r][tco.c] = 0;
						tco.r = r;
						tco.c = c;
					}
				}
			}
		}
		total_dis += dis;
	}
	return total_dis;
}

State *new_state(char g[][MAPSIZE])
{
	int r, c, u = 0;
	State *st = (State *)malloc(sizeof(State));
	st->mark_val = 0;
	for(r = 0; r < mele->row; r++){
		for(c = 0; c < mele->col; c++){
			st->m[r][c] = g[r][c];
			if(g[r][c] == mele->box_g){
				st->mark_val ^= mark[0][r][c];
				st->bcoor[u].r = r;
				st->bcoor[u].c = c;
				u++;
			}
			if(g[r][c] == mele->man_g){
				st->man_r = r;
				st->man_c = c;
			}
		}
	}
	st->g = st->move = 0;
	st->f = st->h = H(st);
	return st;
}

int go_there(State *st, int mr, int mc, int er, int ec)
{
	int f = 0, r = 1;
	int i;
	int col = mele->col;
	int len[MAPSIZE*MAPSIZE];
	int vis[MAPSIZE*MAPSIZE];
	int mque[10000];
	memset(vis, 0, sizeof(vis));
	memset(len, 0, sizeof(len));
	mque[f] = mr * col + mc;
	vis[mque[f]] = 1;
	while(f < r){
		int p = mque[f++];
		int jr = p / col;
		int jc = p % col;
		if(jr == er && jc == ec){
			return len[p];
		}
		for(i = 0; i < 4; i++){
			int nr = jr + gr[i];
			int nc = jc + gc[i];
			int np = nr * col + nc;
			if(st->m[nr][nc] != mele->wall_g && st->m[nr][nc] != mele->box_g && !vis[np]){
				vis[np] = 1;
				len[np] = len[p] + 1;
				mque[r++] = np;
			}
		}
	}
	return -1;
}

int judge_2state(Hashele *ht, State *s2, int flag)
{
	if(ht->key != s2->mark_val) /* 如果他们到64位键值不同，基本认为箱子的位置不同 */
		return 0;
	if(flag){
		if(go_there(s2, ht->mr,  ht->mc, s2->man_r, s2->man_c) >= 0)
			return 1;
		else
			return 0;
	}
	return 1;
}

State *get_end_state()
{
	State *end_st;
	int r, c, u = 0;
	end_st = (State *)malloc(sizeof(State));
	end_st->mark_val = 0;
	for(r = 0; r < mele->row; r++){
		for(c = 0; c < mele->col; c++){
			if(NIL_BOX[r][c] == mele->nil_box_g){
				end_st->mark_val ^= mark[0][r][c];
				end_st->bcoor[u].r = r;
				end_st->bcoor[u].c = c;
				u++;
			}
		}
	}
	return end_st;
}

void clear_sub_hash(Hashele *ht)
{
	if(ht == NULL)
		return;
	clear_sub_hash(ht->next);
	free(ht);
}

void init_hash()
{
	int i;
	for(i = 0; i < HASH_SIZE; i++){
		hashtable[i] = NULL;
	}
}

void clear_hash()
{
	int i;
	for(i = 0; i < HASH_SIZE; i++){
		if(hashtable[i] != NULL){
			clear_sub_hash(hashtable[i]);
			hashtable[i] = NULL;
		}
	}
}

void init_tools()
{
	init_hash();
	init_time();
	build_zobrist_board();
	co.state_count = 0;
	co.hit_count = 0;
	co.same_count = 0;
	co.sac = 0;
}

int get_hashval(State *st)
{
	return (st->mark_val) % HASH_SIZE;
#if 0
	char str[300]; /* !!! */
	str[0] = '\0';
	get_string(st, str);
	return ELF_Hash(str, strlen(str));
#endif
#if 0
	int val = 0;
	int i;
	int row = mele->row;
	for(i = 0; i < row; i++){
		int num = st->r[i];
		if(num)
			val = (107 * (val^num)) % HASH_SIZE;
		else 
			val = (107 * (val^hele[i])) % HASH_SIZE;
	}
	return val;
#endif
}

int try_match(Hashele *ht, State *st)
{
	Hashele *htmp = ht, *new_ht;
	int c = 0;
	int mat = 0;
	while(1){
		if(judge_2state(htmp, st, 1)){
			co.same_count++;
			return 1;
		}
		if(htmp->key == st->mark_val)
			mat = 1;
		if(htmp->next == NULL){
			break;
		}
		htmp = htmp->next;
		c++;
	}
	new_ht = (Hashele *)malloc(sizeof(Hashele));
	new_ht->key = st->mark_val;
	new_ht->mr = st->man_r;
	new_ht->mc = st->man_c;
	new_ht->next = NULL;
	htmp->next = new_ht;
	if(!mat) co.hit_count++;
	if(c > co.sac) co.sac = c;
	return 0;
}

int find_hash(State *st, int val)
{
	int ins = val;
	if(hashtable[ins] != NULL){
		return try_match(hashtable[ins], st);
	}
	Hashele *new_ele = (Hashele *)malloc(sizeof(Hashele));
	new_ele->key = st->mark_val;
	new_ele->mr = st->man_r;
	new_ele->mc = st->man_c;
	new_ele->next = NULL;
	hashtable[ins] = new_ele;
	return 0;
}

int try_insert(State *st)
{
	int hashvalue = get_hashval(st);
	if(!find_hash(st, hashvalue))
		return 1;
	return 0;
}

State *try_move(State *fa, Coor *coor, int d1, int d2)
{
	int br = coor->r;
	int bc = coor->c;
	int cr = fa->man_r;
	int cc = fa->man_c;
	int golen;
	State *p = NULL;
	golen = go_there(fa, fa->man_r, fa->man_c, br-d1, bc-d2);
	if(golen >= 0){
		if(fa->m[br+d1][bc+d2] != mele->wall_g && fa->m[br+d1][bc+d2] != mele->box_g){
			/* !!!!!!!!!!!!!!!!!! */
			fa->m[br][bc] = mele->man_g;
			fa->m[cr][cc] = mele->empty_g;
			fa->m[br+d1][bc+d2] = mele->box_g;
			p = new_state(fa->m);
			p->fa = fa;
			p->man_r = br;
			p->man_c = bc;
			p->d1 = d1;
			p->d2 = d2;
			p->move = fa->move + golen + 1;
			p->g = fa->g + 1;
			p->f = (p->g) + (p->h);
#if 0
			if(!try_insert(p)){
				free(p);
				return NULL;
			}
#endif
			fa->m[br][bc] = mele->box_g;
			fa->m[br+d1][bc+d2] = mele->empty_g;
			fa->m[cr][cc] = mele->man_g;
		}
	}
	return p;
}

void man_real_go(int r, int c, int mfa[][MAPSIZE], int dir[][MAPSIZE])
{
	int col = mele->col;
	int cr = mfa[r][c] / col;
	int cc = mfa[r][c] % col;
	if(cr != r || cc != c){
		int d1 = 0, d2 = 0;
		man_real_go(cr, cc, mfa, dir);
		switch(dir[r][c]){
			case 0: d1 = -1; d2 = 0; break;
			case 1: d1 = 1; d2 = 0; break;
			case 2: d1 = 0; d2 = -1; break;
			case 3: d1 = 0; d2 = 1; break;
		}
		usleep(200*1000);
		move_it(cur_win, mele, mele->man_g, cr, cc, d1, d2);
	}
}

void man_go(State *pp, int er, int ec)
{
	int vis[MAPSIZE*MAPSIZE];
	int dir[MAPSIZE][MAPSIZE];
	int mfa[MAPSIZE][MAPSIZE];
	int mque[10000];
	int f = 0, r = 1;
	int i, j;
	int col = mele->col;
	int p = (pp->man_r) * col + (pp->man_c);
	memset(vis, 0, sizeof(vis));
	for(i = 0; i < MAPSIZE; i++){
		for(j = 0; j < MAPSIZE; j++){
			mfa[i][j] = dir[i][j] = -1;
		}
	}

	mque[f] = p;
	vis[p] = 1;
	mfa[pp->man_r][pp->man_c] = p;
	while(f < r){
		int jr, jc;
		p = mque[f++];
		jr = p / col;
		jc = p % col;
		if(jr == er && jc == ec)
			break;
		for(i = 0; i < 4; i++){
			int nr = jr + gr[i];
			int nc = jc + gc[i];
			int np = nr * col + nc;
			if(pp->m[nr][nc] != mele->wall_g && pp->m[nr][nc] != mele->box_g && !vis[np]){
				vis[np] = 1;
				mque[r++] = np;
				mfa[nr][nc] = p;
				dir[nr][nc] = i;
			}
		}
	}
	if(r > 1)
		man_real_go(er, ec, mfa, dir);
}

void print_ans(State *p, State *sub)
{
	int d1 = sub->d1;
	int d2 = sub->d2;
	if(p->fa == NULL){
		man_go(p, sub->man_r-d1, sub->man_c-d2);
		usleep(500*1000);
		move_it(cur_win, mele, mele->box_g, sub->man_r, sub->man_c, d1, d2);
		move_it(cur_win, mele, mele->man_g, sub->man_r-d1, sub->man_c-d2, d1, d2);
		return;
	}

	print_ans(p->fa, p);
	man_go(p, sub->man_r-d1, sub->man_c-d2);
	usleep(500*1000);
	move_it(cur_win, mele, mele->box_g, sub->man_r, sub->man_c, d1, d2);
	move_it(cur_win, mele, mele->man_g, sub->man_r-d1, sub->man_c-d2, d1, d2);
}

void print_count(WINDOW *win_ptr, State *st)
{
	int n = 0;
	chose_color(win_ptr, 1);
	mvwprintw(win_ptr, n++, 0, "stat: %d", co.state_count);
	mvwprintw(win_ptr, n++, 0, "hit:  %d", co.hit_count);
	mvwprintw(win_ptr, n++, 0, "same: %d", co.same_count);
	mvwprintw(win_ptr, n++, 0, "time: %.2lf s", use_time);
	mvwprintw(win_ptr, n++, 0, "push: %d", st->g);
	mvwprintw(win_ptr, n++, 0, "move: %d", st->move);
	mvwprintw(win_ptr, n++, 0, "len:  %d", co.sac);
	wrefresh(win_ptr);
}

State *DFS(State *cur_st, State *end_st, int depth)
{
	int cnt;
#if 0
	mvwprintw(cur_win, 0, 0, "(%d, %d)", cur_st->bcoor[0].r, cur_st->bcoor[0].c);
	wrefresh(cur_win);
	sleep(3);
#endif
	if(cur_st->mark_val == end_st->mark_val){
		return cur_st;
	}
	if(cur_st->f > depth){
		return NULL;
	}
	for(cnt = 0; cnt < mele->box_count; cnt++){
		Coor tco;
		int i;
		tco.r = cur_st->bcoor[cnt].r;
		tco.c = cur_st->bcoor[cnt].c;
		for(i = 0; i < 4; i++){
			State *st = try_move(cur_st, &tco, gr[i], gc[i]);
			if(st != NULL){
				State *ans = DFS(st, end_st, depth);
				if(ans != NULL){
					return ans;
				}
				free(st);
			}
		}
	}
	return NULL;
}

void *IDA_star(void *arg)
{
	Thread_arg targ = *(Thread_arg *)arg;
	WINDOW *count_win = targ.win_ptr;
	clock_t beg_time;
	clock_t end_time;
	beg_time = clock();
	State *beg_st = targ.beg_st;
	State *end_st = targ.end_st;
	State *ans;
	int res;
	int depth;
	res = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	if(res != 0){
		fprintf(stderr, "thread pthread_setcancelstate failed\n");
		exit(EXIT_FAILURE);
	}
#if 0
	res = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
#endif
	res = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	if(res != 0){
		fprintf(stderr, "thread pthread_setcanceltype failed\n");
		exit(EXIT_FAILURE);
	}
	depth = beg_st->h - 1;
	ans = NULL;
	while(ans == NULL){
		depth += 1;
		clear_hash();
		ans = DFS(beg_st, end_st, depth);
	}
	end_time = clock();
	use_time = (double)(end_time - beg_time) / CLOCKS_PER_SEC;
	print_count(count_win, ans);
	print_ans(ans->fa, ans);
	pthread_exit(NULL);
}


void computer_play(WINDOW *win_ptr, MAPELE *mapele)
{
	State *end_st, *beg_st;
	WINDOW *count_win;
	pthread_t IDA_star_thread;
	Thread_arg arg;
	int res;
	int key = -1;

	init_tools();

	count_win = newwin(10, 15, 0, 0);
	mele = mapele;
	cur_win = win_ptr;

	end_st = get_end_state();
	beg_st = new_state(BOX);


	arg.win_ptr = count_win;
	arg.beg_st = beg_st;
	arg.end_st = end_st;
	res = pthread_create(&IDA_star_thread, NULL, IDA_star, (void *)&arg);
	if(res != 0){
		fprintf(stderr, "A_star thread creation failed.\n");
	}

	while(1){
		key = wgetch(win_ptr);
		if(key == KEY_ENTER || key == '\n' || key == 'q'){
			res = pthread_cancel(IDA_star_thread);
			if(res != 0){
				fprintf(stderr, "A_star thread cancel failed.\n");
			}
			break;
		}
	}

	res = pthread_join(IDA_star_thread, NULL);
	if(res != 0){
		fprintf(stderr, "A_star thread join failed.\n");
	}

	clear_hash();
	free(end_st);

	wclear(count_win);
	wrefresh(count_win);
	delwin(count_win);
}
