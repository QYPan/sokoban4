#include "action.h"
#include "ai.h"

int row;
int col;

int judge_nil(int r, int c)
{
	if(NIL_BOX[r][c] == 'O')
		return 1;
	return 0;
}

int judge_bw(int r, int c)
{
	if(BOX[r][c] == '#' || BOX[r][c] == '@')
		return 1;
	return 0;
}

	/*      @@
	 *      @@
	 */    
int kill1(int br, int bc, int d1, int d2)
{
#if 0
	return 0;
#endif
	int count = 0;
	int countb = 0;
	if(judge_nil(br, bc))
		count++;
	if(judge_nil(br+d1, bc+d2))
		count++;
	if(judge_nil(br, bc+d2))
		count++;
	if(judge_nil(br+d1, bc))
		count++;
	if(BOX[br][bc] == '@')
		countb++;
	if(BOX[br+d1][bc+d2] == '@')
		countb++;
	if(BOX[br+d1][bc] == '@')
		countb++;
	if(BOX[br][bc+d2] == '@')
		countb++;
	if((BOX[br+d1][bc+d2] == '@' || BOX[br+d1][bc+d2] == '#') && (BOX[br][bc+d2] == '@' || BOX[br][bc+d2] == '#') && (BOX[br+d1][bc] == '@' || BOX[br+d1][bc] == '#')){
		if(count != countb)
			return 1;
	}
	return 0;
}

	/*    #####
	 *    # @ #
	 */    
int kill2(int br, int bc, int d1, int d2)
{
#if 0
	return 0;
#endif
	if(BOX[br+d1][bc+d2] == '#'){
		int i, j;
		if(judge_nil(br, bc))
			return 0;
		if(d1){
			int ok1 = 1, ok2 = 1;
			i = br + d1;
			for(j = bc - 1; j >= 0; j--){
				if(BOX[i][j] != '#' || judge_nil(i - d1, j)){
					ok1 = 0;
					break;
				}
				if(BOX[i-d1][j] == '#')
					break;
			}
			for(j = bc + 1; j < col; j++){
				if(BOX[i][j] != '#' || judge_nil(i - d1, j)){
					ok2 = 0;
					break;
				}
				if(BOX[i-d1][j] == '#')
					break;
			}
			if(ok1 && ok2)
				return 1;
		}
		else if(d2){
			int ok1 = 1, ok2 = 1;
			j = bc + d2;
			for(i = br - 1; i >= 0; i--){
				if(BOX[i][j] != '#' || judge_nil(i, j - d2)){
					ok1 = 0;
					break;
				}
				if(BOX[i][j-d2] == '#')
					break;
			}
			for(i = br + 1; i < row; i++){
				if(BOX[i][j] != '#' || judge_nil(i, j - d2)){
					ok2 = 0;
					break;
				}
				if(BOX[i][j-d2] == '#')
					break;
			}
			if(ok1 && ok2)
				return 1;
		}
	}
	return 0;
}

	/*    ######
	 *    @    @
	 *    ######
	 */    
int kill3(int br, int bc, int d1, int d2)
{
#if 0
	return 0;
#endif
	if(d1){
		int left = bc - 1;
		int right = bc + 1;
		int i;
		int count = 0;
		for(i = br; i >= 0 && i < row; i += d1){
			if((BOX[i][left] == '#' && BOX[i][right] == '#') || (((BOX[i][left] == '#' || BOX[i][right] == '#')) && BOX[i][bc] == '@')){
				if(judge_nil(i, bc))
					count += 1;
				if(i != br){
					if(BOX[i][bc] == '@' && count < 2)
						return 1;
				}
				if(BOX[i][bc] == '#' && count < 1)
					return 1;
			}
			else 
				break;
		}
	}
	else if(d2){
		int up = br - 1;
		int down = br + 1;
		int i;
		int count = 0;
		for(i = bc; i >= 0 && i < col; i += d2){
			if((BOX[up][i] == '#' && BOX[down][i] == '#') || (((BOX[up][i] == '#' || BOX[down][i] == '#')) && BOX[br][i] == '@')){
				if(judge_nil(br, i))
					count += 1;
				if(i != bc){
					if(BOX[br][i] == '@' && count < 2)
						return 1;
				}
				if(BOX[br][i] == '#' && count < 1)
					return 1;
			}
			else 
				break;
		}
	}
	return 0;
}

	/*    #@    
	 *    @ @   
	 *     @#   
	 */    
