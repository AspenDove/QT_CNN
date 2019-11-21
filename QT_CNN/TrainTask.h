#pragma once
#include "matrix.h"
#include "Tensor.h"
#include "mnist.h"
#include "matlog.h"
#include "mainwindow.h"

#include <iostream>
#include <ostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <random>
#include <iomanip>
#include <tbb/tbb.h>
#include <cilk/cilk.h>
#include <tbb/concurrent_vector.h>
#include <tbb/scalable_allocator.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <crtdbg.h>
#include <QThread>  
#include <QVector>

using namespace std;

#define BATCH_SIZE 120
#define SIZE_OF_DISPLAY 500
extern bool isCorrect[SIZE_OF_DISPLAY];

class TrainTask : public QThread
{
	Q_OBJECT
public:
	explicit TrainTask(QObject *parent = 0);

protected:
	void run();

signals:
	void SetKernel1(cv::Mat kernel1);
	void SetKernel2(cv::Mat kernel2);
	void SetTestMat(cv::Mat testmat);

	void SetTestAccuracy(double);
	void SetTrainAccuracy(double);
	void SetCrossEntropy(double);
	
	void SetCovPlotVisible(bool);
	void SetC1Result(cv::Mat kernel1);
	void SetC2Result(cv::Mat kernel1);
	void SetFinalResult(QVector<double> result);

	void SetInput(cv::Mat);
	void SetLayer1(QVector<cv::Mat>);
	void SetLayer2(QVector<cv::Mat>);
	void SetResult(QVector<double>);

public slots :
	void OnSelected(int);
	void OnUnSelected(void);
	void OnIsGLmode(bool);
private:

	double test(Tensor<double>& X, const Matrix<double>& Y,
		Tensor<double>& kernels1, Matrix<double>& B1,
		Tensor<double>& kernels2, Matrix<double>& B2,
		std::vector<Matrix<double>>& W, std::vector<Matrix<double>>& B,bool bAccu = false);

	bool bSelected, bGLmode = false;
	int iPosition;
};
