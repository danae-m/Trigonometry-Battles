#include "Vec2.h"

Vec2::Vec2() {}
Vec2::Vec2(float xin, float yin) : x(xin), y(yin)
{

}

//basic operations
Vec2 Vec2::operator + (const Vec2& v) const
{
	return Vec2(x + v.x, y + v.y);
}

Vec2 Vec2::operator - (const Vec2& v) const
{
	return Vec2(x - v.x, y - v.y);
}

Vec2 Vec2::operator * (const float s) const
{
	return Vec2(x * s, y * s);
}

Vec2 Vec2::operator / (const float d) const
{
	return Vec2(x / d, y / d);
}

//comparator operations
bool Vec2::operator == (const Vec2& v) const
{
	return (x == v.x) && (y == v.y);
}

bool Vec2::operator != (const Vec2& v) const
{
	return !((x == v.x) && (y == v.y));
}

//base changing operations
void Vec2::operator += (const Vec2& v)
{
	x += v.x;
	y += v.y;
}

void Vec2::operator -= (const Vec2& v)
{
	x -= v.x;
	y -= v.y;
}

void Vec2::operator *= (float s)
{
	x *= s;
	y *= s;
}

void Vec2::operator /= (float d)
{
	x /= d;
	y /= d;
}

float Vec2::dist(Vec2 v)
{
	float xCh = (x - v.x);
	float yCh = (y - v.y);
	return (xCh * xCh) + (yCh * yCh);
}

float Vec2::angle(Vec2 v)
{
	float xCh = (x - v.x);
	float yCh = (y - v.y);
	return atan2(yCh, xCh);
}
