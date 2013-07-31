#include<queue>
#include<ctime>
#include<vector>
#include<cstdlib>
#include<cstring>
#include<algorithm>
using namespace std;
#include"Tank.h" 

#define ABS(x) ((x)<0?(-x):(x))

//费用流相关BEGIN
#define INT_INF 0x7fffffff
#define REAL_INF 1000000000.0
#define MAX_VTX 1003
#define MAX_EDG 100000

int atkVal,grbVal;
double atkFac,grbFac;

struct TaskFlow{
	struct Edge{
		int vtx,cap;
		double wgt;
		int next;
		Edge(){}
		Edge(int v,int c,double w,int n):
		vtx(v),cap(c),wgt(w),next(n){}
	}E[MAX_EDG];
	int head[MAX_VTX],size;
	int pre[MAX_VTX],cur[MAX_VTX];
	double wgt[MAX_VTX];
	bool vis[MAX_VTX];
	queue<int> que;

	void initGra();
	void instGra(int u,int v,int c,double w);
	int runFlow(int s,int t);
	int getAssign(int id);
}TFlow;
//费用流相关END


#define MAX_TASK_NUM 500  //任务最大值

const int DIR[4][2]={{-1,0},{1,0},{0,-1},{0,1}}; //0:UP 1:DOWN 2:LEFT 3:RIGHT

int SOURCE_NUM;
FlagType myFlg;
SourceType mySrc;

struct POS{
	int row,col;
	POS():row(-1),col(-1){}
	POS(int r,int c):row(r),col(c){}
	POS(Point pos):row(pos.row),col(pos.col){}

	bool legal(){
		if(row==-1||col==-1) return false;
		return true;
	}

	friend bool operator== (const POS& a,const POS& b){
		if(a.row==b.row&&a.col==b.col) return true;
		else return false;
	}

	friend int mhtDis(const POS& a,const POS& b){
		return ABS((a.row-b.row))+ABS((a.col-b.col));
	}

	POS shift(int dir){
		return POS(row+DIR[dir][0],col+DIR[dir][1]);
	}

};

struct CELL{
	POS pos;
	CellType type;
	short source;
	short tank;

	CELL(){}
	CELL(POS p,MapCell cell,Point src[MAX_SOURCE_NUM]){
		pos=p;
		type=cell.type;
		if(cell.isHereSource==NotSource) source=-1;
		else{
			for(int i=0;i<SOURCE_NUM;i++){
				if(pos==POS(src[i])) source = i;
			}
		}
	}

	void updata(MapCell cell){
		type=cell.type;
		tank=cell.whoIsHere;
	}
};

struct TANK{
	short ID;
	FlagType flag;
	POS pos;
	TankType type;
	short life;

	TANK(){}
	TANK(TankData tnk){
		ID=tnk.ID;
		flag=tnk.flag;
		type=tnk.type;
	}

	void updata(TankData tnk){
		pos=POS(tnk.row,tnk.col);
		life=tnk.life;
	}
};

struct SOURCE{
	short ID;
	POS pos;
	SourceType flag;

	SOURCE(){}
	SOURCE(short _ID,POS p){
		ID=_ID;
		pos=p;
	}

	void updata(MapCell map[MAP_HEIGHT+2][MAP_WIDTH+2]){
		switch(map[pos.row][pos.col].isHereSource){
		case RedSource: flag=RedSource; break;
		case BlueSource: flag=BlueSource; break;
		case NeutralSource: flag=NeutralSource; break;
		default: flag=NotSource;
		}
	}
};

typedef enum {ATTACK,ESCAPE,GRABSRC,PROTECT,FOLLOW,WALK} TaskType;

struct TASK{
	POS pos;
	TaskType type;
	short status;  //-1:空任务 -2:公有任务，i:第i个坦克的私有任务
	short value;
	int DIS[MAP_HEIGHT+2][MAP_WIDTH+2];

	TASK():status(-1){}
	TASK(POS _pos,TaskType _type,short _status,short _value):
	pos(_pos),type(_type),status(_status),value(_value){}

	void getDis();
	int dis(POS p){
		return DIS[p.row][p.col];
	}
};

struct ORDER{
	Order order;
	double val;

	ORDER(){}
	ORDER(Order _order,double _val):
		order(_order),val(_val){}

