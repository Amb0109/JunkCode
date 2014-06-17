#include <cstdio>
#include <cstring>
using namespace std;

#define N 8
#define BP 10000019

int turn;
char maze[N+1][N+1];
int loop;
int sdeep = 3;

inline void check(int& vs, int& hs)
{
	vs = 0;
	hs = 0;
	for (int i=0; i<N; ++i)
	{
		for (int j=0; j<N; ++j)
		{
			if (maze[i][j] == '-')
			{
				if (i+1 < N && maze[i+1][j] == '-') ++vs;
				if (j+1 < N && maze[i][j+1] == '-') ++hs;
			}
		}
	}
}

#define ULL unsigned long long

inline void zip(ULL& v, ULL& h)
{ 
	for(int i=0; i<N; i++)
	{
		for(int j=0; j<N; j++)
		{
			if(maze[i][j]=='v') v |= (ULL)1 << (i*N+j);
			else
			if(maze[i][j]=='h') h |= (ULL)1 << (i*N+j);
		} 
	}  
} 

ULL vis[BP+1];
int mem[BP+1];
#define NOT_EXIST -123456

int h(char player)
{
	int vs, hs;
	check(vs, hs);
	int my_val = player == 'v' ? vs : hs ;
	int oth_val = player == 'v' ? hs : vs ;
	int val = 64;
	if (oth_val != 0) val = my_val - oth_val;
	return val;
}

int save(int val)
{
	ULL v, h;
	zip(v, h);
	ULL _hash = (v * 1313131 + h) % BP;
	ULL _hash2 = (v + h * 1313131);

	vis[_hash] = _hash2;
	mem[_hash] = val;

	return val;		
}

int get()
{
	ULL v, h;
	zip(v, h);
	ULL _hash = (v * 1313131 + h) % BP;
	ULL _hash2 = (v + h * 1313131);

	if (vis[_hash] == _hash2)
	{
		return mem[_hash];
	}
	return NOT_EXIST;
}


inline void show()
{
	for(int i=0; i<N; i++)
	{
		for(int j=0; j<N; j++)
		{
			putchar(maze[i][j]);
		} 
		puts("");
	}
	puts("======");
}

int best_x, best_y;

int negamax(int dep, char player)
{
	int mem_val = get();
	if (mem_val != NOT_EXIST) return mem_val;

	++loop;
	if (dep - turn >= sdeep) return save(h(player));

	int best = -65;
	
	for (int i=0; i<N; ++i)
	{
		for (int j=0; j<N; ++j)
		{
			if (maze[i][j] != '-') continue;

			int val = 0;

			if (player == 'v'
				&& i+1 < N
				&& maze[i+1][j] == '-')
			{
				maze[i][j] = player;
				maze[i+1][j] = player;
				val = -negamax(dep+1, 'h');
				maze[i][j] = '-';
				maze[i+1][j] = '-';
			}

			else if (player == 'h'
				&& j+1 < N
				&& maze[i][j+1] == '-')
			{
				maze[i][j] = player;
				maze[i][j+1] = player;
				val = -negamax(dep+1, 'v');
				maze[i][j] = '-';
				maze[i][j+1] = '-';
			}

			else continue;

			if (val > best)
			{
				best = val;
				if (dep == turn)
				{
					best_x = i;
					best_y = j;
				}
			}

			if (best >= 64) return save(best);
		}
	}

	if (best == -65) best = -64;
	return save(best);
}

int main(){ 
	char player[5];
	scanf("%s", player);

	for(int i=0; i<N; i++){
		scanf("%s", maze[i]); 
	} 
	
	turn = 0;
	int cnt = 0;
	memset(vis, 0, sizeof(vis)); 
	for (int i=0; i<N; i++)
	{
		for (int j=0; j<N; j++)
		{
			if(maze[i][j] != '-')
			{
				cnt ++;
			}
		}
	}

	if (cnt == 0)
	{
		if (player[0] == 'v') puts("0 1");
		if (player[0] == 'h') puts("1 0");
		return 0;
	}

	if (cnt >= 20) sdeep = 4;
	if (cnt >= 30) sdeep = 5;
	if (cnt >= 36) sdeep = 6;
	if (cnt >= 40) sdeep = 7;
	if (cnt >= 42) sdeep = 8;

	loop = 0;
	
	best_x = -1;
	best_y = -1;
	negamax(turn, player[0]);
	printf("%d %d\n", best_x, best_y);
	//printf("-- %d\n", loop);
} 