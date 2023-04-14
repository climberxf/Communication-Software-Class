/***************v3.4版本******************
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
    int dist[point_max+1];  //该点到其他点的最短距离
    int path[point_max+1];  //路径
    int table[point_max+1][3]; //转发表
    int closePointsDist[point_max+1]; //到邻接节点的距离
};
typedef struct graph
{//图
    int n;  //图的节点数
    int edges[point_max+1][point_max+1];  //图的邻接矩阵
}GRAPH;
typedef struct pth
{//用于存放洪范函数所需要的变量
    GRAPH *gra;//图
    int num;//要洪范的id
}PTH;
/******************函数******************/
void djk(int s,GRAPH G,struct point p[]);
void printRes(int k,GRAPH G,struct point p[]);
void sleep(int count);
DWORD WINAPI Initial(LPVOID g);
void updateNode(GRAPH *G,struct point p[]);
void addNode(GRAPH *G,struct point p[]);
void deleteNode(GRAPH *G,struct point p[]);
void printMatrix(GRAPH *G,struct point p[]);
void listDelete(int deleteId,struct point p[],GRAPH G);

int main()
{
    GRAPH G; //初始化图
    G.n=numPoints;
    for(int i=1;i<=point_max;i++)
        for(int j=1;j<=point_max;j++)
            G.edges[i][j]=inf;//初始化邻接矩阵
    
    struct point pointInfo[point_max+1]; //每个节点的相关信息
    for(int i=1;i<=point_max;i++)
        for(int j=1;j<=point_max;j++)
            pointInfo[i].closePointsDist[j]=inf;

    PTH pt[point_max+1];
 	for(int i=0;i<G.n;i++)
    {
        pt[i].gra=&G;
        pt[i].num=i+1;
    }

    HANDLE threads[point_max+1];
    DWORD dwThreads[point_max+1];
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
        for(int i=1;i<=G.n;i++)
        {
            if(G.edges[i][i]==0)
                num_points++;
        }
        if(num_points==G.n)
            break;
    }

    for(int i=1;i<=G.n;i++)
        for(int j=1;j<=G.n;j++)
        {//赋值节点到相邻节点的距离及节点ID
            pointInfo[i].id=i;
            pointInfo[i].closePointsDist[j] = G.edges[i][j];
        }
    
    for(int i = 1; i <= G.n; i++)
    {//进行最短路径计算
        djk(i,G,pointInfo);
    }
    printf("邻接矩阵:\n");
    printMatrix(&G,pointInfo);
    for(int i = 1; i <= G.n; i++)
        printRes(i,G,pointInfo); //进行转发表打印

    while(1)
    {//功能选择
        int func;
        printf("请选择功能:\n");
        printf("1.已有节点权重变化\n2.增加节点\n3.删除节点\n");
        printf("请选择功能（输入功能序号）：");
        scanf("%d",&func);
        switch(func)
        {
            case 1:
                updateNode(&G,pointInfo); break;
            case 2:
                addNode(&G,pointInfo);    break; 
            case 3:
                deleteNode(&G,pointInfo); break;
        }
        for(int i = 1; i <= G.n; i++)
        {//进行最短路径计算
            djk(i,G,pointInfo);
        }
        printf("最新的邻接矩阵:\n");
        printMatrix(&G,pointInfo);
        printf("最新的转发表:\n");
        for(int i = 1; i <= G.n; i++)
            printRes(i,G,pointInfo); //进行路由表打印
    }
}
/*********************************************************
*函数功能：计算图G中s到其他点的最短路径
*函数原型： void djk(int s,GRAPH G,struct point p[])
*函数说明： s为起点节点在节点数组中的第几个(1<=s<=G.n)，G为图的结构体，p[]为节点信息的结构体数组
*返回值：void型
*创建人：奚兴发
*修改记录：
*v1.0    2023.4.9
*********************************************************/
void djk(int s,GRAPH G,struct point p[])
{
    int start=p[s].id;
    int flag[point_max+1]={0}, n_min;
    for(int i = 1; i <= G.n; i++)
    {
        p[s].dist[p[i].id]=inf;
        p[s].path[p[i].id]=-1;
    }

    p[s].dist[start] = 0;
    for(int i = 1; i <= G.n; i++)
    {
        int min = inf;
        for(int j=1;j<=G.n;j++)
        {
            if(flag[p[j].id] == 0 && p[s].dist[p[j].id]<min)
            {
                n_min = j;
                min = p[s].dist[p[j].id];
            }
        }
        flag[p[n_min].id] = 1;
        for(int v = 1; v <= G.n;v++)
        {
            if(p[s].dist[p[v].id] > p[s].dist[p[n_min].id] + p[n_min].closePointsDist[p[v].id] && p[n_min].closePointsDist[p[v].id]<inf)
            {
                p[s].dist[p[v].id] = p[s].dist[p[n_min].id] + p[n_min].closePointsDist[p[v].id];
                p[s].path[p[v].id]=p[n_min].id;
            }
        }
    }
}
/*********************************************************
*函数功能：输出第k个节点的转发表
*函数原型： void printRes(int k,GRAPH G,struct point p[])
*函数说明： k为第k个节点，p[]为节点信息结构体数组，G为图结构体
*返回值：void型
*创建人：奚兴发
*修改记录：
*v1.0    2023.4.9
*********************************************************/
void printRes(int k,GRAPH G,struct point p[])
{
    int a[point_max],count,x,i=0;
    printf("节点%d:\n",p[k].id);
    for(int e=1;e<=G.n;e++)
    {
        if(p[e].id == p[k].id || p[k].path[p[e].id] == -1)
            continue;
        count=0;
        x=p[e].id;
        while(p[k].id!=x)
        {
            a[count++]=x;
            x=p[k].path[x];
        }
        count--;
        p[k].table[i][0]=p[e].id;
        p[k].table[i][1]=a[count];
        p[k].table[i][2]=p[k].closePointsDist[a[count]];
        i++;   
    }
    printf("目的地址\t下一跳地址\t到下一跳的距离\n");
    for(int j=0;j<i;j++)
    {
        printf("%-8d\t%-8d\t%-8d\n", p[k].table[j][0], p[k].table[j][1], p[k].table[j][2]);
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
        case 1:
            G->gra->edges[G->num][1]=0;
            G->gra->edges[G->num][2]=2;
            G->gra->edges[G->num][3]=4;
            G->gra->edges[G->num][4]=22;break;
        case 2:
            G->gra->edges[G->num][1]=2;
            G->gra->edges[G->num][2]=0;
            G->gra->edges[G->num][3]=1;
            G->gra->edges[G->num][4]=6;break;
        case 3:
            G->gra->edges[G->num][1]=4;
            G->gra->edges[G->num][2]=1;
            G->gra->edges[G->num][3]=0;
            G->gra->edges[G->num][4]=1;
            G->gra->edges[G->num][5]=4;break;
        case 4:
            G->gra->edges[G->num][1]=22;
            G->gra->edges[G->num][2]=6;
            G->gra->edges[G->num][3]=1;
            G->gra->edges[G->num][4]=0;
            G->gra->edges[G->num][5]=10;
            G->gra->edges[G->num][6]=5;break;
        case 5:
            G->gra->edges[G->num][3]=4;
            G->gra->edges[G->num][4]=10;
            G->gra->edges[G->num][5]=0;
            G->gra->edges[G->num][6]=3;break;
        case 6:
            G->gra->edges[G->num][4]=5;
            G->gra->edges[G->num][5]=3;
            G->gra->edges[G->num][6]=0;break;
    }
}
/*********************************************************
*函数功能：更新图的权重信息
*函数原型： void updateNode(GRAPH *G,struct point p[])
*函数说明： G指向图的结构体，p[]为节点信息结构体数组
*返回值：void 型
*创建人：奚兴发
*修改记录：
*v1.0    2023.4.9
*********************************************************/
void updateNode(GRAPH *G,struct point p[])
{
    int num;
    printf("请输入要更新几条权重数据:");
    scanf("%d",&num);
    printf("请输入更新的权重数据(如输入:a->b=c):\n");
    while(num--)
    {
        int a,b,dist;
        scanf("%d->%d=%d",&a,&b,&dist);
        p[a].closePointsDist[b]=dist;
        p[b].closePointsDist[a]=dist;
    }
}
/*********************************************************
*函数功能：增加图的节点
*函数原型： void addNode(GRAPH *G,struct point p[])
*函数说明： G指向图的结构体，p[]为节点信息结构体数组
*返回值：void 型
*创建人：奚兴发
*修改记录：
*v1.0    2023.4.9
*********************************************************/
void addNode(GRAPH *G,struct point p[])
{
    int add_node;
    printf("请输入要添加几个节点:");
    scanf("%d",&add_node);
    for(int i=1;i<=add_node;i++)
    {
        int add_dist;
        printf("请输入要对于第%d个节点要添加几组权重数据:",i);
        scanf("%d",&add_dist);
        printf("请添加第%d个节点的相关权重(如输入:b-c表示从添加的第%d个节点到b节点有c的距离):\n",i,i);
        int b,dist;
        G->n+=1;
        p[G->n].id=p[G->n-1].id+1;
        for(int j=1;j<=add_dist;j++)
        {
            scanf("%d-%d",&b,&dist);
            p[G->n].closePointsDist[b]=dist;
            p[b].closePointsDist[p[G->n].id]=dist;
        }
        p[G->n].closePointsDist[p[G->n].id]=0;
    }
}
/*********************************************************
*函数功能：删除节点
*函数原型： void deleteNode(GRAPH *G,struct point p[])
*函数说明： G指向图的结构体，p[]为节点信息结构体数组
*返回值：void 型
*创建人：奚兴发
*修改记录：
*v1.0    2023.4.9
*********************************************************/
void deleteNode(GRAPH *G,struct point p[])
{
    int deleteId[point_max]={0};
    printf("请输入要删除的节点个数:");
    int deleteNodesNum;
    scanf("%d",&deleteNodesNum);
    printf("请输入节点ID(如1 2 3 4代表删除1 2 3 4节点):\n");
    for(int i=0;i<deleteNodesNum;i++)
    {
        scanf("%d",&deleteId[i]);
    }
    //删除相关ID的节点信息，即pointInfo[i]
    for(int i=0;i<deleteNodesNum;i++)
    {
        listDelete(deleteId[i],p,*G);
        G->n--;
    }
}
/*********************************************************
*函数功能：打印邻接矩阵
*函数原型： void printMatrix(GRAPH *G,struct point p[])
*函数说明： G指向图的结构体，p[]为节点信息结构体数组
*返回值：void 型
*创建人：奚兴发
*修改记录：
*v1.1    2023.4.14
*********************************************************/
void printMatrix(GRAPH *G,struct point p[])
{
    printf("\t");
    for(int i=1;i<=G->n;i++)
        printf("NODE[%d]\t",p[i].id);
    printf("\n");
    for(int i=1;i<=G->n;i++)
    {
        printf("NODE[%d] ",p[i].id);
        for(int j=1;j<=G->n;j++)
        {
            printf("%-5d\t",p[i].closePointsDist[p[j].id]);
        }
        printf("\n");
    }
}
/*********************************************************
*函数功能：在顺序表p[]中删除ID为deleteId的节点
*函数原型： void listDelete(int deleteId,struct point p[],GRAPH G)
*函数说明： G指向图的结构体，deleteId为待删除的节点ID，p[]为节点信息结构体数组
*返回值：void 型
*创建人：奚兴发
*修改记录：
*v1.1   2023.4.14
*********************************************************/
void listDelete(int deleteId,struct point p[],GRAPH G)
{
    int delete_n;
    for(int i=1;i<=G.n;i++)
    {
        if(p[i].id==deleteId)
        {
            delete_n = i;
            break;
        }
    }
    for(int i=delete_n;i<=G.n-1;i++)
        p[i]=p[i+1];
    for(int i=1;i<=G.n-1;i++)
        p[i].closePointsDist[deleteId]=inf;
}