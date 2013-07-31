////////////////////////////////////////////////////////////////////////////////////
// Renju_simple AI for w-ai.org
// Author: Amb
// url: amb-bai.me
// email: amb.main@gmail.com
// 
// Valuation function is sick.
////////////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <algorithm>
using namespace std;
#include <unistd.h>

char buf[16];
int flg;
int bd[16][16];

//============
int turncnt;
void calc(int&, int&);

//============

int main() {
    fprintf(stderr, "%d: i am Amb\n", getpid());
    scanf("%s", buf);
    fprintf(stderr, "%d: i recv %s\n", getpid(), buf);
    flg = strcmp(buf, "first")?2:1;
    turncnt=0; 
    memset(bd, 0, sizeof(bd));
    if (flg == 2) {
        int x, y;
        scanf("%d %d", &x, &y);
        bd[x][y] = 3 - flg;
        turncnt++; 
    }
    while (1) {
        int x, y;
        calc(x, y);
        printf("%d %d\n", x, y);
        fflush(stdout);
        bd[x][y] = flg;
        turncnt++; 
        scanf("%d %d", &x, &y);
        bd[x][y] = 3 - flg;
        turncnt++; 
    }
    return 0;
}

//my AI================================== 

//constants =============================
#define EMPTY (0)
#define FLG (flg)
#define anti_FLG (3-flg)
#define WALL (3)

const int dir_x[4] = {1, 1, 0, 1};
const int dir_y[4] = {0, 1, 1, -1};
const int MAPX = 15;
const int MAPY = 15;
const int INF = 10000000;

const int SEARCH_DEEP = 6;
const int SEARCH_WIDTH = 8;

const int base_val[2][8] = {
	// 0: 00001 - 1
	// 1: 00011 - 2
	// 2: 00101 - 2
	// 3: 01110 - 3
	// 4: 10011 - 3
	// 5: 00111 - 3
	// 6: 10111 - 4
	// 7: 01111 - 4
	{15, 70, 60, 150, 100, 120, 9000, 50000},
	{10, 60, 55, 200, 150, 170, 500, 6000}
};

const int extra_val[2][5][6] = {
	// 0: X.....X
	// 1: F.....X
	// 2: E.....X
	// 3: F.....F
	// 4: F.....E
	// 5: E.....E
	// { {},
	// 	{-2, 0, 0, 0, 0, 0},
	// 	{-5, 3, 0, 10, 6, 2},
	// 	{-15, 8, 0, 25, 16, 5},
	// 	{0, 0, 0, 0, 0, 0}
	// }, { {},
	// 	{-2, 0, 0, 0, 0, 0},
	// 	{-5, 3, 0, 10, 6, 2},
	// 	{-15, 8, 0, 25, 16, 5},
	// 	{0, 0, 0, 0, 0, 0}
	// }

	{ {},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0}
	}, { {},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0}
	}
};


//functions =============================
inline int cntbit(int);
inline bool inboard(int, int);
bool checkwin(int, int);
bool checktie();

void showbd();

//structs ===============================

struct P{
	unsigned x, y;
	int val;
	P(){}
	P(unsigned _x, unsigned _y, int _v = 0):
		x(_x), y(_y), val(_v){}

	friend bool operator< (const P& a, const P& b){
		return a.val < b.val;
	}

	void show(){
		printf("(%d,%d:%d ", x, y, val);
	}
};

template <unsigned MAXSIZE>
struct PLIST{
	P p[MAXSIZE];
	int size;

	PLIST(): size(0) {}
	PLIST(const PLIST& npl);

	void push(const P& np);
	void show();
	P& operator[] (unsigned i){ return p[i];}
};
PLIST<SEARCH_WIDTH> nextslist[SEARCH_DEEP];

int next_x, next_y;

//myfunctions ===========================

inline bool close(int, int);
inline int get_base_id(int, int);
inline int get_extra_id(int, int, int);

template <unsigned MAXSIZE>
PLIST<MAXSIZE> getnextlist(int);
int getcellscore(int, int, int);
int evaluate(int, int, int, int);

int minimax(int, int);

//--======================================

inline bool close(int end, int flgtype){
	if (end!=EMPTY && end!=flgtype) return true;
	return false;
}

inline int get_base_id(int line, int cnt) {
	switch (cnt) {
		case 1: return 0;
		case 2:
			if (cntbit(line/3) == 1) return 1;
			else return 2;
		case 3:
			if (line == 0xE) return 3;
			else if ((line&0x11) == 0x11) return 4;
			else return 5;
		case 4:
			if ((line&0x11) == 0x11) return 6;
			else return 7;
	}
}

