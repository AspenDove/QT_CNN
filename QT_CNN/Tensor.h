#pragma once
#include "matrix.h"

template<typename T>
class Tensor :public Matrix<Matrix<T>>
{
public:
	Tensor(const Matrix<Matrix<T>>& tensor) :Matrix(tensor) {}
	Tensor(const std::vector<Matrix<T>>& vec) :Matrix(vec) {}
	Tensor(const size_t& row, const size_t& col, const T& init = static_cast<T>(0)) :Matrix(row, col, init) {}
	Tensor(const Tensor& tensor)
	{
		auto telement = new Matrix<T>*[tensor.row];
		for (size_t i = 0; i != tensor.row; ++i)
			telement[i] = new Matrix<T>[tensor.col];

		this->row = tensor.row;
		this->col = tensor.col;
		for (size_t i = 0; i != tensor.row; ++i)
			for (size_t j = 0; j != tensor.col; ++j)
				telement[i][j] = tensor.element[i][j];
		this->element = telement;
	}
	Tensor(Tensor&& m)noexcept : Matrix(std::move(m)) {}
	Tensor(Matrix<Matrix<T>>&& m)noexcept : Matrix(std::move(m)) {}
	Tensor& operator=(const Tensor& m)&noexcept
	{
		auto telement = new Matrix<T>*[m.row];
		for (size_t i = 0; i != m.row; ++i)
			telement[i] = new Matrix<T>[m.col];

		for (size_t i = 0; i != m.row; ++i)
			for (size_t j = 0; j != m.col; ++j)
				telement[i][j] = m.element[i][j];

		if (this->element)
		{
			for (size_t i = 0; i < this->row; ++i)
				delete[] this->element[i];
			delete[] this->element;
		}
		this->row = m.row;
		this->col = m.col;

		this->element = telement;
		return *this;
	}
	Tensor& operator=(Tensor&& m)&noexcept
	{
		for (size_t i = 0; i != row; ++i)
			delete[] element[i];
		delete[] element;
		this->row = m.row;
		this->col = m.col;

		this->element = m.element;
		m.row = m.col = 0;
		m.element = nullptr;
		return *this;
	}

