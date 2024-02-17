#ifndef UTILITY_H
#define UTILITY_H

#include <SFML/Graphics.hpp>

class Indicator : public sf::Drawable
{
public:

	Indicator() {}
	Indicator(const Indicator& other) : box(other.box), emptyFill(other.emptyFill), fillBar(other.fillBar) {}
	Indicator& operator=(const Indicator& other)
	{
		if (this != &other)
		{
			box = other.box;
			emptyFill = other.emptyFill;
			fillBar = other.fillBar;
		}
		return *this;
	}
	~Indicator() {}

	Indicator(sf::RectangleShape* b, sf::RectangleShape* e, sf::RectangleShape* f) : box(b), emptyFill(e), fillBar(f)
	{
		box->setPosition(20, 40);
		box->setFillColor(sf::Color{ 200, 200, 200 });
		box->setOutlineThickness(2.0f);
		box->setOutlineColor(sf::Color{ 150, 150, 150 });

		emptyFill->setPosition(30, 45);
		emptyFill->setFillColor(sf::Color{ 100, 100, 100, 100 });
		emptyFill->setOutlineThickness(2.0f);
		emptyFill->setOutlineColor(sf::Color{ 150, 150, 150 });

		fillBar->setPosition(30, 45);
		fillBar->setFillColor(sf::Color::Red);
		fillBar->setOutlineThickness(0.0f);
		fillBar->setOutlineColor(sf::Color::Black);
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		target.draw(*box, states);
		target.draw(*emptyFill, states);
		target.draw(*fillBar, states);
	}
	
	

private:
	sf::RectangleShape* box;
	sf::RectangleShape* emptyFill;
	sf::RectangleShape* fillBar;
};


#endif //UTILITY_H