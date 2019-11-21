#include "SelectionLabel.h"
#include <QDebug>

SelectionLabel::SelectionLabel(QWidget *parent) :
	QLabel(parent)
{
	setCursor(Qt::CrossCursor); //设置鼠标为十字星
}

void SelectionLabel::mousePressEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		QPoint pt = event->globalPos();//获取全局位置
		pt = mapFromGlobal(pt);

		emit Selected(ceil((this->height() - pt.y()) / 28) * 25 + pt.x() / 28);
	}
	else if(event->buttons() & Qt::RightButton)
	{
		emit UnSelected();
	}
}

SelectionLabel::~SelectionLabel()
{
}
