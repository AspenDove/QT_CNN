#include "ResultDistribute.h"
#include <QPainter>
#include <algorithm>

ResultDistribute::ResultDistribute(QWidget *parent)
	: QWidget(parent)
{
}

ResultDistribute::~ResultDistribute()
{
	result.clear();
}


void ResultDistribute::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	
	int width = this->width(), height = this->height();
	int pointx = width/20, pointy = height*0.9;
	//绘制刻度线
	QPen penDegree;
	penDegree.setColor(Qt::black);
	penDegree.setWidth(2);
	painter.setPen(penDegree);
	//画上x轴刻度线
	for (int i = 0; i!=10; i++)//分成10份
	{
		//选取合适的坐标，绘制一段长度为4的直线，用于表示刻度
		painter.drawLine(pointx+i*width / 10, pointy, pointx+i*width / 10, pointy + 4);
		painter.drawText(pointx+(i-0.1)*width / 10,
			pointy + this->fontMetrics().height(),
			QString::number(i));
	}
	if (result.empty())return;
	QVector<QLine> pts(9);
	double scale = (double)this->height();

	for (int i = 0; i != 9; ++i)
	{
		pts[i] = QLine(pointx+i*this->width() / 10, pointy*(1 - result[i]),
			pointx+(i + 1)*this->width() / 10, pointy*(1 - result[i + 1]));
	}
	painter.drawLines(pts);
}

void ResultDistribute::PushResult(QVector<double> result)
{
	this->result = result;
	update();
}