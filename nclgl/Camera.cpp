#include "Camera.h"
#include "Window.h"
#include <algorithm>

void Camera::UpdateCamera(float dt) {
	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);
	float speed = 300.0f * dt;

	if (yaw < 0)
		yaw += 360.0f;
	if (yaw > 360.0f)
		yaw -= 360.0f;

	if (automated) {
		timeElapsed += dt;
		std::cout << timeElapsed << "\n";

		if (timeElapsed < 12.0f) { // Show Map
			position -= forward * speed;
		}
		else if (timeElapsed >= 12.0f && timeElapsed < 14.0f) {
			if (yaw < 90.0f)
				yaw += speed * 0.5f;
			else
				yaw = 90.0f;
		}
		else if (timeElapsed >= 14.0f && timeElapsed < 19.0f) {
			position += forward * speed;
		}
		else if (timeElapsed >= 19.0f && timeElapsed < 22.0f) {
			if (yaw > 1.0f)
				yaw -= speed * 0.5f;
			else
				yaw = 0.0f;
		}
		else if (timeElapsed >= 22.0f && timeElapsed < 36.0f) {
			position += forward * speed;
		}
		else if (timeElapsed >= 36.0f && timeElapsed < 39.0f) {
			if (yaw < 330.0f || yaw > 332.0f)
				yaw -= speed * 0.5f;
			else
				yaw = 330.0f;
		}
	}
	else {
		pitch -= (Window::GetMouse()->GetRelativePosition().y);
		yaw -= (Window::GetMouse()->GetRelativePosition().x);
		roll -= (Window::GetKeyboard()->KeyDown(KEYBOARD_Q));
		roll += (Window::GetKeyboard()->KeyDown(KEYBOARD_E));

		pitch = std::min(pitch, 90.0f);
		pitch = std::max(pitch, -90.0f);

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_W))
			position += forward * speed;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_S))
			position -= forward * speed;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_A))
			position -= right * speed;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_D))
			position += right * speed;

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT))
			position.y -= speed;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE))
			position.y += speed;
	}
}

Matrix4 Camera::BuildViewMatrix() {
	return	Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Rotation(-roll, Vector3(0, 0, 1)) *
		Matrix4::Translation(-position);
}