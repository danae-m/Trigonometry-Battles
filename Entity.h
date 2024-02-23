#ifndef ENTITY_H
#define ENTITY_H

#include "Components.h"


class Entity
{
	size_t m_id;
	std::string m_tag;
	bool m_alive = true;

	friend class EntityManager;

	Entity();
	Entity(const std::string tag, size_t id);

public:
	std::shared_ptr<CTransform> cTransform = nullptr;
	std::shared_ptr<CShape> cShape = nullptr;
	std::shared_ptr<CClicked> cClicked = nullptr;
	std::shared_ptr<CInput> cInput = nullptr;
	std::shared_ptr<CScore> cScore = nullptr;
	std::shared_ptr<CLifespan> cLifespan = nullptr;
	std::shared_ptr<CRect> cRect = nullptr;

	void destroy();
	const bool isAlive();
	const std::string& tag() const;
	const size_t id() const;

};

#endif //ENTITY_H
