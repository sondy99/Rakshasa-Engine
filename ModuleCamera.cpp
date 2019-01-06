#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"

#include "GameObject.h"

#include "Component.h"
#include "ComponentCamera.h"

ModuleCamera::ModuleCamera()
{
}

// Destructor
ModuleCamera::~ModuleCamera()
{
}

// Called before render is available
bool ModuleCamera::Init()
{
	sceneCamera = new ComponentCamera(nullptr, ComponentType::CAMERA);
	selectedCamera = sceneCamera;

	selectedCamera->cameraPosition = math::float3(0.0f, 300.0f, 4000.0f);
	selectedCamera->InitFrustum(nullptr);
	
	return true;
}

update_status ModuleCamera::PreUpdate()
{
	if (viewPortIsFocused && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		clickOnViewPort = true;
	}
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_UP)
	{
		clickOnViewPort = false;
		selectedCamera->firstMouse = true;
	}

	if (clickOnViewPort)
	{
		CameraMovementKeyboard();
		CameraMovementMouse();

		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		{
			FocusObject(sceneCenter);
		}

		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN)
		{
			selectedCamera->cameraSpeed = selectedCamera->cameraSpeed * 3;
			selectedCamera->rotationSpeed = selectedCamera->rotationSpeed * 3;
		}
		else if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP)
		{
			selectedCamera->cameraSpeed = selectedCamera->cameraSpeed / 3;
			selectedCamera->rotationSpeed = selectedCamera->rotationSpeed / 3;
		}
	}

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleCamera::CleanUp()
{
	RELEASE(sceneCamera);
	selectedCamera = nullptr;

	CleanUpFromList(nullptr);

	return true;
}

void ModuleCamera::CleanUpFromList(ComponentCamera * componentCamera)
{
	for (std::list<ComponentCamera*>::iterator iterator = cameras.begin(); iterator != cameras.end();)
	{
		if (componentCamera == nullptr)
		{
			iterator = CleanUpIterator(iterator);
		}
		else
		{
			if (componentCamera == *iterator)
			{
				iterator = CleanUpIterator(iterator);
			}
			else
			{
				++iterator;
			}
		}
	}
}

std::list<ComponentCamera*>::iterator ModuleCamera::CleanUpIterator(std::list<ComponentCamera*>::iterator iterator)
{
	RELEASE(*iterator);
	return cameras.erase(iterator);
}

void ModuleCamera::SetScreenNewScreenSize(unsigned newWidth, unsigned newHeight)
{
	selectedCamera->SetScreenNewScreenSize(newWidth, newHeight);
}

void ModuleCamera::MoveCamera(CameraMovement cameraSide)
{
	float normCameraSpeed = selectedCamera->cameraSpeed * App->renderer->deltaTime;

	switch (cameraSide) {
	case UP:
		selectedCamera->cameraPosition += selectedCamera->cameraUp.Normalized() * normCameraSpeed;
		break;
	case DOWN:
		selectedCamera->cameraPosition -= selectedCamera->cameraUp.Normalized() * normCameraSpeed;
		break;
	case LEFT:
		selectedCamera->cameraPosition += selectedCamera->cameraUp.Cross(selectedCamera->cameraFront).Normalized() * normCameraSpeed;
		break;
	case RIGHT:
		selectedCamera->cameraPosition -= selectedCamera->cameraUp.Cross(selectedCamera->cameraFront).Normalized() * normCameraSpeed;
		break;
	case FORWARD:
		selectedCamera->cameraPosition += selectedCamera->cameraFront.Normalized() * normCameraSpeed;
		break;
	case BACKWARDS:
		selectedCamera->cameraPosition -= selectedCamera->cameraFront.Normalized() * normCameraSpeed;
		break;
	}
}

