#include <vector>
#include <iostream>
#include <map>

#include "Game.h"


int main()
{
	Game g("config.json");
	g.run();
}



/* example game engine
void GameEngine::mainLoop()
{
	m_entityManager.update();
	sUserInput();
	sMovement();
	sCollision();
	sRender();
	m_currentFrame++;
}*/

/*
example collision system
void sCollision()
{
	EntityVec bullets;
	EntityVec tiles;
	for (auto& b : bullets)
	{
		for (auto& t : tiles)
		{
			if (Physics::IsCollusion(b,t))
			{
				bullets.erase(b)
			}
		}
	}
}

example enemy spawn and collision checks
EntityManager EM;
void spawnEnemy()
{
	auto e = EM.addEntity("enemy");
	e->cTransform = std::make_shared<CTransform>(args);
	e->cShape = std::make_shared<CShape>(args);
}
void collisions()
{
	for (auto b : EM.getEntities("bullet"))
	{
		for (auto e : EM.getEntities("enemy"))
		{
			if (Physics::CheckCollusion(b, e))
			{
				b->destroy();
				e->destroy();
			}
		}
	}
}*/