////////////////////////////////////////////////////////////////////////////////////
// Othello AI for w-ai.org
// Author: Amb
// url: amb-bai.me
// email: amb.main@gmail.com
// 
// There is some bugs in the hash part.
////////////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <algorithm>
using namespace std;
#include <unistd.h>

#define SEARCH_DEEP 8
#define SEARCH_WIDTH 10
#define HASHTABLE_SIZE 100007
#define INF 1000000

const int N = 8;
const int dir[8][2] = {
	{-1, -1}, {-1, 0}, {-1, 1},
	{0, -1}, {0, 1},
	{1, -1}, {1, 0}, {1, 1}
};


const int cell_lv[N/2][N/2] = {
	{6, 5, 5, 5},
	{5, 4, 3, 3},
	{5, 3, 2, 1},
	{5, 3, 1, 0},
};

inline int get_cell_lv(int x, int y) {
	if (x >= 4) x = 7-x;
	if (y >= 4) y = 7-y;
	return cell_lv[x][y];
}

int board[N][N];
int flg, turn;

bool calc (int&, int&);
void put_chess (int, int, int);
int valid (int, int, int);
void zorbrist_init();

int main() {
	board[3][3] = board[4][4] = 1;
	board[3][4] = board[4][3] = 2;
	zorbrist_init();

	char buf[16];
	scanf("%s", buf);
	fprintf(stderr, "%d: I get {%s}\n", getpid(), buf);
	flg = strcmp(buf, "first")?2:1;

	turn = 0;

	int x, y;
	if (flg == 2) {
		scanf("%d %d", &x, &y);
		put_chess(x, y, 3 - flg);
		turn ++;
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
		turn ++;
		//if(game_over(tmp)) break;
		
		scanf("%d %d", &x, &y);
		if (~x) {
			if (!valid(x, y, 3-flg)) break;
			put_chess(x, y, 3-flg);
		}
		turn ++;
		//if(game_over(tmp)) break;
	}
	return 0;
}


//====================================================
struct Move {
	int x, y, fcnt, val;

	Move(){}
	Move(int _x, int _y, int _f):x(_x), y(_y), fcnt(_f) {
		getval();
	}

	void getval();

	friend bool operator< (const Move& a, const Move& b) {
		return a.val > b.val;
	}

	void show(){
		printf("%d,%d:%d ", x, y, val);
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

	void show(){
		printf("(= ");
		for(int i=0; i<size; i++){
			M[i].show();
		} puts("");
	}
} MList[SEARCH_DEEP];

MLIST valids (int);
int getval(int, int, int);
inline bool inboard(int, int);

typedef unsigned long long ULL;

struct Status{
	ULL black, white;

	Status():black(0), white(0){}

	void zip(){
		black = white = 0;
		for (int x=0; x<N; x++) {
			for (int y=0; y<N; y++) {
				int p = x*N + y;
				if (board[x][y] == 1) black |= (1ULL<<p);
				if (board[x][y] == 2) white |= (1ULL<<p);
			}
		}
	}

	void release(){
		for (int x=0; x<N; ++x) {
			for (int y=0; y<N; ++y) {
				int p = x*N + y;
				board[x][y] = 0;
				if (black&(1ULL<<p)) board[x][y] = 1;
				if (white&(1ULL<<p)) board[x][y] = 2;
			}
		}
	}

