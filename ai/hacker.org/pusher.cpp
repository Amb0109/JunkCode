#include<cstdio>
#include<cstring>
#include<vector>
#include<string>
using namespace std;
#include<winsock.h>

int lx,ly;
char maze[50][50];
char tmaze[50][50];
char ans[1000];
int ax,ay;

void clear(){
    for(int i=0;i<lx;i++){
        strcpy(tmaze[i],maze[i]);
    }
}

int dep;
int dir[4][2]={{1,0},{0,1},{0,-1},{-1,0}};
char sdir[4]={'D','R','L','U'};
bool dfs(int x,int y,int d){
	if(d==dep) return true;
    for(int i=0;i<4;i++){
        int tx=x+dir[i][0];
        int ty=y+dir[i][1];
        if(tx<0||ty<0||tx>=lx||ty>=ly) continue;
        if(tmaze[tx][ty]!='.') continue;
        while(true){
            tx=tx+dir[i][0];
            ty=ty+dir[i][1];
            if(tx<0||ty<0||tx>=lx||ty>=ly) break;
            if(tmaze[tx][ty]=='.') continue;
            if(tmaze[tx][ty]!='.'){
                int ttx=tx+dir[i][0];
                int tty=ty+dir[i][1];
                if(ttx<0||tty<0||ttx>=lx||tty>=ly) break;
                int tc=tmaze[ttx][tty];
                if(tmaze[tx][ty]=='a'){
                    tmaze[tx][ty]='.';
                    ans[dep-d-1]=sdir[i];
                    if(dfs(tx,ty,d+1)) return true;
                    tmaze[tx][ty]='a';
                    break;
                }else{
                    char c=tmaze[tx][ty];
                    if(tc=='.'){
                        tmaze[ttx][tty]=c-1;
                    }else{
                        tmaze[ttx][tty]+=c-'a';
                    }
                    tmaze[tx][ty]='.';
                    ans[dep-d-1]=sdir[i];
                    if(dfs(tx,ty,d+1)) return true;
                    tmaze[tx][ty]=c;
                    tmaze[ttx][tty]=tc;
                    break;
                }
            }
        }
    }
    return false;
}

SOCKET mysock;
void _init(){
	SOCKET id;
	WSADATA wsaData;
	SOCKADDR_IN addr;
	
	if(WSAStartup(MAKEWORD(2,0),&wsaData)){
		printf("Socket Error!");
		return;
	}
	
	id = socket(AF_INET, SOCK_STREAM, 0);
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(5002);
	bind(id, (sockaddr *)&addr, sizeof(addr));
	
	listen(id, 1);
	
	SOCKADDR_IN Client;
	int len = sizeof(SOCKADDR);
	
	mysock = accept(id,(SOCKADDR *)&Client,&len);
}

char msg[1024];
void _send(int ax,int ay){
	memset(msg,0,sizeof(msg));
	sprintf(msg,"%d,%d,",ay,ax);
	strcat(msg,ans);
	puts(msg);
	send(mysock,msg,1024,0);
}

char buff[1024];
void _recv(int &x,int &y){
	recv(mysock, buff, 1024,0);
	puts(buff);
	
	int p=2;
	y=0;
	for(;buff[p]<='9'&&buff[p]>='0';p++){
		y*=10; y+=buff[p]-'0';
	}
	
	p+=3;
	
	x=0;
	for(;buff[p]<='9'&&buff[p]>='0';p++){
		x*=10; x+=buff[p]-'0';
	}
	
	printf("%d %d\n",x,y);
	p+=7;
	for(int i=0;i<x;i++){
		for(int j=0;j<y;j++){
			maze[i][j]=buff[p++];
		}
		maze[i][y]=0;
		p++;
	}
}

int main(){
	_init();
	
	while(true){
		_recv(lx,ly);
		
		dep=0;
		for(int i=0;i<lx;i++){
			for(int j=0;j<ly;j++){
				if(maze[i][j]=='.') continue;
				dep+=maze[i][j]-'a'+1;
			}
		}
		
		printf("%d\n",dep);
        bool flag=false;
        for(int i=0;i<lx;i++){
            for(int j=0;j<ly;j++){
                if(maze[i][j]!='.') continue;
                if(dfs(i,j,0)){
                    ax=i; ay=j;
                    flag=true;
                    break;
                }
            }
            if(flag) break;
        }
        
        _send(ax,ay);
    }
}