	Tensor convallchannels(const Tensor& kernels, const Matrix<T>& bias)const
	{
		Tensor result(this->row, kernels.col);
		tbb::parallel_for(tbb::blocked_range2d<size_t>(0, this->row, 0, kernels.col),
			[&](const tbb::blocked_range2d<size_t>& r)
		{
			for (size_t i = r.rows().begin(); i != r.rows().end(); ++i)
				for (size_t j = r.cols().begin(); j != r.cols().end(); ++j)
				{
					Matrix<T> sum = this->element[i][0].conv(kernels.element[0][j], valid);
					for (size_t k = 1; k < this->col; ++k)
						sum += this->element[i][k].conv(kernels.element[k][j], valid);
					result.element[i][j] = sum + bias[j];
				}
		});
		return std::move(result);
	}
	Tensor convallchannels(const Tensor& kernels)const
	{
		Tensor result(this->row, kernels.col);
		tbb::parallel_for(tbb::blocked_range2d<size_t>(0, this->row, 0, kernels.col),
			[&](const tbb::blocked_range2d<size_t>& r)
		{
			for (size_t i = r.rows().begin(); i != r.rows().end(); ++i)
				for (size_t j = r.cols().begin(); j != r.cols().end(); ++j)
				{
					Matrix<T> sum = this->element[i][0].rot180().conv(kernels.element[0][j], valid);
					for (size_t k = 1; k < this->col; ++k)
						sum += this->element[i][k].rot180().conv(kernels.element[k][j], valid);
					result.element[i][j] = sum;
				}
		});
		return std::move(result);
	}
	Tensor backconv(const Tensor& kernels)const
	{
		Tensor result(this->row, kernels.col);
		tbb::parallel_for(tbb::blocked_range2d<size_t>(0, this->row, 0, kernels.col),
			[&](const tbb::blocked_range2d<size_t>& r)
		{
			for (size_t i = r.rows().begin(); i != r.rows().end(); ++i)
				for (size_t j = r.cols().begin(); j != r.cols().end(); ++j)
				{
					Matrix<T> sum = this->element[i][0].conv(kernels.element[0][j].rot180(), full);
					for (size_t k = 1; k < this->col; ++k)
						sum += this->element[i][k].conv(kernels.element[k][j].rot180(), full);
					result.element[i][j] = sum;
				}
		});
		return std::move(result);
	}
	Tensor Tr(void)const
	{
		return std::move(Tensor<T>(this->col, this->row)
			.m_foreach([&](size_t&i, size_t&j, Matrix<T>&e) {e = this->element[j][i]; }));
	}
	Tensor lrn(const size_t& depth_radius = 2, const T& bias = 0,
		const T& alpha = 1, const double& beta = 1.0)const//Local Response Normalization
	{
		Tensor result(*this);
		Tensor square(this->each([&](Matrix<double>& mat) {return mat.each([](const T&e) {return e*e; }); }));
		for (size_t i = 0; i != this->row; ++i)
		{
			for (size_t j = 0; j != this->col; ++j)
			{
				Matrix<T> sum((**element).getrow(), (**element).getcol(), 0);
				for (size_t r = max(0, (int)j - (int)depth_radius);
					r != min(this->col - 1, j + depth_radius); ++r)
					sum += square.element[i][r];

				if (beta == 1)
					result.element[i][j] / (sum*alpha + bias);
				else
					result.element[i][j] / ((sum*alpha + bias).each([&](const T&e) {return pow(e, beta); }));
			}
		}
		return std::move(result);
	}
	Tensor dot(const Tensor& tensor)const
	{
		if (this->col != tensor.col)
			throw std::invalid_argument("Hadamard乘积的两张量列数不相等.");
		if (this->row != tensor.row)
			throw std::invalid_argument("Hadamard乘积的两张量行数不相等.");

		Tensor result(this->row, this->col);

		tbb::parallel_for(tbb::blocked_range2d<size_t>(0, row, 0, col),
			[&](const tbb::blocked_range2d<size_t>& r)
		{
			for (size_t i = r.rows().begin(); i != r.rows().end(); ++i)
				for (size_t j = r.cols().begin(); j != r.cols().end(); ++j)
					result.element[i][j] = this->element[i][j].dot(tensor.element[i][j]);
		});
		return std::move(result);
	}
	Matrix expand(void)const
	{
		size_t row = this->row;
		size_t elesize = this->element[0][0].getrow()*this->element[0][0].getcol();//25
		size_t col = this->col*elesize;
		Matrix<T> result(row, col);

		for (size_t i = 0; i != row; ++i)
			for (size_t j = 0; j != this->col; ++j)
				memcpy(result.getele()[i] + elesize*j, this->element[i][j].reshape(1, elesize).getele()[0], sizeof(T)*elesize);

		return std::move(result);
	}
	cv::Mat MergecvMat(const size_t& setrow,const size_t& setcol,
					   const double& xscale = 1.0, const double& yscale = 1.0)const
	{
		size_t row, col;

		if (setrow*setcol > this->row*this->col)
		{
			row = this->row;
			col = this->col;
		}
		else
		{
			row = setrow;
			col = setcol;
		}

		cv::Mat result;		
		std::vector<cv::Mat> col_mats,row_mats;
		for (size_t i = 0; i != row; ++i)
		{
			cv::Mat temp;
			for (size_t j = 0; j != col; ++j)
			{
				//this->element[(i*col + j) / this->col][i*col + j - ((i*col + j) / this->col)* this->col];
				col_mats.push_back(this->element[(i*col + j) / this->col][i*col + j - ((i*col + j) / this->col)* this->col].cvMat(xscale, yscale));//this->element[i][j]
				//cv::hconcat(this->element[i][j].cvMat(xscale, yscale), temp);
			}
			cv::hconcat(col_mats, temp);
			row_mats.push_back(temp);
			col_mats.clear();
		}
		cv::vconcat(row_mats, result);
		return result.clone();
	}

	~Tensor()
	{
	}
};

struct CNNstructure
{
	size_t nConv, nFC;
};

struct param
{
	size_t batch,nconvlayer;
	//struct{size_t height, width, depth;}[nconvlayer];
	
};
