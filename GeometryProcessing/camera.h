#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <../../scene.h>

enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera
{
	
public:
	glm::vec3 m_position, m_front, m_up, m_worldUp = glm::vec3(0, 1, 0);
	float m_yaw = -90, m_pitch = 0;
	float m_movementSpeed = 0.05;
	float m_rotateSpeed = 0.05;

	Camera(glm::vec3 position = glm::vec3(0, 0, 3), glm::vec3 up = glm::vec3(0, 1, 0), glm::vec3 front = glm::vec3(0, 0, -1), float yaw = -90, float pitch = 0)
	{
		m_position = position;
		m_up = up;
		m_front = front;
		m_yaw = yaw;
		m_pitch = pitch;
		CalculateUpandFront();
	}

	void processMouseMovement(float xOffset, float yOffset, bool rangeLock = true)
	{
		m_yaw += xOffset * m_rotateSpeed;
		m_pitch += yOffset * m_rotateSpeed;

		if (rangeLock)
		{
			if (m_pitch < -89)
				m_pitch = -89;
			if (m_pitch > 89)
				m_pitch = 89;
			if (m_yaw < -179)
				m_yaw = -179;
			if (m_yaw > 179)
				m_yaw = 179;
		}
		CalculateUpandFront();
	}

	void processKeyboard(Camera_Movement movement)
	{
		float speed = m_movementSpeed;
		if (movement == FORWARD)
			m_position.z -= speed;
		if (movement == BACKWARD)
			m_position.z += speed;
		if (movement == LEFT)
			m_position.x -= speed;
		if (movement == RIGHT)
			m_position.x += speed;
	}

	glm::mat4 GetCameraMatrix()
	{
		return glm::lookAt(m_position, m_position + m_front, m_up);
	}
	//~Camera();

private:
	void CalculateUpandFront()
	{
		m_front.x += cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		m_front.y += sin(glm::radians(m_pitch));
		m_front.z += sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		m_front = glm::normalize(m_front);
		glm::vec3 right = glm::normalize(glm::cross(m_front, m_worldUp));
		m_up = glm::normalize(glm::cross(right, m_front));
	}
};

#endif // !CAMERA_H
