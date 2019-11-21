#include "mainwindow.h"
#include "traintask.h"
#include <QApplication>
#include <tbb/tbb.h>

int main(int argc, char *argv[])
{
	tbb::task_scheduler_init tbbinit(tbb::task_scheduler_init::automatic);

	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