	bool operator ==(const Status& ts){
		return ts.black == black && ts.white == white;
	}
}sta[SEARCH_DEEP];

ULL zobristB[N][N][2], zobristW[N][N][2];
ULL zobristSWAP[2];

ULL get_random() {
	ULL res = 0;
	for (int i=0; i<4; i++) {
		res <<= 20;
		res ^= rand();
	}
	return res;
}

void zorbrist_init() {
	zobristSWAP[0] = get_random();
	zobristSWAP[1] = get_random();

	for(int x = 0; x < N ; x++) {
		for(int y = 0; y < N; y++) {
			zobristB[x][y][0] = get_random();
			zobristW[x][y][0] = get_random();
			zobristB[x][y][1] = get_random();
			zobristW[x][y][1] = get_random();
		}
	}
}

void get_zobrist(ULL& hc1, ULL& hc2){
	hc1 = hc2 = 0;
	for(int x = 0; x < N ; x++) {
		for(int y = 0; y < N; y++) {
			if (board[x][y] == 1) hc1 ^= zobristB[x][y][0];
			if (board[x][y] == 2) hc1 ^= zobristW[x][y][0];
			if (board[x][y] == 1) hc2 ^= zobristB[x][y][1];
			if (board[x][y] == 2) hc2 ^= zobristW[x][y][1];
		}
	}
	if (turn&1){
		hc1 ^= zobristSWAP[0];
		hc2 ^= zobristSWAP[1];
	}
}

struct HashNode{
	ULL lock;
	int left, right;
	int best_x, best_y;
	int dep;

	HashNode():dep(0){}
	HashNode(ULL _lock, int _left, int _right, int _best_x, int _best_y, int _dep):
		lock(_lock), left(_left), right(_right), best_x(_best_x), best_y(_best_y), dep(_dep){}

