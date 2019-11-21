#pragma once
#include <iostream>
#include <fstream>
#include <inttypes.h>
#include <stdexcept>
#include <initializer_list>
#include <vector>
#include <random>
#include <thread>
#include <tbb/tbb.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <tbb\scalable_allocator.h>
#include <tbb/blocked_range2d.h>

auto activation = [](double x) { return (x > 0) ? x : 0.5*x; };//return 1 / (1 + exp(-x));
auto dactivation = [](double x) {return (x < 0) ? 0.5 : 1; };//return x*(1 - x); 

template<typename T>
class Tensor;

enum Method { full, same, valid };
enum BoundaryCondition { zero, bound, period };
enum PoolingMethod { maxium, average };

template <typename T>
class Matrix
{
public:
	void* operator new (size_t size) throw (std::bad_alloc)
	{
		if (size == 0) size = 1;
		if (void* ptr = scalable_malloc(size))
			return ptr;
		throw std::bad_alloc();
	}
	void* operator new[](size_t size) throw (std::bad_alloc)
	{
		return operator new (size);
	}
		void* operator new (size_t size, const std::nothrow_t&) throw ()
	{
		if (size == 0) size = 1;
		if (void* ptr = scalable_malloc(size))
			return ptr;
		return NULL;
	}
	void* operator new[](size_t size, const std::nothrow_t&) throw ()
	{
		return operator new (size, std::nothrow);
	}
		void operator delete (void* ptr) throw ()
	{
		scalable_free(ptr);
	}
	void operator delete[](void* ptr, size_t size) throw ()
	{
		operator delete (ptr);
	}
		void operator delete[](void* ptr) throw ()
	{
		operator delete (ptr);
	}
		void operator delete (void* ptr, const std::nothrow_t&) throw ()
	{
		scalable_free(ptr);
	}
	void operator delete[](void* ptr, const std::nothrow_t&) throw ()
	{
		operator delete (ptr, std::nothrow);
	}

