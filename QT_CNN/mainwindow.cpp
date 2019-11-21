#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	traintask(new TrainTask)
{
	ui->setupUi(this);
	ui->CovPlot->setVisible(false);

	qRegisterMetaType<cv::Mat>("cv::Mat");
	qRegisterMetaType<QVector<double>>("QVector<double>");
	qRegisterMetaType<QVector<cv::Mat>>("QVector<cv::Mat>");

	
	connect(traintask, SIGNAL(SetKernel1(cv::Mat)), this, SLOT(OnKernel1(cv::Mat)));
	connect(traintask, SIGNAL(SetKernel2(cv::Mat)), this, SLOT(OnKernel2(cv::Mat)));
	connect(traintask, SIGNAL(SetTestMat(cv::Mat)), this, SLOT(OnTestMat(cv::Mat)));

	connect(traintask, SIGNAL(SetTestAccuracy(double)), this, SLOT(OnTestAccuracy(double)));
	connect(traintask, SIGNAL(SetTrainAccuracy(double)), this, SLOT(OnTrainAccuracy(double)));
	connect(traintask, SIGNAL(SetCrossEntropy(double)), this, SLOT(OnCrossEntropy(double)));

	connect(traintask, SIGNAL(SetCrossEntropy(double)), ui->EntropyPlot, SLOT(PushValue(double)));

	connect(ui->test_mat, SIGNAL(Selected(int)), traintask, SLOT(OnSelected(int)));
	connect(ui->test_mat, SIGNAL(UnSelected(void)), traintask, SLOT(OnUnSelected(void)));

	connect(traintask, SIGNAL(SetCovPlotVisible(bool)), this, SLOT(OnCovPlotVisible(bool)));
	connect(traintask, SIGNAL(SetC1Result(cv::Mat)), this, SLOT(OnC1Result(cv::Mat)));
	connect(traintask, SIGNAL(SetC2Result(cv::Mat)), this, SLOT(OnC2Result(cv::Mat)));
	connect(traintask, SIGNAL(SetFinalResult(QVector<double>)), ui->ResultPlot, SLOT(PushResult(QVector<double>)));

	connect(this, SIGNAL(SetGLmode(bool)), traintask, SLOT(OnIsGLmode(bool)));
	connect(traintask, SIGNAL(SetInput(cv::Mat)), ui->openGLWidget, SLOT(OnInput(cv::Mat)));
	connect(traintask, SIGNAL(SetLayer1(QVector<cv::Mat>)), ui->openGLWidget, SLOT(OnLayer1(QVector<cv::Mat>)));
	connect(traintask, SIGNAL(SetLayer2(QVector<cv::Mat>)), ui->openGLWidget, SLOT(OnLayer2(QVector<cv::Mat>)));
	connect(traintask, SIGNAL(SetResult(QVector<double>)), ui->openGLWidget, SLOT(OnResult(QVector<double>)));

	traintask->start();
}

MainWindow::~MainWindow()
{
	delete traintask;
	delete ui;
}
void MainWindow::OnKernel1(cv::Mat kernel1)
{
	cvMatLabel(ui->kernel1_mat, kernel1);
}
void MainWindow::OnKernel2(cv::Mat kernel2)
{
	cvMatLabel(ui->kernel2_mat, kernel2);
}
void MainWindow::OnTestMat(cv::Mat testmat)
{
	cvMatLabel(ui->test_mat, testmat);
}
void MainWindow::OnTestAccuracy(double accuracy)
{
	ui->TestAccuracy->setValue(accuracy);
}
void MainWindow::OnTrainAccuracy(double accuracy)
{
	ui->TrainAccuracy->setValue(accuracy);
}
void MainWindow::OnCrossEntropy(double entropy)
{
	ui->CrossEntropy->setText(QString().sprintf("%.6f", entropy));
}

void MainWindow::OnC1Result(cv::Mat c1result)
{
	cvMatLabel(ui->C1Result, c1result);
}
void MainWindow::OnC2Result(cv::Mat c2result)
{
	cvMatLabel(ui->C2Result, c2result);
}
void MainWindow::OnCovPlotVisible(bool bVisible)
{
	ui->CovPlot->setVisible(bVisible);
	if (!bVisible)this->resize(800,600);
}

void MainWindow::cvMatLabel(QLabel* label, const cv::Mat& mat)
{
	cv::cvtColor(mat, mat, CV_BGR2RGB);
	QImage img = QImage((const unsigned char*)(mat.data), mat.cols,
		mat.rows, mat.cols*mat.channels(), QImage::Format_RGB888);
	label->resize(img.size());
	label->setPixmap(QPixmap::fromImage(img));
}
void MainWindow::keyPressEvent(QKeyEvent * event)
{
	static int glmode;
	switch (event->key())
	{
	case Qt::Key_Tab:
		ui->stackedWidget->setCurrentIndex(glmode = !glmode);
		emit SetGLmode(glmode);
		return;
	}
	return QWidget::keyPressEvent(event);
}