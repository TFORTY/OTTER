#include "Transform.h"

	Transform::Transform()
	{
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		scale = glm::vec3(1.0f, 1.0f, 1.0f);
		rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		eulerRot = glm::vec3(0.0f);
		model = glm::mat4(1.0f);
	}

	glm::vec3 Transform::GetPosition()
	{
		return position;
	}

	float Transform::GetPositionX()
	{
		return position.x;
	}

	float Transform::GetPositionY()
	{
		return position.y;
	}

	float Transform::GetPositionZ()
	{
		return position.z;
	}

	glm::vec3 Transform::GetScale()
	{
		return scale;
	}

	glm::vec3& Transform::GetRotation()
	{
		return eulerRot;
	}


	glm::mat4 Transform::GetModelMatrix()
	{	
		return  glm::mat4(glm::translate(glm::mat4(1.0f), position) * glm::toMat4(glm::normalize(rotation)) * glm::scale(glm::mat4(1.0f), scale));
	}

	void Transform::SetPosition(glm::vec3 newPos)
	{
		position = newPos;
	}

	void Transform::SetPositionX(float xPos)
	{
		position.x = xPos;
	}

	void Transform::SetPositionY(float yPos)
	{
		position.y = yPos;
	}

	void Transform::SetPositionZ(float zPos)
	{
		position.z = zPos;
	}

	void Transform::SetScale(glm::vec3 newScale)
	{
		scale = newScale;
	}

	void Transform::SetScale(float x, float y, float z)
	{
		scale.x = x;
		scale.y = y;
		scale.z = z;
	}

	void Transform::SetRotation(float x, float y, float z)
	{
		eulerRot.x = x;
		eulerRot.y = y;
		eulerRot.z = z;

		rotation = glm::quat(glm::radians(eulerRot));
	}

	void Transform::SetRotation(glm::vec3 euler)
	{
		eulerRot = euler;

		rotation = glm::quat(glm::radians(eulerRot));
	}

	void Transform::SetRotationX(float newAngle)
	{
		eulerRot.x = newAngle;

		rotation = glm::quat(glm::radians(eulerRot));
	}

	void Transform::SetRotationY(float newAngle)
	{

		eulerRot.y = newAngle;

		rotation = glm::quat(glm::radians(eulerRot));
	}

	void Transform::SetRotationZ(float newAngle)
	{
		eulerRot.z = newAngle;

		rotation = glm::quat(glm::radians(eulerRot));
	}