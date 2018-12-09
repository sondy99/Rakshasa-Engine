#include "ComponentTransformation.h"

#include "GameObject.h"

#include "MathGeoLib/include/Geometry/AABB.h"

ComponentTransformation::ComponentTransformation()
{
}

ComponentTransformation::ComponentTransformation(GameObject* gameObjectParent,
	ComponentType componentType, float3 position, float3 scale, Quat rotation) :
	Component(gameObjectParent, componentType),
	position(position), scale(scale), rotation(rotation)
{
	UpdateLocalModelMatrix();
}

ComponentTransformation::~ComponentTransformation()
{
}

void ComponentTransformation::UpdateLocalModelMatrix()
{
	if (!identity)
	{
		localModelMatrix = float4x4::FromTRS(position, rotation, scale);
	} 
	else
	{
		localModelMatrix = float4x4::identity;
	}
}

void ComponentTransformation::DrawProperties()
{
	bool changed = false;
	if (ImGui::CollapsingHeader("Transformation"))
	{
		UpdateLocalModelMatrix();

		ImGui::Checkbox("Watch as model identity", &identity);
		
		if (ImGui::Button("Transform to model identity"))
		{
			localModelMatrix = math::float4x4::identity;
			position = { 0.0f,0.0f,0.0f };
			scale = { 1.0f,1.0f,1.0f };
			rotation = { 0.0f,0.0f,0.0f,1.0f };
		}

		ImGui::NewLine();
		ImGui::PushItemWidth(75);
		ImGui::Text("Position:");
		ImGui::Text("X:");
		ImGui::SameLine();
		ImGui::PushID("1");
		if (ImGui::InputFloat("", &position.x,
			0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
		{
			changed = true;
		}
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Y:");
		ImGui::SameLine();
		ImGui::PushID("2");
		if (ImGui::InputFloat("", &position.y,
			0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
		{
			changed = true;
		}
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Z:");
		ImGui::SameLine();
		ImGui::PushID("3");
		if (ImGui::InputFloat("", &position.z,
			0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
		{
			changed = true;
		}
		ImGui::PopID();

		math::float3 auxRotation = rotation.ToEulerXYZ();
		auxRotation *= 57.295779513082320876f;
		ImGui::Text("Rotation:");
		ImGui::Text("X:");
		ImGui::SameLine();
		ImGui::PushID("4");
		if (ImGui::InputFloat("", &auxRotation.x, 0.0f, 0.0f, "%.3f",
			ImGuiInputTextFlags_EnterReturnsTrue))
		{
			changed = true;
		}
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Y:");
		ImGui::SameLine();
		ImGui::PushID("5");
		if (ImGui::InputFloat("", &auxRotation.y, 0.0f, 0.0f, "%.3f",
			ImGuiInputTextFlags_EnterReturnsTrue))
		{
			changed = true;
		}
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Z:");
		ImGui::SameLine();
		ImGui::PushID("6");
		if (ImGui::InputFloat("", &auxRotation.z, 0.0f, 0.0f, "%.3f",
			ImGuiInputTextFlags_EnterReturnsTrue))
		{
			changed = true;
		}
		ImGui::PopID();
		auxRotation *= 0.0174532925199432957f;
		rotation = rotation.FromEulerXYZ(auxRotation.x, auxRotation.y, auxRotation.z);

		ImGui::Text("Scale:");
		ImGui::Text("X:");
		ImGui::SameLine();
		ImGui::PushID("7");
		if (ImGui::InputFloat("", &scale.x, 0.0f, 0.0f, "%.3f",
			ImGuiInputTextFlags_EnterReturnsTrue))
		{
			changed = true;
		}
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Y:");
		ImGui::SameLine();
		ImGui::PushID("8");
		if (ImGui::InputFloat("", &scale.y, 0.0f, 0.0f, "%.3f",
			ImGuiInputTextFlags_EnterReturnsTrue))
		{
			changed = true;
		}
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Z:");
		ImGui::SameLine();
		ImGui::PushID("9");
		if (ImGui::InputFloat("", &scale.z, 0.0f, 0.0f, "%.3f",
			ImGuiInputTextFlags_EnterReturnsTrue))
		{
			changed = true;
		}
		ImGui::PopID();
		ImGui::PopItemWidth();

		if (changed)
		{
			localModelMatrix.Set(float4x4::FromTRS(position, rotation, scale));
		}
	}
}

Component* ComponentTransformation::Clone()
{
	ComponentTransformation* result = new ComponentTransformation();

	result->componentType = componentType;
	result->position = position;
	result->scale = scale;
	result->rotation = rotation;
	result->identity = identity;

	result->localModelMatrix = localModelMatrix;
	result->globalModelMatrix = globalModelMatrix;

	return result;
}
