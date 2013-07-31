#include<cstdio>
#include<cstring>
using namespace std;

int vmax,vmin;
int lx,ly;
char maze[500000];
char path[500000];

char msg[500000];
void _send(){
	memset(msg, 0, sizeof(msg));
	strcpy(msg, "path=");
	strcat(msg, path);
	puts(msg);
	fflush(stdout);
}

void _recv(){
	scanf("%s", msg);
	//puts(msg);
	//FVterrainString=..X...X..&FVinsMax=2&FVinsMin=2&FVboardX=3&FVboardY=3&FVlevel=0
	int p=16;
	for(int i=0;;p++,i++){
		if(msg[p]=='&'){
			maze[i]=0;
			break;
		}
		maze[i]=msg[p];
	}

	sscanf(msg+p,"&FVinsMax=%d&FVinsMin=%d&FVboardX=%d&FVboardY=%d",&vmax,&vmin,&lx,&ly);
}

int llx,lly,dep;
char buff[500000];
bool vis[500000];
char sdir[]="RD";
bool dfs(int x,int y){
	vis[y+x*lly]=true;
	if(x+y==dep) return true;
	
	for(int i=0;i<2;i++){
		int tx=x, ty=y;
		i?tx+=1:ty+=1;
		if(tx>=llx||ty>=lly) continue;
		if(buff[ty+tx*lly]=='X') continue;
		if(vis[ty+tx*lly]) continue;
		path[x+y]=sdir[i];
		if(dfs(tx,ty)) return true;
	}
	return false;
}

void run(){
	for(dep=vmin;dep<=vmax;dep++){
		for(llx=1;llx<=dep+1;llx++){
			lly=dep-llx+2;
			memset(buff,'.',sizeof(buff));
			memset(vis,false,sizeof(vis));
			for(int k=0;;k++){
				if(k*llx-k>=lx) break;
				if(k*lly-k>=ly) break;
				for(int x=0;x<llx;x++){
					if(x+k*llx-k>=lx) continue;
					for(int y=0;y<lly;y++){
						if(y+k*lly-k>=ly) continue;
						if(maze[y+k*lly-k+(x+k*llx-k)*ly]=='.') continue;
						else buff[y+x*lly]='X';
					}
				}
				//printf("- %d %d\n",k*llx-k,k*lly-k);
			}
			buff[llx*lly]=0;
			if(buff[0]=='X') continue;
			if(buff[llx*lly-1]=='X') continue;
			if(dfs(0,0)){
				path[dep]=0;
				return;
			}
		}
	}
}

int main(){

	while(true){
		_recv();

		run();

		_send();
	}

	return 0 ;
}