/***************v3.1版本******************
*功能实现：线程实现随机性，最短路径实现，节点老化，增加节点功能
******************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#define point_max 20
#define  inf 65535
#define numPoints 6
/******************结构体******************/
struct point
{
    int id;  //点的ID
    int dist[point_max];  //该点到其他点的最短距离
    int path[point_max];  //路径
    int table[point_max][3]; //转发表
};
typedef struct graph
{//图
    int n;  //图的节点数
    int edges[point_max][point_max];  //图的邻接矩阵
}GRAPH;
typedef struct pth
{//用于存放洪范函数所需要的变量
    GRAPH *graph;//图
    int num;//要洪范的id
}PTH;
/******************函数******************/
void djk(int s,GRAPH G,struct point *p);
void printRes(int p_id,GRAPH G,struct point *p);
void sleep(int count);
DWORD WINAPI Initial(LPVOID g);
void updateNode(GRAPH *G);
void addNode(GRAPH *G);

int main()
{
    GRAPH G; //初始化图
    struct point pointInfo[point_max]; //每个节点的相关信息
    G.n=numPoints;
    for(int i=0;i<G.n;i++)
        for(int j=0;j<G.n;j++)
            G.edges[i][j]=inf;//初始化邻接矩阵
    PTH pt[point_max];
 	for(int i=0;i<G.n;i++)
    {
        pt[i].graph=&G;
        pt[i].num=i;
    }

    HANDLE threads[point_max];
    DWORD dwThreads[point_max];
    while(1)
    {//利用线程对节点进行洪范
        int num_points=0;
        for(int i=0;i<G.n;i++)
        {
            threads[i]=CreateThread(NULL,0,Initial,&pt[i],0,&dwThreads[i]);
        }
        WaitForMultipleObjects(point_max,threads,TRUE,INFINITE);
        for(int i=0;i<G.n;i++)
        {
            CloseHandle(threads[i]);
        }
        for(int i=0;i<G.n;i++)
        {
            if(G.edges[i][i]==0)
                num_points++;
        }
        if(num_points==G.n)
            break;
    }
    
    for(int i = 0; i < G.n; i++)
    {//进行最短路径计算
        djk(i,G,&pointInfo[i]);
    }
    for(int i = 0; i < G.n; i++)
        printRes(i,G,&pointInfo[i]); //进行转发表打印

    while(1)
    {//功能选择
        int func;
        printf("请选择功能:\n");
        printf("1.节点老化\n2.增加节点\n");
        printf("请选择功能（输入功能序号）：");
        scanf("%d",&func);
        switch(func)
        {
            case 1:
                updateNode(&G); break;
            case 2:
                addNode(&G);    break;
        }
        for(int i = 0; i < G.n; i++)
        {//进行最短路径计算
            djk(i,G,&pointInfo[i]);
        }
        printf("最新的转发表:\n");
        for(int i = 0; i < G.n; i++)
            printRes(i,G,&pointInfo[i]); //进行路由表打印
    }
}
/*********************************************************
*函数功能：计算图G中s到其他点的最短路径
*函数原型： void djk(int s,GRAPH G,struct point *p)
*函数说明： s为起点编号，G为图的结构体，*p为节点信息的结构体
*返回值：void型
*创建人：奚兴发
*修改记录：
*v1.0    2023.4.9
*********************************************************/
void djk(int s,GRAPH G,struct point *p)
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
*函数功能：输出p_id节点的转发表
*函数原型： void printRes(int p_id,GRAPH G,struct point *p)
*函数说明： p_id为起点节点编号，p为起点节点信息结构体指针，G为图指针
*返回值：void型
*创建人：奚兴发
*修改记录：
*v1.0    2023.4.9
*********************************************************/
void printRes(int p_id,GRAPH G,struct point *p)
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
    printf("目的地址\t下一跳地址\t到下一跳的距离\n");
    for(int j=0;j<i;j++)
    {
        printf("%-8d\t%-8d\t%-8d\n",p->table[j][0],p->table[j][1],p->table[j][2]);
    }
}
/*********************************************************
*函数功能：进行睡眠延时
*函数原型： void sleep(int count)
*函数说明： count为睡眠时间控制因子
*返回值：void型
*创建人：吴春阳
*修改记录：
*v1.0    2023.4.9
*********************************************************/
void sleep(int count)
{
    int i,j;
    for(i=count;i>0;i--)
    for(j=32;j>0;j--);
}
/*********************************************************
*函数功能：线程函数，随机洪范
*函数原型： DWORD WINAPI Initial(LPVOID g)
*函数说明： g指向变量G（图）
*返回值：void*型
*创建人：奚兴发
*修改记录：
*v1.0    2023.4.9
*********************************************************/
DWORD WINAPI Initial(LPVOID g)
{
    PTH *G=(PTH *)g;
    srand((unsigned)time(NULL));
    sleep(rand()%100);//随机休眠

    switch(G->num)
    {
        case 0:
            G->graph->edges[G->num][0]=0;
            G->graph->edges[G->num][1]=2;
            G->graph->edges[G->num][2]=4;
            G->graph->edges[G->num][3]=22;break;
        case 1:
            G->graph->edges[G->num][0]=2;
            G->graph->edges[G->num][1]=0;
            G->graph->edges[G->num][2]=1;
            G->graph->edges[G->num][3]=6;break;
        case 2:
            G->graph->edges[G->num][0]=4;
            G->graph->edges[G->num][1]=1;
            G->graph->edges[G->num][2]=0;
            G->graph->edges[G->num][3]=1;
            G->graph->edges[G->num][4]=4;break;
        case 3:
            G->graph->edges[G->num][0]=22;
            G->graph->edges[G->num][1]=6;
            G->graph->edges[G->num][2]=1;
            G->graph->edges[G->num][3]=0;
            G->graph->edges[G->num][4]=10;
            G->graph->edges[G->num][5]=5;break;
        case 4:
            G->graph->edges[G->num][2]=4;
            G->graph->edges[G->num][3]=10;
            G->graph->edges[G->num][4]=0;
            G->graph->edges[G->num][5]=3;break;
        case 5:
            G->graph->edges[G->num][3]=5;
            G->graph->edges[G->num][4]=3;
            G->graph->edges[G->num][5]=0;break;
    }
}
/*********************************************************
*函数功能：更新图的权重信息
*函数原型： void updateNode(GRAPH *G)
*函数说明： G指向图的结构体
*返回值：void 型
*创建人：奚兴发
*修改记录：
*v1.0    2023.4.9
*********************************************************/
void updateNode(GRAPH *G)
{
    int num;
    printf("请输入要更新几条权重数据:");
    scanf("%d",&num);
    printf("请输入更新的权重数据(如输入:a->b=c):\n");
    while(num--)
    {
        int a,b,dist;
        scanf("%d->%d=%d",&a,&b,&dist);
        G->edges[a-1][b-1]=dist;
        G->edges[b-1][a-1]=dist;
    }
}
/*********************************************************
*函数功能：更新图的节点
*函数原型： void addNode(GRAPH *G)
*函数说明： G指向图的结构体
*返回值：void 型
*创建人：奚兴发
*修改记录：
*v1.0    2023.4.9
*********************************************************/
void addNode(GRAPH *G)
{
    int add_node;
    printf("请输入要添加几个节点:");
    scanf("%d",&add_node);
    G->n+=add_node;
    for(int i=0;i<add_node;i++)
    {
        int add_dist;
        printf("请输入要对于第%d个节点要添加几组权重数据:",i+1);
        scanf("%d",&add_dist);
        printf("请添加第%d个节点的相关权重(如输入:b-c表示从添加的第%d个节点到b节点有c的距离):\n",i+1,i+1);
        int b,dist;
        for(int j=0;j<add_dist;j++)
        {
            scanf("%d-%d",&b,&dist);
            G->edges[(G->n)+i][b-1]=dist;
            G->edges[b-1][(G->n)+i]=dist;
        }
    }
}