#ifndef PHYSICS_H
#define PHYSICS_H

#include "EntityManager.h"


class Physics
{
public:

	static bool isCollision(std::shared_ptr<Entity> e1, std::shared_ptr<Entity> e2)
	{
		float minDist = e1->cShape->circle.getRadius() + e2->cShape->circle.getRadius();
		float xCh = e1->cTransform->pos.x - e2->cTransform->pos.x;
		float yCh = e1->cTransform->pos.y - e2->cTransform->pos.y;

		if ((minDist * minDist) >= ((xCh * xCh) + (yCh * yCh)))
		{
			return true;
		}
		return false;
	}

	static bool isBoundsCollisionX(std::shared_ptr<Entity> entity, float m_westBound, float m_eastBound)
	{
		if (entity->cTransform->pos.x < m_westBound || entity->cTransform->pos.x > m_eastBound)
		{
			return true;
		}
		return false;
	}

	static bool isBoundsCollisionY(std::shared_ptr<Entity> entity, float m_northBound, float m_southBound)
	{
		if (entity->cTransform->pos.y < m_northBound || entity->cTransform->pos.y > m_southBound)
		{
			return true;
		}
		return false;
	}
};

#endif //PHYSICS_H