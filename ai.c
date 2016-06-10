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
int dist[MAPSIZE][MAPSIZE][BOXCOUNT];
LL mark[2][MAPSIZE][MAPSIZE];
Coor nil_bps[100];

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

void init_distant()
{
	int mque[10000];
	int vis[MAPSIZE*MAPSIZE];
	int len[MAPSIZE*MAPSIZE];
	int r, c, k;
	int row = mele->row;
	int col = mele->col;
	for(r = 0; r < row; r++){
		for(c = 0; c < col; c++){
			for(k = 0; k < mele->box_count; k++){
				dist[r][c][k] = inf;
				int er = nil_bps[k].r;
				int ec = nil_bps[k].c;
				if(NIL_BOX[r][c] != mele->wall_g){
					int front = 0, rear = 1;
					int node = r * col + c;
					memset(vis, 0, sizeof(vis));
					memset(len, 0, sizeof(len));
					mque[front] = node;
					vis[node] = 1;
					while(front < rear){
						int cur = mque[front++];
						int cr = cur / col;
						int cc = cur % col;
						int i;
						if(cr == er && cc == ec){
							dist[r][c][k] = len[cur];
							break;
						}
						for(i = 0; i < 4; i++){
							int nr = cr + gr[i];
							int nc = cc + gc[i];
							int nnode = nr * col + nc;
							if(nr < 0 || nr >= row || nc < 0 || nc >= col)
								continue;
							if(NIL_BOX[nr][nc] != mele->wall_g && NIL_BOX[nr][nc] != mele->box_g && !vis[nnode]){
								vis[nnode] = 1;
								len[nnode] = len[cur] + 1;
								mque[rear++] = nnode;
							}
						}
					}
				}
			}
		}
	}

}

int state_cmp(const void *_s1, const void *_s2)
{
	State **s1 = (State **)_s1;
	State **s2 = (State **)_s2;
	if((*s1)->f != (*s2)->f)
		return (*s1)->f - (*s2)->f;
	return (*s1)->mlen - (*s2)->mlen;
}

void go_there(State *st, int len[][MAPSIZE])
{
	int mque[10000];
	int vis[MAPSIZE*MAPSIZE];
	int f = 0, r = 1;
	int i;
	int col = mele->col;
	int mr = st->man_r;
	int mc = st->man_c;
	memset(vis, 0, sizeof(vis));
	mque[f] = mr * col + mc;
	vis[mque[f]] = 1;
	len[mr][mc] = 0;
	while(f < r){
		int p = mque[f++];
		int jr = p / col;
		int jc = p % col;
		for(i = 0; i < 4; i++){
			int nr = jr + gr[i];
			int nc = jc + gc[i];
			int np = nr * col + nc;
			if(NIL_BOX[nr][nc] != mele->wall_g && st->m[nr][nc] != mele->box_g && !vis[np]){
				vis[np] = 1;
				len[nr][nc] = len[jr][jc] + 1;
				mque[r++] = np;
			}
		}
	}
}

int dfs(int u, int **arr, int m[][BOXCOUNT])
{  
	int v;
	int box_count = mele->box_count;
	arr[2][u] = 1;
    for(v = 0; v < box_count; v++){  
		if(!arr[3][v]){
			int t = arr[0][u] + arr[1][v] - m[u][v];
			if(!t){
				arr[3][v] = 1;
				if(arr[5][v] == -1 || dfs(arr[5][v], arr, m)){  
					arr[5][v] = u;  
					return 1;  
				}  
			}
			else
				if(t < arr[4][v])
					arr[4][v] = t;
		}
    }  
    return 0;  
}  

int bfs(int cur_u, int **arr, int m[][BOXCOUNT])
{
	int que[20000];
	int pre[BOXCOUNT];
	int front = 0;
	int rear = 0;
	int ok = 0;
	int bcount = mele->box_count;
	memset(pre, -1, sizeof(pre));
	que[rear++] = cur_u;
	while(front < rear){
		int u = que[front++];
		int v;
		arr[2][u] = 1;
		for(v = 0; v < bcount; v++){
			int t;
			if(arr[3][v])
				continue;
			t = arr[0][u] + arr[1][v] - m[u][v];
			if(!t){
				arr[3][v] = 1;
				if(arr[5][v] == -1){
					int l = u;
					int r = v;
					ok = 1;
					while(l != -1){
						int tmp = arr[6][l];
						arr[5][r] = l;
						arr[6][l] = r;
						l = pre[l];
						r = tmp;
					}
					break;
				}
				else{
					pre[arr[5][v]] = u;
					que[rear++] = arr[5][v];
				}
			}
			else{
				if(t < arr[4][v])
					arr[4][v] = t;
			}
		}
		if(ok) break;
	}
	return ok;
}

