#include "SelectionLabel.h"
#include <QDebug>

SelectionLabel::SelectionLabel(QWidget *parent) :
	QLabel(parent)
{
	setCursor(Qt::CrossCursor); //�������Ϊʮ����
}

void SelectionLabel::mousePressEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		QPoint pt = event->globalPos();//��ȡȫ��λ��
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
