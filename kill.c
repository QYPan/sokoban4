#include <string.h>
#include "action.h"
#include "ai.h"

#define JBW(r, c) \
	(tbox[r][c] == mele->box_p || tbox[r][c] == mele->wall_p)

char tbox[MAPSIZE][MAPSIZE];

int judge_kill(MAPELE *mele, int br, int bc)
{
	Coor Stack[1000];
	int v[MAPSIZE][MAPSIZE];
	int top;
	int dead = 0;
	int dead1 = 0;
	int dead2 = 0;
	int i, k;
	int flag;
	for(k = 0; k < 4; k++){
		if(JBW(br+gr[k], bc+gc[k])){
			Coor dir;
			memset(v, 0, sizeof(v));
			v[br][bc] = 1;
			dir.r = br+gr[k];
			dir.c = bc+gc[k];
			top = 0;
			Stack[top++] = dir;
			v[dir.r][dir.c] = 1;
			if(k < 2)
				flag = 2;
			else 
				flag = 0;
			while(top){
				int beg;
				Coor d = Stack[top];
				if(tbox[d.r][d.c] == mele->wall_p){
					if(k < 2) dead1 = 1;
					else dead2 = 1;
					break;
				}
				vis[d.r][d.c] = 1;
				beg = flag;
				if()
				for(i = beg; i < beg+2; i++){
					Coor nd;
					nd.r = d.r+gr[i];
					nd.c = d.c+gc[i];
					if(JBW(nd.r, nd.c) && !v[nd.r][nd.c]){
						v[nd.r][nd.c] = 1;
						Stack[top++] = nd;
						break;
					}
				}
				if(i == beg+2){
					top--;
				}
				if(beg == 0)
					beg = 2;
				else 
					beg = 0;
			}
		}
	}
}

int kill(char g[][MAPSIZE], MAPELE *mele, int br, int bc)
{
	int r, c;
	for(r = 0; r < MAPSIZE; r++){
		for(c = 0; c < MAPSIZE; c++){
			tbox[r][c] = g[r][c];
		}
	}
	if(judge_kill(mele, br, bc))
		return 1;
}
