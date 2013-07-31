#include <cstdio>
#include <cstring>
using namespace std;

#define N 3

int turn;
char maze[5][5];
bool win(char c){
	int cx[5]={0}, cy[5]={0}, d[5]={0}; 
	for(int i=0; i<N; i++){
		for(int j=0; j<N; j++){
			if(maze[i][j]==c){
				cx[i]++;
				cy[j]++; 
				if(i==j) d[0]++; 
				if(i+j==N-1) d[1]++;
			}  
		}   
	}  
	if(d[0]==N) return true;
	if(d[1]==N) return true; 
	for(int i=0;i<N;i++){
		if(cx[i]==N) return true;
		if(cy[i]==N) return true; 
	} 
	return false;
} 

inline int zip(){
	int res=0; 
	for(int i=0; i<N; i++){
		for(int j=0; j<N; j++){
			res*=3; 
			if(maze[i][j]=='_') res+=0;
			else if(maze[i][j]=='X') res+=1;
			else if(maze[i][j]=='O') res+=2; 
		} 
	} 
	return res; 
} 
 
bool vis[60000];
int mem[60000];
int best_x, best_y;

int negamax(int dep, char player)
{
	int z = zip();
	if (vis[z]) return mem[z];
	if (win((player=='X')?'O':'X')) return vis[z] = true, mem[z] = -1;
	if (dep == N*N) return vis[z] = true, mem[z] = 0;

	int best = -5;
	
	for (int i=0; i<N; ++i)
	{
		for (int j=0; j<N; ++j)
		{
			if (maze[i][j] != '_') continue;
			maze[i][j] = player;
			int val = -negamax(dep+1, (player=='X')?'O':'X');
			maze[i][j] = '_';

			//printf("??? %d %d\n", dep, val);

			if (val > best)
			{
				best = val;
				if (dep == turn)
				{
					best_x = i;
					best_y = j;
				}
			}

			if (best == 1) return vis[z] = true, mem[z] = best;
		}
	}
	return vis[z] = true, mem[z] = best;
}

int main(){ 
	char player[5];
	scanf("%s", player);

	for(int i=0; i<N; i++){
		scanf("%s", maze[i]); 
	} 
	
	turn = 0;
	memset(vis, false, sizeof(vis)); 
	for (int i=0; i<N; i++)
	{
		for (int j=0; j<N; j++)
		{
			if(maze[i][j] != '_')
			{
				turn ++;
			}
		}
	}
	
	best_x = -1;
	best_y = -1;
	negamax(turn, player[0]);
	printf("%d %d\n", best_x, best_y);

} 