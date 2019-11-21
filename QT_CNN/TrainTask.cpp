#include "Traintask.h"
bool isCorrect[SIZE_OF_DISPLAY];

double lrwarmstart(const int iepoch, const double& originlr)
{
	const int period = 20;
	return originlr * cos(3.1415926*(double)(iepoch%period) / period / 2.0);
}

Matrix<double> Softmax(const Matrix<double>& X)
{
	return std::move(X.each(expl) / X.each(expl).accumulate(1));
}

Matrix<double> forward_propagation(const Matrix<double>& X,
	const std::vector<Matrix<double>>& W, const std::vector<Matrix<double>>& B)
{
	size_t nlayer = W.size();
	if (nlayer == 0)throw std::invalid_argument("权重矩阵为空.");
	else if (nlayer == 1)
		return std::move(Softmax((X * W[0] + B[0])));
	else
	{
		Matrix<double> *L = new Matrix<double>[nlayer - 1];
		L[0] = (X*W[0] + B[0]).each(activation);
		for (size_t i = 1; i < nlayer - 1; ++i)
			L[i] = (L[i - 1] * W[i] + B[i]).each(activation);
		Matrix<double> L_last = L[nlayer - 2];
		delete[] L;
		return std::move(Softmax((L_last * W[nlayer - 1] + B[nlayer - 1])));
	}
}

double TrainTask::test(Tensor<double>& X, const Matrix<double>& Y,
	Tensor<double>& kernels1, Matrix<double>& B1,
	Tensor<double>& kernels2, Matrix<double>& B2,
	std::vector<Matrix<double>>& W, std::vector<Matrix<double>>& B,bool bAccu)
{
	Tensor<double> OUT1 = X.convallchannels(kernels1, B1).lrn().each([](Matrix<double>& mat) {return mat.each(activation); });
	Tensor<double> POOL1 = OUT1.each([](Matrix<double>& mat) {return std::move(mat.pooling(2, average)); });

	Tensor<double> OUT2 = POOL1.convallchannels(kernels2, B2).lrn().each([](Matrix<double>& mat) {return mat.each(activation); });
	Tensor<double> POOL2 = OUT2.each([](Matrix<double>& mat) {return std::move(mat.pooling(2, average)); });
	Matrix<double> result = forward_propagation(POOL2.expand(), W, B);

	double correct = 0;

	for (size_t inum = 0; inum != X.getrow(); ++inum)
	{
		size_t maxindex = 0, labindex = 0;
		double maxoutput = result[inum][0];
		for (size_t i = 0; i != 10; ++i)
		{
			double icurrent = result[inum][i];
			if (icurrent > maxoutput)
			{
				maxindex = i;
				maxoutput = icurrent;
			}
			if (Y[inum][i] == 1.0)
				labindex = i;
		}
		if (maxindex == labindex)
		{
			correct += 1.;
			if(bAccu)isCorrect[inum] = 1;
		}
		else
		{
			if (bAccu)isCorrect[inum] = 0;
		}
	}
	return correct / X.getrow();
}

double crossentropy(const Matrix<double>& prediction, const Matrix<double>& Y)
{
	double entropy = 0.0;
	for (size_t inum = 0; inum != Y.getrow(); ++inum)
		for (size_t i = 0; i != 10; ++i)
			if (Y[inum][i] == 1.0)entropy += -log(prediction[inum][i]);
	return entropy;
}

TrainTask::TrainTask(QObject *parent) :
	QThread(parent), bSelected(false), bGLmode(false), iPosition(0)
{}

void TrainTask::OnSelected(int iPosition)
{
	bSelected = true;
	this->iPosition = iPosition;
	emit SetCovPlotVisible(true);
}
void TrainTask::OnIsGLmode(bool bglmode)
{
	this->bGLmode = bglmode;
}
void TrainTask::OnUnSelected(void)
{
	bSelected = false;
	emit SetCovPlotVisible(false);
}

