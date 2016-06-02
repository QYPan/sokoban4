#include <string.h>
#include "action.h"
#include "ai.h"

#define JBW(r, c) \
	(tbox[r][c] == mele->box_g || tbox[r][c] == mele->wall_g)

int judge_kill(char tbox[][MAPSIZE], MAPELE *mele, int br, int bc)
{
	Coor Stack[1000];
	int v[MAPSIZE][MAPSIZE];
	int top;
	int dead = 0;
	int dead1 = 0;
	int dead2 = 0;
	int i, k;
	int flag;
	int beg;
	int boxcount = 0;
	int fillcount = 0;
	memset(v, 0, sizeof(v));

	v[br][bc] = 1;
	if(NIL_BOX[br][bc] == mele->nil_box_g)
		fillcount++;

	for(k = 0; k < 4; k++){
		if(JBW(br+gr[k], bc+gc[k])){
			Coor dir;
			dir.r = br+gr[k];
			dir.c = bc+gc[k];
			top = 0;
			Stack[top++] = dir;
			v[dir.r][dir.c] = 1;
			if(tbox[dir.r][dir.c] == mele->box_g)
				boxcount = 2;
			if(NIL_BOX[dir.r][dir.c] == mele->nil_box_g)
				fillcount++;
			if(k < 2)
				flag = 2;
			else 
				flag = 0;
			while(top && !dead){
				Coor d = Stack[top-1];
				if(tbox[d.r][d.c] == mele->wall_g){
					if(k < 2) dead1 = 1;
					else dead2 = 1;
					break;
				}
				beg = flag;
				for(i = beg; i < beg+2; i++){
					Coor nd;
					nd.r = d.r+gr[i];
					nd.c = d.c+gc[i];
					if(JBW(nd.r, nd.c) && !v[nd.r][nd.c]){
						int tbeg;
						if(flag == 2) tbeg = 0;
						else tbeg = 2;
						if(tbox[nd.r][nd.c] == mele->box_g && (v[nd.r+gr[tbeg]][nd.c+gc[tbeg]] || v[nd.r+gr[tbeg+1]][nd.c+gc[tbeg+1]])){
							dead = 1;
							break;
						}
						v[nd.r][nd.c] = 1;
						Stack[top++] = nd;
						if(tbox[nd.r][nd.c] == mele->box_g)
							boxcount++;
						if(NIL_BOX[nd.r][nd.c] == mele->nil_box_g)
							fillcount++;
						break;
					}
				}
				if(i == beg+2){
					top--;
				}
				if(flag == 0)
					flag = 2;
				else 
					flag = 0;
			}
		}
	}
	printf("fillcount: %d\n", fillcount);
	printf("boxcount: %d\n", boxcount);
	printf("dead: %d\n", dead);
	printf("dead1: %d\n", dead1);
	printf("dead2: %d\n", dead2);
	if(fillcount == boxcount)
		return 0;
	if(dead || (dead1 && dead2))
		return 1;
	return 0;
}

int kill(char g[][MAPSIZE], MAPELE *mele, int br, int bc)
{
	if(judge_kill(g, mele, br, bc))
		return 1;
}
