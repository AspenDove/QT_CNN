#include "PlotWidget.h"
#include <QPainter>
#include <algorithm>

PlotWidget::PlotWidget(QWidget *parent)
	: QWidget(parent)
{
	values.resize(100);
	for (auto &x : values)x = -1;
}

PlotWidget::~PlotWidget()
{
	values.clear();
}


void PlotWidget::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	drawCurves(painter);  //»æÖÆÇúÏß
}

void PlotWidget::drawCurves(QPainter &painter)
{
	painter.setPen(QPen(Qt::red));
	QVector<QLine> pts(100);
	double scale = (double)this->height() / (double)*std::max_element(values.begin(), values.end());

	for (int i = 1; i != 100; ++i)
	{
		pts[i] = QLine(i*this->width() / 100, this->height() - values[i - 1] * scale,
				 (i + 1)*this->width() / 100, this->height() - values[i] * scale);
	}
	painter.drawLines(pts);
}

void PlotWidget::PushValue(double value)
{
	values.pop_front();
	values.push_back(value);
	update();
}