void TrainTask::run() {
	try {
		tbb::tick_count t0, t1;
		t0 = tbb::tick_count::now();
		matlog mats("mat.dat");
		mnist data(std::string("mnist/train-images.idx3-ubyte"), std::string("mnist/train-labels.idx1-ubyte"));
		mnist testdata(std::string("mnist/t10k-images.idx3-ubyte"), std::string("mnist/t10k-labels.idx1-ubyte"));
		
		testdata.next_batch(500);
		testdata.next_batch(500);
		//Matrix<double> X_test(testdata.Image);
		//Matrix<double> Y_test(testdata.Label, true, 10);
		//data.shuffle_batch(BATCH_SIZE);
		//std::vector<Matrix<double>> x_data;
		//for (size_t i = 0; i != BATCH_SIZE; ++i)
		//	x_data.push_back(cv::Mat(cv::Size(28, 28), CV_8UC1, data.Image.ptr<uchar[28 * 28]>(i)).clone());
		//Tensor<double> X(x_data);
		//Matrix<double> Y(data.Label, true, 10);

		std::vector<Matrix<double>> x_testdata;
		for (size_t i = 0; i != 500; ++i)
			x_testdata.push_back(cv::Mat(cv::Size(28, 28), CV_8UC1, testdata.Image.ptr<uchar[28 * 28]>(i)).clone());
		Tensor<double> X_test(x_testdata);
		Matrix<double> Y_test(testdata.Label, true, 10);

		std::default_random_engine engine(time(nullptr));

		Tensor<double> kernels1(1, 6);
		kernels1.eachself([&](Matrix<double>& m) {return Matrix<double>(5, 5, engine, -0., .06); });
		Matrix<double> B1(1, 6, engine, 0, .001);

		Tensor<double> kernels2(6, 12);
		kernels2.eachself([&](Matrix<double>& m) {return Matrix<double>(3, 3, engine, -0., .06); });
		Matrix<double> B2(1, 12, engine, 0, .001);

		std::vector<Matrix<double>> W = {
			Matrix<double>(300, 500, engine, -.3, .3),
			Matrix<double>(500, 10, engine, -.3, .3)
		};
		std::vector<Matrix<double>> B = {
			Matrix<double>(1 ,500, engine, 0, .01),
			Matrix<double>(1, 10, engine, 0, .01)
		};
		std::vector<Matrix<double>> mimpluse = W;

		
		for(int iepoch = 0;; iepoch++)
		{			
			data.shuffle_batch(BATCH_SIZE);
			std::vector<Matrix<double>> x_data;
			for (size_t i = 0; i != BATCH_SIZE; ++i)
				x_data.push_back(cv::Mat(cv::Size(28, 28), CV_8UC1, data.Image.ptr<uchar[28 * 28]>(i)).clone());
			Tensor<double> X(x_data);
			Matrix<double> Y(data.Label, true, 10);

			Tensor<double> OUT1 = X.convallchannels(kernels1, B1).lrn(4).each([](Matrix<double>& mat) {return mat.each(activation); });
			Tensor<double> POOL1 = OUT1.each([](Matrix<double>& mat) {return std::move(mat.pooling(2, average)); });

			Tensor<double> OUT2 = POOL1.convallchannels(kernels2, B2).lrn(4).each([](Matrix<double>& mat) {return mat.each(activation); });
			Tensor<double> POOL2 = OUT2.each([](Matrix<double>& mat) {return std::move(mat.pooling(2, average)); });

			Matrix<double> FC = POOL2.expand();
			size_t ntrain = FC.getrow();
			size_t nlayer = W.size();
			////////////////////////////////forward///////////////////////////////////
			Matrix<double> *L = new Matrix<double>[nlayer];
			L[0] = (FC.dropout(W[0], engine, 1) + B[0]).each(activation);
			if (nlayer == 1)
				*L = Softmax((FC * W[0] + B[0]));
			else
			{
				for (size_t i = 1; i < nlayer - 1; ++i)
				{
					L[i] = (L[i - 1].dropout(W[i], engine, 1) + B[i]).each(activation);
				}
				L[nlayer - 1] = Softmax((L[nlayer - 2].dropout(W[nlayer - 1], engine, 1) + B[nlayer - 1]));
			}
			emit SetTrainAccuracy(test(X, Y, kernels1, B1, kernels2, B2, W, B) * 100);
			emit SetTestMat(testdata.MergecvMat(isCorrect, 20, 25));

			emit SetTestAccuracy(test(X_test, Y_test, kernels1, B1, kernels2, B2, W, B, true) * 100);
			emit SetCrossEntropy(crossentropy(L[nlayer - 1], Y));//(Y - L[nlayer - 1]).each(fabsl).mean()
			////////////////////////////////forward///////////////////////////////////
			if (bSelected)
			{
				QVector<cv::Mat> mats;
				if (bGLmode)
				{
					//Tensor<double>(X_test[iPosition]).MergecvMat(1, 1, 1, 1);
					emit SetInput(Tensor<double>(X_test[iPosition]).MergecvMat(1, 1, 1, 1));
					//result.push_back();
				}
				Tensor<double> out1 = Tensor<double>(X_test[iPosition]).convallchannels(kernels1, B1);
				if (bGLmode)
				{
					mats.clear();
					for (int i = 0; i != 6; ++i)
						mats.push_back(Tensor<double>(out1[i]).MergecvMat(1, 1, 1, 1));
					emit SetLayer1(mats);
				}
				emit SetC1Result(out1.MergecvMat(2, 3, 2, 2));

				Tensor<double> pool1 = out1.lrn().each([](Matrix<double>& mat) {return mat.each(activation); })
					.each([](Matrix<double>& mat) {return std::move(mat.pooling(2, average)); });
				Tensor<double> out2 = pool1.convallchannels(kernels2, B2);
				if (bGLmode)
				{
					mats.clear();
					for (int i = 0; i != 12; ++i)
						mats.push_back(Tensor<double>(out2[i]).MergecvMat(1, 1, 1, 1));
					emit SetLayer2(mats);
				}
				emit SetC2Result(out2.MergecvMat(3, 4, 4, 4));

				Tensor<double> pool2 = out2.lrn().each([](Matrix<double>& mat) {return mat.each(activation); })
					.each([](Matrix<double>& mat) {return std::move(mat.pooling(2, average)); });
				Matrix<double> result = forward_propagation(pool2.expand(), W, B);

				QVector<double> res(10);
				for (size_t i = 0; i != 10; ++i)res[i] = result[i];
				if (bGLmode)emit SetResult(res);
				emit SetFinalResult(res);
			}
			////////////////////////////backpropagation///////////////////////////////

			double lr = lrwarmstart(iepoch, .00005);

			double impluse = 0.3;

			Matrix<double> *L_delta = new Matrix<double>[nlayer];
			L_delta[nlayer - 1] = (L[nlayer - 1] - Y);//.dot(L[nlayer - 1].each(dactivation));//tr here

			for (int i = nlayer - 2; i >= 0; --i)L_delta[i] = (L_delta[i + 1] * W[i + 1].Tr()).dot(L[i].each(dactivation));

			double regularization = 500;

			W[0] -= mimpluse[0] = (FC.Tr()*(L_delta[0])*lr*(1 - impluse) + mimpluse[0] * impluse);// + W[0] * (lr*regularization / ntrain));
			B[0] -= L_delta[0].accumulate()*lr;// *(lr / L_delta[0].getrow());
			for (size_t i = 1; i != nlayer; ++i)
			{
				W[i] -= mimpluse[i] = (L[i - 1].Tr()*(L_delta[i])*lr*(1 - impluse) + mimpluse[i] * impluse);// +W[i] * (lr*regularization / ntrain));
				B[i] -= L_delta[i].accumulate()*lr;// *(lr / L_delta[i].getrow()); 
			}
			auto FCdelta = (L_delta[0] * W[0].Tr());
			delete[] L;
			delete[] L_delta;
			//n*36*10*10
			Tensor<double> C2delta = FCdelta.fold(5, 5)
				.each([](Matrix<double>& mat) {return std::move(mat.uppooling(2, average)); });
			C2delta = C2delta.dot(OUT2.each([&](Matrix<double>& mat) {return mat.each(dactivation); }));
			//n*16*24*24
			Tensor<double> C1delta = C2delta.backconv(kernels2.Tr())
				.each([](Matrix<double>& mat) {return std::move(mat.uppooling(2, average)); });
			C1delta = C1delta.dot(OUT1.each([&](Matrix<double>& mat) {return mat.each(dactivation); }));

			lr = lrwarmstart(iepoch, 0.0001);

			Tensor<double> dw1 = X.Tr().convallchannels(C1delta);
			kernels1 -= dw1.each([&](Matrix<double>& mat) {return mat*lr; });
			emit SetKernel1(kernels1.MergecvMat(1, 6, 10, 10));
			B1 -= C1delta.each([&](Matrix<double>& mat) {return std::move(mat.sum()); }).accumulate()*lr;

			lr = lrwarmstart(iepoch, 0.0001);

			Tensor<double> dw2 = POOL1.Tr().convallchannels(C2delta);
			kernels2 -= dw2.each([&](Matrix<double>& mat) {return mat*lr; });
			emit SetKernel2(kernels2.MergecvMat(6, 12, 10, 10));
			B2 -= C2delta.each([&](Matrix<double>& mat) {return std::move(mat.sum()); }).accumulate()*lr;
			////////////////////////////backpropagation///////////////////////////////

		}
		//for (size_t i = 0; i != 20000; ++i)
		//{
		//	Matrix<double> X(data.Image);
		//	Matrix<double> Y(data.Label, true, 10);
		//	int p = 10;//
		//	update(X, Y, W, B, mimpluse, lr*cos(3.1415926*(double)(i%p) / p / 2.0),
		//		impluse*cos(3.1415926*(double)(i%p) / p / 2.0), 100);//更新权值
		//	if (i % 1 == 0)
		//	{
		//		result = forward_propagation(X, W, B);
		//		std::cout << "Error:   " << (Y - result).each(fabsl).mean() << '\n';
		//		curaccuracy = test(X_test, Y_test, W, B) * 100;
		//		std::cout << "accuracy:" << curaccuracy << "%";// << std::endl;
		//		if (curaccuracy > maxaccuracy || i % 100 == 0)
		//		{
		//			mats.save(W, B);
		//			std::cout << "\tsaved!";
		//			maxaccuracy = curaccuracy;
		//		}
		//		else
		//		{
		//			if (i % 2 == 0)
		//				testdata.next_batch(500);
		//			data.next_batch(100);
		//		}
		//		std::cout << std::endl;
		//	}
		//}

		t1 = tbb::tick_count::now();
		std::cout << "elapsed time:" << (t1 - t0).seconds() << std::endl;
	}
	catch (const invalid_argument& ia) {
		cerr << "Invalid argument: " << ia.what() << '\n';
	}

	return;
}