int kill4(int br, int bc, int d1, int d2)
{
#if 0
	return 0;
#endif 
	if(d1){
		if(0 <= br+d1+d1 && br+d1+d1 < row){
			int count = 0;
			int countb = 0;
			if(judge_nil(br, bc))
				count++;
#if 1
			if(judge_nil(br+d1, bc+d2))
				count++;
#endif
			if(judge_nil(br+d1, bc-1))
				count++;
			if(judge_nil(br+d1, bc+1))
				count++;
			if(judge_nil(br+d1+d1, bc))
				count++;
			if(BOX[br][bc] == '@')
				countb++;
			if(BOX[br+d1][bc-1] == '@')
				countb++;
			if(BOX[br+d1][bc+1] == '@')
				countb++;
			if(BOX[br+d1+d1][bc] == '@')
				countb++;
			if((BOX[br+d1][bc-1] == '#' || BOX[br+d1][bc-1] == '@') && (BOX[br+d1][bc+1] == '#' || BOX[br+d1][bc+1] == '@') && (BOX[br+d1+d1][bc] == '#' || BOX[br+d1+d1][bc] == '@')){
				if((BOX[br+d1+d1][bc-1] == '#' || BOX[br+d1+d1][bc-1] == '@') && (BOX[br][bc+1] == '#' || BOX[br][bc+1] == '@')){
					if(judge_nil(br, bc+1))
						count++;
					if(judge_nil(br+d1+d1, bc-1))
						count++;
					if(BOX[br][bc+1] == '@')
						countb++;
					if(BOX[br+d1+d1][bc-1] == '@')
						countb++;
					if(count < countb)
						return 1;
				}
				else if((BOX[br+d1+d1][bc+1] == '#' || BOX[br+d1+d1][bc+1] == '@') && (BOX[br][bc-1] == '#' || BOX[br][bc-1] == '@')){
					if(judge_nil(br, bc-1))
						count++;
					if(judge_nil(br+d1+d1, bc+1))
						count++;
					if(BOX[br][bc-1] == '@')
						countb++;
					if(BOX[br+d1+d1][bc+1] == '@')
						countb++;
					if(count < countb)
						return 1;
				}
			}
		}
	}
	else if(d2){
		if(0 <= bc+d2+d2 && bc+d2+d2 < col){
			int count = 0;
			int countb = 0;
			if(judge_nil(br, bc))
				count++;
#if 1
			if(judge_nil(br+d1, bc+d2))
				count++;
#endif
			if(judge_nil(br-1, bc+d2))
				count++;
			if(judge_nil(br+1, bc+d2))
				count++;
			if(judge_nil(br, bc+d2+d2))
				count++;
			if(BOX[br][bc] == '@')
				countb++;
			if(BOX[br-1][bc+d2] == '@')
				countb++;
			if(BOX[br+1][bc+d2] == '@')
				countb++;
			if(BOX[br][bc+d2+d2] == '@')
				countb++;
			if((BOX[br-1][bc+d2] == '#' || BOX[br-1][bc+d2] == '@') && (BOX[br+1][bc+d2] == '#' || BOX[br+1][bc+d2] == '@') && (BOX[br][bc+d2+d2] == '#' || BOX[br][bc+d2+d2] == '@')){
				if((BOX[br-1][bc+d2+d2] == '#' || BOX[br-1][bc+d2+d2] == '@') && (BOX[br+1][bc] == '#' || BOX[br+1][bc] == '@')){
					if(judge_nil(br+1, bc))
						count++;
					if(judge_nil(br-1, bc+d2+d2))
						count++;
					if(BOX[br+1][bc] == '@')
						countb++;
					if(BOX[br-1][bc+d2+d2] == '@')
						countb++;
					if(count < countb)
						return 1;
				}
				else if((BOX[br+1][bc+d2+d2] == '#' || BOX[br+d1][bc+d2+d2] == '@') && (BOX[br-1][bc] == '#' || BOX[br-1][bc] == '@')){
					if(judge_nil(br-1, bc))
						count++;
					if(judge_nil(br+1, bc+d2+d2))
						count++;
					if(BOX[br-1][bc] == '@')
						countb++;
					if(BOX[br+1][bc+d2+d2] == '@')
						countb++;
					if(count < countb)
						return 1;
				}
			}
		}
	}
	return 0;
}