void ModuleCamera::RotateCamera(CameraMovement cameraSide)
{
	switch (cameraSide) {
	case UP:
		selectedCamera->pitch += selectedCamera->rotationSpeed * App->renderer->deltaTime;
		break;
	case DOWN:
		selectedCamera->pitch -= selectedCamera->rotationSpeed * App->renderer->deltaTime;
		break;
	case LEFT:
		selectedCamera->yaw -= selectedCamera->rotationSpeed * App->renderer->deltaTime;
		break;
	case RIGHT:
		selectedCamera->yaw += selectedCamera->rotationSpeed * App->renderer->deltaTime;
		break;
	}

	selectedCamera->pitch = math::Clamp(selectedCamera->pitch, -80.0f, 80.0f);

	math::float3 rotation;
	rotation.x = SDL_cosf(math::DegToRad(selectedCamera->yaw)) * SDL_cosf(math::DegToRad(selectedCamera->pitch));
	rotation.y = SDL_sinf(math::DegToRad(selectedCamera->pitch));
	rotation.z = SDL_sinf(math::DegToRad(selectedCamera->yaw)) * SDL_cosf(math::DegToRad(selectedCamera->pitch));
	selectedCamera->cameraFront = rotation.Normalized();
}

void ModuleCamera::MouseUpdate(const iPoint& mousePosition)
{
	if (selectedCamera->firstMouse) {
		selectedCamera->lastX = mousePosition.x;
		selectedCamera->lastY = mousePosition.y;
		selectedCamera->firstMouse = false;
	}

	float xoffset = mousePosition.x - selectedCamera->lastX;
	float yoffset = selectedCamera->lastY - mousePosition.y;
	selectedCamera->lastX = mousePosition.x;
	selectedCamera->lastY = mousePosition.y;

	xoffset *= selectedCamera->mouseSensitivity;
	yoffset *= selectedCamera->mouseSensitivity;

	selectedCamera->yaw += xoffset;
	selectedCamera->pitch += yoffset;

	selectedCamera->pitch = math::Clamp(selectedCamera->pitch, -80.0f, 80.0f);

	math::float3 rotation;
	rotation.x = SDL_cosf(math::DegToRad(selectedCamera->yaw)) * SDL_cosf(math::DegToRad(selectedCamera->pitch));
	rotation.y = SDL_sinf(math::DegToRad(selectedCamera->pitch));
	rotation.z = SDL_sinf(math::DegToRad(selectedCamera->yaw)) * SDL_cosf(math::DegToRad(selectedCamera->pitch));
	selectedCamera->cameraFront = rotation.Normalized();
}

void ModuleCamera::Zooming(bool positive)
{
	if (positive)
		selectedCamera->fovX += 10.0;
	else
		selectedCamera->fovX -= 10.0;

	selectedCamera->fovX = math::Clamp(selectedCamera->fovX, 0.0f, 100.0f);

	selectedCamera->zoomValue = 45.0f / selectedCamera->fovX;

	selectedCamera->SetHorizontalFOV(selectedCamera->fovX);
}

void ModuleCamera::FocusObject(math::float3& objectCenterPos)
{
	selectedCamera->firstMouse = true;
	selectedCamera->cameraFront = objectCenterPos - selectedCamera->cameraPosition;
	selectedCamera->pitch = math::RadToDeg(sinf(-selectedCamera->cameraFront.y));
	selectedCamera->yaw = math::RadToDeg(atan2f(selectedCamera->cameraFront.z, selectedCamera->cameraFront.x)) + 90.0f;
}

void ModuleCamera::CameraMovementMouse()
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		SDL_ShowCursor(SDL_DISABLE);
		MouseUpdate(App->input->GetMousePosition());
	}
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_UP)
	{
		SDL_ShowCursor(SDL_ENABLE);
		selectedCamera->firstMouse = true;
	}
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_X1) == KEY_DOWN)
	{
		Zooming(true);
	}
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_X2) == KEY_DOWN)
	{
		Zooming(false);
	}
}

void ModuleCamera::CameraMovementKeyboard()
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
		{
			MoveCamera(UP);
		}
		else if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
		{
			MoveCamera(DOWN);
		}
		else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			MoveCamera(LEFT);
		}
		else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			MoveCamera(RIGHT);
		}
		else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			MoveCamera(FORWARD);
		}
		else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			MoveCamera(BACKWARDS);
		}
		else if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		{
			RotateCamera(UP);
		}
		else if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			RotateCamera(DOWN);
		}
		else if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			RotateCamera(LEFT);
		}
		else if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			RotateCamera(RIGHT);
		}
	}
}

