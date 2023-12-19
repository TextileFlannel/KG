#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

#include "math.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}

void normal(double A[], double B[], double C[], double k)
{
	double BC[] = { A[0] - B[0], A[1] - B[1], A[2] - B[2] };
	double BA[] = { C[0] - B[0], C[1] - B[1], C[2] - B[2] };
	double N[] = { (k * (BA[1] * BC[2]) - (BC[1] * BA[2])), k * ((BA[0] * BC[2] * -1) + (BC[0] * BA[2])), k * ((BA[0] * BC[1]) - (BC[0] * BA[1])) };
	double length = sqrt(pow(N[0], 2) + pow(N[1], 2) + pow(N[2], 2));

	N[0] /= length;
	N[1] /= length;
	N[2] /= length;

	glNormal3d(N[0], N[1], N[2]);
}

void normal_walls(double A[], double B[], double C[])
{
	double BC[] = { A[0] - B[0], A[1] - B[1], A[2] - B[2] };
	double BA[] = { C[0] - B[0], C[1] - B[1], C[2] - B[2] };
	double N[] = { -1 * ((BA[1] * BC[2]) - (BC[1] * BA[2])), ((BA[0] * BC[2] * -1) + (BC[0] * BA[2])), ((BA[0] * BC[1]) - (BC[0] * BA[1])) };
	double length = sqrt(pow(N[0], 2) + pow(N[1], 2) + pow(N[2], 2));

	N[0] /= length;
	N[1] /= length;
	N[2] /= length;

	glNormal3d(N[0], N[1], N[2]);
}


void figure() {
#pragma region down
	double A1[] = { 3, 7, 0 };
	double B1[] = { 3, 13, 0 };
	double C1[] = { 9, 10, 0 };
	double D1[] = { 8, 7, 0 };

	double A2[] = { 8, 7, 0 };
	double B2[] = { 9, 10, 0 };
	double C2[] = { 10, 8, 0 };
	double D2[] = { 10, 3, 0 };

	double A3[] = { 9, 10, 0 };
	double B3[] = { 14, 15, 0 };
	double C3[] = { 18, 11, 0 };
	double D3[] = { 10, 8, 0 };

	glBegin(GL_QUADS);
	glColor3d(0.2, 0.7, 0.7);
	glVertex3dv(A1);
	glVertex3dv(B1);
	glVertex3dv(C1);
	glVertex3dv(D1);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0.1, 0.3, 0.9);
	glVertex3dv(A2);
	glVertex3dv(B2);
	glVertex3dv(C2);
	glVertex3dv(D2);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0.5, 0.1, 0.2);
	glVertex3dv(A3);
	glVertex3dv(B3);
	glVertex3dv(C3);
	glVertex3dv(D3);
	glEnd();
#pragma endregion

#pragma region up
	double A4[] = { 3, 7, 3 };
	double B4[] = { 3, 13, 3 };
	double C4[] = { 9, 10, 3 };
	double D4[] = { 8, 7, 3 };

	double A5[] = { 8, 7, 3 };
	double B5[] = { 9, 10, 3 };
	double C5[] = { 10, 8, 3 };
	double D5[] = { 10, 3, 3 };

	double A6[] = { 9, 10, 3 };
	double B6[] = { 14, 15, 3 };
	double C6[] = { 18, 11, 3 };
	double D6[] = { 10, 8, 3 };

	glBegin(GL_QUADS);
	glColor3d(0.2, 0.7, 0.7);
	glVertex3dv(A4);
	glVertex3dv(B4);
	glVertex3dv(C4);
	glVertex3dv(D4);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0.1, 0.3, 0.9);
	glVertex3dv(A5);
	glVertex3dv(B5);
	glVertex3dv(C5);
	glVertex3dv(D5);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0.5, 0.1, 0.2);
	glVertex3dv(A6);
	glVertex3dv(B6);
	glVertex3dv(C6);
	glVertex3dv(D6);
	glEnd();
#pragma endregion

