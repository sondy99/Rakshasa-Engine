#include "ComponentCamera.h"

ComponentCamera::ComponentCamera()
{
	InitFrustum(nullptr);
}

ComponentCamera::ComponentCamera(GameObject* gameObjectParent, ComponentType componentType)
	: Component(gameObjectParent, componentType)
{
	InitFrustum(gameObjectParent);
}

ComponentCamera::~ComponentCamera()
{
}

void ComponentCamera::InitFrustum(GameObject* gameObjectParent)
{
	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.pos = float3::zero;
	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = gameObjectParent != nullptr ? 100.0f : 350.0f;

	SetVerticalFOV(fovY);
}

void ComponentCamera::SetHorizontalFOV(float& fovXDegrees)
{
	fovX = fovXDegrees;
	frustum.horizontalFov = math::DegToRad(fovX);
	frustum.verticalFov = 2.0f * atanf(tanf(frustum.horizontalFov * 0.5) * ((float)screenHeight / (float)screenWidth));
}

void ComponentCamera::SetVerticalFOV(float& fovYDegrees)
{
	fovY = fovYDegrees;
	frustum.verticalFov = math::DegToRad(fovY);
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * ((float)screenWidth / (float)screenHeight));
}

void ComponentCamera::DrawProperties()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		DrawDeleteComponent();

		if (ImGui::CollapsingHeader("Camera properties"))
		{
			float camPos[3] = { cameraPosition.x, cameraPosition.y, cameraPosition.z };
			ImGui::InputFloat3("Camera position", camPos, "%.3f");
			cameraPosition = math::float3(camPos[0], camPos[1], camPos[2]);
			float vectorFront[3] = { cameraFront.x, cameraFront.y, cameraFront.z };
			ImGui::InputFloat3("Vector front", vectorFront, "%.3f");
			cameraFront = math::float3(vectorFront[0], vectorFront[1], vectorFront[2]);
			float vectorUp[3] = { cameraUp.x, cameraUp.y, cameraUp.z };
			ImGui::InputFloat3("Vector up", vectorUp, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::Separator();
			ImGui::InputFloat("Pitch", &pitch, 0, 0, 0);
			ImGui::InputFloat("Yaw", &yaw, 0, 0, 0);
		}
		if (ImGui::CollapsingHeader("Camera configurations"))
		{
			ImGui::SliderFloat("Near Plane", &frustum.nearPlaneDistance, 0.1f, frustum.farPlaneDistance);
			ImGui::SliderFloat("Far Plane", &frustum.farPlaneDistance, 0.1f, 500.0f);
		}
	}
}

Component* ComponentCamera::Clone()
{
	ComponentCamera* result = new ComponentCamera();

	result->gameObjectParent = gameObjectParent;
	result->componentType = componentType;

	return result;
}

math::float4x4 ComponentCamera::ProjectionMatrix()
{
	return frustum.ProjectionMatrix();
}

math::float4x4 ComponentCamera::LookAt(math::float3& cameraPosition, math::float3& cameraFront, math::float3& cameraUp)
{
	cameraFront.Normalize();
	math::float3 cameraSide = cameraFront.Cross(cameraUp);
	cameraSide.Normalize();
	math::float3 auxCameraUp = cameraSide.Cross(cameraFront);

	frustum.pos = cameraPosition;
	frustum.front = cameraFront;
	frustum.up = auxCameraUp;

	return frustum.ViewMatrix();
}

void ComponentCamera::SetScreenNewScreenSize(unsigned width, unsigned height)
{
	screenWidth = width;
	screenHeight = height;
	screenRatio = (float)screenWidth / (float)screenHeight;

	SetHorizontalFOV(fovX);
	SetVerticalFOV(fovY);
}