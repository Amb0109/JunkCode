#include<cstdio>
#include<cstring>
#include<vector>
#include<string>
using namespace std;
#include<winsock.h>
#include<windows.h>

int lx,ly;
char maze[50][50];
const int dir[4][2]={{1,0},{0,1},{0,-1},{-1,0}};
const char sdir[4]={'D','R','L','U'};
int ax,ay;

bool finded;
int dep;

char ans[1000];
char tans[50][1000];
char tmaze[50][50][50];
bool dfs(int x,int y,int d,int l){
	if(finded) return false;
	if(d==dep) return true;
    for(int i=0;i<4;i++){
        int tx=x+dir[i][0];
        int ty=y+dir[i][1];
        if(tx<0||ty<0||tx>=lx||ty>=ly) continue;
        if(tmaze[l][tx][ty]!='.') continue;
        while(true){
            tx=tx+dir[i][0];
            ty=ty+dir[i][1];
            if(tx<0||ty<0||tx>=lx||ty>=ly) break;
            if(tmaze[l][tx][ty]=='.') continue;
            if(tmaze[l][tx][ty]!='.'){
                int ttx=tx+dir[i][0];
                int tty=ty+dir[i][1];
                if(ttx<0||tty<0||ttx>=lx||tty>=ly) break;
                int tc=tmaze[l][ttx][tty];
                if(tmaze[l][tx][ty]=='a'){
                    tmaze[l][tx][ty]='.';
                    tans[l][d]=sdir[i];
                    if(dfs(tx,ty,d+1,l)) return true;
                   	if(finded) return false;
                    tmaze[l][tx][ty]='a';
                    break;
                }else{
                    char c=tmaze[l][tx][ty];
                    if(tc=='.'){
                        tmaze[l][ttx][tty]=c-1;
                    }else{
                        tmaze[l][ttx][tty]+=c-'a';
                    }
                    tmaze[l][tx][ty]='.';
                    tans[l][d]=sdir[i];
                    if(dfs(tx,ty,d+1,l)) return true;
                   	if(finded) return false;
                    tmaze[l][tx][ty]=c;
                    tmaze[l][ttx][tty]=tc;
                    break;
                }
            }
        }
    }
    return false;
}

DWORD WINAPI fun(LPVOID lpParam){
	int x = *(int *)lpParam;
	for(int y=ly-1;y>=0;y--){
		
		if(maze[x][y]!='.') continue;
		for(int i=0;i<lx;i++){
			strcpy(tmaze[x][i],maze[i]);
		}
		if(dfs(x, y, 0, x)){
			tans[x][dep]=0;
			strcpy(ans,tans[x]);
			ax = x;
			ay = y;
			finded = true;
			break;
		}
		if(finded) break;
	}
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
		
		DWORD id[50];
		HANDLE hThread[50];
		int line[50];
		for(int i=0;i<50;i++) line[i]=i;
		
		finded = false;
		for(int i=0;i<lx;i++){
			hThread[i] = CreateThread (NULL, 0, fun, &line[i], 0, &id[i]);
		}
		
		while(!finded);
		
		_send(ax,ay);
	}
	
	return 0 ;
}