#pragma region wall
	double A7[] = { 3, 7, 0 };
	double B7[] = { 3, 7, 3 };
	double C7[] = { 3, 13, 3 };
	double D7[] = { 3, 13, 0 };

	glBegin(GL_QUADS);
	glColor3d(0.2, 0.7, 0.7);
	glVertex3dv(A7);
	glVertex3dv(B7);
	glVertex3dv(C7);
	glVertex3dv(D7);
	glEnd();

	double A8[] = { 9, 10, 0 };
	double B8[] = { 9, 10, 3 };
	double C8[] = { 3, 13, 3 };
	double D8[] = { 3, 13, 0 };

	glBegin(GL_QUADS);
	glColor3d(0.2, 0.7, 0.7);
	glVertex3dv(A8);
	glVertex3dv(B8);
	glVertex3dv(C8);
	glVertex3dv(D8);
	glEnd();

	double A9[] = { 8, 7, 0 };
	double B9[] = { 8, 7, 3 };
	double C9[] = { 3, 7, 3 };
	double D9[] = { 3, 7, 0 };

	glBegin(GL_QUADS);
	glColor3d(0.2, 0.7, 0.7);
	glVertex3dv(A9);
	glVertex3dv(B9);
	glVertex3dv(C9);
	glVertex3dv(D9);
	glEnd();

	double A10[] = { 8, 7, 0 };
	double B10[] = { 8, 7, 3 };
	double C10[] = { 10, 3, 3 };
	double D10[] = { 10, 3, 0 };

	glBegin(GL_QUADS);
	glColor3d(0.1, 0.3, 0.9);
	glVertex3dv(A10);
	glVertex3dv(B10);
	glVertex3dv(C10);
	glVertex3dv(D10);
	glEnd();

	double A11[] = { 8, 7, 0 };
	double B11[] = { 8, 7, 3 };
	double C11[] = { 10, 8, 3 };
	double D11[] = { 10, 8, 0 };

	glBegin(GL_QUADS);
	glColor3d(0.1, 0.3, 0.9);
	glVertex3dv(A11);
	glVertex3dv(B11);
	glVertex3dv(C11);
	glVertex3dv(D11);
	glEnd();

	double A12[] = { 14, 15, 0 };
	double B12[] = { 14, 15, 3 };
	double C12[] = { 18, 11, 3 };
	double D12[] = { 18, 11, 0 };

	glBegin(GL_QUADS);
	glColor3d(0.5, 0.1, 0.2);
	glVertex3dv(A12);
	glVertex3dv(B12);
	glVertex3dv(C12);
	glVertex3dv(D12);
	glEnd();

	double A13[] = { 10, 8, 0 };
	double B13[] = { 10, 8, 3 };
	double C13[] = { 18, 11, 3 };
	double D13[] = { 18, 11, 0 };

	glBegin(GL_QUADS);
	glColor3d(0.5, 0.1, 0.2);
	glVertex3dv(A13);
	glVertex3dv(B13);
	glVertex3dv(C13);
	glVertex3dv(D13);
	glEnd();

	double A14[] = { 14, 15, 0 };
	double B14[] = { 14, 15, 3 };
	double C14[] = { 9, 10, 3 };
	double D14[] = { 9, 10, 0 };

	glBegin(GL_QUADS);
	glColor3d(0.5, 0.1, 0.2);
	glVertex3dv(A14);
	glVertex3dv(B14);
	glVertex3dv(C14);
	glVertex3dv(D14);
	glEnd();
#pragma endregion

#pragma region MyRegion
	glColor3d(0, 1, 0);
	for (int i = -45; i < 133; i += 2)
	{
		double x = 16 + 2.80843 * cos(i * (3.14 / 180.0));
		double y = 13 + 2.80843 * sin(i * (3.14 / 180.0));
		double x1 = 16 + 2.80843 * cos((i + 5) * (3.14 / 180.0));
		double y1 = 13 + 2.80843 * sin((i + 5) * (3.14 / 180.0));
		glBegin(GL_TRIANGLES);
		glVertex3d(16, 13, 0);
		glVertex3d(x, y, 0);
		glVertex3d(x1, y1, 0);
		glEnd();

		glBegin(GL_TRIANGLES);
		glVertex3d(16, 13, 3);
		glVertex3d(x, y, 3);
		glVertex3d(x1, y1, 3);
		glEnd();

		double A[] = { x, y, 0 };
		double B[] = { x1, y1, 0 };
		double C[] = { x1, y1, 3 };
		double D[] = { x, y, 3 };
		glBegin(GL_QUADS);
		glVertex3dv(A);
		glVertex3dv(B);
		glVertex3dv(C);
		glVertex3dv(D);
		glEnd();
	}
#pragma endregion
}



void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);

	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  
	glColor3d(0.5, 0.5, 0.5);
	figure();
	
   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}