	friend bool operator< (const ORDER& a,const ORDER& b){
		return a.val>b.val;
	}
};

struct STATUS{
	CELL map[MAP_HEIGHT+2][MAP_WIDTH+2];
	TANK tank[MAX_TANK_NUM];
	SOURCE source[MAX_SOURCE_NUM];
	short glddif,srcdif;

	STATUS(){}
	STATUS(DataForAI data){
		for(int i=0;i<MAX_TANK_NUM;i++) tank[i]=TANK(data.tank[i]);
		for(int i=0;i<SOURCE_NUM;i++) source[i]=SOURCE(i,data.source[i]);
		for(int i=0;i<MAP_HEIGHT+2;i++){
			for(int j=0;j<MAP_WIDTH+2;j++){
				map[i][j]=CELL(POS(i,j),data.map[i][j],data.source);
			}
		}
		glddif=data.blueGoldNum-data.redGoldNum;
		srcdif=data.blueSource-data.redSource;
		if(myFlg==RED){
			glddif=-glddif;
			srcdif=-srcdif;
		}
	}

	void updata(DataForAI data){
		for(int i=0;i<MAX_TANK_NUM;i++) tank[i].updata(data.tank[i]);
		for(int i=0;i<SOURCE_NUM;i++) source[i].updata(data.map);
		for(int i=0;i<MAP_HEIGHT+2;i++){
			for(int j=0;j<MAP_WIDTH+2;j++){
				map[i][j].updata(data.map[i][j]);
			}
		}
	}

	CELL get(POS pos){
		return map[pos.row][pos.col];
	}
}S;



double TankAss[MAX_TANK_NUM][MAX_TASK_NUM];  //坦克和对应任务的重要值
int TankTsk[MAX_TANK_NUM];  //坦克分到的任务
vector<ORDER> OdrList[MAX_TANK_NUM];
Order TankOdr[MAX_TANK_NUM];  //坦克的走法
TASK Task[MAX_TASK_NUM];

int defTsk(POS,TaskType,short);  //生成当前所有任务
void PnrAss(int); //评估Pioneer
void StkAss(int); //评估Striker
void SnpAss(int); //评估Sniper
void PnrOdr(int); //得到走法
void StkOdr(int);
void SnpOdr(int);
void tarOdr(); //目标分配 

void initAI(DataForAI);
void initRound(DataForAI); //初始化值
void adjVal();
void getTsk(); //更新距离
void getAss(); //得到评估
void getOdr(); //得到走法

void OrderMgr(); //... 
Order RandOrder(TANK);  //得到一个随机走法 


int pre_round;
//平台0回合时调用此函数获取AI名称及坦克类型信息，请勿修改此函数声明。
extern "C" InitiateInfo chooseType(){
	puts(">chooseType");
	InitiateInfo Info;
	Info.tank[0]=Striker;
	Info.tank[1]=Striker;
	Info.tank[2]=Pioneer;
	Info.tank[3]=Pioneer;
	Info.tank[4]=Pioneer;
	strcpy(Info.aiName,"Amb"); //AI名请勿使用中文。
	pre_round=-1;
	return Info;
}

//平台从第1回合开始调用此函数获得每回合指令，请勿修改此函数声明。
extern "C" Order makeOrder(DataForAI data){
	if(pre_round==-1) initAI(data);
	if(data.round>pre_round){
		puts(">makeOrder");
		initRound(data);
		OrderMgr();
		pre_round=data.round;
	}
	return TankOdr[data.myID];
}



////////////////////////////////////////////////////////////////////////////////

void initAI(DataForAI data){
	SOURCE_NUM = data.totalSource;
	myFlg = data.myFlag;
	if(myFlg == RED) mySrc = RedSource;
	else mySrc = BlueSource;
	S = STATUS(data);
	//	adjVal();
}

void initRound(DataForAI data){
	puts(">>initRound");
	S.updata(data);
	for(int i=0;i<MAX_TASK_NUM;i++) Task[i]=TASK();
	for(int i=0;i<MAX_TANK_NUM;i++){
		OdrList[i].clear();
		for(int j=0;j<MAX_TASK_NUM;j++){
			TankAss[i][j]=-1.0;
		}
	}
	memset(TankTsk,-1,sizeof(TankTsk));
	adjVal();
	puts("<<initRound");
}

