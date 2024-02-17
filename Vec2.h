#ifndef VEC2_H
#define VEC2_H

#include <iostream>
#include <cmath>

class Vec2
{
public:
	float x = 0;
	float y = 0;

	Vec2();
	Vec2(float xin, float yin);

	Vec2 operator + (const Vec2& rhs) const;
	Vec2 operator - (const Vec2& rhs) const;
	Vec2 operator * (const float val) const;
	Vec2 operator / (const float val) const;

	bool operator == (const Vec2& rhs) const;
	bool operator != (const Vec2& rhs) const;

	void operator += (const Vec2& rhs);
	void operator -= (const Vec2& rhs);
	void operator *= (const float val);
	void operator /= (const float val);

	float dist(Vec2 v);
	float angle(Vec2 v);
};

#endif //VEC2_H