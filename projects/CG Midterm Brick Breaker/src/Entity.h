#pragma once

#include "entt.hpp"


class Entity
{
public:

	//Creates a new entity
	static Entity Create();

	//Constructor
	Entity(entt::entity ent);
	Entity();

	//Adds a component to an entity
	template<typename T, typename ... Args>
	T& Add(Args&&... arg)
	{
		return ECS.emplace<T>(entID, std::forward<Args>(arg)...);
	}

	//Gets a component from an entity to edit
	template<typename T>
	static T& Get();

	//Removes a component from an entity
	template<typename T>
	void Remove();

private:
	//Registry and entity ID for use with entities in main
	static entt::registry ECS;
	entt::entity entID;
};