void getTsk(){
	puts(">>getTsk");
	for(int i=0;i<MAX_TANK_NUM;i++){
		if(S.tank[i].flag == myFlg) continue; 
		defTsk(S.tank[i].pos,ATTACK,atkVal);
		defTsk(S.tank[i].pos,ESCAPE,atkVal);
	}
	for(int i=0;i<SOURCE_NUM;i++){
		if(S.source[i].flag == mySrc) continue;
		defTsk(S.source[i].pos,GRABSRC,grbVal);
	}
	puts("<<getTsk");
}

void getAss(){
	puts(">>getAss");
	for(int i=0;i<MAX_TANK_NUM;i++){
		if(S.tank[i].flag != myFlg) continue;
		if(S.tank[i].life == 0) continue;
		if(S.tank[i].type==Striker) StkAss(i);
		if(S.tank[i].type==Sniper) SnpAss(i);
		if(S.tank[i].type==Pioneer) PnrAss(i);
	}
	puts("<<getAss");
}

void getOdr(){
	puts(">>getOdr");
	tarOdr();
	for(int i=0;i<MAX_TANK_NUM;i++){
		if(S.tank[i].flag != myFlg) continue;
		if(S.tank[i].life == 0) continue;
		if(S.tank[i].type==Striker) StkOdr(i);
		if(S.tank[i].type==Sniper) SnpOdr(i);
		if(S.tank[i].type==Pioneer) PnrOdr(i);
	}
	puts("<<getOdr");
}

void OrderMgr(){
	puts(">OrderMgr");
	getTsk();
	getAss();
	getOdr();

	for(int i=0;i<MAX_TANK_NUM;i++){
		printf(" %d",TankTsk[i]);
	}puts("");
	puts("<OrderMgr");
}


int defTsk(POS src,TaskType typ,short val){
	if(!src.legal()) return -1;
	int id=-1;
	for(int i=0;i<MAX_TASK_NUM;i++){
		if(~Task[i].status) continue;
		Task[i]=TASK(src,typ,1,val);
		Task[i].getDis();
		id=i; break;
	}
	return id;
}


void StkAss(int id){
	POS me=S.tank[id].pos;
	for(int i=0;i<MAX_TASK_NUM;i++){
		if(Task[i].status==-1) break;
		TankAss[id][i]=-1.0;
		if(Task[i].type==GRABSRC){
			TankAss[id][i]=1.0/(Task[i].dis(me)+2.0);
		}else if(Task[i].type==ATTACK){
			int tdis=mhtDis(Task[i].pos,me);
			if(tdis>STRIKER_RANGE) continue;  //如果坦克在视野范围之外，无视
			TANK enemy = S.tank[S.get(Task[i].pos).tank];
			if(enemy.type==Pioneer){
				if(tdis>1) TankAss[id][i]=0.95;
				else if(enemy.life<=STRIKER_ATTACK) TankAss[id][i]=1.0;
			}else{
				TankAss[id][i]=0.75;
			}
		}else if(Task[i].type==ESCAPE){
			int tdis=mhtDis(Task[i].pos,me);
			if(tdis>SIGHT) continue;
			TANK enemy = S.tank[S.get(Task[i].pos).tank];
			if(enemy.type==Pioneer){
				if(tdis==1&&enemy.life>STRIKER_ATTACK) TankAss[id][i]=1.0;
			}
		}
	}
}

void SnpAss(int id){
}

void PnrAss(int id){
	//	puts(">>>PnrAss");
	POS me=S.tank[id].pos;
	for(int i=0;i<MAX_TASK_NUM;i++){
		if(Task[i].status==-1) break;
		TankAss[id][i]=-1.0;
		if(Task[i].type==GRABSRC){
			TankAss[id][i]=1.0/(Task[i].dis(me)+2.0);
		}else if(Task[i].type==ATTACK){
			int tdis=mhtDis(Task[i].pos,me);
			if(tdis>PIONEER_RANGE) continue;
			TankAss[id][i]=0.2;
		}else if(Task[i].type==ESCAPE){
			int tdis=mhtDis(Task[i].pos,me);
			if(tdis>SIGHT) continue;
			TANK enemy = S.tank[S.get(Task[i].pos).tank];
			if(enemy.type==Pioneer){
				if(tdis>2) continue;
				}else{
					if(enemy.life>=S.tank[id].life) TankAss[id][i]=1.0;
					else TankAss[id][i]=0.75;
				}
			if(enemy.type==Striker){
				if(tdis>3) TankAss[id][i]=1.0;
			}else if(enemy.type==Sniper){
				TankAss[id][i]=0.5;
			}
		}
	}
	//	puts("<<<PnrAss");
}