inline int get_extra_id(int endl, int endr, int flgtype) {
	if (close(endl, flgtype) && close(endr, flgtype)) return 0;
	else if (close(endl, flgtype) || close(endr, flgtype)) {
		if (endl==flgtype || endr==flgtype) return 1;
		else return 2;
	} else {
		if (endl==flgtype && endr==flgtype) return 3;
		else if (endl==flgtype || endr==flgtype) return 4;
		else return 5;
	}
}

int evaluate(int line, int anti_line, int endl, int endr){
	int cnt = cntbit( line );
	int anti_cnt = cntbit( anti_line );

	if (cnt!=0 && anti_cnt!=0) return 1;
	else if (cnt){
		int base_id = get_base_id(line, cnt);
		int extra_id = get_extra_id(endl, endr, FLG);
		return base_val[0][base_id] + extra_val[0][cnt][extra_id];
	} else if (anti_cnt) {
		int base_id = get_base_id(anti_line, anti_cnt);
		int extra_id = get_extra_id(endl, endr, anti_FLG);
		return base_val[1][base_id] + extra_val[1][cnt][extra_id];
	} else return 3;
}

int getcellscore(int x, int y, int flgtype){
	int result = 0;
	for(int d = 0; d < 4; d++) {
		for(int k = 0; k < 5; k++){
			int s_x = x - k * dir_x[d];
			int s_y = y - k * dir_y[d];
			int e_x = x + (4-k) * dir_x[d];
			int e_y = y + (4-k) * dir_y[d];
			if(inboard(s_x, s_y) && inboard(e_x, e_y)) {

				//check line status
				int line = 0, anti_line = 0;
				for(int s = 0; s < 5; s++){
					int tx = s_x + s * dir_x[d];
					int ty = s_y + s * dir_y[d];
					if(bd[tx][ty]) {
						if(bd[tx][ty] == flgtype) line |= 1<<s;
						else anti_line |= 1<<s;
					}
				}

				int end_x, end_y, endl, endr;
				
				//check left end
				end_x = s_x - dir_x[d];
				end_y = s_y - dir_y[d];
				if(!inboard(end_x, end_y)) endl = WALL;
				else endl = bd[end_x][end_y];

				//check right end
				end_x = e_x + dir_x[d];
				end_y = e_y + dir_y[d];
				if(!inboard(end_x, end_y)) endr = WALL;
				else endr = bd[end_x][end_y];

				result += evaluate(line, anti_line, endl, endr);
				//result.push(evaluate(line, anti_line, endl, endr));
			}
		}
	}
	return result;
}


template <unsigned MAXSIZE>
PLIST<MAXSIZE> getnextlist(int flgtype){
	bool marknext[MAPX][MAPY];
	memset(marknext, false, sizeof(marknext)); 
	for(int x=0; x<MAPX; x++) {
		for(int y=0; y<MAPY; y++) {
			if(bd[x][y] == 0) continue;
			for(int kx = -2; kx <= 2; kx++) {
				for(int ky = -2; ky <= 2; ky++) {
					int tx = x+kx, ty = y+ky; 
					if(inboard(tx, ty) && bd[tx][ty]==0){
						marknext[tx][ty] = true;
					} 
				} 
			} 
		}
	}

	PLIST<MAXSIZE> reslist;
	for(int x=0; x<MAPX; x++) {
		for(int y=0; y<MAPY; y++) {
			if(marknext[x][y]){
				reslist.push(P(x, y, getcellscore(x, y, flgtype)));
			} 
		}
	}

	return reslist;
}

int get_evaluate(int flgtype) {
	int val = 0;
	PLIST <10> reslist;
	reslist = getnextlist <10> ( flgtype );
	for (int i=0; i<reslist.size; i++) val += reslist[i].val;
	reslist = getnextlist <10> ( 3-flgtype );
	for (int i=0; i<reslist.size; i++) val -= reslist[i].val;
	return val;
}

