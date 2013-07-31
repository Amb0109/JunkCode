// 别人的AI，测试用。From Internet.



///////////////////////////////////////////////////////////////////////
//作者:伟人 QQ:270173053 E-mail:abigwc@gmail.com 欢迎交流        //////
///////////////////////////////////////////////////////////////////////
#include "Tank.h"
#include <string.h>
//请勿修改以上头文件
/*
您可以在这里添加您所需头文件
*/
#define INF 32767
#define _clr(x,n) memset(x,-1,sizeof(int)*n)
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<malloc.h>
/*
您可以在这里添加您的自定义函数
*/
int finish=0; //Astar寻路是否完成，未完成为0，完成为1 
int minLength;//存放坦克到所有金矿之间距离的最小值 
int i,j;//循环的临时变量 
//Astar寻路结点结构体定义 
struct node 
{
	int	x, y;				//	坐标
	int g, h, f;			//	三个函数值
	struct node* parent;	// 父结点
	struct node* next;		// 下一个结点
}*opened,*closed;            //定义opened表和closed表 
int	x_extends[4], y_extends[4];	//上下左右四个方向扩展节点	
int	g, h, f;//g,h,f值 
struct node* p;            //临时节点 
struct node* q;            //临时节点
struct node* last;         //临时节点
int posCount[5];
int enemyPosCount[5];
 
int dis[5][13];              //每个坦克到每个金矿的距离矩阵 
 
