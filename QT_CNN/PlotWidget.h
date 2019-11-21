#pragma once

#include <QWidget>
#include <QVector>

class PlotWidget : public QWidget
{
	Q_OBJECT

public:
	PlotWidget(QWidget *parent = 0);
	~PlotWidget();
	void drawCurves(QPainter &painter);
	void paintEvent(QPaintEvent *);

	QVector<double> values;

signals:

public slots :
	void PushValue(double);
};
