#pragma once

#include <QWidget>
#include <QVector>

class ResultDistribute : public QWidget
{
	Q_OBJECT

public:
	ResultDistribute(QWidget *parent = 0);
	~ResultDistribute();
	void drawCurves(QPainter &painter);
	void paintEvent(QPaintEvent *);

	QVector<double> result;
signals:

public slots:
		void PushResult(QVector<double>);
};