int dis_i,dis_j;
char map[23][23];          //寻路的临时地图 
int power=1;               //地形的权 
short ableSource[MAX_SOURCE_NUM];    //可到达的金矿的序号数组 
int totalAble;                 //可到达的金矿的数量 
int	x_start = 0, y_start = 0, x_end = 0, y_end = 0;//寻路的起点和终点坐标 
int x_next=0,y_next=0;   //下一步的坐标
int x_target,y_target;
int x_enemynext,y_enemynext;//敌人的下一步的坐标 
int fireFlag[5];
int haveWayFlag=0;       //是否有路标记 
int length=0;            // 路径长度 
short AI_target[5]={-1,-1,-1,-1,-1};   //打击目标 
short enemy_target[5]={-1,-1,-1,-1,-1};  //敌人的打击目标 
int canHaveTarget[5];               //可用坦克 
int canBeTarget[13];                //可以打击的目标 
int tempLife[5];                    //临时的生命数组 
int powers[3];                      //各种地形的权,0:没东西，1：半损坏的砖头，2：好好的砖头 
struct posInfo
{
    int row,col;
}enemyStart[5],lastPos[5],lastEnemyPos[5];
struct tankInfomation               //敌人信息结构体 
{
    int row,col;                    //位置 
    int life;                       //生命
    TankType type;                  //类型 
    int range;                      //射程 
    int state;                      //状态   0表示普通,1表示被追赶,2表示开火 
    int cansee;                     //可见性,0表示不可见,1表示可见 
    int lost;                       //预测丢失  若丢失，则为1，否则为0 
    int kasi;                       //坦克是否卡死   若卡死,则为1，否则为0 
    int fire;                       //
    posInfo zhuan;                  //向砖块开火 
}mine[5],enemy[5],enemyNext[5];
int myround=0;            //记录自己的回合数 
int rush=0;
int totalKasi=0;
int totalEnemyKasi=0;
int match[5];
int orders[24][4];
int nn=0;
int ran=0;
int haveFlag; 
int runType=0;
//产生一个大于等于iMin，小于等于iMax的int类型的数 
int RandomInt(int iMin,   int iMax)     
{   
    static int  iInitSeed;   
    if(iInitSeed != 1 )   
    {   
	    srand((unsigned int)time(NULL));   
		iInitSeed  = 1;   
	}   
    return iMin  +  rand() % (iMax - iMin + 1);   
} 
//获取全排列序号 
void swap(int *a, int *b) 
{     
    int m;     
     m = *a;     
    *a = *b;     
    *b = m; 
}  
void perm(int list[], int k, int m) 
{     
    int i;     
    if(k > m)     
     {          
        for(i = 0; i <= m; i++) orders[nn][i]=list[i];             
         nn++;     
     }     
    else     
     {         
        for(i = k; i <= m; i++)         
         {             
             swap(&list[k], &list[i]);             
             perm(list, k + 1, m);             
             swap(&list[k], &list[i]);         
         }     
     } 
} 
//初始化opened表和closed表 
int InitTables(struct node* opened, struct node* closed)
{
	opened->parent = opened->next = NULL;
	opened->f = opened->g = opened->h = opened->x = opened->y = -1;
	closed->f = closed->g = closed->h = closed->x = closed->y = -1;
	closed->parent = closed->next = NULL;
	return 0;
}
//将坐标为x,y的结点插入到opened表中, 返回-1表失败
int InsertToOpened(int x, int y, struct node* parent, struct node* opened)
{
	p = (struct node*)malloc(sizeof(struct node));
	if (!p) return -1;
	p->x = x;
	p->y = y;
	p->parent = parent;
	p->g = parent == NULL ? 0 : parent->g + power;
	p->h = abs(x_end - x) + abs(y_end - y);		// 预测到终点距离
	p->f = p->g + p->h;
	
	q = opened->next;
	last=opened;	
	if(q)
    while(q)
	{
		if(p->f<=q->f) break;
		last=q;
		q=q->next;
	}
	last->next=p;
	p->next=q;
	return 0;
}
// 将结点插入到closed表中
void InsertToClosed(struct node* p, struct node* closed)
{
	p->next = closed->next;
	closed->next = p;
	if (p->x == x_end && p->y == y_end)
	{
		finish=1; 
	}
}
//在closed表中找结点,若有,返回指针,否则返回空
struct node* FindNodeInOpened(int x, int y, struct node* opened)
{
	p = opened->next;
	while (p)
	{
		if (p->x == x && p->y == y)
		{
			return p;
		}
		p = p->next;
	}
	return NULL;
}
//在closed表中找结点,若有,返回指针,否则返回空
struct node* FindNodeInClosed(int x, int y, struct node* closed)
{
	p = closed->next;
	while (p)
	{
		if (p->x == x && p->y == y)
		{   
			return p;
		}
		p = p->next;
	}
	return NULL;
}
//将opened表中的元素移动到closed表中
void MoveFromOpenedToClosed(struct node *p, struct node* opened, struct node* closed)
{
	q = opened;
	while (q->next != p)
	{
		q = q->next;
	}
	q->next = p->next;
	InsertToClosed(p, closed);
}
//返回opened表中F值最小的元素，即返回opened表中的第一个元素 
struct node* GetFSmallestInOpened(struct node* opened)
{
	return opened->next;
}
//扩展结点P
int	ExtendNode(struct node* p, struct node* opened, struct node* closed, char map[MAP_HEIGHT+2][MAP_WIDTH+2])
{
    int i=0;
    g=0;
    h=0;
    f=0;
	x_extends[orders[ran][0]] = p->x;
    x_extends[orders[ran][1]] = p->x;		// 周围可扩展的点，上下左右
	x_extends[orders[ran][2]] = p->x - 1;
	x_extends[orders[ran][3]] = p->x + 1;
	y_extends[orders[ran][0]] = p->y - 1;
	y_extends[orders[ran][1]] = p->y + 1;
	y_extends[orders[ran][2]] =p->y;
    y_extends[orders[ran][3]] = p->y;
	
	MoveFromOpenedToClosed(p, opened, closed);
	for (i = 0; i < 4; i++)
	{
		if ((map[y_extends[i]][x_extends[i]] == '3'||FindNodeInClosed(x_extends[i], y_extends[i], closed)!=NULL))
		{
			continue;
		}	
		if(map[y_extends[i]][x_extends[i]]=='1')power=powers[2];
		else if (map[y_extends[i]][x_extends[i]]=='2') power=powers[1];
            else if(map[y_extends[i]][x_extends[i]]=='0')power=powers[0]; 
		if ((q = FindNodeInOpened(x_extends[i], y_extends[i], opened)))
		{
            g=p->g+power;
			h = abs(x_end - x_extends[i]) + abs(y_end - y_extends[i]);
			f = g + h;		
			if (g < q->g)	
			{
				q->g = g;
				q->h = h;
				q->f = f;
				q->parent = p;
			}
			continue;
		}	// 结点存在于opened中		
		InsertToOpened(x_extends[i], y_extends[i], p, opened);
	}
	return 0;
}
//Astar寻路
int AStar(struct node* opened, struct node* closed, char map[MAP_HEIGHT+2][MAP_WIDTH +2])
{
	InsertToOpened(x_start, y_start, NULL, opened);
	while ((p = GetFSmallestInOpened(opened)) != NULL && finish != 1)
	{
		ExtendNode(p, opened, closed, map);
	}
	if (finish==1) return 1;// 没有找到路径,返回0	
	return 0;
}
//获取下一步的指令 
OrderType GetNextStep(DataForAI data)
{
    int i,j,haveWayFlag=0;
    struct node* p,*q;
    InitTables(opened,closed);
    for(i=0;i<23;i++)
        for(j=0;j<23;j++)
        {
            map[i][j]=data.map[i][j].type+48;
        }
    for(i=0;i<5;i++)
    {
        if(i+data.myFlag*5!=data.myID)
        {
            if(data.tank[data.myID].type!=Pioneer)
            {
                if(abs(mine[i].row-data.tank[data.myID].row)+abs(mine[i].col-data.tank[data.myID].col)==1||mine[i].kasi==1)
                {
                    map[mine[i].row][mine[i].col]+=3; 
                }
            }
            else
            if(data.tank[data.myID].type==Pioneer&&mine[i].type!=Pioneer) map[mine[i].row][mine[i].col]+=3;           
        }
    }
    haveWayFlag=AStar(opened,closed,map);
	p = closed->next;
    while (p->parent->parent)
	{
		q = p;
		p = p->parent;
	}
	x_next=p->x;y_next=p->y;
	p = closed->next;
	while (p)
	{
		q=p;
		p=p->next;
		free(q);
	}
	p=opened->next;
	while (p)
	{
		q=p;
		p=p->next;
		free(q);
	}
	finish=0;
    if(haveWayFlag==0) return STOP;   
	if(x_next>x_start) return GORIGHT;
	if(y_next>y_start) return GODOWN;
	if(x_next<x_start) return GOLEFT;
	if(y_next<y_start) return GOUP;
	return STOP;
}
//获取敌人下一步的指令 
void GetEnemyNextStep(DataForAI data)
{
    int i,j,haveWayFlag=0;
    struct node* p,*q;
    InitTables(opened,closed);
    for(i=0;i<23;i++)
        for(j=0;j<23;j++)
        {
            map[i][j]=data.map[i][j].type+48;
        }
    haveWayFlag=AStar(opened,closed,map);
   	p = closed->next;
    while (p->parent->parent)
	{
		q = p;
		p = p->parent;
	}
	x_next=p->x;y_next=p->y;
	p = closed->next;
	x_enemynext=p->x;
    y_enemynext=p->y;
	while (p)
	{
		q=p;
		p=p->next;
		free(q);
	}
	p=opened->next;
	while (p)
	{
		q=p;
		p=p->next;
		free(q);
	}
	finish=0;
}
//获取两点之间的耗费最少的路径的耗费 
short GetLengthToEnd(DataForAI data)
{
    int i,j,haveWayFlag=0,length=0;
    InitTables(opened,closed);
    for(i=0;i<23;i++)
        for(j=0;j<23;j++)
        {
            map[i][j]=data.map[i][j].type+48;
        }
 for(i=0;i<5;i++)
    {
        if(i+data.myFlag*5!=data.myID)
        {
            if(data.tank[data.myID].type!=Pioneer)
            {
                if(abs(mine[i].row-data.tank[data.myID].row)+abs(mine[i].col-data.tank[data.myID].col)==1||mine[i].kasi==1)
                {
                    map[mine[i].row][mine[i].col]+=3; 
                }
            }
            else
            if(data.tank[data.myID].type==Pioneer&&mine[i].type!=Pioneer) map[mine[i].row][mine[i].col]+=3;           
        }
    }
    haveWayFlag=AStar(opened,closed,map);
    struct node* p,*q;
       	
    q = closed;
	p = q->next;
	length=p->f;
	p = closed->next;
	while (p)
	{
		q=p;
		p=p->next;
		free(q);
	}
	p=opened->next;
	while (p)
	{
		q=p;
		p=p->next;
		free(q);
	}
	finish=0; 
	if(haveWayFlag==0) length=32767;
    return length; 
}
//获取敌方两点之间的耗费最少的路径的耗费 
short GetEnemyLengthToEnd(DataForAI data)
{
    int i,j,haveWayFlag=0,length=0;
    InitTables(opened,closed);
    for(i=0;i<23;i++)
        for(j=0;j<23;j++)
        {
            map[i][j]=data.map[i][j].type+48;
        }
    haveWayFlag=AStar(opened,closed,map);
    struct node* p,*q;
       	
    q = closed;
	p = q->next;
	length=p->f;
	p = closed->next;
	while (p)
	{
		q=p;
		p=p->next;
		free(q);
	}
	p=opened->next;
	while (p)
	{
		q=p;
		p=p->next;
		free(q);
	}
	finish=0; 
	if(haveWayFlag==0) length=32767;
    return length; 
}
//判断参数金矿的序号No是否在打击的目标数组Target中,若在，返回1，否则返回0 
short isInTarget(short No)
{
    short i;
    for(i=0;i<5;i++)
        if(AI_target[i]==No) return 1;
    return 0;
}
//km二分最优匹配 
void kuhn_munkras(int m,int n,int mat[][MAX_SOURCE_NUM])
{
       int s[MAX_SOURCE_NUM],t[MAX_SOURCE_NUM],l1[MAX_SOURCE_NUM],l2[MAX_SOURCE_NUM],p,q,i,j,k;
       int match2[MAX_SOURCE_NUM];
       for (i=0;i<m;i++)
             for(l1[i]=-INF,j=0;j<n;j++)
                   l1[i]=mat[i][j]>l1[i]?mat[i][j]:l1[i];
      for(i=0;i<n;l2[i++]=0);
      for(_clr(match,5),_clr(match2,MAX_SOURCE_NUM),i=0;i<m;i++)
      {
           for(_clr(t,MAX_SOURCE_NUM),s[p=q=0]=i;p<=q&&match[i]<0;p++)
                for (k=s[p],j=0;j<n&&match[i]<0;j++)
                      if(l1[k]+l2[j]==mat[k][j]&&t[j]<0)
                      {
                            s[++q]=match2[j],t[j]=k;
                            if (s[q]<0)
                                  for(p=j;p>=0;j=p)
                                       match2[j]=k=t[j],p=match[k],match[k]=j;
                      }
          if(match[i]<0)
          {
              for(i--,p=INF,k=0;k<=q;k++)
                      for (j=0;j<n;j++)
                            if (t[j]<0&&l1[s[k]]+l2[j]-mat[s[k]][j]<p)
                                   p=l1[s[k]]+l2[j]-mat[s[k]][j];
              for(j=0;j<n;l2[j]+=t[j]<0?0:p, j++);
              for(k=0;k<=q;l1[s[k++]]-=p);
          }
      }
}
//获取新的打击目标 
void GetNewTargets(DataForAI data)
{
    int i,j;
    int goFor=0;
    
    //计算需要去占领的金矿的个数，存放于goFor中
    for(i=0;i<data.totalSource;i++)
        if(data.map[data.source[i].row][data.source[i].col].isHereSource!=data.myFlag+1) goFor++;
  
    //有5个及以上需要占领的金矿 
    if(goFor>=5)
    {
        dis_i=0;
        for(i=0;i<5;i++)
        { 
            //如果坦克的生命值大于0
            if(data.tank[i+data.myFlag*5].life!=0)
            {           
                canHaveTarget[dis_i]=i;
                dis_j=0;
                for(j=0;j<data.totalSource;j++)
                {   
                    if(data.map[data.source[j].row][data.source[j].col].isHereSource!=data.myFlag+1)   
                    {                     
                         canBeTarget[dis_j]=j;                 
                         x_start=data.tank[i+data.myFlag*5].col;
                         y_start=data.tank[i+data.myFlag*5].row;  
                         x_end=data.source[j].col;
                         y_end=data.source[j].row; 
                         dis[dis_i][dis_j]=-GetLengthToEnd(data);
                         dis_j++; 
                    }
                }
                dis_i++;              
            }
            else canHaveTarget[i]=0;
        }
        kuhn_munkras(dis_i,dis_j,dis);  
        for (i=0;i<dis_i;i++) AI_target[canHaveTarget[i]]=canBeTarget[match[i]];
        
    }
    else//需要占领的金矿小于5 
    if(data.map[data.source[AI_target[data.myID-data.myFlag*5]].row][data.source[AI_target[data.myID-data.myFlag*5]].col].isHereSource==data.myFlag+1)
    {
        minLength=INF;
        int t; 
        //先对所有的需要占领的金矿进行遍历。如果有金矿没有被作为某个坦克的目标，则将该金矿作为这个坦克的目标 
        for(j=0;j<data.totalSource;j++)
        {
            x_start=data.tank[data.myID].col;
            y_start=data.tank[data.myID].row;  
            x_end=data.source[j].col;
            y_end=data.source[j].row;   
            if(!isInTarget(j)&&data.map[data.source[j].row][data.source[j].col].isHereSource!=data.myFlag+1) 
            {      
                t=GetLengthToEnd(data);
                if(t<minLength)                                                                                                                               
                {
                    AI_target[data.myID-5*data.myFlag]=j;
                    minLength=t;
                }
            }
        } 
        
        //不存在没有人作为目标的未占领的金矿，则将离自己最近的那个已经有队友在打的金矿作为自己的目标   
        if(minLength==INF) 
        {
            for(j=0;j<data.totalSource;j++)
            {
                x_start=data.tank[data.myID].col;
                y_start=data.tank[data.myID].row;  
                x_end=data.source[j].col;
                y_end=data.source[j].row;   
                if(isInTarget(j)&&data.map[data.source[j].row][data.source[j].col].isHereSource!=data.myFlag+1) 
                {      
                    t=GetLengthToEnd(data);
                    if(t<minLength)                                                                                                                               
                    {
                        AI_target[data.myID-5*data.myFlag]=j;
                        minLength=t;
                    }
                }
            }
            
            //所有的金矿都被自己占领了，则随机找一个金矿去转转 
            if(minLength==INF) 
            {   
                totalAble=-1;
                for(j=0;j<data.totalSource;j++)   
                {
                    x_start=data.tank[data.myID].col;
                    y_start=data.tank[data.myID].row;  
                    x_end=data.source[j].col;
                    y_end=data.source[j].row;       
                    if((t=GetLengthToEnd(data))!=minLength)                                                                                                                               
                    {   
                        totalAble++;
                        ableSource[totalAble]=j;                  
                    }
                }             
                AI_target[data.myID-data.myFlag*5]=ableSource[RandomInt(0,totalAble)];
            }
        }      
    }
  
}
//根据坦克的种类选择不同地形的权重
void ChoosePower(TankType tanktype)
{
    if(tanktype==Sniper||tanktype==Pioneer) 
        for(i=0;i<3;i++) powers[i]=i+1;
    else if(tanktype==Striker)
         {    
             powers[0]=1;
             powers[1]=2;
             powers[2]=2;
         }
}
        int isdanger[5];
