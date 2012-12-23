#include<stdio.h>
#include<iostream>
#include<string.h>
#include<queue>
#include <math.h>
using namespace std;
#define inf 1000000000
#define M 600
int pre[M],cost[M],pv[M],head[89999];
bool vis[M];
queue<int>q;
struct node{
	int c , w , pos , next;
}E[89999];
int Max(int a,int b)
{
	if(a > b)
		return a;
	return b;
}
int Min(int a,int b)
{
	if(a < b) 
		return a;
	return b;
}
int MCMF(int s,int t,int need)
{
	int maxflow = 0,mincost = 0;
	int mi,i;
	while(!q.empty()) q.pop();
	while(1)
	{
		//memset(pre,-1,sizeof(pre));
		memset(pv,-1,sizeof(pv));
		memset(vis,0,sizeof(vis));
		for(i = 0;i <= t;i ++)
			cost[i] = -inf;
		cost[s] = 0;
		q.push(s);
		while(!q.empty())
		{
			int u = q.front();
			q.pop();
			vis[u] = 0;
			for(i = head[u];i != -1;i = E[i].next)
			{
				int v = E[i].pos;
				if(E[i].c && cost[v] < cost[u] + E[i].w)
				{
					cost[v] = cost[u] + E[i].w;
					if(!vis[v])
					{
						vis[v] = 1;
						q.push( v );
					}
					pre[v] = i; pv[v] = u;
				}
			}
		}
		if(pv[t] == -1) break;
		mi = inf;
		for(i = t;i != s;i = pv[i])
		{
			mi = Min(mi,E[pre[i]].c);
		}
		for(i = t;i != s;i = pv[i])
		{
			E[pre[i]].c -= mi;
			E[pre[i]^1].c += mi;
		}
		maxflow +=mi;
		mincost += cost[t] * mi;
	}
	if(maxflow != need) return -1;
	return mincost;
}
int NE;
void insert(int u,int v,int w,int c,int cc = 0) 
{
	E[NE].c = c;    E[NE].pos = v;
	E[NE].w = w;    E[NE].next = head[u];    head[u] = NE++;
	E[NE].c = cc;    E[NE].pos = u;
	E[NE].w = -w;    E[NE].next = head[v];    head[v] = NE++;
}
int n,m,K;
int a[1002],b[1002];
int main(){
	//freopen("data.in","r",stdin);
	//freopen("data.out","w",stdout);
	int i,j,s,t,need,sum,temp,tt,T;
	int k = 32;
	int CN[1000],CM[1000],mark[40][40],num1[40],num2[1000];
	for(i = 0;i <= 30;i ++){
		for(j = i;j <= 30;j ++){
			CN[k] = j-i;
			CM[k] = i+j;
			mark[i][j] = mark[j][i] = k ++;
		}
	}
	scanf("%d",&T);
	int cas = 1;
	while(T --){
		scanf("%d%d%d",&n,&m,&K);
		if(n == 0 || m == 0) while(1);
		if(m < K) while(1);
		if(K >= n || K <= 0) while(1);

		for(int i = 0;i < n;i ++)
		{
			scanf("%d",&a[i]);
			if(a[i] > 30 || a[i] < 0) while(1);
		}
		for(int i = 0;i < m;i ++)
		{
			scanf("%d",&b[i]);
			if(b[i] > 30 || b[i] < 0) while(1);
		}

		memset(head,-1,sizeof(head));
		memset(num1,0,sizeof(num1));
		memset(num2,0,sizeof(num2));
		NE = 0;

		for(i = 0;i < m;i ++){
			num1[b[i]] ++;
		}
		sum = 0;
		for(i = 1;i < n;i ++){
			num2[ mark[a[i]][a[i-1]] ] ++;
			sum += abs(a[i]-a[i-1]);
		}

		temp = k;
		s = k+1;
		t = k+2;
		need = 0;
		insert(s,temp,0,K,0);
		for(i = 0;i <= 30;i ++){
			if(num1[i] == 0) continue;
			insert(temp,i,0,num1[i],0);
			need += num1[i];
		}
		insert(temp,t,0,inf,0);
		for(i = 32;i < k;i ++){
			if(num2[i] == 0) continue;
			insert(i,t,0,num2[i],0);
		}
		for(i = 0;i <= 30;i ++){
			for(j = 32;j < k;j ++){
				tt = abs(CM[j] - i-i)-CN[j];
				if(tt <= 0) continue;
				insert(i,j,tt,inf,0);
			}
		}
		printf("Case %d: %d\n",cas ++,MCMF(s,t,K)+sum);
	}
	return 0;
}
