#include <string.h>
#include "action.h"
#include "ai.h"

#define JBW(r, c) \
	(tbox[r][c] == mele->box_g || tbox[r][c] == mele->wall_g)

typedef struct{
	int r, c;
	int boxcount;
	int fillcount;
}Ncoor;

/* 之字形死锁 */
int judge_kill1(char tbox[][MAPSIZE], int br, int bc)
{
	Ncoor Stack[1000];
	int v[MAPSIZE][MAPSIZE];
	int top;
	int dead4 = 0;
	int dead[4];
	int fill[4];
	int i, k;
	int flag;
	int beg;
	int bcount = 0;
	int fcount = 0;

	bcount = 1;
	if(NIL_BOX[br][bc] == mele->nil_box_g)
		fcount = 1;

	for(k = 0; k < 4; k++){
		int bbr = br+gr[k];
		int bbc = bc+gc[k];
		dead[k] = 0;
		fill[k] = 1;
		if(JBW(bbr, bbc)){
			Ncoor dir;
			dir.r = bbr;
			dir.c = bbc;
			dir.boxcount = bcount;
			dir.fillcount = fcount;
			memset(v, 0, sizeof(v));
			v[br][bc] = 1;
			v[dir.r][dir.c] = 1;
			if(tbox[dir.r][dir.c] == mele->box_g)
				dir.boxcount++;
			if(NIL_BOX[dir.r][dir.c] == mele->nil_box_g)
				dir.fillcount++;
			if(k < 2)
				flag = 2;
			else 
				flag = 0;
			top = 0;
			Stack[top++] = dir;
			while(top){
				Ncoor d = Stack[top-1];
				if(tbox[d.r][d.c] == mele->wall_g){
					if(d.boxcount != d.fillcount){
						fill[k] = 0;
					}
					dead[k] = 1;
					break;
				}
				beg = flag;
				for(i = beg; i < beg+2; i++){
					Ncoor nd;
					int tbr;
					int tbc;
					nd.r = d.r+gr[i];
					nd.c = d.c+gc[i];
					nd.boxcount = d.boxcount;
					nd.fillcount = d.fillcount;
					tbr = nd.r;
					tbc = nd.c;
					if(JBW(tbr, tbc) && !v[tbr][tbc]){
						int tbeg;
						if(flag == 2) tbeg = 0;
						else tbeg = 2;
						v[tbr][tbc] = 1;
						if(tbox[tbr][tbc] == mele->box_g){
							int ndr1 = tbr + gr[tbeg];
							int ndc1 = tbc + gc[tbeg];
							int ndr2 = tbr + gr[tbeg+1];
							int ndc2 = tbc + gc[tbeg+1];
							nd.boxcount++;
							if(v[ndr1][ndc1] && tbox[ndr1][ndc1] == mele->box_g){
								if(ndr1 == br && ndc1 == bc)
									dead4 = 1;
							}
							else if(v[ndr2][ndc2] && tbox[ndr2][ndc2] == mele->box_g){
								if(ndr2 == br && ndc2 == bc)
									dead4 = 1;
							}
						}
						if(NIL_BOX[tbr][tbc] == mele->nil_box_g)
							nd.fillcount++;
						if(dead4){
							dead[k] = 1;
							if(nd.boxcount != nd.fillcount)
								fill[k] = 0;
							dead4 = 0;
							break;
						}
						Stack[top++] = nd;
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
	if(dead[0] && dead[2] && !(fill[0] && fill[2]))
		return 1;
	if(dead[0] && dead[3] && !(fill[0] && fill[3]))
		return 1;
	if(dead[1] && dead[2] && !(fill[1] && fill[2]))
		return 1;
	if(dead[1] && dead[3] && !(fill[1] && fill[3]))
		return 1;
	return 0;
}

int kill(char g[][MAPSIZE], int br, int bc)
{
	if(judge_kill1(g, br, bc))
		return 1;
	return 0;
}
