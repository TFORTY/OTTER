#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtx/quaternion.hpp"

	class Transform
	{
	public:

		//Constructor
		Transform();

		//Position-Getters
		glm::vec3 GetPosition();
		float GetPositionX();
		float GetPositionY();
		float GetPositionZ();

		//Scale-Getters
		glm::vec3 GetScale();

		//Rotation-Getters
		glm::vec3& GetRotation();

		//Position-Setters
		void SetPosition(glm::vec3 newPos);
		void SetPositionX(float xPos);
		void SetPositionY(float yPos);
		void SetPositionZ(float zPos);

		//Scale-Setters
		void SetScale(glm::vec3 newScale);
		void SetScale(float x, float y, float z);

		//Rotation-Setters
		void SetRotation(float x, float y, float z);
		void SetRotation(glm::vec3 euler);
		void SetRotationX(float newAngle);
		void SetRotationY(float newAngle);
		void SetRotationZ(float newAngle);

		//Gets the model matrix
		glm::mat4 GetModelMatrix();


	private:
		glm::vec3 position;
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 eulerRot;
		glm::mat4 model;
	};