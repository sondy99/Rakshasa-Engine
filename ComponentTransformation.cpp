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

void ComponentTransformation::RotationToEuler()
{
	eulerRotation = rotation.ToEulerXYZ();
	eulerRotation.x = math::RadToDeg(eulerRotation.x);
	eulerRotation.y = math::RadToDeg(eulerRotation.y);
	eulerRotation.z = math::RadToDeg(eulerRotation.z);
}

void ComponentTransformation::DrawProperties()
{
	bool changed = false;
	if (ImGui::CollapsingHeader("Transformation"))
	{
		UpdateLocalModelMatrix();

		ImGui::Button("Transformation options");

		if (ImGui::IsItemClicked(0))
		{
			ImGui::OpenPopup("TransformationOptionsContextualMenu");
		}

		if (ImGui::BeginPopup("TransformationOptionsContextualMenu"))
		{
			ImGui::Checkbox("Watch as model identity", &identity);

			if (ImGui::Button("Transform to model identity"))
			{
				localModelMatrix = math::float4x4::identity;
				position = { 0.0f, 0.0f, 0.0f };
				eulerRotation = { 0.0f, 0.0f, 0.0f };
				scale = { 1.0f, 1.0f, 1.0f };
				rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
			}
			ImGui::EndPopup();
		}

		ImGui::Separator();

		ImGui::PushItemWidth(75);
		ImGui::Text("Position:");
		ImGui::Text("X:");
		ImGui::SameLine();
		ImGui::PushID("1");
		if (ImGui::DragFloat("", &position.x, 10.0f, -100000.f, 100000.f))
		{
			changed = true;
		}
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Y:");
		ImGui::SameLine();
		ImGui::PushID("2");
		if (ImGui::DragFloat("", &position.y, 10.0f, -100000.f, 100000.f))
		{
			changed = true;
		}
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Z:");
		ImGui::SameLine();
		ImGui::PushID("3");
		if (ImGui::DragFloat("", &position.z, 10.0f, -100000.f, 100000.f))
		{
			changed = true;
		}
		ImGui::PopID();

		ImGui::Text("Rotation:");
		ImGui::Text("X:");
		ImGui::SameLine();
		ImGui::PushID("4");
		if (ImGui::DragFloat("", &eulerRotation.x, 0.5f, -180.f, 180.f))
		{
			changed = true;
		}
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Y:");
		ImGui::SameLine();
		ImGui::PushID("5");
		if (ImGui::DragFloat("", &eulerRotation.y, 0.5f, -180.f, 180.f))
		{
			changed = true;
		}
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Z:");
		ImGui::SameLine();
		ImGui::PushID("6");
		if (ImGui::DragFloat("", &eulerRotation.z, 0.5f, -180.f, 180.f))
		{
			changed = true;
		}
		ImGui::PopID();
		rotation = rotation.FromEulerXYZ(math::DegToRad(eulerRotation.x),
			math::DegToRad(eulerRotation.y), math::DegToRad(eulerRotation.z));

		ImGui::Text("Scale:");
		ImGui::Text("X:");
		ImGui::SameLine();
		ImGui::PushID("7");
		if (ImGui::DragFloat("", &scale.x, 10.0f, 10.0f, 100000.f))
		{
			changed = true;
		}
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Y:");
		ImGui::SameLine();
		ImGui::PushID("8");
		if (ImGui::DragFloat("", &scale.y, 10.0f, 10.0f, 100000.f))
		{
			changed = true;
		}
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Z:");
		ImGui::SameLine();
		ImGui::PushID("9");
		if (ImGui::DragFloat("", &scale.z, 10.0f, 10.0f, 100000.f))
		{
			changed = true;
		}
		ImGui::PopID();
		ImGui::PopItemWidth();

		if (changed)
		{
			identity = false;
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
