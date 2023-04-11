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
    int table[point_max][3];
}POINT;
typedef struct graph
{
    int n;
    int edges[point_max][point_max];
}GRAPH;
/******************函数******************/
void djk(int s,GRAPH G,POINT *p);
void printRes(int p_id,GRAPH G,POINT *p);

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
        printRes(i,G,&pointInfo[i]);
}
/*********************************************************
*函数功能：计算图G中s到其他点的最短路径
*函数原型： void djk(int s,GRAPH G,POINT *p)
*函数说明： s为起点编号，G为图的结构体，*p为节点信息的结构体
*返回值：void型
*创建人：奚兴发
*修改记录：
*v1.0    2023.4.9
*********************************************************/
void djk(int s,GRAPH G,POINT *p)
{
    int flag[point_max]={0}, n_min;
    p->id=s;
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
/*********************************************************
*函数功能：输出从p_id为起点到各个节点的路径与最小值
*函数原型： void printRes(int p_id,POINT p,int n)
*函数说明： p_id为起点节点编号，p为起点节点信息，n为节点数
*返回值：void型
*创建人：奚兴发
*修改记录：
*v1.0    2023.4.9
*********************************************************/
void printRes(int p_id,GRAPH G,POINT *p)
{
    int a[point_max],count,x,i=0;
    printf("节点%d:\n",p_id+1);
    for(int e=0;e<G.n;e++)
    {
        if(e == p_id || p->path[e] == -1)
            continue;
        count=0;
        x=e;
        while(p_id!=x)
        {
            a[count++]=x;
            x=p->path[x];
        }
        count--;
        p->table[i][0]=e+1;
        p->table[i][1]=a[count]+1;
        p->table[i][2]=G.edges[p_id][a[count]];
        i++;   
    }
    printf("目的地址\t下一跳地址\t最小距离\n");
    for(int j=0;j<i;j++)
    {
        printf("%-8d\t%-8d\t%-8d\n",p->table[j][0],p->table[j][1],p->table[j][2]);
    }
}