void ModuleCamera::DrawProperties()
{
	if (toggleCameraProperties)
	{
		ImGui::Begin("Camera", &toggleCameraProperties);
		if (ImGui::CollapsingHeader("Camera properties", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::NewLine();
			ImGui::PushItemWidth(75);
			ImGui::Text("Position:");
			ImGui::Text("X:");
			ImGui::SameLine();
			ImGui::PushID("1");
			ImGui::InputFloat("", &sceneCamera->cameraPosition.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Y:");
			ImGui::SameLine();
			ImGui::PushID("2");
			ImGui::InputFloat("", &sceneCamera->cameraPosition.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Z:");
			ImGui::SameLine();
			ImGui::PushID("3");
			ImGui::InputFloat("", &sceneCamera->cameraPosition.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::PopID();

			ImGui::PushItemWidth(75);
			ImGui::Text("Vector front:");
			ImGui::Text("X:");
			ImGui::SameLine();
			ImGui::PushID("4");
			ImGui::InputFloat("", &sceneCamera->cameraFront.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Y:");
			ImGui::SameLine();
			ImGui::PushID("5");
			ImGui::InputFloat("", &sceneCamera->cameraFront.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Z:");
			ImGui::SameLine();
			ImGui::PushID("6");
			ImGui::InputFloat("", &sceneCamera->cameraFront.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::PopID();

			ImGui::Text("Vector up:");
			ImGui::Text("X:");
			ImGui::SameLine();
			ImGui::PushID("7");
			ImGui::InputFloat("", &sceneCamera->cameraUp.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Y:");
			ImGui::SameLine();
			ImGui::PushID("8");
			ImGui::InputFloat("", &sceneCamera->cameraUp.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Z:");
			ImGui::SameLine();
			ImGui::PushID("9");
			ImGui::InputFloat("", &sceneCamera->cameraUp.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::PopID();

			ImGui::Separator();
			ImGui::PushItemWidth(200);
			ImGui::Text("Pitch:");
			ImGui::PushID("10");
			ImGui::InputFloat("##", &sceneCamera->pitch, 0, 0, 0);
			ImGui::PopID();
			ImGui::Text("Yaw:");
			ImGui::PushID("11");
			ImGui::InputFloat("##", &sceneCamera->yaw, 0, 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
		}
		if (ImGui::CollapsingHeader("Camera configurations", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PushItemWidth(200);
			ImGui::Text("Mov Speed:");
			ImGui::PushID("12");
			ImGui::SliderFloat("##", &sceneCamera->cameraSpeed, 10.0f, 10000.0f);
			ImGui::PopID();
			ImGui::Text("Mouse Sens:");
			ImGui::PushID("13");
			ImGui::SliderFloat("##", &sceneCamera->mouseSensitivity, 0.0f, 1.0f);
			ImGui::PopID();
			ImGui::Separator();
			ImGui::Text("Near Plane:");
			ImGui::PushID("14");
			ImGui::SliderFloat("##", &sceneCamera->frustum.nearPlaneDistance, 10.0f, sceneCamera->frustum.farPlaneDistance);
			ImGui::PopID();
			ImGui::Text("Far Plane:");
			ImGui::PushID("15");
			ImGui::SliderFloat("##", &sceneCamera->frustum.farPlaneDistance, 10.0f, 50000.0f);
			ImGui::PopID();
			ImGui::PopItemWidth();
		}
		ImGui::End();
	}
}

ComponentCamera* ModuleCamera::CreateComponentCamera(GameObject * gameObjectParent, ComponentType componentType)
{
	ComponentCamera* result = new ComponentCamera(gameObjectParent, componentType);
	cameras.push_back(result);

	return result;
}

void ModuleCamera::RemoveCameraComponent(Component * componentToBeRemove)
{
	if (componentToBeRemove->componentType == ComponentType::CAMERA)
	{
		cameras.remove((ComponentCamera*)componentToBeRemove);
	}
}
