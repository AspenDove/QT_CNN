#pragma once

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>

class SelectionLabel : public QLabel
{
	Q_OBJECT

public:
	SelectionLabel(QWidget *parent = 0);
	~SelectionLabel();

protected:
	void mousePressEvent(QMouseEvent *event);

signals:
	void Selected(int);
	void UnSelected(void);

public slots :
};
