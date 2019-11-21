#pragma once

#include <QOpenGLWidget>
#include <QWheelEvent>
#include <QVector>
#include "quaternion.h"
#include "mnist.h"
#include "matrix.h"
#include "tensor.h"

class GLWidget : public QOpenGLWidget
{
	Q_OBJECT

public:
	explicit GLWidget(QWidget *parent = 0);

signals:

public slots :
	void OnInput(cv::Mat);
	void OnLayer1(QVector<cv::Mat>);
	void OnLayer2(QVector<cv::Mat>);
	void OnResult(QVector<double>);

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int w, int h);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
	void drawCube(float length);

	private slots:

private:
	bool m_bDrag = false;
	bool m_bReady = false;
	quaternion m_qNow, m_qDown;
	struct vec3 { float a, b, c; };
	struct mat4 { float mat[16]; };
	vec3 m_vDownPt, m_vCurrentPt;
	Tensor<double> *x;
	QPoint lastPos;

	double scale = 1;
	cv::Mat input,result;
	QVector<cv::Mat> layer1, layer2;

	inline float DotProduct3(const vec3 u, const vec3 v)
	{
		return u.a * v.a + u.b * v.b + u.c * v.c;
	}
	inline vec3 CrossProduct(const vec3 u, const vec3 v)
	{
		return{ u.b * v.c - v.b * u.c,
			-u.a * v.c + v.a * u.c,
			u.a * v.b - v.a * u.b };
	}
	vec3 ScreenToVector(float fScreenPtX, float fScreenPtY, int nWidth, int nHeight)
	{
		//SCALE TO SCREEN
		FLOAT x = (fScreenPtX - nWidth / 2) / (nWidth / 2);
		FLOAT y = -(fScreenPtY - nHeight / 2) / (nHeight / 2);

		FLOAT z = 0.0f;
		FLOAT mag = x*x + y*y;
		if (mag > 1.0f)
		{
			FLOAT scale = 1.0f / sqrt(mag);
			x *= scale;
			y *= scale;
		}
		else
			z = sqrt(1.0f - mag);

		//return vector
		return vec3{ x, y, -z };
	}
	quaternion QuatFromBallPoints(const vec3& vFrom, const vec3& vTo)
	{
		vec3 vPart;
		float fDot = DotProduct3(vFrom, vTo);
		vPart = CrossProduct(vFrom, vTo);

		return quaternion(fDot, vPart.a, vPart.b, vPart.c);
	}
	mat4 getMatrix(quaternion q)
	{
		float x2 = q.b * q.b;
		float y2 = q.c * q.c;
		float z2 = q.d * q.d;
		float xy = q.b * q.c;
		float xz = q.b * q.d;
		float yz = q.c * q.d;
		float wx = q.a * q.b;
		float wy = q.a * q.c;
		float wz = q.a * q.d;

		// This calculation would be a lot more complicated for non-unit length quaternions
		// Note: The constructor of Matrix4 expects the Matrix in column-major format like expected by
		//   OpenGL
		return mat4{ 1.0f - 2.0f * (y2 + z2), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f,
			2.0f * (xy + wz), 1.0f - 2.0f * (x2 + z2), 2.0f * (yz - wx), 0.0f,
			2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (x2 + y2), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f };
	}
	void DrawArray(const cv::Mat& mat, float length = 0.1f, float gap = 0.1f / 3);
	void DrawLayer(const QVector<cv::Mat>& mat, float length, float gap);
};