#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "Vec2.h"
#include <SFML/Graphics.hpp>


class CTransform
{
public:
	Vec2 pos = { 0.0, 0.0 };
	Vec2 velocity = { 0.0, 0.0 };
	int speed = 0;
	float angle = 0.0f;

	CTransform(Vec2 p, int s, float a) : pos(p), speed(s), angle(a) {}
	CTransform(Vec2 p, Vec2 v, float a) : pos(p), velocity(v), angle(a) {}
	CTransform(Vec2 p, Vec2 v, int s, float a) : pos(p), velocity(v), speed(s), angle(a) {}

	Vec2 getPosition()
	{
		return pos;
	};
};

class CShape
{
public:
	sf::CircleShape circle;

	CShape(float radius, int points, const sf::Color fill, const sf::Color outline, float thickness) : circle(radius, points)
	{
		circle.setFillColor(fill);
		circle.setOutlineColor(outline);
		circle.setOutlineThickness(thickness);
		circle.setOrigin(radius, radius);
	}
};

class CRect
{
public:
	sf::RectangleShape rect;

	CRect(float width, float height, const sf::Color fill, const sf::Color outline, float thickness) : rect(sf::Vector2f{ width, height })
	{
		rect.setFillColor(fill);
		rect.setOutlineColor(outline);
		rect.setOutlineThickness(thickness);
	}
};

class CClicked
{
public:
	bool click = false;
	CClicked(bool c) : click(c) {}

	void setClicked()
	{
		click = true;
	}
};

class CScore
{
public:
	int score = 0;
	CScore(int s) : score(s) {}

	void incScore(int v)
	{
		score += v;
	}
};

class CLifespan
{
public:
	int remaining = 0;
	int total = 0;
	CLifespan(int total) : remaining(total), total(total) {}

	void decRemaining()
	{
		remaining -= 1;
	}
};

class CInput
{
public:
	bool up = false;
	bool left = false;
	bool right = false;
	bool down = false;
	bool shoot = false;

	CInput() {}
};

#endif //COMPONENTS_H
