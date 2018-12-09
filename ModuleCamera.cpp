#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"
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

	selectedCamera->cameraPosition = math::float3(0.0f, 3.0f, 40.0f);
	selectedCamera->InitFrustum(nullptr);
	
	return true;
}

update_status ModuleCamera::PreUpdate()
{
	if (viewPortIsFocused)
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
	return true;
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
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
		if (ImGui::CollapsingHeader("Camera properties", ImGuiTreeNodeFlags_DefaultOpen))
		{
			float camPos[3] = { sceneCamera->cameraPosition.x, sceneCamera->cameraPosition.y, sceneCamera->cameraPosition.z };
			ImGui::InputFloat3("Camera position", camPos, "%.3f", ImGuiInputTextFlags_ReadOnly);
			float vectorFront[3] = { sceneCamera->cameraFront.x, sceneCamera->cameraFront.y, sceneCamera->cameraFront.z };
			ImGui::InputFloat3("Vector front", vectorFront, "%.3f", ImGuiInputTextFlags_ReadOnly);
			float vectorUp[3] = { sceneCamera->cameraUp.x, sceneCamera->cameraUp.y, sceneCamera->cameraUp.z };
			ImGui::InputFloat3("Vector up", vectorUp, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::Separator();
			ImGui::InputFloat("Pitch", &sceneCamera->pitch, 0, 0, 0);
			ImGui::InputFloat("Yaw", &sceneCamera->yaw, 0, 0, 0);
		}
		if (ImGui::CollapsingHeader("Camera configurations", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::SliderFloat("Mov Speed", &sceneCamera->cameraSpeed, 0.0f, 100.0f);
			ImGui::SliderFloat("Rot Speed", &sceneCamera->rotationSpeed, 0.0f, 100.0f);
			ImGui::SliderFloat("Mouse Sens", &sceneCamera->mouseSensitivity, 0.0f, 1.0f);
			ImGui::Separator();
			ImGui::SliderFloat("Near Plane", &sceneCamera->frustum.nearPlaneDistance, 0.1f, sceneCamera->frustum.farPlaneDistance);
			ImGui::SliderFloat("Far Plane", &sceneCamera->frustum.farPlaneDistance, 0.1f, 500.0f);
		}
		ImGui::End();
	}
}
