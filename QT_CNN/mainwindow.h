#pragma once

#include "Traintask.h"
#include <QMainWindow>
#include <QFileDialog>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <QLabel>
#include <QThread>
#include <QKeyEvent>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void OnKernel1(cv::Mat kernel1);
	void OnKernel2(cv::Mat kernel2);
	void OnTestMat(cv::Mat testmat);

	void OnTestAccuracy(double);
	void OnTrainAccuracy(double);
	void OnCrossEntropy(double);

	void OnCovPlotVisible(bool);
	void OnC1Result(cv::Mat);
	void OnC2Result(cv::Mat);

signals:
	void SetGLmode(bool);
private:
//void setparam();
	//bool save();
	//void open();
	//void about();

private:
	void cvMatLabel(QLabel* label,const cv::Mat& mat);
	void keyPressEvent(QKeyEvent *event);

	QThread* traintask;
	Ui::MainWindow *ui;
};