int H(Coor *bcoor)
{
	int m[BOXCOUNT][BOXCOUNT];
	int lx[BOXCOUNT];
	int ly[BOXCOUNT];
	int vx[BOXCOUNT];
	int vy[BOXCOUNT];
	int slack[BOXCOUNT];
	int mylink_l[BOXCOUNT];
	int mylink_r[BOXCOUNT];
	int *arr[7];
	int i, j;
	int bcount = mele->box_count;
	int dis = 0;
	arr[0] = lx;
	arr[1] = ly;
	arr[2] = vx;
	arr[3] = vy;
	arr[4] = slack;
	arr[5] = mylink_l;
	arr[6] = mylink_r;

	memset(mylink_l, -1, sizeof(mylink_l));
	memset(mylink_r, -1, sizeof(mylink_r));
	memset(ly, 0, sizeof(ly));

	for(i = 0; i < bcount; i++){
		lx[i] = -inf;
	}
	for(i = 0; i < bcount; i++){
		Coor beg;
		beg.r = bcoor[i].r;
		beg.c = bcoor[i].c;
		for(j = 0; j < bcount; j++){
			m[i][j] = -dist[beg.r][beg.c][j];
			if(m[i][j] > lx[i])
				lx[i] = m[i][j];
		}
	}

	for(i = 0; i < bcount; i++){
		while(1){
			int d = inf;
			for(j = 0; j < bcount; j++){
				slack[j] = inf;
			}
			memset(vx, 0, sizeof(vx));
			memset(vy, 0, sizeof(vy));
			if(dfs(i, arr, m))
				break;
			for(j = 0; j < bcount; j++){
				if(!vy[j] && slack[j] < d)
					d = slack[j];
			}
			for(j = 0; j < bcount; j++){
				if(vx[j])
					lx[j] -= d;
			}
			for(j = 0; j < bcount; j++){
				if(vy[j])
					ly[j] += d;
			}
		}
	}
	for(i = 0; i < bcount; i++)
		dis += m[mylink_l[i]][i];
	return -dis;
}

int H1(State *st)
{
	int cnt;
	int r, c;
	int total_dis = 0;
	int vis[MAPSIZE][MAPSIZE];
	memset(vis, 0, sizeof(vis));
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
						vis[r][c] = 1;
						if(dis != inf){
							vis[tco.r][tco.c] = 0;
						}
						tco.r = r;
						tco.c = c;
						dis = d;
					}
				}
			}
		}
		total_dis += dis;
	}
	return 2*total_dis;
}

State *new_state(State *old_st, int cnt, int d1, int d2)
{
	State *st = NULL;
	st = (State *)malloc(sizeof(State));
	int old_br = old_st->bcoor[cnt].r;
	int old_bc = old_st->bcoor[cnt].c;
	int i;
	memset(st->m, '\0', sizeof(st->m));
	st->fa = old_st;
	st->g = old_st->g + 1;
	st->man_r = old_br;
	st->man_c = old_bc;
	st->d1 = d1;
	st->d2 = d2;
	st->mark_val = old_st->mark_val;
	st->mark_val ^= mark[0][old_br][old_bc];
	st->mark_val ^= mark[0][old_br+d1][old_bc+d2];

	for(i = 0; i < mele->box_count; i++){
		st->bcoor[i].r = old_st->bcoor[i].r;
		st->bcoor[i].c = old_st->bcoor[i].c;
	}
	st->bcoor[cnt].r = old_br + d1;
	st->bcoor[cnt].c = old_bc + d2;
	for(i = 0; i < mele->box_count; i++)
		st->m[st->bcoor[i].r][st->bcoor[i].c] = mele->box_g;

	st->h = H(st->bcoor);
	st->f = st->g + st->h;
	st->next_count = 0;
	return st;
}

