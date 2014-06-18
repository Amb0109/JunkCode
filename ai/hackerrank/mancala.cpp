#include <cstdio>
#include <cstring>
#include <algorithm>
using namespace std;

#define N 6
#define BP 10000019
#define ULL unsigned long long

#define MAX_DEP 12

int a[N+1], b[N+1];
int loop;
int sdeep = 10;

inline void show()
{
	printf("%d\n", a[0]);
	for(int i=1; i<=N; i++)
	{
		printf("%d ", a[i]);
	}
	puts("");
	printf("%d\n", b[0]);
	for(int i=1; i<=N; i++)
	{
		printf("%d ", b[i]);
	}
	puts("");
	puts("======");
}

inline bool is_no_move(int *p)
{
	for (int i=1; i<=N; ++i)
	{
		if (p[i] != 0) return false;
	}
	return true;
}

int amem[MAX_DEP][N+1];
int bmem[MAX_DEP][N+1];

inline void save(int dep)
{
	memcpy(amem[dep], a, sizeof(a));
	memcpy(bmem[dep], b, sizeof(b));
}

inline void load(int dep)
{
	memcpy(a, amem[dep], sizeof(a));
	memcpy(b, bmem[dep], sizeof(b));
}

inline int do_move(int *p, int *op, int pos)
{
	if (p[pos] == 0) return -1;

	int cnt = p[pos];
	p[pos] = 0;

	int sp = pos;

	int *_p = p;
	int *_op = op;

	while (cnt)
	{
		for (int i=sp+1; i<=N; ++i)
		{
			++_p[i];
			--cnt;
			if (cnt == 0)
			{
				if (_p == p && _p[i] == 1)
				{
					_p[i] += _op[7-i];
					_op[7-i] = 0;
					return 1;
				} else return 0;
			}
		}
		if (cnt)
		{
			++_p[0];
			--cnt;
			if (_p == p && cnt == 0) return 2;
		}
		sp = 0;
		swap(_p, _op);
	}
	return 0;
}

int best_pos;

int negamax(int dep, char player)
{
	++loop;

	int *p = player == '1' ? a : b ;
	int *op = player == '1' ? b : a ;

	if (dep >= sdeep)
	{
		int ret = 0;
		for (int i=0; i<=N; ++i)
		{
			ret += p[i];
			ret -= op[i];
		}

		ret += p[0] - op[0];
		ret >>= 1;

		return ret;
	}

	if (is_no_move(p) || is_no_move(op))
	{
		int ret = 0;
		for (int i=0; i<=N; ++i)
		{
			ret += p[i];
			ret -= op[i];
		}

		return ret;
	}

	save(dep);
	//show();
	//getchar();

	int best = -50;

	for (int i=1; i<=N; ++i)
	{
		if (p[i] == 0) continue;

		int ret = do_move(p, op, i);
		int val = 0;
		if (ret == 2)
		{
			val = negamax(dep+1, player);
		}
		else
		{
			val = -negamax(dep+1, player == '1' ? '2' : '1' );
		}
		load(dep);

		if (val > best)
		{
			best = val;
			if (dep == 0)
			{
				best_pos = i;
			}
		}
	}

	return best;
}

int main(){ 
	char player[5];
	scanf("%s", player);

	for(int i=0; i<=N; i++){
		scanf("%d", &a[i]); 
	} 

	for(int i=0; i<=N; i++){
		scanf("%d", &b[i]); 
	}

	loop = 0;
	
	best_pos = 0;
	negamax(0, player[0]);
	printf("%d\n", best_pos);
	//printf("-- %d\n", loop);
} 