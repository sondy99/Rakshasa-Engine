#include "ComponentCamera.h"

#include "GameObject.h"

#include "Config.h"

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

void ComponentCamera::InitFrustum(const GameObject* gameObjectParent)
{
	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.pos = float3::zero;
	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;
	frustum.nearPlaneDistance = 10.0f;
	frustum.farPlaneDistance = gameObjectParent != nullptr ? 10000.0f : 35000.0f;

	SetHorizontalFOV(fovX);
}

void ComponentCamera::SetHorizontalFOV(float& fovXDegrees)
{
	fovX = fovXDegrees;
	frustum.horizontalFov = math::DegToRad(fovX);
	frustum.verticalFov = 2.0f * atanf(tanf(frustum.horizontalFov * 0.5) * ((float)screenHeight / (float)screenWidth));
}

void ComponentCamera::DrawProperties()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::Button("Camera options");

		if (ImGui::IsItemClicked(0))
		{
			ImGui::OpenPopup("CameraOptionsContextualMenu");
		}

		if (ImGui::BeginPopup("CameraOptionsContextualMenu"))
		{
			ImGui::PushID("DeleteCamera");
			if (ImGui::Button("Delete"))
			{
				isMarkToBeDeleted = true;
			}
			ImGui::PopID();
			ImGui::EndPopup();
		}

		ImGui::Separator();

		if (ImGui::CollapsingHeader("Camera properties"))
		{
			ImGui::NewLine();
			ImGui::PushItemWidth(75);
			ImGui::Text("Position:");
			ImGui::Text("X:");
			ImGui::SameLine();
			ImGui::PushID("10");
			ImGui::InputFloat("", &cameraPosition.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Y:");
			ImGui::SameLine();
			ImGui::PushID("11");
			ImGui::InputFloat("", &cameraPosition.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Z:");
			ImGui::SameLine();
			ImGui::PushID("12");
			ImGui::InputFloat("", &cameraPosition.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::PopID();

			ImGui::PushItemWidth(75);
			ImGui::Text("Vector front:");
			ImGui::Text("X:");
			ImGui::SameLine();
			ImGui::PushID("13");
			ImGui::InputFloat("", &cameraFront.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Y:");
			ImGui::SameLine();
			ImGui::PushID("14");
			ImGui::InputFloat("", &cameraFront.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Z:");
			ImGui::SameLine();
			ImGui::PushID("15");
			ImGui::InputFloat("", &cameraFront.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::PopID();

			ImGui::Text("Vector up:");
			ImGui::Text("X:");
			ImGui::SameLine();
			ImGui::PushID("16");
			ImGui::InputFloat("", &cameraUp.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Y:");
			ImGui::SameLine();
			ImGui::PushID("17");
			ImGui::InputFloat("", &cameraUp.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Z:");
			ImGui::SameLine();
			ImGui::PushID("18");
			ImGui::InputFloat("", &cameraUp.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::PopID();

			ImGui::Separator();
			ImGui::Text("Pitch:");
			ImGui::PushID("19");
			ImGui::InputFloat("##", &pitch, 0, 0, 0);
			ImGui::PopID();
			ImGui::Text("Yaw:");
			ImGui::PushID("20");
			ImGui::InputFloat("##", &yaw, 0, 0, 0);
			ImGui::PopID();
		}
		if (ImGui::CollapsingHeader("Camera configurations"))
		{
			ImGui::PushItemWidth(200);
			ImGui::Text("Near Plane:");
			ImGui::PushItemWidth(100);
			ImGui::PushID("21");
			ImGui::SliderFloat("##", &frustum.nearPlaneDistance, 10.0f, frustum.farPlaneDistance);
			ImGui::PopID();
			ImGui::Text("Far Plane:");
			ImGui::PushID("22");
			ImGui::SliderFloat("##", &frustum.farPlaneDistance, 10.0f, 50000.0f);
			ImGui::PopID();
			ImGui::PopItemWidth();
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

void ComponentCamera::Save(const Config * config)
{
	config->StartObject();
	
	config->AddComponentType("componentType", componentType);

	if (gameObjectParent != nullptr)
	{
		config->AddString("gameObjectParent", gameObjectParent->uuid);
	}

	config->AddFloat("frustum.nearPlaneDistance", frustum.nearPlaneDistance);
	config->AddFloat("frustum.farPlaneDistance", frustum.farPlaneDistance);
	config->AddFloat3("cameraPosition", cameraPosition);
	config->AddFloat3("cameraFront", cameraFront);
	config->AddFloat3("cameraUp", cameraUp);
	config->AddFloat("pitch", pitch);
	config->AddFloat("yaw", yaw);
	
	config->EndObject();
}

void ComponentCamera::Load(const Config* config, rapidjson::Value& value)
{
	frustum.nearPlaneDistance = config->GetFloat("frustum.nearPlaneDistance", value);
	frustum.farPlaneDistance = config->GetFloat("frustum.farPlaneDistance", value);
	cameraPosition = config->GetFloat3("cameraPosition", value);
	cameraFront = config->GetFloat3("cameraFront", value);
	cameraUp = config->GetFloat3("cameraUp", value);
	pitch = config->GetFloat("pitch", value);
	yaw = config->GetFloat("yaw", value);
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
}