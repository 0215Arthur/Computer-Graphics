
#include <GL/glut.h>
#include"draw.h"
#include<iostream>
using namespace std;

int k;
float dr;
int pos;
POINT *p;
bool select = false;//图形点输入完成标志

/*
OpenGL初始化
*/
void myInit(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glColor3f(0.0f, 0.0f, 0.0f);
	glPointSize(2.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 640.0, 0.0, 480.0);
}
/*
图形扫描与显示
*/
void myDisplay(void)
{

	glClear(GL_COLOR_BUFFER_BIT);

	if (select == true)
	{
		//多边形各点
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i<k; i++)
			glVertex2i(p[i].x, p[i].y);
		glEnd();
		//扫描填充
		ScanFill(k, p,dr,pos);
	}
	//双缓存技术
	glutSwapBuffers();
	glFlush();

}


/*
鼠标控制部分
点击来记录点
*/

void myMouse(int button, int state, int x, int y)  
{
	static int number = 0;

	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			p[number].x = x;
			p[number].y = 480 - y;

			glBegin(GL_POINTS);
			glVertex2i(x, 480 - y);
			glEnd();
			glFlush();
			cout << p[number].x << "  " << p[number].y << endl;
			number++;

			if (number == k)
			{
				number = 0;
				select = true;
				glutPostRedisplay();
			}
		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			glClear(GL_COLOR_BUFFER_BIT);
			glFlush();
		}
	}
}







int main(int argc, char *argv[])
{
	cout << "请输入点数" << endl;
	cin >> k;
	cout << "请输入扫描线斜率" << endl;
	cin >> dr;
	cout << "请输入扫描线间距" << endl;
	cin >> pos;
	p = (POINT*)malloc(k*sizeof(POINT));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);
	glutCreateWindow("1552286-影线填充实验");
	glutMouseFunc(myMouse);
	glutDisplayFunc(myDisplay);
	myInit();
	glutMainLoop();
	return 0;
}

