// in:  x=3&y=3&board=CbBbCbAaCbCaCbAbCb
// out: path=0,1_2,1_2,0_0,0_0,2_2,2_1,2_1,0_1,1

#include <cstdio>
#include <cstring>
#include <vector>
#include <algorithm>
#include <ctime>
using namespace std;
#define BIG 5000000

int x, y;
char board[BIG];
char path[BIG];
char tp[100];

vector<int> gra[BIG];

vector<int> ans;
bool vis[BIG];
void dfs(int u, int dep){
	if(dep == x*y) return;
	int p = rand()%gra[u].size();
	int v = gra[u][p];
	while(vis[v]){
		vector<int>::iterator it = ++find(ans.begin(), ans.end(), v);
		if(u == *it) reverse(ans.begin(), ans.end());
		else reverse(it, ans.end());
		it = --ans.end();
		u = *it;
		p = rand()%gra[u].size();
		v = gra[u][p];
	}
	vis[v] = true;
	ans.push_back(v);
	dfs(v, dep+1);
}


void calc(){

	for(int i=0; i<x*y; i++){
		gra[i].clear();
	}

	for(int i=0; i<x; i++){
		for(int j=0; j<y ;j++){

			for(int ii=i+1; ii<x ; ii++){
				if(board[(i*y+j)<<1] == board[(ii*y+j)<<1] || board[(i*y+j)<<1|1] == board[(ii*y+j)<<1|1]){
					gra[i*y+j].push_back(ii*y+j);
					gra[ii*y+j].push_back(i*y+j);
				}
			}

			for(int jj=j+1; jj<y ; jj++){
				if(board[(i*y+j)<<1] == board[(i*y+jj)<<1] || board[(i*y+j)<<1|1] == board[(i*y+jj)<<1|1]){
					gra[i*y+j].push_back(i*y+jj);
					gra[i*y+jj].push_back(i*y+j);
				}
			}

		}
	}

	memset(vis, false, sizeof(vis));
	vis[0] = true;
	ans.clear();
	ans.push_back(0);
	dfs(0, 1);
	path[0]=0;
	for(int i=0;i<x*y;i++){
		if(i) strcat(path, "_");
		sprintf(tp, "%X,%X", ans[i]%y, ans[i]/y);
		strcat(path, tp);
	}
}

char msg[BIG];
void recv(){
	scanf("%s", msg);
	sscanf(msg, "x=%d&y=%d&board=%s", &x, &y, &board);
}

void send(){
	memset(msg, 0, sizeof(msg));
	strcpy(msg, "path=");
	strcat(msg, path);
	puts(msg);
	fflush(stdout);
}

int main(){
	while(true){
		recv();
		calc();
		send();
	}
}