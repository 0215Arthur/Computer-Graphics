#pragma once

#define WINDOW_HEIGHT 1000
#include<iostream>
#include<math.h>

using namespace std;
#define NULL 0
typedef struct Edge
{
	float x;          /* 当前扫描线与边的交点的x值  */
	float dx;         /* 从当前扫描线到下一扫描线之间的x的增量 */
	int dmax;         /*  计算点到y=kx的距离 */
	struct Edge *next;
}Edge;

typedef struct POINT
{
	int x;
	int y;
	int d;

} POINT;
/*
直线Bresonham算法
*/
void Bre_Line(int x0, int y0, int x1, int y1)				
{
	int x, y, dx, dy, e;
	int i, temp;
	int change = 0;
	int Step_X = 1, Step_Y = 1;
	dx = x1 - x0, dy = y1 - y0;
	glBegin(GL_POINTS);
	if (dx<0) {
		dx = -dx;
		Step_X = -1;
	}
	if (dy<0) {
		dy = -dy;
		Step_Y = -1;
	}
	if (dy>dx) {
		temp = dx;
		dx = dy;
		dy = temp;
		change = 1;
	}
	e = -dx, x = x0, y = y0;
	for (i = 0; i <= dx; i++)
	{
		//画点
		glVertex3i(x, y, 0);
		if (change)  y = y + Step_Y;
		else	  	x = x + Step_X;
		e = e + 2 * dy;
		if (e >= 0)
		{
			if (change)	x = x + Step_X;
			else        y = y + Step_Y;
			e = e - 2 * dx;
		}
	}
	glEnd();
}
/*
计算所有点中到y=kx距离的最大值

*/

int MaxD(POINT *pts, int cnt)
{
	float max = pts[0].d;
	for (int i = 0; i<cnt; i++)
	{
		if (max<pts[i].d)
			max = pts[i].d;
	}
	return max;
}

/*
计算每一点到参考线y=kx+-b的间距
利用点到直线的距离公式
*/
int  CalcutaleD(int k, POINT *pts, int cnt,bool dir)
{


	double n = 1 + k*k;
	float m = sqrt(n);

	for (int i = 0; i < cnt; i++)
	{

		pts[i].d = abs((pts[i].y) - k*(pts[i].x)) / m;

	}
	
	if (dir)
	{
		int max = MaxD(pts, cnt);
		int lens = max*sqrt(float(1 + k*k));
		for (int i = 0; i < cnt; i++)
			pts[i].d = abs((pts[i].y) - k*(pts[i].x)+lens) / m;
		return lens;
	}
	return 0;
        
	//return lens;
}

/*****
计算所有点到y=kx距离的最小值
int MinD(POINT *pts, int cnt)

{
	float min = pts[0].d;
	for (int i = 0; i<cnt; i++)
	{
		if (min>pts[i].d)
			min = pts[i].d;
	}
	return min;
}
*****/

/*
按交点的x升序对边进行插入排序
用于建立边表
*/
void InsertEdge(Edge *list, Edge *edge)
{
	Edge *p, *q = list;
	p = q->next;
	while (p)
	{
		if (edge->x<p->x)
			p = NULL;
		else
		{
			q = p;
			p = p->next;
		}
	}
	edge->next = q->next;
	q->next = edge;
}
/*
计算下一条非水平线的d值
到参考扫描线的距离
*/
int  dNext(int k, int cnt, POINT *pts)
{
	int j;
	if ((k + 1)>(cnt - 1))
		j = 0;
	else
		j = k + 1;
	while (pts[k].d == pts[j].d)
	{
		if ((j + 1)>(cnt - 1))
			j = 0;
		else
			j++;
	}
	return(pts[j].d);
}


/*
生成有序边表的一条边 
lower（低点） upper（高点）
构建边表
*/
void MakeEdgeRec(POINT lower, POINT upper, int dComp, Edge *edge, Edge *edges[],float k)
{
	float k1;
	//float k =0;

	int db = 1;

	k1 = (float)(upper.y - lower.y) / (float)(upper.x - lower.x);
	cout << upper.y << endl << endl;
	k1 = k1 - k;
	
	edge->dx =k1? (db)*sqrt(1 + k*k) / k1 :0;

	edge->x = lower.x;
	if (upper.d < dComp)
		edge->dmax = upper.d - 1;//该点既不是局部高点也不是局部低点
	else
		edge->dmax = upper.d;//局部高点
	InsertEdge(edges[lower.d], edge);

}


