#include "ParticleSystem.h"

#include "GameObject.h"
#include "Transform.h"
#include "Application.h"

#include "imGUI\imgui.h"

// PS: ParticleSystem.cpp ---- 
ParticleSystem::ParticleSystem(GameObject* linkedTo) : Component(linkedTo, C_ParticleSystem)
{
	char tmp[NAME_MAX_LEN];
	sprintf(tmp, "Particle System##%i", uid);
	name = tmp;

	// Global ------------
	duration = 5.f;
	looping = false;
	startLifetime = 5.f;
	startSpeed = 5.f;
	startSize = 1.f;
	gravityModifier = .0f;
	maxParticles = 100;

	// Emission ----------
	rate = 10;

	// Shape -------------
	shape = EmitterShape::Cone;
}

void ParticleSystem::EmitParticles()
{}

void ParticleSystem::UpdateNow()
{}

void ParticleSystem::RenderParticles()
{}

bool ParticleSystem::LoadParticleTexture(const std::string& fileName)
{
	return true;
}

void ParticleSystem::Resize(unsigned int numParticles)
{}

void ParticleSystem::BuildBuffer()
{}

void ParticleSystem::EditorContent()
{
	// Duration ----------
	float tmpFloat = duration;
	if(ImGui::DragFloat("Duration", &tmpFloat, .01f))
	{
		duration = tmpFloat;
	}
	// Looping -----------
	ImGui::Checkbox("Looping", &looping);
	// Start Lifetime ----
	tmpFloat = startLifetime;
	if (ImGui::DragFloat("Start Lifetime", &tmpFloat, .01f))
	{
		startLifetime = tmpFloat;
	}
	// Start Speed -------
	tmpFloat = startSpeed;
	if (ImGui::DragFloat("Start Speed", &tmpFloat, .01f))
	{
		startSpeed = tmpFloat;
	}
	// Start Size --------
	tmpFloat = startSize;
	if (ImGui::DragFloat("Start Size", &tmpFloat, .01f))
	{
		startSize = tmpFloat;
	}
	// Gravity Modifier --
	tmpFloat = gravityModifier;
	if (ImGui::DragFloat("Gravity Modifier", &tmpFloat, .01f))
	{
		gravityModifier = tmpFloat;
	}
	// Max particles -----
	int tmpInt = maxParticles;
	if (ImGui::DragInt("Max Particles", &tmpInt, 1))
	{
		maxParticles = tmpInt;
	}

	// Emission ----------
	if (ImGui::TreeNode("Emission"))
	{
		tmpInt = rate;
		if (ImGui::DragInt("Rate/Time", &tmpInt, 1, 0, maxParticles))
		{
			rate = tmpInt;
		}

		ImGui::TreePop();
	}

	// Shape -------------
	if(ImGui::TreeNode("Shape"))
	{
		//ImGui::Text("Shape Type");
		int shapeType = shape;
		int prevShapeType = shapeType;
		ImGui::RadioButton("Cone", &shapeType, EmitterShape::Cone); ImGui::SameLine();
		ImGui::RadioButton("Sphere", &shapeType, EmitterShape::Sphere);
		if (shapeType != prevShapeType)
		{
			shape = shapeType;
		}

		ImGui::TreePop();
	}
}

void ParticleSystem::SaveSpecifics(pugi::xml_node& myNode)
{}

void ParticleSystem::LoadSpecifics(pugi::xml_node & myNode)
{}