Order walkOdr(short tank_id,short task_id){
	Order order;
	order.type=STOP;
	OdrList[tank_id].push_back(ORDER(order,0.4));
	for(int i=0;i<4;i++){
		POS tpos=S.tank[tank_id].pos.shift(i);
		if(S.get(tpos).type==STONE) continue;
		double val=1.0;
		int dis=Task[task_id].dis(S.tank[tank_id].pos);
		if(Task[task_id].dis(tpos)>dis) val*=0.1;
		else if(Task[task_id].dis(tpos)==dis) val*=0.3;
		if(S.get(tpos).type==BREAKBRICK) val*=0.3;
		if(S.get(tpos).type==BRICK) val*=0.1;
		if(~S.get(tpos).tank) val*=0.1;
		order.type=OrderType(order.type+1);
		OdrList[tank_id].push_back(ORDER(order,val));
	}
	order.type=FIRE;
	for(int i=0;i<4;i++){
		POS tpos=S.tank[tank_id].pos.shift(i);
		if(S.get(tpos).type==STONE) continue;
		if(S.get(tpos).type==PERVIOUS) continue;
		double val=1.0;
		if(S.get(tpos).type==BREAKBRICK) val*=0.6;
		if(S.get(tpos).type==BRICK){
			if(S.tank[tank_id].type==Pioneer) val*=0.4;
			else val*=0.6;
		}
		int dis=Task[task_id].dis(S.tank[tank_id].pos);
		if(Task[task_id].dis(tpos)>dis) val*=0.05;
		else if(Task[task_id].dis(tpos)==dis) val*=0.2;
		order.row=tpos.row;
		order.col=tpos.col;
		OdrList[tank_id].push_back(ORDER(order,val));
	}
	sort(OdrList[tank_id].begin(),OdrList[tank_id].end());
	return OdrList[tank_id][0].order;
}

void StkOdr(int id){
	TankOdr[id]=RandOrder(S.tank[id]);
	if(~TankTsk[id]){
		int tdis=-1,td=-1;
		switch(Task[TankTsk[id]].type){	
		case GRABSRC:
		case WALK:
			TankOdr[id]=walkOdr(id,TankTsk[id]);
			break;
		case ATTACK:
			TankOdr[id].type=FIRE;
			TankOdr[id].row=Task[TankTsk[id]].pos.row;
			TankOdr[id].col=Task[TankTsk[id]].pos.col;
			break;
		case ESCAPE:
			for(int i=0;i<4;i++){
				POS tpos=S.tank[id].pos.shift(i);
				if(Task[TankTsk[id]].dis(tpos)==-1) continue;
				if(Task[TankTsk[id]].dis(tpos)>tdis){
					tdis=Task[TankTsk[id]].dis(tpos);
					td=i;
				}
			}
			if(~tdis){
				POS tpos=S.tank[id].pos.shift(td);
				if(S.get(tpos).type==BRICK||S.get(tpos).type==BREAKBRICK){
					TankOdr[id].type=FIRE;
					TankOdr[id].row=tpos.row;
					TankOdr[id].col=tpos.col;
				}else TankOdr[id].type=OrderType(td+1);
			}
			break;
		}
	}
}

void SnpOdr(int id){
}

