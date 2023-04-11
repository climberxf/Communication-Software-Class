/***************v1.1版本******************
*功能实现：手动输入邻接矩阵，最短路径实现
******************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define point_max 10
#define  inf 65535
#define numPoints 6
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
void sleep(int count);

int main()
{
    GRAPH G;
    POINT pointInfo[point_max];
    int fld_done =0,fld_Id[numPoints];
    G.n=numPoints;
    for(int i=0;i<G.n;i++)
        for(int j=0;j<G.n;j++)
            G.edges[i][j]=65535;//初始化邻接矩阵
    
 	while(1)
 	{
    	//srand((unsigned)time(NULL));
    	int routeId=rand()%numPoints;//随机获取节点
    	sleep(rand()%100);//随机休眠
    	if(fld_done==1)
	 		break;//所有节点洪泛完成结束循环
    	else
   		{
    		if(fld_Id[routeId]==1)
				continue;//当前节点已洪泛过就跳过循环
    		else
    		{
        		switch(routeId)
        		{
        		case 0:
            	G.edges[routeId][0]=0;
            	G.edges[routeId][1]=2;
            	G.edges[routeId][2]=4;
            	G.edges[routeId][3]=22;break;
        		case 1:
            	G.edges[routeId][0]=2;
            	G.edges[routeId][1]=0;
            	G.edges[routeId][2]=1;
            	G.edges[routeId][3]=6;break;
        		case 2:
            	G.edges[routeId][0]=4;
            	G.edges[routeId][1]=1;
            	G.edges[routeId][2]=0;
            	G.edges[routeId][3]=1;
            	G.edges[routeId][4]=4;break;
        		case 3:
            	G.edges[routeId][0]=22;
            	G.edges[routeId][1]=6;
            	G.edges[routeId][2]=1;
            	G.edges[routeId][3]=0;
            	G.edges[routeId][4]=10;
            	G.edges[routeId][5]=5;break;
        		case 4:
            	G.edges[routeId][2]=4;
            	G.edges[routeId][3]=10;
            	G.edges[routeId][4]=0;
            	G.edges[routeId][5]=3;break;
        		case 5:
            	G.edges[routeId][3]=5;
            	G.edges[routeId][4]=3;
            	G.edges[routeId][5]=0;break;
        		}
        		fld_Id[routeId]=1;
        		int temp=0;
        		for(int i=0;i<G.n;i++)
        		{
            		if(fld_Id[i]==1)
						temp++;
        		}
        		if(temp==6) fld_done=1;//判断所有节点是否洪泛完成
        	}
    	}
   	}

    for(int i = 0; i < G.n; i++)
    {//进行最短路径计算
        djk(i,G,&pointInfo[i]);
    }
    for(int i = 0; i < G.n; i++)
        printRes(i,pointInfo[i],G.n); //进行最短路径打印
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
/*********************************************************
*函数功能：进行睡眠延时
*函数原型： void sleep(int count)
*函数说明： count为睡眠时间控制因子
*返回值：void型
*创建人：奚兴发
*修改记录：
*v1.0    2023.4.9
*********************************************************/
void sleep(int count)
{
    int i,j;
    for(i=count;i>0;i--)
    for(j=32;j>0;j--);
}