#include "Camera.h"
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

void Camera::SetPerspective(float fov, float aspectR, float n, float f)
{
	projectionTransform= glm::perspective(fov, aspectR, n, f);
	perspectiveSet = true;

	UpdateProjectionViewTransform();
}

void Camera::SetLookAt(vec3 from, vec3 to, vec3 up)
{
	worldTransform = glm::inverse(glm::lookAt(from, to, up));
	UpdateProjectionViewTransform();
}

void Camera::SetPosition(vec3 pos)
{
	worldTransform[3] = glm::vec4(pos, 1);

	UpdateProjectionViewTransform();
}

void Camera::SetTransform(glm::mat4 transform)
{
	worldTransform = transform;
	UpdateProjectionViewTransform();
}

mat4 Camera::GetWorldTransform()
{
	return worldTransform;
}

mat4 Camera::GetView()
{
	return viewTransform;
}

mat4 Camera::GetProjection()
{
	return projectionTransform;
}

mat4 Camera::GetProjectionView()
{
	return projectionViewTransform;
}

void Camera::UpdateProjectionViewTransform()
{
	viewTransform = glm::inverse(worldTransform);
	projectionViewTransform = projectionTransform * viewTransform;
}

void Camera::SetIgnoreInput(bool ignore)
{
	ignoreInput = ignore;
}

glm::vec3 Camera::screenPositionToDirection(float x, float y) const {

	int width = 0, height = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);

	glm::vec3 screenPos(x / width * 2 - 1, (y / height * 2 - 1) * -1, -1);

	screenPos.x /= projectionTransform[0][0];
	screenPos.y /= projectionTransform[1][1];

	return glm::normalize(worldTransform * glm::vec4(screenPos, 0)).xyz();
}

glm::vec3 Camera::pickAgainstPlane(float x, float y, const glm::vec4& plane) const {

	int width = 0, height = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);

	glm::vec3 screenPos(x / width * 2 - 1, (y / height * 2 - 1) * -1, -1);

	screenPos.x /= projectionTransform[0][0];
	screenPos.y /= projectionTransform[1][1];

	glm::vec3 dir = glm::normalize(worldTransform * glm::vec4(screenPos, 0)).xyz();

	float d = (plane.w - glm::dot(worldTransform[3].xyz(), plane.xyz()) / glm::dot(dir, plane.xyz()));

	return worldTransform[3].xyz() + dir * d;
}