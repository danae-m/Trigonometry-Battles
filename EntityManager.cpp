#include "EntityManager.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<std::string, EntityVec> EntityMap;

EntityManager::EntityManager() {};

std::shared_ptr<Entity> EntityManager::addEntity(const std::string tag)
{
	auto e = std::shared_ptr<Entity>(new Entity(tag, m_totalEntities++));
	m_toAdd.push_back(e);
	return e;
}

void EntityManager::destroyEntity(std::shared_ptr<Entity> e)
{
	if (e->isAlive())
	{
		e->destroy();
		m_toDelete.push_back(e);
	}
}

void EntityManager::update()
{
	for (auto e : m_toDelete)
	{
		auto it = std::find(m_entities.begin(), m_entities.end(), e);
		if (it != m_entities.end())
		{
			m_entities.erase(it);
		}
			
		auto it2 = std::find(m_entityMap[e->tag()].begin(), m_entityMap[e->tag()].end(), e);
		if (it2 != m_entityMap[e->tag()].end())
		{
			m_entityMap[e->tag()].erase(it2);
		}
	}
	for (auto e : m_toAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}

	m_toAdd.clear();
	m_toDelete.clear();
}

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
	return m_entityMap[tag];
}

const std::map<std::string, EntityVec>& EntityManager::getEntityMap()
{
	return m_entityMap;
}
