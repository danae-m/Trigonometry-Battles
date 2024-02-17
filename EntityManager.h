#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <vector>
#include <map>
#include <iostream>

#include "Entity.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<std::string, EntityVec> EntityMap;

class EntityManager
{
	EntityVec m_entities;
	EntityVec m_toAdd;
	EntityVec m_toDelete;
	EntityMap m_entityMap;
	size_t m_totalEntities = 0;

public:
	EntityManager();
	void update();
	std::shared_ptr<Entity> addEntity(const std::string tag);
	void destroyEntity(std::shared_ptr<Entity>);
	const EntityVec& getEntities();
	const EntityVec& getEntities(const std::string& tag);
	const std::map<std::string, EntityVec>& getEntityMap(); //do this

};

#endif //ENTITYMANAGER_H