/*
 *  #@@#
 *  #  #
 *  #@@#
 */
int kill5(int br, int bc, int d1, int d2)
{
#if 0
	return 0;
#endif
	if(0 <= br+d1+d1 && br+d1+d1 < row && 0 <= bc + d2 + d2 && bc + d2 + d2 < col){
		int i, j;
		j = bc - d2;
		for(i = br; i != br + 3 * d1; i += d1)
			if(BOX[i][j] != '#')
				return 0;
		j = bc + d2 + d2;
		for(i = br; i != br + 3 * d1; i += d1)
			if(BOX[i][j] != '#')
				return 0;
		if(judge_bw(br, bc) && judge_bw(br, bc + d2) && judge_bw(br + d1 + d1, bc) && judge_bw(br + d1 + d1, bc + d2)){
			int countl = 0;
			int countr = 0;
			int countlb = 0;
			int countrb = 0;
			if(judge_nil(br, bc))
				countl++;
			if(judge_nil(br, bc+d2))
				countr++;
			if(judge_nil(br+d1, bc))
				countl++;
			if(judge_nil(br+d1, bc+d2))
				countr++;
			if(judge_nil(br+d1+d1, bc))
				countl++;
			if(judge_nil(br+d1+d1, bc+d2))
				countr++;
			if(BOX[br][bc] == '@')
				countlb++;
			if(BOX[br][bc+d2] == '@')
				countrb++;
			if(BOX[br+d1+d1][bc] == '@')
				countlb++;
			if(BOX[br+d1+d1][bc+d2] == '@')
				countrb++;
			if(countl < countlb || countr < countrb){
				return 1;
			}
		}
	}
	return 0;
}

/*
 *  ###
 *  @ @
 *  @ @
 *  ###
 */
int kill6(int br, int bc, int d1, int d2)
{
#if 0
	return 0;
#endif
	if(0 <= br+d1+d1 && br+d1+d1 < row && 0 <= bc + d2 + d2 && bc + d2 + d2 < col){
		int i, j;
		i = br + d1 + d1;
		for(j = bc; j != bc + 3 * d2; j += d2)
			if(BOX[i][j] != '#')
				return 0;
		i = br - d1;
		for(j = bc; j != bc + 3 * d2; j += d2)
			if(BOX[i][j] != '#')
				return 0;
		if(judge_bw(br, bc) && judge_bw(br + d1, bc) && judge_bw(br, bc + d2 + d2) && judge_bw(br + d1, bc + d2 + d2)){
			int countu = 0;
			int countd = 0;
			int countub = 0;
			int countdb = 0;
			if(judge_nil(br, bc))
				countd++;
			if(judge_nil(br+d1, bc))
				countu++;
			if(judge_nil(br, bc+d2))
				countd++;
			if(judge_nil(br+d1, bc+d2))
				countu++;
			if(judge_nil(br, bc+d2+d2))
				countd++;
			if(judge_nil(br+d1, bc+d2+d2))
				countu++;
			if(BOX[br][bc] == '@')
				countdb++;
			if(BOX[br+d1][bc] == '@')
				countub++;
			if(BOX[br][bc+d2+d2] == '@')
				countdb++;
			if(BOX[br+d1][bc+d2+d2] == '@')
				countub++;
			if(countu < countub || countd < countdb){
				return 1;
			}
		}
	}
	return 0;
}

/*
 *   #@
 *    @@
 *     #
 */