State *get_beg_state()
{
	State *beg_st;
	int r, c, u = 0;
	int i;
	beg_st = (State *)malloc(sizeof(State));
	memset(beg_st->m, '\0', sizeof(beg_st->m));
	beg_st->mark_val = 0;
	for(r = 0; r < mele->row; r++){
		for(c = 0; c < mele->col; c++){
			if(BOX[r][c] == mele->box_g){
				(beg_st->mark_val) ^= mark[0][r][c];
				beg_st->bcoor[u].r = r;
				beg_st->bcoor[u].c = c;
				u++;
			}
			if(BOX[r][c] == mele->man_g){
				beg_st->man_r = r;
				beg_st->man_c = c;
			}
		}
	}
	for(i = 0; i < u; i++)
		beg_st->m[beg_st->bcoor[i].r][beg_st->bcoor[i].c] = mele->box_g;
	beg_st->g = beg_st->move = 0;
	beg_st->h = H(beg_st->bcoor);
	beg_st->f = beg_st->g + beg_st->h;
	beg_st->fa = NULL;
	beg_st->next_count = 0;
	return beg_st;
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
				(end_st->mark_val) ^= mark[0][r][c];
				end_st->bcoor[u].r = r;
				end_st->bcoor[u].c = c;
				nil_bps[u].r = r;
				nil_bps[u].c = c;
				u++;
			}
		}
	}
	return end_st;
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
			Hashele *next, *cur;
			cur = hashtable[i];
			while(cur){
				next = cur->next;
				free(cur);
				cur = next;
			}
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
	co.hash_count = 0;
	co.same_count = 0;
	co.move_count = 0;
	co.depth = 0;
	co.sac = 0;
}

