#include "Entity.h"


Entity::Entity(std::string tag, size_t id)
{
	m_tag = tag;
	m_id = id;
}

const bool Entity::isAlive()
{
	if (m_alive)
	{
		return true;
	}
	return false;
}

const std::string& Entity::tag() const
{
	return m_tag;
}

void Entity::destroy()
{
	m_alive = false;
}

const size_t Entity::id() const
{
	return m_id;
}
