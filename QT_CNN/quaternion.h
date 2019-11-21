#pragma once

#include <math.h>
class quaternion
{
public:
	float a, b, c, d;
	quaternion() :a(1), b(0), c(0), d(0){}
	quaternion(const float& a, const float& b, const float& c, const float& d)
		:a(a), b(b), c(c), d(d) {}
	quaternion operator*(const float& lambda)const
	{
		return quaternion(lambda*this->a, lambda*this->b, lambda*this->c, lambda*this->d);
	}
	quaternion operator/(const float& lambda)const
	{
		return quaternion(this->a / lambda, this->b / lambda, this->c / lambda, this->d / lambda);
	}
	quaternion operator+(const quaternion& q)const
	{
		return quaternion(q.a + this->a, q.b + this->b, q.c + this->c, q.d + this->d);
	}
	quaternion operator-(const quaternion& q)const
	{
		return quaternion(this->a - q.a, this->b - q.b, this->c - q.c, this->d - q.d);
	}
	bool operator==(const quaternion& q)const
	{
		return (fabs(q.a - a) <= 1.e-10&&fabs(q.b - b) <= 1.e-10&&fabs(q.c - c) <= 1.e-10&&fabs(q.d - d) <= 1.e-10);
	}
	bool operator!=(const quaternion& q)const
	{
		return !(*this == q);
	}
	quaternion operator*(const quaternion& q)const//Hamilton product
	{
		return quaternion(a*q.a - b*q.b - c*q.c - d*q.d,
			a*q.b + b*q.a + c*q.d - d*q.c,
			a*q.c - b*q.d + c*q.a + d*q.b,
			a*q.d + b*q.c - c*q.b + d*q.a);
	}
	float dot(const quaternion& q)const
	{
		return q.a*a + q.b*b + q.c*c + q.d*d;
	}
	quaternion conjugation(void)const
	{
		return quaternion(this->a, -this->b, -this->c, -this->d);
	}
	quaternion normalize(void)const
	{
		float norm = this->normal();
		return quaternion(a / norm, b / norm, c / norm, d / norm);
	}
	quaternion inverse(void)const
	{
		return this->conjugation() / (a*a + b*b + c*c + d*d);
	}
	float normal(void)const
	{
		return sqrt(a*a + b*b + c*c + d*d);
	}
	float arg(void)const
	{
		return acos(a / this->normal());
	}
};
