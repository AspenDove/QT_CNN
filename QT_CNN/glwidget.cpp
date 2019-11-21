#include <QtGui>
#include <QOpenGLWidget>

#include <math.h>
//#include <GL/freeglut.h>
#include "glwidget.h"
#include <initializer_list>

GLWidget::GLWidget(QWidget *parent) :
	QOpenGLWidget(parent)
{
	//testdata.next_batch(500);
	//std::vector<Matrix<double>> x_testdata;
	//for (size_t i = 0; i != 500; ++i)
	//	x_testdata.push_back(cv::Mat(cv::Size(28, 28), CV_8UC1, testdata.Image.ptr<uchar[28 * 28]>(i)).clone());
	//static Tensor<double> X_test(x_testdata);
	//x = &X_test;
	//Matrix<double> Y_test(testdata.Lable, true, 10);
	//
	//input = Tensor<double>((*x)[index]).MergecvMat(1, 1, 1, 1);
	//for (int i = 0; i != 6; ++i)
	//{
	//	layer1.push_back(Tensor<double>((*x)[index + i + 100]).MergecvMat(1, 1, 1, 1));
	//}

	//for (int i = 0; i != 12; ++i)
	//{
	//	layer2.push_back(Tensor<double>((*x)[index + i]).MergecvMat(1, 1, 1, 1));
	//}
	//result.create(10, 1, CV_8UC3);
	//for (int i = 0; i != 10; ++i)
	//{
	//	result.at<cv::Vec3b>(i, 0) = { (uchar)255,0,0 };
	//}
}

void GLWidget::OnLayer1(QVector<cv::Mat> layer1)
{
	this->layer1 = layer1;
	update();
}

void GLWidget::OnLayer2(QVector<cv::Mat> layer2)
{
	this->layer2 = layer2;
	update();
}

void GLWidget::OnResult(QVector<double> res)
{
	//this->result = result;
	result.create(10, 1, CV_8UC3);
	for (int i = 0; i != 10; ++i)
	{
		result.at<cv::Vec3b>(i, 0) = { (uchar)(res[i] * 255),0,0 };
	}
	update();
	m_bReady = true;
}

void GLWidget::OnInput(cv::Mat input)
{
	this->input = input;
	update();
}

void GLWidget::initializeGL()
{
	GLfloat mat_specular[] = { 1.0, 0.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	//glutInit(nullptr, nullptr);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_POLYGON_SMOOTH, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);  // Antialias the lines

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);//GL_POSITION

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}

void GLWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float scale = 10;
	if (w <= h)
		glOrtho(-scale, scale, -scale * (GLfloat)h / (GLfloat)w,
			scale * (GLfloat)h / (GLfloat)w, -100.0, 100.0);
	else
		glOrtho(-scale * (GLfloat)w / (GLfloat)h,
			scale * (GLfloat)w / (GLfloat)h, -scale, scale, -100.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	//lastPos = event->pos();

	if (event->buttons() & Qt::LeftButton)
	{
		m_bDrag = true;
		m_qDown = m_qNow;
		m_vDownPt = ScreenToVector(event->x(), event->y(), width(), height());
	}

}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	//int dx = event->x() - lastPos.x();
	//int dy = event->y() - lastPos.y();

	if (event->buttons() & Qt::LeftButton)
	{
		m_vCurrentPt = ScreenToVector(event->x(), event->y(), width(), height());
		m_qNow = m_qDown * QuatFromBallPoints(m_vDownPt, m_vCurrentPt);
		update();
	}
	else if (event->buttons() & Qt::RightButton)
	{

	}

	//lastPos = event->pos();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
	m_bDrag = false;
	return QWidget::mouseReleaseEvent(event);
}

void GLWidget::wheelEvent(QWheelEvent*event) {

	if (event->delta() > 0) 
	{
		scale += 0.5;
	}
	else scale -= 0.5;

	update();
}
void GLWidget::drawCube(float length)
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//画立方体的6个面  
	glPushMatrix();
