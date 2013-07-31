// ���˵�AI�������á�From Internet.



///////////////////////////////////////////////////////////////////////
//����:ΰ�� QQ:270173053 E-mail:abigwc@gmail.com ��ӭ����        //////
///////////////////////////////////////////////////////////////////////
#include "Tank.h"
#include <string.h>
//�����޸�����ͷ�ļ�
/*
���������������������ͷ�ļ�
*/
#define INF 32767
#define _clr(x,n) memset(x,-1,sizeof(int)*n)
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<malloc.h>
/*
��������������������Զ��庯��
*/
int finish=0; //AstarѰ·�Ƿ���ɣ�δ���Ϊ0�����Ϊ1 
int minLength;//���̹�˵����н��֮��������Сֵ 
int i,j;//ѭ������ʱ���� 
//AstarѰ·���ṹ�嶨�� 
struct node 
{
	int	x, y;				//	����
	int g, h, f;			//	��������ֵ
	struct node* parent;	// �����
	struct node* next;		// ��һ�����
}*opened,*closed;            //����opened���closed�� 
int	x_extends[4], y_extends[4];	//���������ĸ�������չ�ڵ�	
int	g, h, f;//g,h,fֵ 
struct node* p;            //��ʱ�ڵ� 
struct node* q;            //��ʱ�ڵ�
struct node* last;         //��ʱ�ڵ�
int posCount[5];
int enemyPosCount[5];
 
int dis[5][13];              //ÿ��̹�˵�ÿ�����ľ������ 
 