void PnrOdr(int id){
	TankOdr[id]=RandOrder(S.tank[id]);
	if(~TankTsk[id]){
		int tdis=-1,td=-1;
		switch(Task[TankTsk[id]].type){	
		case GRABSRC:
		case WALK:
			TankOdr[id]=walkOdr(id,TankTsk[id]);
			break;
		case ATTACK:
			TankOdr[id].type=FIRE;
			TankOdr[id].row=Task[TankTsk[id]].pos.row;
			TankOdr[id].col=Task[TankTsk[id]].pos.col;
			break;
		case ESCAPE:
			for(int i=0;i<4;i++){
				POS tpos=S.tank[id].pos.shift(i);
				if(Task[TankTsk[id]].dis(tpos)==-1) continue;
				if(Task[TankTsk[id]].dis(tpos)>tdis){
					tdis=Task[TankTsk[id]].dis(tpos);
					td=i;
				}
			}
			if(~tdis){
				POS tpos=S.tank[id].pos.shift(td);
				if(S.get(tpos).type==BRICK||S.get(tpos).type==BREAKBRICK){
					TankOdr[id].type=FIRE;
					TankOdr[id].row=tpos.row;
					TankOdr[id].col=tpos.col;
				}else TankOdr[id].type=OrderType(td+1);
			}
			break;
		}
	}
}

void adjVal(){
	if(S.srcdif>1){
		atkFac=1.0;
		atkVal=1;
		grbFac=1.0;
		grbVal=1;
	}else{
		atkFac=1.0;
		atkVal=1;
		grbFac=1.0;
		grbVal=1;
	}
}

void tarOdr(){
	//	puts(">>>tarOdr");
	TFlow.initGra();
	int FS=MAX_VTX-1;
	int FT=MAX_VTX-2;

	int TankNum=0;
	for(int i=0;i<MAX_TANK_NUM;i++){
		if(S.tank[i].flag != myFlg) continue;
		if(S.tank[i].life == 0) continue;
		TFlow.instGra(FS,i,1,0);
	}
	for(int i=0;i<MAX_TANK_NUM;i++){
		if(S.tank[i].flag != myFlg) continue;
		if(S.tank[i].life == 0) continue;
		for(int j=0;j<MAX_TASK_NUM;j++){
			if(Task[i].status==-1) continue;
			if(TankAss[i][j]<0) continue;
			TFlow.instGra(i,1000-j,1,1.0-TankAss[i][j]);
		}
	}

	for(int i=0;i<MAX_TASK_NUM;i++){
		if(Task[i].status==-1) continue;
		TFlow.instGra(1000-i,FT,Task[i].value,0);
	}

	TFlow.runFlow(FS,FT);

	for(int i=0;i<MAX_TANK_NUM;i++){
		if(S.tank[i].flag != myFlg) continue;
		if(S.tank[i].life == 0) continue;
		TankTsk[i]=TFlow.getAssign(i);
	}
	//	puts("<<<tarOdr");
}

void TASK::getDis(){
	memset(DIS,-1,sizeof(DIS));
	queue<POS> que;
	que.push(pos);
	DIS[pos.row][pos.col]=0;
	while(!que.empty()){
		POS u=que.front(); que.pop();
		for(int i=0;i<4;i++){
			POS v(u.shift(i));
			int td=DIS[u.row][u.col]+1;
			if(S.get(v).type==STONE) continue;
			if(S.get(v).type==BREAKBRICK) td+=1;
			if(S.get(v).type==BRICK) td+=2;
			if((~DIS[v.row][v.col])&&(td>=DIS[v.row][v.col])) continue;
			DIS[v.row][v.col]=td;
			que.push(v);
		}
	}
}


//费用流部分
void TaskFlow::initGra(){
	memset(head,-1,sizeof(head));
	size=0;
}

void TaskFlow::instGra(int u,int v,int c,double w){
	E[size]=Edge(v,c,w,head[u]);
	head[u]=size++;
	E[size]=Edge(u,0,-w,head[v]);
	head[v]=size++;
}