//平台0回合时调用此函数获取AI名称及坦克类型信息，请勿修改此函数声明。
extern "C" InitiateInfo chooseType()
{     
	InitiateInfo Info;
	int i,j;
    int list[] = {0, 1, 2, 3};
    perm(list, 0, 3); 
    opened =(struct node*)malloc(sizeof(struct node));
    closed =(struct node*)malloc(sizeof(struct node));
    for(i=0;i<5;i++)
    AI_target[i]=-1;
    power=1;
    x_start = 0; y_start = 0; x_end = 0; y_end = 0;
    for(i=0;i<23;i++)
        for(j=0;j<23;j++)
            map[i][j]='0';
    InitTables(opened, closed);
	Info.tank[0]=Pioneer;
	Info.tank[1]=Pioneer;
	Info.tank[2]=Sniper;
	Info.tank[3]=Sniper;
	Info.tank[4]=Sniper;	
	strcpy(Info.aiName,"abigwc"); //AI名请勿使用中文。 
	return Info;
}
//平台从第1回合开始调用此函数获得每回合指令，请勿修改此函数声明。
extern "C" Order makeOrder(DataForAI data)
{
    Order order;
    order.type=STOP;
    short left,right,top,bottom;
    short t=0,minLength,minLife;
    int i,j;
    if(data.round==1&&data.myID==data.myFlag*5)
    { 
        int Pcount=0;
        for(i=0;i<5;i++)
        {
            enemy[i].type=data.tank[i+(1-data.myFlag)*5].type;
            if(enemy[i].type==Pioneer) Pcount++;
            enemy[i].range=data.tank[i+(1-data.myFlag)*5].range;
            enemy[i].lost=0;
            if(enemy[i].type==Striker) enemy[i].fire=2;
            else enemy[i].fire=1;
            mine[i].type=data.tank[i+data.myFlag*5].type;
            mine[i].range=data.tank[i+data.myFlag*5].range;
            if(mine[i].type==Striker) mine[i].fire=2;
            else mine[i].fire=1;
            mine[i].zhuan.row=-1;
            mine[i].zhuan.col=-1;
        }
        if(Pcount>=4) runType=1;
        else runType=0;
        for(i=0;i<5;i++)
        {
            enemyNext[i].row=22-data.tank[i+data.myFlag*5].row;
            enemyNext[i].col=22-data.tank[i+data.myFlag*5].col;
            enemyStart[i].row=enemyNext[i].row;
            enemyStart[i].col=enemyNext[i].col;
        }
        
        //在同一位置的计数 
        for(i=0;i<5;i++) posCount[i]=0;
        myround=0;
    }
    
    //打转阶段 
    if(data.round<=15)
    {
        mine[data.myID-data.myFlag*5].zhuan.col=-1;
        mine[data.myID-data.myFlag*5].zhuan.row=-1;
    }
    if(data.tank[data.myID].revive!=-1) 
    {
        return order;
    } 
    
    
    if(myround<data.round)
    {
        myround++;   
        
        ran=RandomInt(0,23);  
        
        //得到我方坦克位置,生命
        totalKasi=0;
        for(i=0;i<5;i++)
        {
            mine[i].row=data.tank[i+data.myFlag*5].row;
            mine[i].col=data.tank[i+data.myFlag*5].col;
            mine[i].life=data.tank[i+data.myFlag*5].life;
            if(mine[i].row==lastPos[i].row&&mine[i].col==lastPos[i].col) posCount[i]++; else posCount[i]=0;
            if(posCount[i]>=5) mine[i].kasi=1; else mine[i].kasi=0;
            lastPos[i].row=mine[i].row;
            lastPos[i].col=mine[i].col;
            totalKasi+=mine[i].kasi;
        } 
        
             
        totalEnemyKasi=0;     
        //根据视野修正预测 
        for(i=0;i<5;i++)
        {
            //获取敌人的生命 
            enemy[i].life=data.tank[i+(1-data.myFlag)*5].life;
            if(enemy[i].life==0) 
            {
                enemy[i].col=-2;
                enemy[i].row=-2;
                enemyNext[i].col=-2;
                enemyNext[i].row=-2;
                enemy[i].cansee=0;
                continue;
            }
            
            
            //刚刚复活的敌人 
            if(enemy[i].life!=0&&enemy[i].col==-2)
            {
                enemy[i].row=enemyStart[i].row;
                enemy[i].col=enemyStart[i].col;
                enemy[i].cansee=0;    
                enemy[i].lost=0;
                continue;
            } 
            
            //可见的敌人 
            if(data.tank[i+(1-data.myFlag)*5].col!=-2)
            {
                enemy[i].row=data.tank[i+(1-data.myFlag)*5].row;
                enemy[i].col=data.tank[i+(1-data.myFlag)*5].col;
                enemy[i].cansee=1;
                enemy[i].lost=0;
            }
            //若敌人不可见，则将上一回合预测的敌人的位置作为现在敌人的位置 
            else
            {
                enemy[i].row=enemyNext[i].row;
                enemy[i].col=enemyNext[i].col;
                enemy[i].cansee=0;
            }  
             
            if(enemy[i].row==lastEnemyPos[i].row&&enemy[i].col==lastEnemyPos[i].col) enemyPosCount[i]++; else enemyPosCount[i]=0;
            if(enemyPosCount[i]>=5) enemy[i].kasi=1; else enemy[i].kasi=0;
            lastEnemyPos[i].row=enemy[i].row;
            lastEnemyPos[i].col=enemy[i].col;
            totalEnemyKasi+=enemy[i].kasi;
        }
           
        for(i=0;i<5;i++) isdanger[i]=0;
        //根据敌人的当前位置预测敌人的下一回合的位置
       
        for(i=0;i<5;i++)
        {
            //如果坦克已经死亡，则取消对该敌方坦克的预测 
            if(enemy[i].life==0) continue;
            
            int canBeSee=0;
            //检查是否预测丢失 
            for(j=0;j<5;j++)
            {
                if(mine[j].life==0) continue;
                if(abs(enemy[i].row-mine[j].row)+abs(enemy[i].col-mine[j].col)<=4)
                {
                    canBeSee=1;
                    break;
                }
            }
            if(!canBeSee)
            for(j=0;j<data.totalSource;j++)
            {                                   
                if(abs(enemy[i].row-data.source[j].row)+abs(enemy[i].col-data.source[j].col)<=2)
                {
                    canBeSee=1;
                    break;
                }
            }
            if(canBeSee!=enemy[i].cansee||enemy[i].lost==1)
            {
                enemy[i].lost=1;
                continue;
            }  
                
            minLength=INF;
                       
            int te=0;
            for(j=0;j<5;j++)
            {
                if(abs(mine[j].row-enemy[i].row)+abs(mine[j].col-enemy[i].col)<=enemy[i].range)
                {
                    te=1;
                    break;
                }
            }
            if(te==1&&enemy[i].type!=Pioneer) 
            {
               continue;
            }
            
            if(enemy[i].col==-2) continue;
            //对当前的敌方坦克选择地形权重 
            ChoosePower(enemy[i].type);
           
           int sml=2;
           int zishaFlag=0; 
           //如果是Pioneer，而且有我方坦克与之距离<=2，则假设其采用自杀战术,预测其位置 
           if(enemy[i].type==Pioneer)
           {
               for(j=0;j<5;j++)
               {
                   if(mine[j].life>0&&abs(enemy[i].col-mine[j].col)+abs(enemy[i].row-mine[j].row)<=sml)
                   {
                       sml=abs(enemy[i].col-mine[j].col)+abs(enemy[i].row-mine[j].row);
                       zishaFlag=1;
                       x_target=mine[j].col;
                       y_target=mine[j].row;
                       isdanger[i]=1; 
                   }
               }
           }
           
           //若该敌人无自杀的目标,则正常预测 
           if(zishaFlag==0)
           {
                //敌方坦克的起始位置 
                x_start=enemy[i].col;
                y_start=enemy[i].row; 
            
                //对金矿进行遍历,选择与该敌方坦克距离最小的地方目标金矿 
                for(j=0;j<data.totalSource;j++)
                { 
                    if(data.map[data.source[j].row][data.source[j].col].isHereSource!=2-data.myFlag) 
                    {     
                        x_end=data.source[j].col;
                        y_end=data.source[j].row; 
                        if(x_start!=x_end||y_start!=y_end)
                        {
                            t=GetEnemyLengthToEnd(data);
                            if(t<minLength)                                                                                                                               
                            {
                                x_target=x_end;
                                y_target=y_end;                                                                                                              
                                minLength=t;                    
                            }
                        }
                    }
                }
            } 
            
            //敌方坦克的起始位置 
            x_start=enemy[i].col;
            y_start=enemy[i].row; 
           
            //如果存在距离最短的敌方坦克可占领金矿,则将其作为敌方的抢占目标 
            if(minLength!=INF||zishaFlag==1)
            {
            
                x_end=x_target;
                y_end=y_target;
                //得到该敌方坦克的下一步位置 
                GetEnemyNextStep(data);
                //如果得到的下一步位置不是砖块,则将预测的敌方位置定为得到的敌方下一步位置 
                if(data.map[y_next][x_next].type!=1&&data.map[y_next][x_next].type!=2)
                {
                    enemyNext[i].row=y_next;
                    enemyNext[i].col=x_next; 
                }
            }       
        }     
        
        for(i=0;i<5;i++) fireFlag[i]=0;
        
        //计算每个坦克追逐情况 
        for(i=0;i<5;i++)
        {
            if(mine[i].life!=0)
            for(j=0;j<5;j++)
            {
                if(enemy[j].life==0) 
                {
                    isdanger[j]=0;
                }
                else
                {
                    if(abs(mine[i].col-enemy[j].col)+abs(mine[i].row-enemy[j].row)==4&&abs(mine[i].col-enemyNext[j].col)+abs(mine[i].row-enemyNext[j].row)==3)
                    {
                        isdanger[j]=1;
                        fireFlag[i]=1;
                        tempLife[j]--;
                        if(tempLife[j]==0) isdanger[j]=0;
                        break;
                    }
                }
            }
        }
        for(i=0;i<5;i++)
        {
            if(fireFlag[i]==0)
            {
                for(j=0;j<5;j++)
                {
                     if(abs(mine[i].col-enemy[j].col)+abs(mine[i].row-enemy[j].row)<=5&&tempLife[j]!=0&&isdanger[j]==1)
                     {
                         fireFlag[i]=1;
                         tempLife[j]--;
                         if(tempLife[j]==0) isdanger[j]=0;
                         break;
                     }
                }
            }
        }
        for(i=0;i<5;i++) tempLife[i]=enemy[i].life;  
        
        //是否强冲 
        if(totalEnemyKasi<totalKasi)rush=1;else rush=0;
    }
    
    int des_x=0;
    int des_y=0;
    haveFlag=0;
    int temp;
    int duanest=5;
    int enemyOrd=-1;
    x_start=data.tank[data.myID].col;
    y_start=data.tank[data.myID].row; 
    
    //Pioneer的坦克执行自杀策略 
    if(data.tank[data.myID].type==Pioneer&&fireFlag[data.myID-data.myFlag*5]!=1)
    {
                                                                                                                
        //看看是否有可占金矿距离现决策坦克的距离小于等于2的 
        for(i=0;i<data.totalSource;i++)
        {                               
            if(abs(data.tank[data.myID].col-data.source[i].col)+abs(data.tank[data.myID].row-data.source[i].row)<=2&&data.map[data.source[i].row][data.source[i].col].isHereSource!=data.myFlag+1)
            {
                x_start=data.tank[data.myID].col;
                y_start=data.tank[data.myID].row; 
                x_end=data.source[i].col;
                y_end=data.source[i].row;
                if(GetLengthToEnd(data)<=2)
                {
                    haveFlag=1;
                    break;
                }
            }        
        }    
        
                                      
        if(haveFlag==0)                                  
        {                                       
            for(i=0;i<5;i++)
            {
                if(enemy[i].life>0&&enemy[i].row!=-2&&enemy[i].type!=Pioneer&&enemy[i].lost==0&&
                abs(data.tank[data.myID].row-enemy[i].row)+abs(data.tank[data.myID].col-enemy[i].col)<=4)
                {
                    x_end=enemy[i].col;
                    y_end=enemy[i].row;
                    temp=GetLengthToEnd(data);                                                                      
                    if(temp<=duanest)
                    {
                        des_x=x_end;
                        des_y=y_end;
                        haveFlag=1;
                        duanest=temp;
                        enemyOrd=i;
                    }
                }
            }
            
            if(haveFlag==1)
            {
                x_start=data.tank[data.myID].col;
                y_start=data.tank[data.myID].row; 
                x_end=des_x;
                y_end=des_y;
                if(x_start!=x_end||y_start!=y_end)
                { 
                    order.type=GetNextStep(data);
                    
                    if(order.type!=STOP) 
                    {
                        //Pioneer是撞还是开火的博弈
                                             
                        int enemyfire=0;
                        //计算敌人火力大小  
                        for(i=0;i<5;i++)
                        {
                            if(enemy[i].life!=0&&enemy[i].col!=-2)
                            {
                                if(abs(enemy[i].col-mine[data.myID-data.myFlag*5].col)+abs(enemy[i].row-mine[data.myID-data.myFlag*5].row)<=enemy[i].range)
                                enemyfire+=enemy[i].fire;
                            }
                        }
                                                    
                        if(abs(x_start-x_end)+abs(y_start-y_end)==1)                                    
                        {
                            if(enemyOrd!=-1&&enemy[enemyOrd].life!=1&&data.tank[data.myID].life>enemyfire)
                            {
                                mine[data.myID-data.myFlag*5].row=y_next;
                                mine[data.myID-data.myFlag*5].col=x_next;  
                                return order;
                            }
                        }
                        else 
                        {
                            mine[data.myID-data.myFlag*5].row=y_next;
                            mine[data.myID-data.myFlag*5].col=x_next;
                            return order;
                        }
                    }
                }
            }
        }
    }
    
    int rundirect[5][4];                 //对于所有敌方坦克的逃跑方向 ,数组第二维的下标0,1,2,3分别表示上下左右 
    int runFlag=0;
    int crossFlag[4];
    
    for(i=0;i<4;i++) crossFlag[i]=0;
   
    //如果敌人靠近而且人家还有较多生命,就逃跑 
    if(data.tank[data.myID].type!=Pioneer)
    for(i=0;i<5;i++)
    {
        if(enemy[i].col==-2||enemy[i].life==1||isdanger[i]==0||enemy[i].lost==1) 
        {
            for(j=0;j<4;j++) rundirect[i][j]=1;
            continue;
        }
               
        if(((enemy[i].type==Striker||runType==1)&&abs(enemy[i].col-data.tank[data.myID].col)+abs(enemy[i].row-data.tank[data.myID].row)==enemy[i].range+1 
        &&abs(enemyNext[i].col-data.tank[data.myID].col)+abs(enemyNext[i].row-data.tank[data.myID].row)==enemy[i].range)
        ||(enemy[i].type==Pioneer&&abs(enemy[i].row-data.tank[data.myID].row)+abs(enemy[i].col-data.tank[data.myID].col)<=1))
        {      
            runFlag=1;   
            for(j=0;j<4;j++) rundirect[i][j]=0; 
            //敌人向下走 
            if(enemyNext[i].row>enemy[i].row)
            {
                
                //我在敌人左边 
                if(enemy[i].col>data.tank[data.myID].col)
                {
                    if(data.map[data.tank[data.myID].row][data.tank[data.myID].col-1].type==0&&data.map[data.tank[data.myID].row][data.tank[data.myID].col-1].whoIsHere==-1)
                        rundirect[i][2]=1;
                    if(data.map[data.tank[data.myID].row+1][data.tank[data.myID].col].type==0&&data.map[data.tank[data.myID].row+1][data.tank[data.myID].col].whoIsHere==-1)
                        rundirect[i][1]=1;;
                    if(data.map[data.tank[data.myID].row-1][data.tank[data.myID].col].type==0&&data.map[data.tank[data.myID].row-1][data.tank[data.myID].col].whoIsHere==-1&&data.tank[data.myID].row-1==enemy[i].row)
                    {
                        rundirect[i][0]=1;  
                        crossFlag[0]=1;
                    }               
                }
                else
                //我在敌人右边 
                if(enemy[i].col<data.tank[data.myID].col)
                {
                    if(data.map[data.tank[data.myID].row][data.tank[data.myID].col+1].type==0&&data.map[data.tank[data.myID].row][data.tank[data.myID].col+1].whoIsHere==-1)
                        rundirect[i][3]=1;
                    if(data.map[data.tank[data.myID].row+1][data.tank[data.myID].col].type==0&&data.map[data.tank[data.myID].row+1][data.tank[data.myID].col].whoIsHere==-1)
                        rundirect[i][1]=1;   
                    if(data.map[data.tank[data.myID].row-1][data.tank[data.myID].col].type==0&&data.map[data.tank[data.myID].row-1][data.tank[data.myID].col].whoIsHere==-1&&data.tank[data.myID].row-1==enemy[i].row)
                    {
                        rundirect[i][0]=1;  
                        crossFlag[0]=1;
                    }         
                }
                else
                //我和敌人在一条线上 
                if(enemy[i].col==data.tank[data.myID].col)
                {
                    if(data.map[data.tank[data.myID].row+1][data.tank[data.myID].col].type==0&&data.map[data.tank[data.myID].row+1][data.tank[data.myID].col].whoIsHere==-1)
                        rundirect[i][1]=1;
                    if(data.map[data.tank[data.myID].row][data.tank[data.myID].col-1].type==0&&data.map[data.tank[data.myID].row][data.tank[data.myID].col-1].whoIsHere==-1)
                        rundirect[i][2]=1; 
                    if(data.map[data.tank[data.myID].row][data.tank[data.myID].col+1].type==0&&data.map[data.tank[data.myID].row][data.tank[data.myID].col+1].whoIsHere==-1)
                        rundirect[i][3]=1;                
                }                
            }
            else
            //敌人向上走 
            if(enemyNext[i].row<enemy[i].row)
            {
                //我在敌人左边 
                if(enemy[i].col>data.tank[data.myID].col)
                {
                    if(data.map[data.tank[data.myID].row][data.tank[data.myID].col-1].type==0&&data.map[data.tank[data.myID].row][data.tank[data.myID].col-1].whoIsHere==-1)
                        rundirect[i][2]=1;
                    if(data.map[data.tank[data.myID].row-1][data.tank[data.myID].col].type==0&&data.map[data.tank[data.myID].row-1][data.tank[data.myID].col].whoIsHere==-1)
                        rundirect[i][0]=1;     
                    if(data.map[data.tank[data.myID].row+1][data.tank[data.myID].col].type==0&&data.tank[data.myID].row+1==enemy[i].row&&data.map[data.tank[data.myID].row+1][data.tank[data.myID].col].whoIsHere==-1)
                    {
                        rundirect[i][1]=1;  
                        crossFlag[1]=1;
                    }           
                }
                else
                //我在敌人右边 
                if(enemy[i].col<data.tank[data.myID].col)
                {
                    if(data.map[data.tank[data.myID].row][data.tank[data.myID].col+1].type==0&&data.map[data.tank[data.myID].row][data.tank[data.myID].col+1].whoIsHere==-1)
                        rundirect[i][3]=1;
                    if(data.map[data.tank[data.myID].row-1][data.tank[data.myID].col].type==0&&data.map[data.tank[data.myID].row-1][data.tank[data.myID].col].whoIsHere==-1)
                        rundirect[i][0]=1;
                    if(data.map[data.tank[data.myID].row+1][data.tank[data.myID].col].type==0&&data.tank[data.myID].row+1==enemy[i].row&&data.map[data.tank[data.myID].row+1][data.tank[data.myID].col].whoIsHere==-1)
                    {
                        rundirect[i][1]=1;  
                        crossFlag[1]=1;
                    }            
                }
                else
                //我和敌人在一条线上 
                if(enemy[i].col==data.tank[data.myID].col)
                {
                    if(data.map[data.tank[data.myID].row-1][data.tank[data.myID].col].type==0&&data.map[data.tank[data.myID].row-1][data.tank[data.myID].col].whoIsHere==-1)
                        rundirect[i][0]=1;
                    if(data.map[data.tank[data.myID].row][data.tank[data.myID].col-1].type==0&&data.map[data.tank[data.myID].row][data.tank[data.myID].col-1].whoIsHere==-1)
                        rundirect[i][2]=1; 
                    if(data.map[data.tank[data.myID].row][data.tank[data.myID].col+1].type==0&&data.map[data.tank[data.myID].row][data.tank[data.myID].col+1].whoIsHere==-1)
                        rundirect[i][3]=1;         
                }
            }
                                  
            //敌人向右走 
            if(enemyNext[i].col>enemy[i].col)
            {
                //我在敌人上边 
                if(enemy[i].row>data.tank[data.myID].row)
                {
                    if(data.map[data.tank[data.myID].row-1][data.tank[data.myID].col].type==0&&data.map[data.tank[data.myID].row-1][data.tank[data.myID].col].whoIsHere==-1)
                        rundirect[i][0]=1;
                    if(data.map[data.tank[data.myID].row][data.tank[data.myID].col+1].type==0&&data.map[data.tank[data.myID].row][data.tank[data.myID].col+1].whoIsHere==-1)
                        rundirect[i][3]=1;  
                    if(data.map[data.tank[data.myID].row][data.tank[data.myID].col-1].type==0&&data.tank[data.myID].col-1==enemy[i].col&&data.map[data.tank[data.myID].row][data.tank[data.myID].col-1].whoIsHere==-1)
                    {
                        rundirect[i][2]=1;  
                        crossFlag[2]=1;
                    }         
                }
                else
                //我在敌人下边 
                if(enemy[i].row<data.tank[data.myID].row)
                {
                    if(data.map[data.tank[data.myID].row+1][data.tank[data.myID].col].type==0&&data.map[data.tank[data.myID].row+1][data.tank[data.myID].col].whoIsHere==-1)
                        rundirect[i][1]=1;
                    if(data.map[data.tank[data.myID].row][data.tank[data.myID].col+1].type==0&&data.map[data.tank[data.myID].row][data.tank[data.myID].col+1].whoIsHere==-1)
                        rundirect[i][3]=1;    
                    if(data.map[data.tank[data.myID].row][data.tank[data.myID].col-1].type==0&&data.tank[data.myID].col-1==enemy[i].col&&data.map[data.tank[data.myID].row][data.tank[data.myID].col+1].whoIsHere==-1)
                    {
                        rundirect[i][2]=1;  
                        crossFlag[2]=1;
                    }         
                }    
                else 
                //我和敌人在一条线上 
                if(enemy[i].row==data.tank[data.myID].row)
                {
                    if(data.map[data.tank[data.myID].row][data.tank[data.myID].col+1].type==0&&data.map[data.tank[data.myID].row][data.tank[data.myID].col+1].whoIsHere==-1)
                        rundirect[i][3]=1;
                    if(data.map[data.tank[data.myID].row-1][data.tank[data.myID].col].type==0&&data.map[data.tank[data.myID].row-1][data.tank[data.myID].col].whoIsHere==-1)
                        rundirect[i][0]=1;
                    if(data.map[data.tank[data.myID].row+1][data.tank[data.myID].col].type==0&&data.map[data.tank[data.myID].row+1][data.tank[data.myID].col].whoIsHere==-1)
                        rundirect[i][1]=1;                   
                }   
            }
            else                     
            //敌人向左走 
            if(enemyNext[i].col<enemy[i].col)
            {
                //我在敌人上边 
                if(enemy[i].row>data.tank[data.myID].row)
                {
                    if(data.map[data.tank[data.myID].row-1][data.tank[data.myID].col].type==0&&data.map[data.tank[data.myID].row-1][data.tank[data.myID].col].whoIsHere==-1)
                        rundirect[i][0]=1;
                    if(data.map[data.tank[data.myID].row][data.tank[data.myID].col-1].type==0&&data.map[data.tank[data.myID].row][data.tank[data.myID].col-1].whoIsHere==-1)
                        rundirect[i][2]=1; 
                    if(data.map[data.tank[data.myID].row][data.tank[data.myID].col+1].type==0&&data.map[data.tank[data.myID].row][data.tank[data.myID].col+1].whoIsHere==-1&&data.tank[data.myID].col+1==enemy[i].col)
                    {
                        rundirect[i][3]=1;  
                        crossFlag[3]=1;
                    }        
                }
                else
                //我在敌人下边 
                if(enemy[i].row<data.tank[data.myID].row)
                {
                    if(data.map[data.tank[data.myID].row+1][data.tank[data.myID].col].type==0&&data.map[data.tank[data.myID].row+1][data.tank[data.myID].col].whoIsHere==-1)
                        rundirect[i][1]=1;
                    if(data.map[data.tank[data.myID].row][data.tank[data.myID].col-1].type==0&&data.map[data.tank[data.myID].row][data.tank[data.myID].col-1].whoIsHere==-1)
                        rundirect[i][2]=1;
                    if(data.map[data.tank[data.myID].row][data.tank[data.myID].col+1].type==0&&data.tank[data.myID].col+1==enemy[i].col&&data.map[data.tank[data.myID].row][data.tank[data.myID].col+1].whoIsHere==-1)
                    {
                        rundirect[i][3]=1;  
                        crossFlag[3]=1;
                    }          
                }    
                else 
                //我和敌人在一条线上 
                if(enemy[i].row==data.tank[data.myID].row)
                {
                    if(data.map[data.tank[data.myID].row][data.tank[data.myID].col-1].type==0&&data.map[data.tank[data.myID].row][data.tank[data.myID].col-1].whoIsHere==-1)
                        rundirect[i][2]=1; 
                    if(data.map[data.tank[data.myID].row-1][data.tank[data.myID].col].type==0&&data.map[data.tank[data.myID].row-1][data.tank[data.myID].col].whoIsHere==-1)
                        rundirect[i][0]=1; 
                    if(data.map[data.tank[data.myID].row+1][data.tank[data.myID].col].type==0&&data.map[data.tank[data.myID].row+1][data.tank[data.myID].col].whoIsHere==-1)
                        rundirect[i][1]=1; 
                  
                }   
            }
        }
        else for(j=0;j<4;j++) rundirect[i][j]=1; 
    }
    
    if(runFlag==1) 
    {   
        int runWayCount=0; 
        int runWays[4];
        int haveP=0; 
        int haveK=0;
        int cannotgo[4];
        int tempi;
        
        
        //判断是有Pioneer在追我 
        for(i=0;i<5;i++)
        {
            if(abs(enemy[i].col-mine[data.myID-data.myFlag*5].col)+abs(enemy[i].row-mine[data.myID-data.myFlag*5].row)==1&&enemy[i].type==Pioneer)
            {
               haveP=1;
               break;
            }
        }
        
        //判断是否有可占领的矿在身边 
        for(i=0;i<data.totalSource;i++)
        {
            if(abs(data.source[i].col-mine[data.myID-data.myFlag*5].col)+abs(data.source[i].row-mine[data.myID-data.myFlag*5].row)==1&&
            data.map[data.source[i].row][data.source[i].col].isHereSource==data.myFlag+1)
            {
               haveK=1;
               tempi=i;
               break;
            }
        }
        
        for(i=0;i<4;i++) cannotgo[i]=0;
        
        if(haveP==1&&haveK==1)
        {
            if(data.source[tempi].row<mine[data.myID-data.myFlag*5].row) cannotgo[0]=1;
            if(data.source[tempi].row>mine[data.myID-data.myFlag*5].row) cannotgo[1]=1;
            if(data.source[tempi].col<mine[data.myID-data.myFlag*5].col) cannotgo[2]=1;
            if(data.source[tempi].col>mine[data.myID-data.myFlag*5].col) cannotgo[3]=1;    
        }
        
        for(i=0;i<4;i++)
        {
            rundirect[0][i]=rundirect[0][i]&rundirect[1][i]&rundirect[2][i]&rundirect[3][i]&rundirect[4][i]; 
            //记录可以的逃跑方向 
            if(rundirect[0][i]==1&&crossFlag[i]==0&&cannotgo[i]==0) 
            {
                runWays[runWayCount]=i;
                runWayCount+=1;
            }
        }  
        int temp=0;
        //有逃跑的方向 
        if(runWayCount>0)
        {
            temp=RandomInt(0,runWayCount-1);
            if(runWays[temp]==0)order.type=GOUP;
            if(runWays[temp]==1) order.type=GODOWN;
            if(runWays[temp]==2) order.type=GOLEFT;
            if(runWays[temp]==3) order.type=GORIGHT;
            return order;
        }
        else
        {
            for(i=0;i<4;i++)
            {
                if(rundirect[0][i]==1)
                {
                    if(i==0) order.type=GOUP;
                    if(i==1) order.type=GODOWN;
                    if(i==2) order.type=GOLEFT;
                    if(i==3) order.type=GORIGHT;
                    return order;
                }  
            } 
        }
    }
       
        
    //对当前的坦克进行地形权重的选择 
    ChoosePower(data.tank[data.myFlag].type);      
    minLength=INF;
    
    //对所有的金矿进行遍历，看看是否存在现在这个坦克所能达到的。若存在，则minLength会小于INF 
    for(j=0;j<data.totalSource;j++)
    {
        x_start=data.tank[data.myID].col;
        y_start=data.tank[data.myID].row;  
        x_end=data.source[j].col;
        y_end=data.source[j].row;       
        if((t=GetLengthToEnd(data))!=minLength)                                                                                                                               
        {   
            minLength=t;
            break;
        }
    }
    
    
    //如果没有任何一个能走通的金矿，则返回stop 
    if(minLength==INF) 
    {
        order.type=STOP;
        return order;
    }
    
    //分配各个坦克的目标 
    GetNewTargets(data);
           
    x_start=data.tank[data.myID].col;
    y_start=data.tank[data.myID].row; 
    
    //得到火力范围矩阵
    if(x_start-data.tank[data.myID].range<1) left=1; else left=x_start-data.tank[data.myID].range;
    if(x_start+data.tank[data.myID].range>MAP_WIDTH) right=MAP_WIDTH;else right=x_start+data.tank[data.myID].range;
    if(y_start-data.tank[data.myID].range<1) top=1;else top=y_start-data.tank[data.myID].range;
    if(y_start+data.tank[data.myID].range>MAP_HEIGHT) bottom=MAP_HEIGHT;else bottom=y_start+data.tank[data.myID].range;
    
    //遍历坦克的火力范围，寻找生命值最小的敌人 
    order.row=-1;
    order.col=-1;
    minLife=10;
    int fireon;
    for(i=top;i<=bottom;i++)
        for(j=left;j<=right;j++)
            {           
                if(abs(i-y_start)+abs(j-x_start)<=data.tank[data.myID].range&&data.map[i][j].whoIsHere!=-1)
                if((data.myFlag==0&&data.map[i][j].whoIsHere>=5)||(data.myFlag==1&&data.map[i][j].whoIsHere<5))
                {              
                    if(data.tank[data.map[i][j].whoIsHere].noharm==0&&enemy[data.map[i][j].whoIsHere+(data.myFlag-1)*5].life<minLife&&enemy[data.map[i][j].whoIsHere+(data.myFlag-1)*5].life>0)
                    {               
                        minLife=enemy[data.map[i][j].whoIsHere+(data.myFlag-1)*5].life;                                                                           
                        order.row=i;
                        order.col=j;
                        fireon=data.map[i][j].whoIsHere+(data.myFlag-1)*5;
                    }            
                }
            }
    x_start=data.tank[data.myID].col;
    y_start=data.tank[data.myID].row; 
    
    
    if(minLife==10)//对于在打击范围内的预测敌方坦克进行打击    
    for(i=0;i<5;i++)
    {
        if(enemy[i].lost==0&&enemy[i].cansee==0&&abs(enemy[i].row-y_start)+abs(enemy[i].col-x_start)<=data.tank[data.myID].range)
        {
            if(enemy[i].life<minLife&&enemy[i].life>0)
            { 
                order.row=enemy[i].row;
                order.col=enemy[i].col;
                minLife=enemy[i].life;
                fireon=i;
            }
        }
    }
        
    if(minLife!=10) //如果火力范围中有可以打击的敌人 
    {
        enemy[fireon].life-=1;
        order.type=FIRE;
        return order;
    }
    
    
    //如果预测到敌人下一步要进入我方坦克的射程之内,则埋伏 
    
    if(data.tank[data.myID].type!=Pioneer&&rush==0)
    for(i=0;i<5;i++)
    {
        if(enemy[i].life<=0||enemy[i].lost==1) continue;
        if(abs(enemyNext[i].row-y_start)+abs(enemyNext[i].col-x_start)<=data.tank[data.myID].range)
        {
             order.type=STOP;
             return order;
        }
    }
    
    //看看是否有可占金矿距离现决策坦克的距离等于1的 ,有的话就顺便夺了 
    for(i=0;i<data.totalSource;i++)
    {                               
        if(abs(data.tank[data.myID].col-data.source[i].col)+abs(data.tank[data.myID].row-data.source[i].row)<=2&&data.map[data.source[i].row][data.source[i].col].isHereSource!=data.myFlag+1)
        {
            x_start=data.tank[data.myID].col;
            y_start=data.tank[data.myID].row; 
            x_end=data.source[i].col;
            y_end=data.source[i].row;
            if(GetLengthToEnd(data)==1)
            {
                order.type=GetNextStep(data);
                return order;
            }
        }        
    }  
    
    //如果以上的啥也没发生,就去抢矿 
    x_start=data.tank[data.myID].col;
    y_start=data.tank[data.myID].row; 
    x_end=data.source[AI_target[data.myID-data.myFlag*5]].col;
    y_end=data.source[AI_target[data.myID-data.myFlag*5]].row;
        
    if(x_start!=x_end||y_start!=y_end)
    {       
        //若寻路的结果是STOP，说明无路，直接返回order  
        if((order.type=GetNextStep(data))==STOP) 
        {
            return order;
        }
        
        //若要走的下一步是砖墙，则向该砖墙开火 
        if(data.map[y_next][x_next].type==1||data.map[y_next][x_next].type==2)         
        {
            order.type=FIRE;
            order.row=y_next;
            order.col=x_next;
            if(data.round<=15&&data.map[y_next][x_next].type==1) 
            {
                mine[data.myID-data.myFlag*5].zhuan.col=x_next;
                mine[data.myID-data.myFlag*5].zhuan.row=y_next;
            }
            return order;
        }
        else//否则说明下一步是可以达到的，将下一步的位置记录
            {
                if(data.round<=15)
                {
                    for(i=0;i<=data.myID-data.myFlag*5;i++)
                    {
                        if(mine[i].zhuan.row!=-1&&
                        abs(mine[data.myID-data.myFlag*5].row-mine[i].zhuan.row)+abs(mine[data.myID-data.myFlag*5].col-mine[i].zhuan.col)<=mine[data.myID-data.myFlag*5].range)
                        {                      
                            order.type=FIRE;
                            order.row=mine[i].zhuan.row;
                            order.col=mine[i].zhuan.col;
                            mine[i].zhuan.row=-1;
                            mine[i].zhuan.col=-1;  
                            return order;
                            
                        }
                    }
                }
                mine[data.myID-data.myFlag*5].row=y_next;
                mine[data.myID-data.myFlag*5].col=x_next;        
            }
    }   
  
   	return order;  
}
