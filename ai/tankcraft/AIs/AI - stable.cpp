// A stable AI. Very weak.
// Date: 2011-12-3
// Author: Amb

#include<cstring>
#include<queue>
using namespace std;
#include"Tank.h"

#define ABS(x) ((x)<0?(-x):(x))
#define INF 0x7fffffff

#define MAX_VTX 1000
#define MAX_EDG 100000
#define MAX_TASK_NUM 100

DataForAI now;

const int dir[4][2]={{-1,0},{1,0},{0,-1},{0,1}}; //0:UP 1:DOWN 2:LEFT 3:RIGHT
int dis[MAX_TASK_NUM][MAP_HEIGHT+2][MAP_WIDTH+2];
void getDis(Point src,int id); //得到点src的距离矩阵，保存到dis[id]中 
void updataDis(); //更新所有source的距离 
Order getOrder(); //安排走法 
void OrderTask(); //... 

int task[MAX_TANK_NUM];
struct TaskMgr{
	struct Edge{
		int vtx,cap,wgt,next;
		Edge(){}
		Edge(int v,int c,int w,int n):
			vtx(v),cap(c),wgt(w),next(n){}
	}E[MAX_EDG];
	int head[MAX_VTX],size;
	int pre[MAX_VTX],wgt[MAX_VTX],cur[MAX_VTX];
	bool vis[MAX_VTX];
	queue<int> que;
	
	void instGra(int u,int v,int c,int w);
	int mcmf(int s,int t);
	void getTask();
}myAI;


//平台0回合时调用此函数获取AI名称及坦克类型信息，请勿修改此函数声明。
extern "C" InitiateInfo chooseType(){
	puts(">chooseType");
	InitiateInfo Info;
	Info.tank[0]=Pioneer;
	Info.tank[1]=Pioneer;
	Info.tank[2]=Pioneer;
	Info.tank[3]=Pioneer;
	Info.tank[4]=Pioneer;
	strcpy(Info.aiName,"Amb"); //AI名请勿使用中文。
	return Info;
}

int pre=-1;
//平台从第1回合开始调用此函数获得每回合指令，请勿修改此函数声明。
extern "C" Order makeOrder(DataForAI data)
{
	puts(">makeOrder");
	now = data;
	if(now.round>pre) OrderTask();
	pre=now.round;
	Order order=getOrder();
	return order;
}



////////////////////////////////////////////////////////////////////////////////



void getDis(Point src,int id){
	if(src.row<0||src.col<0) return;
	memset(dis[id],-1,sizeof(dis[id]));
	queue<Point> que;
	Point s=Point(src.row,src.col);
	que.push(s);
	dis[id][src.row][src.col]=0;
	while(!que.empty()){
		Point u=que.front(); que.pop();
		for(int i=0;i<4;i++){
			int trow=u.row+dir[i][0];
			int tcol=u.col+dir[i][1];
			int td=dis[id][u.row][u.col]+1;
			if(now.map[trow][tcol].type==STONE) continue;
			if(now.map[trow][tcol].type==BREAKBRICK) td+=1;
			if(now.map[trow][tcol].type==BRICK) td+=2;
			if((~dis[id][trow][tcol])&&(td>=dis[id][trow][tcol])) continue;
			dis[id][trow][tcol]=td;
			que.push(Point(trow,tcol));
		}
	}
}

void updataDis(){
	memset(dis,-1,sizeof(dis));
	for(int i=0;i<MAX_SOURCE_NUM;i++){
		getDis(now.source[i],i);
	}
}

Order getOrder(){
	Order order;
	order.type=STOP;
	if(task[now.myID]==-1){
		return order;
	}else{
		int tdis=-1,td=-1;
		for(int i=0;i<4;i++){
			int trow=now.tank[now.myID].row+dir[i][0];
			int tcol=now.tank[now.myID].col+dir[i][1];
			if(dis[task[now.myID]][trow][tcol]==-1) continue;
			if(tdis==-1||dis[task[now.myID]][trow][tcol]<tdis){
				tdis=dis[task[now.myID]][trow][tcol];
				td=i;
			}
		}
		if(tdis==-1) return order;
		else{
			int trow=now.tank[now.myID].row+dir[td][0];
			int tcol=now.tank[now.myID].col+dir[td][1];
			if(now.map[trow][tcol].type==BRICK||
					now.map[trow][tcol].type==BREAKBRICK){
				order.type=FIRE;
				order.row=trow;
				order.col=tcol;
			}else{
				if(td==0) order.type=GOUP;
				if(td==1) order.type=GODOWN;
				if(td==2) order.type=GOLEFT;
				if(td==3) order.type=GORIGHT;
			}
		}
		return order;
	}
	return order;
}