int get_hashval(State *st)
{
	return (st->mark_val)&(HASH_SIZE-1);
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

int try_match(Hashele *ht, State *st, int check[][MAPSIZE])
{
	Hashele *htmp = ht, *new_ht;
	int c = 0;
	int mat = 0;
	while(1){
		if(htmp->key == st->mark_val && check[htmp->mr][htmp->mc] != -1){
			if(st->g < htmp->step){
				htmp->step = st->g;
				htmp->mr = st->man_r;
				htmp->mc = st->man_c;
				co.state_count--;
				return -1;
			}
			co.same_count++;
			return htmp->step;
		}
		if(htmp->key != st->mark_val)
			mat = 1;
		c++;
		if(htmp->next == NULL){
			break;
		}
		htmp = htmp->next;
	}
	new_ht = (Hashele *)malloc(sizeof(Hashele));
	new_ht->key = st->mark_val;
	new_ht->step = st->g;
	new_ht->mr = st->man_r;
	new_ht->mc = st->man_c;
	new_ht->next = NULL;
	htmp->next = new_ht;

	if(mat) co.hit_count++;
	if(c > co.sac) co.sac = c;
	return -1;
}

int find_hash(State *st, int val, int check[][MAPSIZE])
{
	int ins = val;
	if(hashtable[ins] != NULL){
		return try_match(hashtable[ins], st, check);
	}
	Hashele *new_ele = (Hashele *)malloc(sizeof(Hashele));
	new_ele->key = st->mark_val;
	new_ele->step = st->g;
	new_ele->mr = st->man_r;
	new_ele->mc = st->man_c;
	new_ele->next = NULL;

	hashtable[ins] = new_ele;
	co.hash_count++;
	return -1;
}

int try_insert(State *st, int check[][MAPSIZE])
{
	int hashvalue = get_hashval(st);
	return find_hash(st, hashvalue, check);
}

State *go_next(State *fa, int cnt, int d1, int d2)
{
	int br = fa->bcoor[cnt].r;
	int bc = fa->bcoor[cnt].c;
	int ki;
	State *p = NULL;
	fa->m[br][bc] = '\0';
	fa->m[br+d1][bc+d2] = mele->box_g;
	ki = kill(fa->m, br+d1, bc+d2, d1, d2);
	if(!ki){ /* 不产生死锁 */
		p = new_state(fa, cnt, d1, d2);
	}
	fa->m[br][bc] = mele->box_g;
	fa->m[br+d1][bc+d2] = '\0';
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
			if(NIL_BOX[nr][nc] != mele->wall_g && pp->m[nr][nc] != mele->box_g && !vis[np]){
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
	if(st){
		mvwprintw(win_ptr, n++, 0, "stat: %d", co.state_count);
		mvwprintw(win_ptr, n++, 0, "hash: %d", co.hash_count);
		mvwprintw(win_ptr, n++, 0, "hit:  %d", co.hit_count);
		mvwprintw(win_ptr, n++, 0, "same: %d", co.same_count);
		mvwprintw(win_ptr, n++, 0, "time: %.2lf s", use_time);
		mvwprintw(win_ptr, n++, 0, "deep: %d", co.depth);
		mvwprintw(win_ptr, n++, 0, "push: %d", st->g);
		mvwprintw(win_ptr, n++, 0, "move: %d", st->move);
		mvwprintw(win_ptr, n++, 0, "len:  %d", co.sac);
	}
	else{
		mvwprintw(win_ptr, n++, 0, "stat:  %d", co.state_count);
	}
	wrefresh(win_ptr);
}

State *DFS(State *cur_st, State *end_st, int depth, int *minf)
{
	int check[MAPSIZE][MAPSIZE];
	int cnt;
	int step;

	if(cur_st->mark_val == end_st->mark_val){
		return cur_st;
	}

	if(cur_st->f > depth){
		if(cur_st->f < (*minf)){
			*minf = cur_st->f;
		}
		return NULL;
	}

	if(co.state_count == 2000000) /* 限制结点数 */
		return NULL;

	memset(check, -1, sizeof(check));
	go_there(cur_st, check); /* 预处理人所能到达到位置 */

	step = try_insert(cur_st, check);
	if(step != -1){ /* 此状态已搜索过 */
		return NULL;
	}

	co.state_count++;

	cur_st->next_count = 0;
	for(cnt = 0; cnt < (BOXCOUNT<<2); cnt++){
		cur_st->next[cnt] = NULL;
	}

	for(cnt = 0; cnt < mele->box_count; cnt++){
		Coor tco;
		int i;
		tco.r = cur_st->bcoor[cnt].r;
		tco.c = cur_st->bcoor[cnt].c;
		for(i = 0; i < 4; i++){
			int d1 = gr[i];
			int d2 = gc[i];
			char ch = cur_st->m[tco.r+d1][tco.c+d2];
			int len = check[tco.r-d1][tco.c-d2];
			if(len == -1)
				continue;
			if(NIL_BOX[tco.r+d1][tco.c+d2] == mele->wall_g || ch == mele->box_g)
				continue;
			State *st = go_next(cur_st, cnt, d1, d2);
			if(st != NULL){
				st->mlen = len;
				st->move = cur_st->move + len + 1;
				cur_st->next[cur_st->next_count++] = st;
			}
		}
	}

	qsort(cur_st->next, cur_st->next_count, sizeof(cur_st->next[0]), state_cmp);

	for(cnt = 0; cnt < cur_st->next_count; cnt++){
		State *st = cur_st->next[cnt];
		State *ans = DFS(st, end_st, depth, minf);
		if(ans != NULL){
			return ans;
		}
		free(st);
		cur_st->next[cnt] = NULL;
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
	int minf;
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
	depth = beg_st->h;
	ans = NULL;
	minf = inf;
	while(ans == NULL && co.state_count < 2000000){
		clear_hash();
		co.depth++;
		co.state_count = 0;
		co.same_count = 0;
		co.move_count = 0;
		co.hit_count = 0;
		co.hash_count = 0;
		co.sac = 0;
		ans = DFS(beg_st, end_st, depth, &minf);
		depth = minf;
		minf = inf;
	}
	end_time = clock();
	use_time = (double)(end_time - beg_time) / CLOCKS_PER_SEC;

	print_count(count_win, ans);
	print_ans(ans->fa, ans);
	pthread_exit(NULL);
}

void free_tree(State *st)
{
	int cnt;
	if(st == NULL)
		return;
	for(cnt = 0; cnt < st->next_count; cnt++){
		free_tree(st->next[cnt]);
	}
	free(st);
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
	init_distant();
	beg_st = get_beg_state();


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
	free_tree(beg_st);
	free(end_st);

	wclear(count_win);
	wrefresh(count_win);
	delwin(count_win);
}