	template <typename T> friend std::ostream& operator<<(std::ostream& os, const Matrix<T>& x);
	Matrix(void) :col(0), row(0), element(nullptr) {}
	Matrix(const T& init) :Matrix(1, 1, init) {}
	Matrix(const size_t& row, const size_t& col, const T& init = static_cast<T>(0)) :row(row), col(col)
	{
		this->element = new T*[row];
		for (size_t i = 0; i != row; ++i)
			this->element[i] = new T[col];
		m_foreach([&](size_t &i, size_t& j, T& e) {e = init; });
	}
	Matrix(const std::initializer_list<std::initializer_list<T>>& li) :row(li.size()), col((*li.begin()).size())
	{
		this->element = new T*[row];
		for (size_t i = 0; i != row; ++i)
			this->element[i] = new T[col];
		size_t i = 0, j = 0;
		for (auto riter = li.begin(); riter != li.end(); ++riter, ++i, j = 0)
			for (auto citer = (*riter).begin(); citer != (*riter).end(); ++citer, ++j)
				this->element[i][j] = *citer;
	}
	Matrix(const size_t& row, const size_t& col,std::default_random_engine& generator, const T& lower, const T& upper) :row(row), col(col)
	{
		std::uniform_real_distribution<T> distribution(lower, upper);
		this->element = new T*[row];
		for (size_t i = 0; i != row; ++i)
			this->element[i] = new T[col];

		m_foreach([&](size_t &i, size_t& j, T& e) {e = distribution(generator); });
	}
	Matrix(const std::vector<T>& vec)
	{
		row = vec.size();
		if (row == 0)
			throw invalid_argument("用于初始化的vector为空.");
		col = 1;
		this->element = new T*[row];
		for (size_t i = 0; i != row; ++i)
			this->element[i] = new T[1];
		//tbb::parallel_for(tbb::blocked_range<size_t>(0, row),
		//	[&](const tbb::blocked_range<size_t>& r)
		//{	
		//	size_t end = r.end();
		//	for (size_t i = r.begin(); i != end; ++i)
		//		this->element[i] = new T[col];
		//});

		m_foreach([&](size_t &i, size_t& j, T& e) {e = vec[i]; });
	}
	Matrix(const Matrix& m)
	{
		auto telement = new T*[m.row];
		for (size_t i = 0; i != m.row; ++i)
			telement[i] = new T[m.col];

		this->row = m.row;
		this->col = m.col;
		for (size_t i = 0; i != row; ++i)
			memcpy(telement[i], m.element[i], sizeof(T)*col);
		this->element = telement;
	}
	Matrix(Matrix&& m)noexcept:row(m.row), col(m.col), element(m.element)
	{
		m.row = m.col = 0;
		m.element = nullptr;
	}
	Matrix(const cv::Mat& mat, bool _1ofN = false, const size_t& nclass = 0) :row(mat.rows), col(mat.cols)
	{
		if (_1ofN&&col == 1)col = nclass;
		this->element = new T*[row];
		for (size_t i = 0; i != row; ++i)
			this->element[i] = new T[col];

		//	size_t cols = mat.cols, rows = mat.rows;
		//if (mat.isContinuous())
		//{
		//	cols *= rows;
		//	rows = 1;
		//}
		for (size_t i = 0; i != row; ++i)
		{
			const uchar* Mi = mat.ptr<uchar>(i);
			if (_1ofN&&mat.cols == 1)
			{
				for (size_t j = 0; j != col; ++j)
					if (j == Mi[0])
						element[i][Mi[0]] = 1;
					else element[i][j] = 0;
			}
			else
			{
				for (size_t j = 0; j != col; ++j)
					element[i][j] = (unsigned)Mi[j] / 255.;
			}
		}
	}
	Matrix& operator=(const Matrix& m)&
	{
		auto telement = new T*[m.row];
		for (size_t i = 0; i != m.row; ++i)
			telement[i] = new T[m.col];
		//this->row = m.row;
		//this->col = m.col;
		for (size_t i = 0; i != m.row; ++i)
			memcpy(telement[i], m.element[i], sizeof(T)*m.col);

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
	Matrix& operator=(Matrix&& m)&noexcept
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
	operator T(void)const
	{
		return **this->element;
	}
	////////////////////////////////////////////////////////
	operator cv::Mat(void)const
	{
		return this->cvMat();
	}
	cv::Mat cvMat(const double& xscale = 1.0, const double& yscale = 1.0)const
	{
		cv::Mat gray(row, col, CV_8UC1);
		double max_ele = element[0][0],min_ele = element[0][0];
		for (size_t i = 0; i != row; ++i)
			for (size_t j = 0; j != col; ++j)
			{
				if (element[i][j] > max_ele)max_ele = element[i][j];
				else if (element[i][j] < min_ele)min_ele = element[i][j];
			}
		for (size_t i = 0; i != row; ++i)
			for (size_t j = 0; j != col; ++j)
				gray.at<uchar>(i, j) = (uchar)max(0, min((int)((element[i][j]-min_ele) * 255 / (max_ele-min_ele)), (int)255));

		cv::Mat result;
		cv::cvtColor(gray, result, CV_GRAY2RGB);
		cv::resize(result, result, cv::Size(gray.rows * xscale, gray.rows * yscale), 0, 0, CV_INTER_NN);
		return result.clone();
	}
	////////////////////////////////////////////////////////
	Matrix operator+(const Matrix& m)const
	{
		Matrix result(this->row, this->col);
		if (m.col == 1 && m.row == 1)
		{
			//T e = static_cast<T>(m);
			//tbb::parallel_for(tbb::blocked_range<size_t>(0, row),
			//	[&](const tbb::blocked_range<size_t>& r)
			//{
			//	size_t end = r.end();
			//	for (size_t i = r.begin(); i != end; ++i)
			//		for (size_t j = 0; j != col; ++j)
			//			result.element[i][j] = this->element[i][j] + e;
			//});
			return *this + **m.element;
		}
		else if (m.row == 1 && this->col == m.col)
		{
			//for (size_t i = 0; i != row; ++i)
			//	for (size_t j = 0; j != col; ++j)
			//		result.element[i][j] = this->element[i][j] + m.element[0][j];
			tbb::parallel_for(tbb::blocked_range<size_t>(0, row),
				[&](const tbb::blocked_range<size_t>& r)
			{
				size_t end = r.end();
				for (size_t i = r.begin(); i != end; ++i)
					for (size_t j = 0; j != col; ++j)
						result.element[i][j] = this->element[i][j] + m.element[0][j];
			});
		}
		else
		{
			if (this->row != m.row || this->col != m.col)
				throw invalid_argument("相加的两矩阵非同型矩阵.");
			//return std::move(Matrix(*this).m_foreach([&](size_t&i, size_t&j, T&e) {e += m.element[i][j]; }));
			tbb::parallel_for(tbb::blocked_range<size_t>(0, row),
				[&](const tbb::blocked_range<size_t>& r)
			{
				size_t end = r.end();
				for (size_t i = r.begin(); i != end; ++i)
					for (size_t j = 0; j != col; ++j)
						result.element[i][j] = this->element[i][j] + m.element[i][j];
			});
		}
		return std::move(result);
	}
	Matrix operator+(const T& m)const
	{
		Matrix result(this->row, this->col);
		tbb::parallel_for(tbb::blocked_range<size_t>(0, row),
			[&](const tbb::blocked_range<size_t>& r)
		{
			size_t end = r.end();
			for (size_t i = r.begin(); i != end; ++i)
				for (size_t j = 0; j != col; ++j)
					result.element[i][j] = this->element[i][j] + m;
		});
		return std::move(result);
	}
	Matrix operator-(const T& m)const
	{
		Matrix result(this->row, this->col);
		tbb::parallel_for(tbb::blocked_range<size_t>(0, row),
			[&](const tbb::blocked_range<size_t>& r)
		{
			size_t end = r.end();
			for (size_t i = r.begin(); i != end; ++i)
				for (size_t j = 0; j != col; ++j)
					result.element[i][j] = this->element[i][j] - m;
		});
		return std::move(result);
	}
	Matrix& operator+=(const Matrix& m)
	{
		*this = std::move(*this + m);
		return *this;
	}
	Matrix operator-(const Matrix& m)const
	{
		//if (this->row != m.row || this->col != m.col)
		//	throw invalid_argument("相减的两矩阵非同型矩阵.");
		////Matrix t(*this);
		//return std::move(Matrix(*this).m_foreach([&](size_t&i, size_t&j, T&e) {e -= m.element[i][j]; }));
		Matrix result(this->row, this->col);
		if (m.col == 1 && m.row == 1)
		{
			//T e = static_cast<T>(m);
			//tbb::parallel_for(tbb::blocked_range<size_t>(0, row),
			//	[&](const tbb::blocked_range<size_t>& r)
			//{
			//	size_t end = r.end();
			//	for (size_t i = r.begin(); i != end; ++i)
			//		for (size_t j = 0; j != col; ++j)
			//			result.element[i][j] = this->element[i][j] - e;
			//});
			return *this - **m.element;
		}
		else if (m.row == 1 && this->col == m.col)
		{
			//for (size_t i = 0; i != row; ++i)
			//	for (size_t j = 0; j != col; ++j)
			//		result.element[i][j] = this->element[i][j] + m.element[0][j];
			tbb::parallel_for(tbb::blocked_range<size_t>(0, row),
				[&](const tbb::blocked_range<size_t>& r)
			{
				size_t end = r.end();
				for (size_t i = r.begin(); i != end; ++i)
					for (size_t j = 0; j != col; ++j)
						result.element[i][j] = this->element[i][j] - m.element[0][j];
			});
		}
		else
		{
			if (this->row != m.row || this->col != m.col)
				throw invalid_argument("相加的两矩阵非同型矩阵.");
			//return std::move(Matrix(*this).m_foreach([&](size_t&i, size_t&j, T&e) {e += m.element[i][j]; }));
			tbb::parallel_for(tbb::blocked_range<size_t>(0, row),
				[&](const tbb::blocked_range<size_t>& r)
			{
				size_t end = r.end();
				for (size_t i = r.begin(); i != end; ++i)
					for (size_t j = 0; j != col; ++j)
						result.element[i][j] = this->element[i][j] - m.element[i][j];
			});
		}
		return std::move(result);
	}
	Matrix& operator-=(const Matrix& m)
	{
		*this = std::move(*this - m);
		return *this;
	}
	Matrix operator*(const Matrix& m)const
	{
		if (this->col != m.row)
			throw invalid_argument("相乘的两矩阵左边矩阵的列数不等于右边矩阵的行数.");
		Matrix result(this->row, m.col);
		//tbb::combinable<T> sum;
		//return std::move(Matrix(this->row, m.col).m_foreach([&](size_t&i, size_t&j, T&e)
		//{
		//	//sum = 0;
		//	//tbb::parallel_for(
		//	//	tbb::blocked_range<size_t>(0, m.row),
		//	//	[&](tbb::blocked_range<size_t>& br) {

		//	//	for (size_t r = br.begin(); r != br.end(); ++r)
		//	//		sum.local() += this->element[i][r] * m.element[r][j];
		//	//},tbb::affinity_partitioner());
		//	//e = sum.combine([](const T& x, const T& y) {return x + y; });
		//	for (size_t r = 0; r != m.row; ++r)
		//		e += this->element[i][r] * m.element[r][j];
		//}));
		tbb::parallel_for(tbb::blocked_range2d<size_t>(0, row, 0, m.col),
			[&](const tbb::blocked_range2d<size_t>& r)
		{
			for (size_t i = r.rows().begin(); i != r.rows().end(); ++i)
				for (size_t j = r.cols().begin(); j != r.cols().end(); ++j)
				{
					T sum = 0;
#pragma ivdep
					for (size_t k = 0; k < m.row; ++k)
						sum += this->element[i][k] * m.element[k][j];
					result.element[i][j] = sum;
				}
		});
		return std::move(result);
	}
	Matrix dropout(const Matrix& m, std::default_random_engine& generator, const double &p)const
	{
		Matrix result(this->row, m.col);
		std::uniform_real_distribution<double> distribution(0, 1.0);

		tbb::parallel_for(tbb::blocked_range2d<size_t>(0, row, 0, m.col),
			[&](const tbb::blocked_range2d<size_t>& r)
		{
			for (size_t i = r.rows().begin(); i != r.rows().end(); ++i)
				for (size_t j = r.cols().begin(); j != r.cols().end(); ++j)
				{
					T sum = 0;
#pragma ivdep
					for (size_t k = 0; k < m.row; ++k)
					{
						if (distribution(generator) <= p)
							sum += this->element[i][k] * m.element[k][j];
						else continue;
					}
					result.element[i][j] = sum;
				}
		});
		return std::move(result);

	}
	Matrix dot(const Matrix& m)const
	{
		if (this->col != m.col)
			throw std::invalid_argument("Hadamard乘积的两矩阵列数不相等.");
		if (this->row != m.row)
			throw std::invalid_argument("Hadamard乘积的两矩阵行数不相等.");
		Matrix result(this->row, this->col);

		tbb::parallel_for(tbb::blocked_range2d<size_t>(0, row, 0, col),
			[&](const tbb::blocked_range2d<size_t>& r)
		{
			for (size_t i = r.rows().begin(); i != r.rows().end(); ++i)
				for (size_t j = r.cols().begin(); j != r.cols().end(); ++j)
					result.element[i][j] = this->element[i][j] * m.element[i][j];
		});
		return std::move(result);
	}
	Matrix operator*(const T& x)const
	{
		//return std::move(Matrix(this->row, this->col).m_foreach(
		//	[&](size_t&i, size_t&j, T&e) {e = this->element[i][j] * x; }));
		Matrix result(this->row, this->col);

		tbb::parallel_for(tbb::blocked_range<size_t>(0, row),
			[&](const tbb::blocked_range<size_t>& r)
		{
			size_t end = r.end();
			for (size_t i = r.begin(); i != end; ++i)
				for (size_t j = 0; j != col; ++j)
					result.element[i][j] = this->element[i][j] * x;
		});
		return std::move(result);
	}
	Matrix operator/(const T& x)const
	{
		return *this*(static_cast<T>(1) / x);
	}
	Matrix operator/(const Matrix& m)const
	{
		if (this->row == m.row&&this->col == m.col)
		{
			Matrix result(this->row, this->col);

			tbb::parallel_for(tbb::blocked_range<size_t>(0, row),
				[&](const tbb::blocked_range<size_t>& r)
			{
				size_t end = r.end();
				for (size_t i = r.begin(); i != end; ++i)
					for (size_t j = 0; j != col; ++j)
						result.element[i][j] = this->element[i][j] / m.element[i][j];
			});
			return std::move(result);
		}
		else if (this->row == m.row&&m.col == 1)
		{
			Matrix result(this->row, this->col);

			tbb::parallel_for(tbb::blocked_range<size_t>(0, row),
				[&](const tbb::blocked_range<size_t>& r)
			{
				size_t end = r.end();
				for (size_t i = r.begin(); i != end; ++i)
					for (size_t j = 0; j != col; ++j)
						result.element[i][j] = this->element[i][j] / m.element[i][0];
			});
			return std::move(result);
		}
		else if (this->col == m.col&&m.row == 1)
		{
			Matrix result(this->row, this->col);

			tbb::parallel_for(tbb::blocked_range<size_t>(0, row),
				[&](const tbb::blocked_range<size_t>& r)
			{
				size_t end = r.end();
				for (size_t i = r.begin(); i != end; ++i)
					for (size_t j = 0; j != col; ++j)
						result.element[i][j] = this->element[i][j] / m.element[0][j];
			});
			return std::move(result);
		}
		else throw invalid_argument("矩阵相除方法未定义.");
	}
	Matrix operator[](const size_t& row)const
	{
		if (this->row == 1)
		{
			if (row >= this->col)
				throw invalid_argument("访问列越界.");
			return std::move(Matrix(this->element[0][row]));
		}
		else
		{
			if (row >= this->row)
				throw invalid_argument("访问行越界.");
			return std::move(Matrix(1, this->col).m_foreach([&](size_t&i, size_t&j, T&e) {e = this->element[row][j]; }));
		}
	}
	size_t getrow(void)const
	{
		return this->row;
	}
	size_t getcol(void)const
	{
		return this->col;
	}
	T** getele(void)const
	{
		return this->element;
	}
	Matrix Tr(void)const//Transposed Matrix
	{
		return std::move(Matrix(this->col, this->row).m_foreach([&](size_t&i, size_t&j, T&e) {e = this->element[j][i]; }));
	}
	T cofactor(const size_t& row, const size_t& col)const
	{
		if (this->col != this->row)
			throw invalid_argument("选作计算余子式的矩阵非方阵.");
		if (row > this->row)
			throw invalid_argument("余子式行越界.");
		if (col > this->col)
			throw invalid_argument("余子式列越界.");
		Matrix t(this->row - 1, this->col - 1);
		return t.m_foreach([&](size_t&i, size_t&j, T&e)
		{
			size_t r, c;
			if (i >= row - 1)r = i + 1;
			else r = i;
			if (j >= col - 1)c = j + 1;
			else c = j;
			e = this->element[r][c];
		}).determinant();
	}
	T acofacter(const size_t& row, const size_t& col)const
	{
		return this->cofactor(row, col)*static_cast<T>(((row + col) % 2) ? -1 : 1);
	}
	T determinant(void)const
	{
		if (this->row != this->col)
			throw invalid_argument("选作计算行列式的矩阵非方阵.");
		if (this->row == 1)
			return this->element[0][0];
		if (row <= 2)
			return this->element[0][0] * this->element[1][1]
			- this->element[1][0] * this->element[0][1];
		else
		{
			T det(0);
			for (size_t i = 0; i != row; ++i)
				det += this->element[i][0] * static_cast<T>((i % 2) ? -1 : 1)*(this->cofactor(i + 1, 1));
			return det;
		}
	}
	Matrix adjoint(void)const
	{
		if (this->col != this->row)
			throw invalid_argument("选作计算伴随阵的矩阵非方阵.");
		Matrix t(this->row, this->col);
		return t.m_foreach([&](size_t&i, size_t&j, T&e) {e = this->acofacter(j + 1, i + 1); });
	}
	Matrix inverse(void)const
	{
		return this->adjoint() / this->determinant();
	}
	Matrix exchangerow(const size_t& row1, const size_t& row2)const
	{
		if (row1 == row2)return *this;
		for (size_t i = 0; i != this->col; ++i)
			swap(this->element[row1 - 1][i], this->element[row2 - 1][i]);
		return *this;
	}
	Matrix r1pluskr2(const size_t& row1, const T& k, const size_t& row2)
	{
		if (row1 == row2)return *this;
		for (size_t i = 0; i != this->col; ++i)
			this->element[row1 - 1][i] += k*this->element[row2 - 1][i];
		return *this;
	}
	Matrix elementarytrans(const bool& steps = false, std::ostream &os = std::cout)const
	{
		Matrix t(*this);
		for (size_t c = 0, r = 0, i = r; r < t.row&&c < t.col; ++c, ++r, i = r)
		{
			do {
				for (i = r; i != t.row; ++i)//找到并交换非零行
				{
					if (t.element[i][c] != static_cast<T>(0))
					{
						if (i == r)break;
						if (steps)os << 'r' << r + 1 << "<->" << 'r' << i + 1 << endl;
						t.exchangerow(r + 1, i + 1);
						if (steps)os << t << endl;
						break;
					}
				}
				if (i == t.row)c++;//说明这一列全是0。。。
				if (c >= t.col)return t;
			} while (i == t.row);
			if (t.element[r][c] != static_cast<T>(1))
			{
				if (steps)os << 'r' << r + 1 << "/" << t.element[r][c] << endl;
				for (size_t i = t.col - 1; (int)i >= (int)c; --i)t.element[r][i] /= t.element[r][c];
				if (steps)os << t << endl;
			}
			for (int ir = (int)r - 1; ir >= 0; --ir)
				if (t.element[ir][c] != static_cast<T>(0))
				{
					if (steps)
						if (t.element[ir][c] == static_cast<T>(1)) os << 'r' << ir + 1 << '-' << 'r' << r + 1 << endl;
						else if (t.element[ir][c] == static_cast<T>(-1))os << 'r' << ir + 1 << '+' << 'r' << r + 1 << endl;
						else os << 'r' << ir + 1 << ((t.element[ir][c] < static_cast<T>(0)) ? "+" : "") << -t.element[ir][c] << '*' << 'r' << r + 1 << endl;
						t.r1pluskr2(ir + 1, -t.element[ir][c], r + 1);
						if (steps)os << t << endl;
				}

			for (size_t ir = r + 1; ir < t.row; ++ir)
				if (t.element[ir][c] != static_cast<T>(0))
				{
					if (steps)
						if (t.element[ir][c] == static_cast<T>(1)) os << 'r' << ir + 1 << '-' << 'r' << r + 1 << endl;
						else if (t.element[ir][c] == static_cast<T>(-1))os << 'r' << ir + 1 << '+' << 'r' << r + 1 << endl;
						else os << 'r' << ir + 1 << ((t.element[ir][c] < static_cast<T>(0)) ? "+" : "") << -t.element[ir][c] << '*' << 'r' << r + 1 << endl;
						for (size_t ic = t.col - 1; (int)ic >= (int)c; --ic)
							t.element[ir][ic] -= t.element[r][ic] * t.element[ir][c];
						if (steps)os << t << endl;
				}
		}
		return t;
	}
	size_t rank(void)const
	{
		size_t ir = this->row - 1;
		for (; (int)ir >= 0; --ir)
			for (size_t ic = 0; ic != this->col; ++ic)
				if (this->element[ir][ic] != static_cast<T>(0))
					return ir + 1;
		return ir;
	}
	Matrix conv(const Matrix &m, const Method &method = same, const BoundaryCondition &BC = zero)const
	{
		int offsetR = 0, offsetC = 0;
		size_t cR, cC;
		switch (method)
		{
		case full:
			cR = this->row + m.row - 1;
			cC = this->col + m.col - 1;
			break;
		case same:
			cR = this->row;
			cC = this->col;
			offsetR = m.row / 2;
			offsetC = m.col / 2;
			break;
		case valid:
			cR = this->row - m.row + 1;
			cC = this->col - m.col + 1;
			if ((cR < 1) || (cC < 1))
				throw invalid_argument("卷积无效.");
			offsetR = m.row - 1;
			offsetC = m.col - 1;
			break;
		default:
			throw invalid_argument("卷积方法未知.");
		}

		return std::move(Matrix(cR, cC).m_foreach([&](size_t&ri, size_t&rj, T&re)
		{
			for (int mi = 0; mi != m.row; ++mi)
				for (int mj = 0; mj != m.col; ++mj)
					re += m.element[mi][mj] * (this->getele(ri - mi + offsetR, rj - mj + offsetC, BC));
		}));
	}
	Matrix pooling(const size_t& scale, const PoolingMethod& method = maxium)const
	{
		//if(this->row!=this->col)throw invalid_argument("非方阵无法池化方法未知.");
		Matrix result((this->row + 1) / scale, (this->col + 1) / scale);
		switch (method)
		{
		case maxium:
			for (size_t i = 0; i < row; i += scale)
			{
				for (size_t j = 0; j < col; j += scale)
				{
					T maxele = this->element[i][j];
					for (size_t _i = i; _i < i + scale&&_i < row; ++_i)
						for (size_t _j = j; _j < j + scale&&_j < col; ++_j)
							if (this->element[_i][_j] > maxele)
							{
								maxele = this->element[_i][_j];
							}
					result.element[i / scale][j / scale] = maxele;
				}
			}
			break;
		case average:
			for (size_t i = 0; i < row; i += scale)
			{
				for (size_t j = 0; j < col; j += scale)
				{
					T ave = static_cast<T>(0);
					for (size_t _i = i; _i < i + scale&&_i < row; ++_i)
						for (size_t _j = j; _j < j + scale&&_j < col; ++_j)
							ave += this->element[_i][_j];

					result.element[i / scale][j / scale] = ave / (scale*scale);
				}
			}
			break;
		default: throw invalid_argument("池化方法未知.");
		}
		return result;
	}
	Matrix uppooling(const size_t& scale, const PoolingMethod& method = maxium)const
	{
		Matrix result(this->row * scale, this->col * scale);
		switch (method)
		{
		case maxium:
			break;
		case average:
			for (size_t i = 0; i < result.row; ++i)
			{
				for (size_t j = 0; j < result.col; ++j)
				{
					for (size_t _i = i; _i < i + scale&&_i < result.row; ++_i)
						for (size_t _j = j; _j < j + scale&&_j < result.col; ++_j)
							result.element[_i][_j] = this->element[i / scale][j / scale];
				}
			}
			break;
		default: throw invalid_argument("卷积方法未知.");
		}
		return result;

	}
	Matrix concatenate(const Matrix& a, const size_t axis = 0)
	{
		if (axis == 0)
		{
			if (a.row != this->row)
				throw invalid_argument("进行0轴拼接的矩阵必须行数相同.");
			Matrix t(this->row, this->col + a.col);
			return t.m_foreach([&](size_t&i, size_t&j, T&e)
			{
				if (j < this->col)e = this->element[i][j];
				else e = a.element[i][j - this->col];
			});
		}
		else if (axis == 1)
		{
			if (a.col != this->col)
				throw invalid_argument("进行1轴拼接的矩阵必须列数相同.");
			Matrix t(this->row + a.row, this->col);
			return t.m_foreach([&](size_t&i, size_t&j, T&e)
			{
				if (i < this->row)e = this->element[i][j];
				else e = a.element[i - this->row][j];
			});
		}
		else throw std::invalid_argument("进行拼接轴未定义.");
	}
	Matrix accumulate(const size_t& axis = 0)
	{
		if (axis == 0)
		{
			Matrix result(1, this->col);
			for (size_t j = 0; j != col; ++j)
				for (size_t i = 0; i != row; ++i)
					result.element[0][j] += this->element[i][j];
			return std::move(result);
		}
		else if (axis == 1)
		{
			Matrix result(this->row, 1);
			for (size_t i = 0; i != row; ++i)
				for (size_t j = 0; j != col; ++j)
					result.element[i][0] += this->element[i][j];
			return std::move(result);
		}
		else throw std::invalid_argument("进行累加轴未定义.");
	}
	Matrix externto(const size_t& size, const size_t& axis = 0)
	{

	}
	T mean(size_t axis = 0)const
	{
		T sum = 0;
		for (size_t i = 0; i != row; ++i)
			for (size_t j = 0; j != col; ++j)
				sum += element[i][j];
		return sum / (row*col);
	}
	T sum(void)const
	{
		return std::move(tbb::parallel_reduce(
			tbb::blocked_range2d<size_t>(0, row, 0, col),0,
			[&](const tbb::blocked_range2d<size_t>& r, T value)
			{
				for (size_t i = r.rows().begin(); i != r.rows().end(); ++i)
					for (size_t j = r.cols().begin(); j != r.cols().end(); ++j)
						value += element[i][j];
				return value;
			},
			std::plus<T>()
			));
	}
	template<typename F>
	Matrix each(const F& f)const
	{
		/*return Matrix<T>(this->row, this->col).m_foreach(
			[&](size_t&i, size_t&j, T& e)->void {
			e = f(this->element[i][j]);
		});*/
		Matrix<T> result(this->row, this->col);
		tbb::parallel_for(tbb::blocked_range<size_t>(0, row),
			[&](const tbb::blocked_range<size_t>& r)
		{
			size_t end = r.end();
			for (size_t i = r.begin(); i != end; ++i)
				for (size_t j = 0; j != col; ++j)
					result.element[i][j] = f(this->element[i][j]);
		});
		return std::move(result);
	}
	template<typename F>
	void eachself(const F& f)
	{
		tbb::parallel_for(tbb::blocked_range<size_t>(0, row),
			[&](const tbb::blocked_range<size_t>& r)
		{
			size_t end = r.end();
			for (size_t i = r.begin(); i != end; ++i)
				for (size_t j = 0; j != col; ++j)
					this->element[i][j] = f(this->element[i][j]);
		});
	}
	Matrix reshape(const size_t& row, const size_t& col)const
	{
		Matrix result(row, col);
		size_t sum = this->row*this->col;
		if (row*col <= sum)
		{
			tbb::parallel_for(tbb::blocked_range2d<size_t>(0, row, 0, col),
				[&](const tbb::blocked_range2d<size_t>& r)
			{
				for (size_t i = r.rows().begin(); i != r.rows().end(); ++i)
					for (size_t j = r.cols().begin(); j != r.cols().end(); ++j)
					{
						size_t icol = i*col;
						result.element[i][j] = this->element[(icol + j) / this->col][icol + j - ((icol + j) / this->col)* this->col];
					}
			});
			//for (size_t i = 0; i != row; ++i)
			//	for (size_t j = 0; j != col; ++j)
			//	{
			//		size_t icol = i*col;
			//		result.element[i][j] = this->element[(icol + j) / this->col][icol + j - ((icol + j) / this->col)* this->col];
			//	}
		}
		else
		{
			tbb::parallel_for(tbb::blocked_range2d<size_t>(0, row, 0, col),
				[&](const tbb::blocked_range2d<size_t>& r)
			{
				for (size_t i = r.rows().begin(); i != r.rows().end(); ++i)
					for (size_t j = r.cols().begin(); j != r.cols().end(); ++j)
					{
						size_t icol = i*col;
						if (icol + (j + 1) <= sum)
						{
							result.element[i][j] = this->element[(icol + j) / this->col][icol + j - ((icol + j) / this->col)* this->col];
						}
						else result.element[i][j] = static_cast<T>(0);
					}
			});
			//for (size_t i = 0; i != row; ++i)
			//	for (size_t j = 0; j != col; ++j)
			//	{
			//		size_t icol = i*col;
			//		if (icol + (j + 1) <= sum)
			//		{
			//			result.element[i][j] = this->element[(icol + j) / this->col][icol + j - ((icol + j) / this->col)* this->col];
			//		}
			//		else result.element[i][j] = static_cast<T>(0);
			//	}
		}
		return std::move(result);
	}
	Matrix rot180(void)const
	{
		Matrix<T> result(this->row, this->col);
		//for (size_t i = 0; i != row; ++i)
		//	for (size_t j = 0; j != col; ++j)
		//		result.element[i][j] = this->element[this->row - i - 1][this->col - j - 1];

		tbb::parallel_for(tbb::blocked_range2d<size_t>(0, row, 0, col),
			[&](const tbb::blocked_range2d<size_t>& r)
		{
			for (size_t i = r.rows().begin(); i != r.rows().end(); ++i)
				for (size_t j = r.cols().begin(); j != r.cols().end(); ++j)
				{
					T sum = 0;
					result.element[i][j] = this->element[this->row - i - 1][this->col - j - 1];
				}
		});

		return std::move(result);
	}
	Tensor<T> fold(const size_t& _row, const size_t& _col)
	{
		size_t row = this->row;
		size_t elesize = _row*_col;//25
		size_t col = this->col / elesize;

		Tensor<T> result(row, col);
		for (size_t i = 0; i != row; ++i)
			for (size_t j = 0; j != col; ++j)
			{
				Matrix<T> temp(1, elesize);
				memcpy(temp.getele()[0], j*elesize + this->element[i], sizeof(T)*elesize);
				result.getele()[i][j] = temp.reshape(5, 5);
			}
		return std::move(result);
	}
	void save(std::fstream& ofs)
	{
		ofs.write(reinterpret_cast<char*>(&row), sizeof(size_t));
		ofs.write(reinterpret_cast<char*>(&col), sizeof(size_t));
		for (size_t i = 0; i != row; ++i)
			ofs.write(reinterpret_cast<char*>(element[i]), col * sizeof(T));
	}
	void load(std::fstream& ifs)
	{
		for (size_t i = 0; i != row; ++i)
			delete[] element[i];
		delete[] element;
		ifs.read(reinterpret_cast<char*>(&row), sizeof(size_t));
		ifs.read(reinterpret_cast<char*>(&col), sizeof(size_t));
		if (row == 0 || col == 0)throw std::invalid_argument("未能从文件读取到矩阵大小.");

		this->element = new T*[row];
		for (size_t i = 0; i != row; ++i)
			this->element[i] = new T[col];

		for (size_t i = 0; i != row; ++i)
			ifs.read(reinterpret_cast<char*>(element[i]), col * sizeof(T));

	}
	~Matrix()
	{
		if (element == nullptr)return;
		for (size_t i = 0; i != row; ++i)
			delete[] element[i];
		delete[] element;
	}
protected:
	template <typename F>
	inline Matrix& m_foreach(const F& f)
	{
		for (size_t i = 0; i != row; ++i)
#pragma ivdep
			for (size_t j = 0; j != col; ++j)
				f(i, j, element[i][j]);
		return *this;
	}
	T getele(int irow, int icol, const BoundaryCondition BC)const
	{
		switch (BC)
		{
		case zero:      //索引超出界限认为0
			if ((irow < 0) || (irow > (int)this->row) || (icol < 0) || (icol > (int)this->col))
				return T(0);
		case bound:     //超出索引部分和边界值相等
			if (irow < 0)irow = 0;
			else if (irow >= (int)this->row)irow = this->row - 1;
			if (icol < 0)icol = 0;
			else if (icol >= (int)this->col)
				icol = this->col - 1;
			return element[irow][icol];
		case period:
			while ((irow < 0) || (irow >= (int)this->row))
			{
				if (irow < 0)irow += this->row;
				else irow -= this->row;
			}
			while ((icol < 0) || (icol >= (int)this->col))
			{
				if (icol < 0)icol += this->col;
				else icol -= this->col;
			}
			return element[irow][icol];
		default:
			throw invalid_argument("边界处理方法未知.");
		}
	}
	//row:行 col:列!
	size_t row, col;
	T **element;
};
template <typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& x)
{
	for (size_t i = 0; i != x.row; ++i)
	{
		for (size_t j = 0; j != x.col; ++j)
			os << std::setw(10) << std::setprecision(5) << x.element[i][j] << ' ';
		os << '\n';
	}
	return os;
}