/*
建立有序边表
*/
void BuildEdgeList(int cnt, POINT *pts, Edge *edges[],float k)
{
	Edge *edge;
	POINT v1, v2;
	int i, dPrev = pts[cnt - 2].d;
	v1 = pts[cnt - 1];
	for (i = 0; i<cnt; i++)
	{
		v2 = pts[i];
		if (v1.d != v2.d)
		{
			edge = (Edge *)malloc(sizeof(Edge));
			if ((v1.d < v2.d))
				MakeEdgeRec(v1, v2, dNext(i, cnt, pts), edge, edges,k);      //高点的下一个点
			else
				MakeEdgeRec(v2, v1, dPrev, edge, edges,k);
		}
		dPrev = v1.d;   
		v1 = v2;
	}
}

/*
建立活性边表
scan 对应有序边表扫描线
*/
void BuildActiveList(int scan, Edge * active, Edge *edges[])
{
	Edge *p, *q;
	p = edges[scan]->next;
	while (p)
	{
		q = p->next;
		InsertEdge(active, p);
		p = q;
	}
}

/*
对扫描线填充
计算各点y坐标
利用直线算法计算 
*/
void FillScan(int scan, Edge * active,int k,bool dir, int lens)
{
	Edge *p1, *p2;
	p1 = active->next;
	while (p1)
	{
		p2 = p1->next;
		int x1 = p1->x;
		int x2 = p2->x;
		int y1, y2;
		if (dir)
		{
		    y1 = k * x1 + scan*sqrt((float)(1 + k*k)) - lens;
			y2 = k * x2 + scan*sqrt((float)(1 + k*k)) - lens;
			
		}
		else
		{
		
			y1 = k * x1 + scan*sqrt((float)(1 + k*k));
			y2 = k * x2 + scan*sqrt((float)(1 + k*k));
		}
		
		//cout<<x1<<"    "<<x2<<"   "<<y1<<"    "<<y2<<endl;

		Bre_Line(x1, y1, x2, y2);		
		p1 = p2->next;

	}
}

/*
删除不再相交的边
*/
void DeleteAfter(Edge *q)
{
	Edge *p = q->next;
	q->next = p->next;
	free(p);
}

/*、
更新下一条扫描线
*/
void  UpdateActiveList(int scan, Edge *active)
{
	Edge *q = active, *p = active->next;
	while (p)
		if (scan >= p->dmax)
		{
			p = p->next;
			DeleteAfter(q);
		}
		else
		{
			p->x = p->x + p->dx;   
			q = p;
			p = p->next;
		}
}

int db = 0;
/*
活性边表重新排序

*/
void ResortActiveList(Edge *active)
{
	Edge *q;
	Edge *p = active->next;
	active->next = NULL;
	while (p)
	{
		q = p->next;
		InsertEdge(active, p);
		p = q;
	}

}


/*
对图形进行扫描填充
*/
void ScanFill(int cnt, POINT *pts,int k,int _db)
{

	bool dir;
	//int k=1;
	dir = k > 0 ? true : false;
	int lens=CalcutaleD(k, pts, cnt,dir);
	

	Edge *edges[WINDOW_HEIGHT], *active;
	int i, scan;
	for (i = 0; i<WINDOW_HEIGHT; i++)
	{
		edges[i] = (Edge *)malloc(sizeof(Edge));
		edges[i]->next = NULL;
	}
	BuildEdgeList(cnt, pts, edges,k);

	/*for(int i=0;i<WINDOW_HEIGHT;i++)
	{p=edges[i]->next;
	while(p!=NULL)
	{
	p=p->next;
	}}*/

	active = (Edge *)malloc(sizeof(Edge));
	active->next = NULL;


	for (scan = 0; scan<WINDOW_HEIGHT; scan++)
	{
		BuildActiveList(scan, active, edges);
		if (active->next)
		{
			if (db % _db == 0)//每4个间距画一条线。
			{
				FillScan(scan, active,k,dir,lens);
			}
			UpdateActiveList(scan, active);
			ResortActiveList(active);
		}
		db++;
	}
}

