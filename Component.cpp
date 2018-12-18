#include "Globals.h"

#include "Component.h"

#include "GameObject.h"

void Component::DrawDeleteComponent()
{
	if (ImGui::Button("Delete"))
	{
		isMarkToBeDeleted = true;
	}
}