//	glScalef(length, length, length);

	glBegin(GL_QUADS);
	glNormal3f(0.0F, 0.0F, 1.0F);
	glVertex3f(length/2.f, length/2.f, length/2.f);
	glVertex3f(-length/2.f, length/2.f, length/2.f);
	glVertex3f(-length/2.f, -length/2.f, length/2.f);
	glVertex3f(length/2.f, -length/2.f, length/2.f);
	//1----------------------------  
	glNormal3f(0.0F, 0.0F, -1.0F);
	glVertex3f(-length/2.f, -length/2.f, -length/2.f);
	glVertex3f(-length/2.f, length/2.f, -length/2.f);
	glVertex3f(length/2.f, length/2.f, -length/2.f);
	glVertex3f(length/2.f, -length/2.f, -length/2.f);
	//2----------------------------  
	glNormal3f(0.0F, 1.0F, 0.0F);
	glVertex3f(length/2.f, length/2.f, length/2.f);
	glVertex3f(length/2.f, length/2.f, -length/2.f);
	glVertex3f(-length/2.f, length/2.f, -length/2.f);
	glVertex3f(-length/2.f, length/2.f, length/2.f);
	//3----------------------------  
	glNormal3f(0.0F, -1.0F, 0.0F);
	glVertex3f(-length/2.f, -length/2.f, -length/2.f);
	glVertex3f(length/2.f, -length/2.f, -length/2.f);
	glVertex3f(length/2.f, -length/2.f, length/2.f);
	glVertex3f(-length/2.f, -length/2.f, length/2.f);
	//4----------------------------  
	glNormal3f(1.0F, 0.0F, 0.0F);
	glVertex3f(length/2.f, length/2.f, length/2.f);
	glVertex3f(length/2.f, -length/2.f, length/2.f);
	glVertex3f(length/2.f, -length/2.f, -length/2.f);
	glVertex3f(length/2.f, length/2.f, -length/2.f);
	//5----------------------------  
	glNormal3f(-1.0F, 0.0F, 0.0F);
	glVertex3f(-length/2.f, -length/2.f, -length/2.f);
	glVertex3f(-length/2.f, -length/2.f, length/2.f);
	glVertex3f(-length/2.f, length/2.f, length/2.f);
	glVertex3f(-length/2.f, length/2.f, -length/2.f);
	//6----------------------------*/  
	glEnd();
	glPopMatrix();

}
void GLWidget::paintGL()
{
	if (!m_bReady)return;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	int w = width(), h = height();
	if (w <= h) glOrtho(-scale, scale, -scale * (GLfloat)h / (GLfloat)w,scale * (GLfloat)h / (GLfloat)w, -100.0, 100.0);
	else glOrtho(-scale * (GLfloat)w / (GLfloat)h,scale * (GLfloat)w / (GLfloat)h, -scale, scale, -100.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	{
		float length = 0.1f;
		float gap = length / 3.f;

		float arraylength = 28 * length + 27 * gap;
		float arraygap = arraylength / 3.f;

		float layergap = arraylength*3;
		
		glLoadMatrixf(getMatrix(m_qNow).mat);

		glPushMatrix();
		{
			glTranslatef(0, 0, -layergap*1.5);
			DrawArray(input, 0.1f, 0.1f / 3);
		}
		glPopMatrix();

		glPushMatrix();
		{
			glTranslatef(0, 0, -layergap*0.5);
			DrawLayer(layer1, 0.1f, 0.1f / 3);
		}
		glPopMatrix();

		glPushMatrix();
		{
			glTranslatef(0, 0, layergap*0.5);
			DrawLayer(layer2, 0.1f, 0.1f / 3);
		}
		glPopMatrix();

		glPushMatrix();
		{
			glTranslatef(0, 0, layergap * 1.5);
			DrawArray(result);
		}
		glPopMatrix();
	}
	glPopMatrix();
	glFlush();
}

void GLWidget::DrawArray(const cv::Mat& mat, float length, float gap)
{
	//float length = 0.1f;
	//float gap = length / 3.f;
	int cx = mat.cols, cy = mat.rows;
	glTranslatef(-(cx - 1)*(length + gap) / 2, -(cy - 1)*(length + gap) / 2, 0);
	for (int i = 0; i != cy; ++i)
	{
		glPushMatrix();
		{
			for (int j = 0; j != cx; ++j)
			{
				cv::Vec3b color = mat.at<cv::Vec3b>(cy - i - 1, j);
				float alpha = 0;
				if (color[0] | color[1] | color[2])alpha = 1.f;
				GLfloat clr[4] = { color[2] / 255.f, color[1] / 255.f, color[0] / 255.f , alpha };
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, clr);
				drawCube(length);
				//glutSolidCube(length);
				glTranslatef(length + gap, 0, 0);
			}
		}
		glPopMatrix();
		glTranslatef(0, length + gap, 0);
	}
}

void GLWidget::DrawLayer(const QVector<cv::Mat>& mat, float length, float gap)
{
	//float length = 0.1f;
	//float gap = length / 3.f;
	float arraylength = mat[0].cols * length + (mat[0].cols - 1) * gap;
	float arraygap = arraylength / 5.f;
	int cx = mat.size();

	glTranslatef(-(cx - 1)*(arraylength + arraygap) / 2, 0, 0);

	for (int i = 0; i != cx; ++i)
	{
		glPushMatrix();
			DrawArray(mat[i]);
		glPopMatrix();
		glTranslatef(arraylength + arraygap, 0, 0);
	}
}


