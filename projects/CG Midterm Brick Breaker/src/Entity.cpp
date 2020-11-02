#include "Entity.h"


entt::registry Entity::ECS;

Entity::Entity(entt::entity ent)
{
	entID = ent;
}

Entity::Entity()
{
	entID = ECS.create();
}

Entity Entity::Create()
{
	entt::entity entity = ECS.create();

	return Entity(entity);
}

template<typename T>
T& Entity::Get()
{
	return ECS.get<T>(entID);
}

template<typename T>
void Entity::Remove()
{
	ECS.remove<T>(entID);
}