	void update(int _left, int _right, int _best_x, int _best_y){
		if (_left > left) left = _left;
		if (_right < right) right = _right;
		best_x = _best_x;
		best_y = _best_y;
	}
};

struct HashTable{
	struct HashNode deepest, newest;
}hashtable[HASHTABLE_SIZE];

void hash_update(ULL hc1, ULL hc2, int left, int right, int best_x, int best_y, int dep){
	HashTable &ht = hashtable[hc1 % HASHTABLE_SIZE];
	HashNode &deepest = ht.deepest, &newest = ht.newest;

	if (hc2 == deepest.lock && dep == deepest.dep) deepest.update(left, right, best_x, best_y);
	else if (hc2 == newest.lock && dep == newest.dep) newest.update(left, right, best_x, best_y);
	else if (dep > deepest.dep) newest = deepest, deepest = HashNode(hc2, left, right, best_x, best_y, dep);
	else newest = HashNode(hc2, left, right, best_x, best_y, dep);
}

HashNode* get_hash(ULL hc1, ULL hc2, int dep){
	HashTable &ht = hashtable[hc1 % HASHTABLE_SIZE];
	HashNode &deepest = ht.deepest, &newest = ht.newest;

	if (hc2 == deepest.lock && dep == deepest.dep) return &deepest;
	if (hc2 == newest.lock && dep == newest.dep) return &newest;
	return NULL;
}

const int cell_base_val[7] = {10, 10, 12, 7, 5, 15, 18};
const int cell_calc_val[7] = {24, 20, 46, 8, 24, 80, 680};

void Move::getval(){
	int lv = get_cell_lv(x, y);
	val = cell_base_val[lv] + fcnt * 10;
}

bool game_over(int& res){
	int black = 0, white = 0, empty = 0;
	for(int x=0; x<N; ++x) {
		for(int y=0; y<N; ++y) {
			if (board[x][y] == 0) empty ++;
			if (board[x][y] == 1) black ++;
			if (board[x][y] == 2) white ++;
		}
	}

	if(empty==0 || black==0 || white==0){
		if (flg == 1) res = black - white;
		if (flg == 2) res = white - black;
		if (res > 0) res += 10000;
		if (res < 0) res -= 10000;
		return true;
	} else return false;
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

void show(){
	for(int x=0; x<N; x++) {
		for(int y=0; y<N; y++) {
			if(board[x][y] == 0) putchar('.');
			else if(board[x][y] == 1) putchar('X');
			else if(board[x][y] == 2) putchar('O');
		}puts("");
	}puts("============================");
}

int next_x, next_y;

int negamax(int dep, int alpha, int beta) {
	int val = 0;

	if(game_over(val)) return val*((dep&1)?-1:1);
	if(dep == SEARCH_DEEP) return get_global_val()*((dep&1)?-1:1);

	sta[dep].zip();

	//hashtable
	ULL hashcode1, hashcode2;
	get_zobrist(hashcode1, hashcode2);
	HashNode *hashnode = get_hash(hashcode1, hashcode2, turn+dep);

	int best_x = -1, best_y = -1;

	if (hashnode) {
		best_x = hashnode->best_x;
		best_y = hashnode->best_y;
		
		if (dep == 0){
			next_x = best_x;
			next_y = best_y;
		}

		//printf("%llu\n", hashcode);
		if(hashnode->left > alpha) {
			alpha = hashnode->left;
			if (alpha >= beta) return alpha;
		}
		if(hashnode->right < beta) {
			beta = hashnode->right;
			if (beta <= alpha) return beta;
		}
	}
	//

	MList[dep] = valids( (dep&1)?3-flg:flg );

#define _DEBUG

#ifdef DEBUG
	show(); 
	MList[dep].show();
	system("pause");
#endif

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
			// 	if (val <= alpha) {sta[dep].release(); continue;}
			// 	if (val >= beta) {sta[dep].release(); return val;}
			// 	best = alpha = val;
			// 	best_x = now.x;
			// 	best_y = now.y;
			// }
			// //

			val = -negamax(dep+1, -beta, -alpha);
			sta[dep].release();

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

	//hashtable

	if (dep == 0){
		next_x = best_x;
		next_y = best_y;
	}

	//printf("|| %d %d\n", best_x, best_y);
	if (best >= beta) {
		hash_update(hashcode1, hashcode2, best, INF, best_x, best_y, turn+dep);
	} else if (best <= alpha) {
		hash_update(hashcode1, hashcode2, -INF, best, best_x, best_y, turn+dep);
	} else {
		hash_update(hashcode1, hashcode2, best, best, best_x, best_y, turn+dep);
	}

	// //
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

	//ULL hashcode = get_zobrist();
	//HashNode *hashnode = get_hash(hashcode, sta[0], turn);
	//x = hashnode->best_x;
	//y = hashnode->best_y;

	x = next_x;
	y = next_y;

	if (~x) return true;
	else return false;
}

//====================================================

inline bool inboard(int x, int y) {
	return x >= 0 && x < N && y >= 0 && y < N;
}

int valid(int x, int y, int flgtype) {
	if (!inboard(x,y)) return 0;
	if (board[x][y] != 0) return 0;
	int flipcnt = 0;
	for (int d = 0 ; d < 8 ; d++) {
		int tx = x + dir[d][0];
		int ty = y + dir[d][1];
		if (inboard(tx,ty) && board[tx][ty] == 3 - flgtype) {
			while (true) {
				tx += dir[d][0];
				ty += dir[d][1];
				if (!inboard(tx,ty)) break;
				if (board[tx][ty] == 0) break;
				if (board[tx][ty] == flgtype) {
					tx -= dir[d][0];
					ty -= dir[d][1];
					while (tx != x || ty != y) {
						++flipcnt;
						tx -= dir[d][0];
						ty -= dir[d][1];
					}
					break;
				}
			}
		} 
	}
	return flipcnt;
}

MLIST valids(int flgtype) {
	MLIST res;
	for (int x=0; x<N; x++) {
		for (int y=0; y<N; y++) {
			if (board[x][y]==0){
				int flipcnt = valid(x, y, flgtype);
				if (flipcnt) res.push(x, y, flipcnt);
			}
		}
	}
	res.order();
	return res;
}

void put_chess(int x, int y, int flgtype) {
	board[x][y] = flgtype;
	int tx, ty;
	for (int d = 0 ; d < 8 ; ++d) {
		tx = x + dir[d][0];
		ty = y + dir[d][1];
		while (inboard(tx,ty) && board[tx][ty] == 3 - flgtype) {
			tx += dir[d][0];
			ty += dir[d][1];
		}
		if (inboard(tx,ty) && board[tx][ty] == flgtype) {
			tx -= dir[d][0];
			ty -= dir[d][1];
			while (tx != x || ty != y) {
				board[tx][ty] = flgtype;
				tx -= dir[d][0];
				ty -= dir[d][1];
			}
		}
	}
}