int kill7(int br, int bc, int d1, int d2)
{
	if(br+d1+d1 < 0 || br+d1+d1 >= row || bc+d2+d2 < 0 || bc+d2+d2 >= col)
		return 0;
	if(BOX[br+d1][bc] == '@' && BOX[br+d1][bc+d2] == '@' && BOX[br][bc-d2] == '#' && BOX[br+d1+d1][bc+d2] == '#'){
		int count = 0;
		if(judge_nil(br, bc))
			count++;
		if(judge_nil(br+d1, bc))
			count++;
		if(judge_nil(br+d1, bc+d2))
			count++;
		if(count < 3)
			return 1;
	}
	else if(BOX[br][bc+d2] == '@' && BOX[br+d1][bc+d2] == '@' && BOX[br-d1][bc] == '#' && BOX[br+d1][bc+d2+d2] == '#'){
		int count = 0;
		if(judge_nil(br, bc))
			count++;
		if(judge_nil(br, bc+d2))
			count++;
		if(judge_nil(br+d1, bc+d2))
			count++;
		if(count < 3)
			return 1;
	}
	return 0;
}

/*
 *   #@
 *    @@
 *     #
 */
int kill8(int br, int bc, int d1, int d2)
{
	if(BOX[br+d1][bc] == '@' && BOX[br][bc+d2] == '@' && BOX[br-d1][bc+d2] == '#' && BOX[br+d1][bc-d2] == '#'){
		int count = 0;
		if(judge_nil(br, bc))
			count++;
		if(judge_nil(br+d1, bc))
			count++;
		if(judge_nil(br, bc+d2))
			count++;
		if(count < 3)
			return 1;
	}
	return 0;
}

int kill_state(int br, int bc)
{
#if 0
	return 0;
#endif
	/*     ###
	 *     #@
	 *     #
	 */    
	row = mele->row;
	col = mele->col;
	if(!judge_nil(br, bc)){
		if(BOX[br-1][bc] == '#' && BOX[br][bc-1] == '#')
			return 1;
		if(BOX[br+1][bc] == '#' && BOX[br][bc+1] == '#')
			return 1;
		if(BOX[br-1][bc] == '#' && BOX[br][bc+1] == '#')
			return 1;
		if(BOX[br+1][bc] == '#' && BOX[br][bc-1] == '#')
			return 1;
	}

	if(kill1(br, bc, -1, 1))
		return 1;
	if(kill1(br, bc, 1, 1))
		return 1;
	if(kill1(br, bc, -1, -1))
		return 1;
	if(kill1(br, bc, 1, -1))
		return 1;

	if(kill2(br, bc, -1, 0))
		return 1;
	if(kill2(br, bc, 1, 0))
		return 1;
	if(kill2(br, bc, 0, -1))
		return 1;
	if(kill2(br, bc, 0, 1))
		return 1;

	if(kill3(br, bc, -1, 0))
		return 1;
	if(kill3(br, bc, 1, 0))
		return 1;
	if(kill3(br, bc, 0, -1))
		return 1;
	if(kill3(br, bc, 0, 1))
		return 1;

	if(kill4(br, bc, -1, 0))
		return 1;
	if(kill4(br, bc, 1, 0))
		return 1;
	if(kill4(br, bc, 0, -1))
		return 1;
	if(kill4(br, bc, 0, 1))
		return 1;

	if(kill5(br, bc, -1, 1))
		return 1;
	if(kill5(br, bc, -1, -1))
		return 1;
	if(kill5(br, bc, 1, 1))
		return 1;
	if(kill5(br, bc, 1, -1))
		return 1;

	if(kill6(br, bc, -1, 1))
		return 1;
	if(kill6(br, bc, -1, -1))
		return 1;
	if(kill6(br, bc, 1, 1))
		return 1;
	if(kill6(br, bc, 1, -1))
		return 1;

	if(kill7(br, bc, -1, 1))
		return 1;
	if(kill7(br, bc, -1, -1))
		return 1;
	if(kill7(br, bc, 1, 1))
		return 1;
	if(kill7(br, bc, 1, -1))
		return 1;

	if(kill8(br, bc, -1, 1))
		return 1;
	if(kill8(br, bc, -1, -1))
		return 1;
	if(kill8(br, bc, 1, 1))
		return 1;
	if(kill8(br, bc, 1, -1))
		return 1;

	return 0;
}