int negamax(int dep, int alpha, int beta) {
	int val = 0;

	if (dep+turncnt == 225) return 0;
	if (dep == SEARCH_DEEP) return get_evaluate( FLG );

	nextslist[dep] = getnextlist <SEARCH_WIDTH>( (dep&1)?anti_FLG:FLG );

	int best = -INF;

	for(int i = 0; i < nextslist[dep].size; i++){
		P &now = nextslist[dep][i];

		bd[now.x][now.y] = (dep&1)?anti_FLG:FLG;
		if(checkwin(now.x, now.y)) val = 1000000;
		else{
			//pvs Minimal Window Search
			if (best == alpha) {
				val = -negamax(dep+1, -alpha-1, -alpha);
				if (val <= alpha) {bd[now.x][now.y] = 0; continue;}
				if (val >= beta) {bd[now.x][now.y] = 0; return val;}
				best = alpha = val;
				next_x = now.x;
				next_y = now.y;
			}
			//
			
			val = -negamax(dep+1, -beta, -alpha);
		}
		bd[now.x][now.y] = 0;

		if (val > best) {
			best = val;
			if (dep == 0){
				next_x = now.x;
				next_y = now.y;
			}
			if (val > alpha) {
				alpha = val;
				if (val >= beta) return val;
			}
		}
	}
	return best;
}

//--==========================================
//--==========================================
//--==========================================

template <unsigned MAXSIZE>
PLIST <MAXSIZE>::PLIST(const PLIST<MAXSIZE>& npl){
	size = npl.size;
	for (int i=0; i<size; ++i) p[i] = npl.p[i];
}

template <unsigned MAXSIZE>
void PLIST <MAXSIZE> ::push(const P& np) {
	if (size == 0) {
		p[size++] = np;
		return;
	}

	int pos;
	if (size == MAXSIZE) {
		pos = size - 1;
		if (p[pos] < np) p[pos] = np;
		else return ;
	} else {
		pos = size++;
		p[size-1] = np;
	}
	for (--pos; ~pos; --pos) {
		if (p[pos] < p[pos+1]) swap(p[pos], p[pos+1]);
		else return;
	}
}

template <unsigned MAXSIZE>
void PLIST<MAXSIZE>::show(){
	printf("= %d: ", size);
	for(int i=0;i<size;i++){
		p[i].show();
	} puts("");
}

//--==========================================
inline int cntbit(int x){
	x = (x & 0x55555555) + ((x >> 1) & 0x55555555);
	x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
	x = (x & 0x0F0F0F0F) + ((x >> 4) & 0x0F0F0F0F);
	x = (x & 0x00FF00FF) + ((x >> 8) & 0x00FF00FF);
	x = (x & 0x0000FFFF) + ((x >> 16) & 0x0000FFFF); 
	return x;
}

inline bool inboard(int x, int y){
	if(x<0 || x>=MAPX) return false;
	if(y<0 || y>=MAPY) return false;
	return true; 
}

void showbd(){
	for(int i=0;i<MAPX;i++){
		for(int j=0;j<MAPY;j++){
			if (bd[i][j] == 0) putchar('.');
			else if(bd[i][j] == 1) putchar('B');
			else if(bd[i][j] == 2) putchar('W');
		}puts("");
	}puts("==========");
}

bool checkwin(int x, int y){
	for(int d = 0; d < 4; d++) {
		for(int k = 0; k < 5; k++) {
			int s_x = x - k * dir_x[d];
			int s_y = y - k * dir_y[d];
			int e_x = x + (4-k) * dir_x[d];
			int e_y = y + (4-k) * dir_y[d];
			if(inboard(s_x, s_y) && inboard(e_x, e_y)) {
				int status_a = 0, status_b = 0;
				for(int s = 0; s < 5; s++){
					int tx = s_x + s * dir_x[d];
					int ty = s_y + s * dir_y[d];
					if(bd[tx][ty] == 0) continue;
					if(bd[tx][ty] == bd[x][y]) status_a |= 1<<s;
					else status_b |= 1<<s;
				}
				if(cntbit(status_a)==5 || cntbit(status_b)==5) return true; 
			}
		} 
	}
	return false; 
}

bool checktie(){
	for(int x = 0; x<MAPX ; x++){
		for(int y = 0; y<MAPY ; y++){
			if(bd[x][y]==0) return false;
		}
	}
	return true;
}

void calc(int& x, int& y){
	if(turncnt == 0){
		x = y = 7;
	}/* else if(turncnt == 1) {
		if(bd[7][7]){ x = 6; y = 7; }
		else x = y = 7;
	} */else {
		//------------------ minimax

		next_x = next_y = -1;

#define _CNTTIME

#ifdef CNTTIME
		long long START_TIME = clock();
#endif

		negamax(0, -INF, INF);

#ifdef CNTTIME
		long long END_TIME = clock();

		printf("cost: %lld ms\n", (END_TIME - START_TIME)/1000);
#endif

		x = next_x;
		y = next_y;
	}
}
