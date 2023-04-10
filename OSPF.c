/***************v1.1版本******************
*功能实现：手动输入邻接矩阵，最短路径实现
******************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define point_max 10
#define  inf 65535
/******************结构体******************/
typedef struct point
{
    int id;  //点的ID
    int dist[point_max];  //该点到其他点的最短距离
    int path[point_max];  //路径
}POINT;
typedef struct graph
{
    int n;
    int edges[point_max][point_max];
}GRAPH;
/******************函数******************/
void djk(int s,GRAPH G,POINT *p);
void printRes(int p_id,POINT p,int n);

int main()
{
    GRAPH G;
    POINT pointInfo[point_max];

    printf("请输入有几个节点:");
    scanf("%d", &G.n);
    printf("请输入邻接矩阵:\n");
    for(int i = 0; i < G.n; i++)
        for(int j = 0; j < G.n; j++)
        {
            scanf("%d",&(G.edges[i][j]));
        }
    for(int i = 0; i < G.n; i++)
    {
        djk(i,G,&pointInfo[i]);
    }
    for(int i = 0; i < G.n; i++)
        printRes(i,pointInfo[i],G.n);
}
void djk(int s,GRAPH G,POINT *p)
{
    int flag[point_max]={0}, n_min;
    for(int i = 0; i < G.n; i++)
    {
        p->dist[i]=inf;
        p->path[i]=-1;
    }

    p->dist[s] = 0;
    for(int i = 0; i < G.n; i++)
    {
        int min = inf;
        for(int j=0;j<G.n;j++)
        {
            if(flag[j] == 0 && p->dist[j]<min)
            {
                n_min = j;;
                min = p->dist[j];
            }
        }
        flag[n_min] = 1;
        for(int v = 0; v < G.n;v++)
        {
            if(p->dist[v]>p->dist[n_min]+G.edges[n_min][v] && G.edges[n_min][v]<inf)
            {
                p->dist[v]=p->dist[n_min]+G.edges[n_min][v];
                p->path[v]=n_min;
            }
        }
    }
}

void printRes(int p_id,POINT p,int n)
{
    int a[point_max],count,x;
    printf("节点%d:\n",p_id+1);
    for(int e=0;e<n;e++)
    {
        if(e == p_id)
            printf("%d->%d =0\n",p_id+1,e+1);
        else if(p.path[e]==-1)
            printf("%d->%d 无路径\n",p_id+1,e+1);
        else
        {
            count=0;
            x=e;
            while(p_id!=x)
            {
                a[count++]=x;
                x=p.path[x];
            }
            a[count]=x;
            for(int i=count;i>0;i--)
                printf("%d->",a[i]+1);
            printf("%d =%d\n",a[0]+1,p.dist[e]);
        }
    }
}