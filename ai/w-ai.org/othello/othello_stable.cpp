////////////////////////////////////////////////////////////////////////////////////
// Othello AI for w-ai.org
// Author: Amb
// url: amb-bai.me
// email: amb.main@gmail.com
// 
// Stable version
////////////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <algorithm>
using namespace std;
#include <unistd.h>

#define SEARCH_DEEP 8
#define SEARCH_WIDTH 12
#define INF 1000000

#define CELL_EMPTY 0
#define CELL_BLACK 1
#define CELL_WHITE 2

typedef unsigned long long STATUS_BIT;

const int N = 8;
const int DIR_X[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
const int DIR_Y[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

const int cell_lv[N/2][N/2] = {
	{6, 5, 5, 5},
	{5, 4, 3, 3},
	{5, 3, 2, 1},
	{5, 3, 1, 0},
};

const int cell_base_val[7] = {10, 10, 12, 7, 5, 15, 18};
const int cell_calc_val[7] = {24, 20, 46, 8, 24, 80, 680};

int board[N][N];
int flg, turn;


inline bool inboard(int, int);
inline int get_cell_lv(int, int);

//====================================================
struct Move {
	int x, y, fcnt, val;

	Move(){}
	Move(int _x, int _y, int _f):x(_x), y(_y), fcnt(_f) {
		val = cell_base_val[get_cell_lv(x, y)] + fcnt * 10;
	}

	friend bool operator< (const Move& a, const Move& b) {
		return a.val > b.val;
	}

	void show(){
		fprintf(stderr, "%d,%d:%d ", x, y, val);
	}
};

struct MLIST{
	Move M[60];
	int size;

	MLIST(): size(0){}
	MLIST(const MLIST& nml){
		size = nml.size;
		for(int i=0; i<size; i++) {
			M[i] = nml.M[i];
		}
	}

	void push(int x, int y, int fcnt) {
		M[size++] = Move(x, y, fcnt);
	}

	void order() {
		sort(M, M+size);
	}

	Move& operator[] (unsigned idx){
		return M[idx];
	}

	void show() {
		fprintf(stderr, "(= ");
		for(int i=0; i<size; i++) M[i].show();
		fprintf(stderr, "\n");
	}
} MList[SEARCH_DEEP], MLtmp;

struct Status{
	STATUS_BIT black, white;

	Status():black(0), white(0){}

	void compress();

	void decompress();

	bool operator ==(const Status& ts){
		return ts.black == black && ts.white == white;
	}
}sta[SEARCH_DEEP];


bool valids (int, MLIST&, bool toend = true);
int getval(int, int, int);

bool calc (int&, int&);
void put_chess (int, int, int);
int valid (int, int, int);
bool game_over(int&);
void show();

inline int get_over_score (int black_cnt, int white_cnt) {
	int res = 0;
	if (flg == 1) res = black_cnt - white_cnt;
	if (flg == 2) res = white_cnt - black_cnt;
	if (res > 0) res += 10000;
	if (res < 0) res -= 10000;
	return res;
}

int get_global_val(){
	int res = 0;
	for(int x=0; x<N; x++) {
		for(int y=0; y<N; y++) {
			if (board[x][y] == 0) continue;
			int lv = get_cell_lv(x, y);
			if (board[x][y] == flg) res += cell_calc_val[lv];
			if (board[x][y] == 3-flg) res -= cell_calc_val[lv];
		}
	}
	return res;
}

int next_x, next_y;
int negamax(int dep, int alpha, int beta) {
	int val = 0;

	if(game_over(val)) return val*((dep&1)?-1:1);
	if(dep == SEARCH_DEEP) return get_global_val()*((dep&1)?-1:1);

	sta[dep].compress();

	int best_x = -1, best_y = -1;

	valids( (dep&1)?3-flg:flg, MList[dep]);

	int best = -INF;

	if (MList[dep].size==0) {
		best = -negamax(dep+1, -beta, -alpha);
	} else {
		for (int i=0; i < min(MList[dep].size, SEARCH_WIDTH) ; i++) {
			Move& now = MList[dep][i];
			
			put_chess(now.x, now.y, (dep&1)?3-flg:flg );

			// //pvs Minimal Window Search
			// if (best == alpha) {
			// 	val = -negamax(dep+1, -alpha-1, -alpha);
			// 	if (val <= alpha) {sta[dep].decompress(); continue;}
			// 	if (val >= beta) {sta[dep].decompress(); return val;}
			// 	best = alpha = val;
			// 	best_x = now.x;
			// 	best_y = now.y;
			// }
			// //

			val = -negamax(dep+1, -beta, -alpha);
			sta[dep].decompress();

			if (val > best){
				best = val;
				best_x = now.x;
				best_y = now.y;
				if (val > alpha) {
					alpha = val;
					if (val >= beta) return val;
				}
			}
		}
	}

	if (dep == 0){
		next_x = best_x;
		next_y = best_y;
	}

	return best;
}

bool calc(int &x, int &y) {

	if (turn == 0) {
		x = 2;
		y = 4;
		return true;
	}

	next_x = next_y = -1;

	negamax(0, -INF, INF);

	x = next_x;
	y = next_y;

	if (~x) return true;
	else return false;
}

//====================================================

void Status::compress() {
	black = white = 0;
	for (int x=0; x<N; x++) {
		for (int y=0; y<N; y++) {
			int p = x*N + y;
			if (board[x][y] == CELL_BLACK) black |= (1ULL<<p);
			if (board[x][y] == CELL_WHITE) white |= (1ULL<<p);
		}
	}
}

void Status::decompress() {
	for (int x=0; x<N; ++x) {
		for (int y=0; y<N; ++y) {
			int p = x*N + y;
			board[x][y] = CELL_EMPTY;
			if (black&(1ULL<<p)) board[x][y] = CELL_BLACK;
			if (white&(1ULL<<p)) board[x][y] = CELL_WHITE;
		}
	}
}

//=====================================================

inline bool inboard(int x, int y) {
	return x >= 0 && x < N && y >= 0 && y < N;
}

int valid(int x, int y, int flgtype) {
	if (!inboard(x,y)) return 0;
	if (board[x][y] != CELL_EMPTY) return 0;
	int flipcnt = 0;
	for (int d = 0 ; d < 8 ; d++) {
		int tx = x + DIR_X[d];
		int ty = y + DIR_Y[d];
		if (inboard(tx,ty) && board[tx][ty] == 3 - flgtype) {
			while (true) {
				tx += DIR_X[d];
				ty += DIR_Y[d];
				if (!inboard(tx,ty)) break;
				if (board[tx][ty] == CELL_EMPTY) break;
				if (board[tx][ty] == flgtype) {
					tx -= DIR_X[d];
					ty -= DIR_Y[d];
					while (tx != x || ty != y) {
						++flipcnt;
						tx -= DIR_X[d];
						ty -= DIR_Y[d];
					}
					break;
				}
			}
		} 
	}
	return flipcnt;
}

bool valids(int flgtype, MLIST& res, bool toend) {
	res.size = 0;
	for (int x=0; x<N; x++) {
		for (int y=0; y<N; y++) {
			if (board[x][y] == CELL_EMPTY){
				int flipcnt = valid(x, y, flgtype);
				if (flipcnt){
					if(toend == false) return true;
					res.push(x, y, flipcnt);
				}
			}
		}
	}
	if(toend == false) return false;
	if(res.size) {
		res.order();
		return true;
	} return false;
}


bool game_over(int& res){
	int black = 0, white = 0, empty = 0;
	for(int x=0; x<N; ++x) {
		for(int y=0; y<N; ++y) {
			if (board[x][y] == CELL_EMPTY) ++ empty;
			if (board[x][y] == CELL_BLACK) ++ black;
			if (board[x][y] == CELL_WHITE) ++ white;
		}
	}

	if(empty==0 || black==0 || white==0){
		res = get_over_score(black, white);
		return true;
	} else{
		if (valids(CELL_BLACK, MLtmp, false)) return false;
		if (valids(CELL_WHITE, MLtmp, false)) return false;
		res = get_over_score(black, white);
		return true;
	}
}

int get_cell_lv(int x, int y) {
	if (x >= 4) x = 7-x;
	if (y >= 4) y = 7-y;
	return cell_lv[x][y];
}

void put_chess(int x, int y, int flgtype) {
	board[x][y] = flgtype;
	int tx, ty;
	for (int d = 0 ; d < 8 ; ++d) {
		tx = x + DIR_X[d];
		ty = y + DIR_Y[d];
		while (inboard(tx,ty) && board[tx][ty] == 3 - flgtype) {
			tx += DIR_X[d];
			ty += DIR_Y[d];
		}
		if (inboard(tx,ty) && board[tx][ty] == flgtype) {
			tx -= DIR_X[d];
			ty -= DIR_Y[d];
			while (tx != x || ty != y) {
				board[tx][ty] = flgtype;
				tx -= DIR_X[d];
				ty -= DIR_Y[d];
			}
		}
	}
}


void show(){
	for(int x=0; x<N; x++) {
		for(int y=0; y<N; y++) {
			if(board[x][y] == CELL_EMPTY) fprintf(stderr, ".");
			else if(board[x][y] == CELL_BLACK) fprintf(stderr, "B");
			else if(board[x][y] == CELL_WHITE) fprintf(stderr, "W");
		}
		fprintf(stderr, "\n");
	}fprintf(stderr, "\n");
}

int main() {
	board[3][3] = board[4][4] = CELL_BLACK;
	board[3][4] = board[4][3] = CELL_WHITE;

	char buf[16];
	scanf("%s", buf);
	fprintf(stderr, "%d: I get {%s}\n", getpid(), buf);
	flg = strcmp(buf, "first")?CELL_WHITE:CELL_BLACK;

	turn = 0;

	int x, y;
	if (flg == 2) {
		scanf("%d %d", &x, &y);
		put_chess(x, y, 3 - flg);
		++turn;
	}

	while (true) {
		if (calc(x, y)) {
			printf("%d %d\n", x, y);
			fflush(stdout);
			put_chess(x, y, flg);
		} else {
			printf("-1 -1\n");
			fflush(stdout);
		}
		++turn;
		
		scanf("%d %d", &x, &y);
		if (~x) {
			if (!valid(x, y, 3-flg)) break;
			put_chess(x, y, 3-flg);
		}
		turn ++;
	}
	return 0;
}