int TaskFlow::runFlow(int s,int t){
	int flow = 0;
	while(true){
		memset(pre,-1,sizeof(pre));
		memset(vis,false,sizeof(vis));
		for(int i=0;i<MAX_VTX;i++) wgt[i] = REAL_INF;
		wgt[s]=0.0;
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
		int mf=INT_INF;
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

int TaskFlow::getAssign(int id){
	for(int i=head[id];~i;i=E[i].next){
		int v=E[i].vtx;
		if(E[i].cap) continue;
		if(v<=1000) return 1000-v;
	}
	return -1;
}


//得到一个随机的走法 
Order RandOrder(TANK tnk){
	Order order;
	order.type=STOP;

	srand(clock());

	int cnt=5;
	while(order.type==STOP&&(cnt--)){
		int tr=rand()%5; 
		if(tr){
			int td=tr-1;
			POS tpos(tnk.pos.shift(td));
			if(S.get(tpos).type==BRICK||
				S.get(tpos).type==BREAKBRICK){
					order.type=FIRE;
					order.row=tpos.row;
					order.col=tpos.col;
			}else if(S.get(tpos).type==PERVIOUS){
				order.type=OrderType(tr);
			}
		}
	}
	return order;
}














/*
Order StrikerOrder(){
Order order=RandOrder();
if(~task[S.myID]){
vector<TankData> tar;
for(int i=0;i<MAX_TASK_NUM;i++){
enm=S.tank[i];
if(enm.flag==S.myFlag) continue;
if(enm.row<0||enm.col<0) continue;
if(MhtDis(enm,me)<=STRIKER_RANGE) tar.push_back(enm);
}
if(tar.empty()){
int tdis=-1,td=-1;
for(int i=0;i<4;i++){
int trow=me.row+dir[i][0];
int tcol=me.col+dir[i][1];
if(dis[task[S.myID]][trow][tcol]==-1) continue;
if(tdis==-1||dis[task[S.myID]][trow][tcol]<tdis){
tdis=dis[task[S.myID]][trow][tcol];
td=i;
}
}
if(td!=-1){
int trow=me.row+dir[td][0];
int tcol=me.col+dir[td][1];
if(S.map[trow][tcol].type==BRICK||
S.map[trow][tcol].type==BREAKBRICK){
order.type=FIRE;
order.row=trow;
order.col=tcol;
}else order.type=OrderType(td+1);
}
}else{
int tdis=MhtDis(enm,me);
if(tdis>1){
order.type=FIRE;
order.row=enm.row;
order.col=tcol;
break;
}else{
if(S.tank[i].type==Pioneer){
getDis(Point(enm.row,enm.col),Stk_TID);
for(int j=0;j<4;j++){

}
}else{
order.type=FIRE;
order.row=trow;
order.col=tcol;
flag=true;
break;
}
}
}
return order;
}

Order SniperOrder(){
Order order;
order=RandOrder();
if(task[S.myID]==-1){
return order;
}else{
bool flag=false;
for(int i=0;i<MAX_TASK_NUM;i++){
if(S.tank[i].flag==S.myFlag) continue;
if(S.tank[i].row<0||S.tank[i].col<0) continue;
if(ABS((S.tank[i].row-me.row))+
ABS((S.tank[i].col-me.col))<=5){
order.type=FIRE;
order.row=S.tank[i].row;
order.col=S.tank[i].col;
flag=true;
break;
}
}
if(!flag){
int tdis=-1,td=-1;
for(int i=0;i<4;i++){
int trow=me.row+dir[i][0];
int tcol=me.col+dir[i][1];
if(dis[task[S.myID]][trow][tcol]==-1) continue;
if(tdis==-1||dis[task[S.myID]][trow][tcol]<tdis){
tdis=dis[task[S.myID]][trow][tcol];
td=i;
}
}
if(tdis>=4&&td!=-1){
int trow=me.row+dir[td][0];
int tcol=me.col+dir[td][1];
if(S.map[trow][tcol].type==BRICK||
S.map[trow][tcol].type==BREAKBRICK){
order.type=FIRE;
order.row=trow;
order.col=tcol;
}else order.type=OrderType(td+1);
}
}
return order;
}
return order;
}



Order PioneerOrder(){
Order order;
order=RandOrder();
if(task[S.myID]==-1){
return order;
}else{
int tdis=-1,td=-1;
for(int i=0;i<4;i++){
int trow=me.row+dir[i][0];
int tcol=me.col+dir[i][1];
if(dis[task[S.myID]][trow][tcol]==-1) continue;
if(tdis==-1||dis[task[S.myID]][trow][tcol]<tdis){
tdis=dis[task[S.myID]][trow][tcol];
td=i;
}
}
if(tdis==-1) return order;
else{
int trow=me.row+dir[td][0];
int tcol=me.col+dir[td][1];
if(S.map[trow][tcol].type==BRICK||
S.map[trow][tcol].type==BREAKBRICK){
order.type=FIRE;
order.row=trow;
order.col=tcol;
}else order.type=OrderType(td+1);
}
return order;
}
return order;
} 

*/