void OrderTask(){
	puts(">TaskMgr");
	updataDis();
	myAI.getTask();
}


void TaskMgr::instGra(int u,int v,int c,int w){
	E[size]=Edge(v,c,w,head[u]);
	head[u]=size++;
	E[size]=Edge(u,0,-w,head[v]);
	head[v]=size++;
}

int TaskMgr::mcmf(int s,int t){
	int flow=0;
	while(true){
		memset(pre,-1,sizeof(pre));
		memset(vis,false,sizeof(vis));
		memset(wgt,0x7f,sizeof(wgt));
		wgt[s]=0;
		que.push(s);
		while(!que.empty()){
			int u=que.front(); que.pop();
			vis[u]=false;
			for(int i=head[u];~i;i=E[i].next){
				int v=E[i].vtx;
				if(E[i].cap&&wgt[u]+E[i].wgt<wgt[v]){
					wgt[v]=wgt[u]+E[i].wgt;
					if(!vis[v]){
						vis[v]=true;
						que.push(v);
					}
					pre[v]=u; cur[v]=i;
				}
			}
		}
		if(pre[t]==-1) break;
		int mf=0x7fffffff;
		for(int v=t;v!=s;v=pre[v]){
			mf=min(mf,E[cur[v]].cap);
		}
		for(int v=t;v!=s;v=pre[v]){
			E[cur[v]].cap-=mf;
			E[cur[v]^1].cap+=mf;
		}
		flow+=mf;
	}
	return flow;
}


void TaskMgr::getTask(){
	memset(task,-1,sizeof(task));
	memset(head,-1,sizeof(head));
	size=0;
	
	int s=999,t=998;
	int Tnum=0,Snum=0;
	
	//源点与坦克建边 
	for(int i=0;i<MAX_TANK_NUM;i++){
		if(now.tank[i].flag==now.myFlag){
			if(now.tank[i].life) Tnum++;
		}
	}
		
	if(Tnum==0) return;
	
	for(int i=0;i<MAX_TANK_NUM;i++){
		if(now.tank[i].flag==now.myFlag){
			if(now.tank[i].life) instGra(s,i,1,0);
		}
	}
	
	//资源与汇点建边 
	for(int i=0;i<MAX_SOURCE_NUM;i++){
		int trow=now.source[i].row;
		int tcol=now.source[i].col;
		if(trow<0||tcol<0) continue;
		if(now.map[trow][tcol].isHereSource==now.myFlag+1) continue;
		Snum++;
	}
	
	if(Snum==0) return;
	int cap=Tnum/Snum+1;
	for(int i=0;i<MAX_SOURCE_NUM;i++){
		int trow=now.source[i].row;
		int tcol=now.source[i].col;
		if(trow<0||tcol<0) continue;
		if(now.map[trow][tcol].isHereSource==now.myFlag+1) continue;
		instGra(i+MAX_TANK_NUM,t,cap,0);
	}
	
	//坦克与资源之间建边
	for(int i=0;i<MAX_TANK_NUM;i++){
		if(now.tank[i].flag!=now.myFlag) continue;
		for(int j=0;j<MAX_SOURCE_NUM;j++){
			if(dis[j][now.tank[i].row][now.tank[i].col]==-1) continue;
			instGra(i,j+MAX_TANK_NUM,1,dis[j][now.tank[i].row][now.tank[i].col]); 
		}
	}
	
	mcmf(s,t);
	
	for(int i=0;i<MAX_TANK_NUM;i++){
		if(now.tank[i].flag!=now.myFlag) continue;
		for(int j=head[i];~j;j=E[j].next){
			if(E[j].vtx<=900){
				if(E[j].cap==0){
					task[i]=E[j].vtx-MAX_TANK_NUM;
				}
			}
		}
	}
	
	for(int i=0;i<MAX_TANK_NUM;i++){
		printf("%d ",task[i]);
	}puts("");
}