int dis_i,dis_j;
char map[23][23];          //Ѱ·����ʱ��ͼ 
int power=1;               //���ε�Ȩ 
short ableSource[MAX_SOURCE_NUM];    //�ɵ���Ľ���������� 
int totalAble;                 //�ɵ���Ľ������� 
int	x_start = 0, y_start = 0, x_end = 0, y_end = 0;//Ѱ·�������յ����� 
int x_next=0,y_next=0;   //��һ��������
int x_target,y_target;
int x_enemynext,y_enemynext;//���˵���һ�������� 
int fireFlag[5];
int haveWayFlag=0;       //�Ƿ���·��� 
int length=0;            // ·������ 
short AI_target[5]={-1,-1,-1,-1,-1};   //���Ŀ�� 
short enemy_target[5]={-1,-1,-1,-1,-1};  //���˵Ĵ��Ŀ�� 
int canHaveTarget[5];               //����̹�� 
int canBeTarget[13];                //���Դ����Ŀ�� 
int tempLife[5];                    //��ʱ���������� 
int powers[3];                      //���ֵ��ε�Ȩ,0:û������1�����𻵵�שͷ��2���úõ�שͷ 
struct posInfo
{
    int row,col;
}enemyStart[5],lastPos[5],lastEnemyPos[5];
struct tankInfomation               //������Ϣ�ṹ�� 
{
    int row,col;                    //λ�� 
    int life;                       //����
    TankType type;                  //���� 
    int range;                      //��� 
    int state;                      //״̬   0��ʾ��ͨ,1��ʾ��׷��,2��ʾ���� 
    int cansee;                     //�ɼ���,0��ʾ���ɼ�,1��ʾ�ɼ� 
    int lost;                       //Ԥ�ⶪʧ  ����ʧ����Ϊ1������Ϊ0 
    int kasi;                       //̹���Ƿ���   ������,��Ϊ1������Ϊ0 
    int fire;                       //
    posInfo zhuan;                  //��ש�鿪�� 
}mine[5],enemy[5],enemyNext[5];
int myround=0;            //��¼�Լ��Ļغ��� 
int rush=0;
int totalKasi=0;
int totalEnemyKasi=0;
int match[5];
int orders[24][4];
int nn=0;
int ran=0;
int haveFlag; 
int runType=0;
//����һ�����ڵ���iMin��С�ڵ���iMax��int���͵��� 
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
//��ȡȫ������� 
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
//��ʼ��opened���closed�� 
int InitTables(struct node* opened, struct node* closed)
{
	opened->parent = opened->next = NULL;
	opened->f = opened->g = opened->h = opened->x = opened->y = -1;
	closed->f = closed->g = closed->h = closed->x = closed->y = -1;
	closed->parent = closed->next = NULL;
	return 0;
}
//������Ϊx,y�Ľ����뵽opened����, ����-1��ʧ��
int InsertToOpened(int x, int y, struct node* parent, struct node* opened)
{
	p = (struct node*)malloc(sizeof(struct node));
	if (!p) return -1;
	p->x = x;
	p->y = y;
	p->parent = parent;
	p->g = parent == NULL ? 0 : parent->g + power;
	p->h = abs(x_end - x) + abs(y_end - y);		// Ԥ�⵽�յ����
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
// �������뵽closed����
void InsertToClosed(struct node* p, struct node* closed)
{
	p->next = closed->next;
	closed->next = p;
	if (p->x == x_end && p->y == y_end)
	{
		finish=1; 
	}
}
//��closed�����ҽ��,����,����ָ��,���򷵻ؿ�
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
//��closed�����ҽ��,����,����ָ��,���򷵻ؿ�
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
//��opened���е�Ԫ���ƶ���closed����
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
//����opened����Fֵ��С��Ԫ�أ�������opened���еĵ�һ��Ԫ�� 
struct node* GetFSmallestInOpened(struct node* opened)
{
	return opened->next;
}
//��չ���P
int	ExtendNode(struct node* p, struct node* opened, struct node* closed, char map[MAP_HEIGHT+2][MAP_WIDTH+2])
{
    int i=0;
    g=0;
    h=0;
    f=0;
	x_extends[orders[ran][0]] = p->x;
    x_extends[orders[ran][1]] = p->x;		// ��Χ����չ�ĵ㣬��������
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
		}	// ��������opened��		
		InsertToOpened(x_extends[i], y_extends[i], p, opened);
	}
	return 0;
}
//AstarѰ·
int AStar(struct node* opened, struct node* closed, char map[MAP_HEIGHT+2][MAP_WIDTH +2])
{
	InsertToOpened(x_start, y_start, NULL, opened);
	while ((p = GetFSmallestInOpened(opened)) != NULL && finish != 1)
	{
		ExtendNode(p, opened, closed, map);
	}
	if (finish==1) return 1;// û���ҵ�·��,����0	
	return 0;
}
//��ȡ��һ����ָ�� 
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
//��ȡ������һ����ָ�� 
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
//��ȡ����֮��ĺķ����ٵ�·���ĺķ� 
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
//��ȡ�з�����֮��ĺķ����ٵ�·���ĺķ� 
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
//�жϲ����������No�Ƿ��ڴ����Ŀ������Target��,���ڣ�����1�����򷵻�0 
short isInTarget(short No)
{
    short i;
    for(i=0;i<5;i++)
        if(AI_target[i]==No) return 1;
    return 0;
}
//km��������ƥ�� 
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
//��ȡ�µĴ��Ŀ�� 
void GetNewTargets(DataForAI data)
{
    int i,j;
    int goFor=0;
    
    //������Ҫȥռ��Ľ��ĸ����������goFor��
    for(i=0;i<data.totalSource;i++)
        if(data.map[data.source[i].row][data.source[i].col].isHereSource!=data.myFlag+1) goFor++;
  
    //��5����������Ҫռ��Ľ�� 
    if(goFor>=5)
    {
        dis_i=0;
        for(i=0;i<5;i++)
        { 
            //���̹�˵�����ֵ����0
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
    else//��Ҫռ��Ľ��С��5 
    if(data.map[data.source[AI_target[data.myID-data.myFlag*5]].row][data.source[AI_target[data.myID-data.myFlag*5]].col].isHereSource==data.myFlag+1)
    {
        minLength=INF;
        int t; 
        //�ȶ����е���Ҫռ��Ľ����б���������н��û�б���Ϊĳ��̹�˵�Ŀ�꣬�򽫸ý����Ϊ���̹�˵�Ŀ�� 
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
        
        //������û������ΪĿ���δռ��Ľ�������Լ�������Ǹ��Ѿ��ж����ڴ�Ľ����Ϊ�Լ���Ŀ��   
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
            
            //���еĽ�󶼱��Լ�ռ���ˣ��������һ�����ȥתת 
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
//����̹�˵�����ѡ��ͬ���ε�Ȩ��
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
//ƽ̨0�غ�ʱ���ô˺�����ȡAI���Ƽ�̹��������Ϣ�������޸Ĵ˺���������
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
	strcpy(Info.aiName,"abigwc"); //AI������ʹ�����ġ� 
	return Info;
}
//ƽ̨�ӵ�1�غϿ�ʼ���ô˺������ÿ�غ�ָ������޸Ĵ˺���������
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
        
        //��ͬһλ�õļ��� 
        for(i=0;i<5;i++) posCount[i]=0;
        myround=0;
    }
    
    //��ת�׶� 
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
        
        //�õ��ҷ�̹��λ��,����
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
        //������Ұ����Ԥ�� 
        for(i=0;i<5;i++)
        {
            //��ȡ���˵����� 
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
            
            
            //�ոո���ĵ��� 
            if(enemy[i].life!=0&&enemy[i].col==-2)
            {
                enemy[i].row=enemyStart[i].row;
                enemy[i].col=enemyStart[i].col;
                enemy[i].cansee=0;    
                enemy[i].lost=0;
                continue;
            } 
            
            //�ɼ��ĵ��� 
            if(data.tank[i+(1-data.myFlag)*5].col!=-2)
            {
                enemy[i].row=data.tank[i+(1-data.myFlag)*5].row;
                enemy[i].col=data.tank[i+(1-data.myFlag)*5].col;
                enemy[i].cansee=1;
                enemy[i].lost=0;
            }
            //�����˲��ɼ�������һ�غ�Ԥ��ĵ��˵�λ����Ϊ���ڵ��˵�λ�� 
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
        //���ݵ��˵ĵ�ǰλ��Ԥ����˵���һ�غϵ�λ��
       
        for(i=0;i<5;i++)
        {
            //���̹���Ѿ���������ȡ���Ըõз�̹�˵�Ԥ�� 
            if(enemy[i].life==0) continue;
            
            int canBeSee=0;
            //����Ƿ�Ԥ�ⶪʧ 
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
            //�Ե�ǰ�ĵз�̹��ѡ�����Ȩ�� 
            ChoosePower(enemy[i].type);
           
           int sml=2;
           int zishaFlag=0; 
           //�����Pioneer���������ҷ�̹����֮����<=2��������������ɱս��,Ԥ����λ�� 
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
           
           //���õ�������ɱ��Ŀ��,������Ԥ�� 
           if(zishaFlag==0)
           {
                //�з�̹�˵���ʼλ�� 
                x_start=enemy[i].col;
                y_start=enemy[i].row; 
            
                //�Խ����б���,ѡ����õз�̹�˾�����С�ĵط�Ŀ���� 
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
            
            //�з�̹�˵���ʼλ�� 
            x_start=enemy[i].col;
            y_start=enemy[i].row; 
           
            //������ھ�����̵ĵз�̹�˿�ռ����,������Ϊ�з�����ռĿ�� 
            if(minLength!=INF||zishaFlag==1)
            {
            
                x_end=x_target;
                y_end=y_target;
                //�õ��õз�̹�˵���һ��λ�� 
                GetEnemyNextStep(data);
                //����õ�����һ��λ�ò���ש��,��Ԥ��ĵз�λ�ö�Ϊ�õ��ĵз���һ��λ�� 
                if(data.map[y_next][x_next].type!=1&&data.map[y_next][x_next].type!=2)
                {
                    enemyNext[i].row=y_next;
                    enemyNext[i].col=x_next; 
                }
            }       
        }     
        
        for(i=0;i<5;i++) fireFlag[i]=0;
        
        //����ÿ��̹��׷����� 
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
        
        //�Ƿ�ǿ�� 
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
    
    //Pioneer��̹��ִ����ɱ���� 
    if(data.tank[data.myID].type==Pioneer&&fireFlag[data.myID-data.myFlag*5]!=1)
    {
                                                                                                                
        //�����Ƿ��п�ռ�������־���̹�˵ľ���С�ڵ���2�� 
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
                        //Pioneer��ײ���ǿ���Ĳ���
                                             
                        int enemyfire=0;
                        //������˻�����С  
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
    
    int rundirect[5][4];                 //�������ез�̹�˵����ܷ��� ,����ڶ�ά���±�0,1,2,3�ֱ��ʾ�������� 
    int runFlag=0;
    int crossFlag[4];
    
    for(i=0;i<4;i++) crossFlag[i]=0;
   
    //������˿��������˼һ��н϶�����,������ 
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
            //���������� 
            if(enemyNext[i].row>enemy[i].row)
            {
                
                //���ڵ������ 
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
                //���ڵ����ұ� 
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
                //�Һ͵�����һ������ 
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
            //���������� 
            if(enemyNext[i].row<enemy[i].row)
            {
                //���ڵ������ 
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
                //���ڵ����ұ� 
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
                //�Һ͵�����һ������ 
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
                                  
            //���������� 
            if(enemyNext[i].col>enemy[i].col)
            {
                //���ڵ����ϱ� 
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
                //���ڵ����±� 
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
                //�Һ͵�����һ������ 
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
            //���������� 
            if(enemyNext[i].col<enemy[i].col)
            {
                //���ڵ����ϱ� 
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
                //���ڵ����±� 
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
                //�Һ͵�����һ������ 
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
        
        
        //�ж�����Pioneer��׷�� 
        for(i=0;i<5;i++)
        {
            if(abs(enemy[i].col-mine[data.myID-data.myFlag*5].col)+abs(enemy[i].row-mine[data.myID-data.myFlag*5].row)==1&&enemy[i].type==Pioneer)
            {
               haveP=1;
               break;
            }
        }
        
        //�ж��Ƿ��п�ռ��Ŀ������ 
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
            //��¼���Ե����ܷ��� 
            if(rundirect[0][i]==1&&crossFlag[i]==0&&cannotgo[i]==0) 
            {
                runWays[runWayCount]=i;
                runWayCount+=1;
            }
        }  
        int temp=0;
        //�����ܵķ��� 
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
       
        
    //�Ե�ǰ��̹�˽��е���Ȩ�ص�ѡ�� 
    ChoosePower(data.tank[data.myFlag].type);      
    minLength=INF;
    
    //�����еĽ����б����������Ƿ�����������̹�����ܴﵽ�ġ������ڣ���minLength��С��INF 
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
    
    
    //���û���κ�һ������ͨ�Ľ���򷵻�stop 
    if(minLength==INF) 
    {
        order.type=STOP;
        return order;
    }
    
    //�������̹�˵�Ŀ�� 
    GetNewTargets(data);
           
    x_start=data.tank[data.myID].col;
    y_start=data.tank[data.myID].row; 
    
    //�õ�������Χ����
    if(x_start-data.tank[data.myID].range<1) left=1; else left=x_start-data.tank[data.myID].range;
    if(x_start+data.tank[data.myID].range>MAP_WIDTH) right=MAP_WIDTH;else right=x_start+data.tank[data.myID].range;
    if(y_start-data.tank[data.myID].range<1) top=1;else top=y_start-data.tank[data.myID].range;
    if(y_start+data.tank[data.myID].range>MAP_HEIGHT) bottom=MAP_HEIGHT;else bottom=y_start+data.tank[data.myID].range;
    
    //����̹�˵Ļ�����Χ��Ѱ������ֵ��С�ĵ��� 
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
    
    
    if(minLife==10)//�����ڴ����Χ�ڵ�Ԥ��з�̹�˽��д��    
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
        
    if(minLife!=10) //���������Χ���п��Դ���ĵ��� 
    {
        enemy[fireon].life-=1;
        order.type=FIRE;
        return order;
    }
    
    
    //���Ԥ�⵽������һ��Ҫ�����ҷ�̹�˵����֮��,����� 
    
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
    
    //�����Ƿ��п�ռ�������־���̹�˵ľ������1�� ,�еĻ���˳����� 
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
    
    //������ϵ�ɶҲû����,��ȥ���� 
    x_start=data.tank[data.myID].col;
    y_start=data.tank[data.myID].row; 
    x_end=data.source[AI_target[data.myID-data.myFlag*5]].col;
    y_end=data.source[AI_target[data.myID-data.myFlag*5]].row;
        
    if(x_start!=x_end||y_start!=y_end)
    {       
        //��Ѱ·�Ľ����STOP��˵����·��ֱ�ӷ���order  
        if((order.type=GetNextStep(data))==STOP) 
        {
            return order;
        }
        
        //��Ҫ�ߵ���һ����שǽ�������שǽ���� 
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
        else//����˵����һ���ǿ��Դﵽ�ģ�����һ����λ�ü�¼
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
