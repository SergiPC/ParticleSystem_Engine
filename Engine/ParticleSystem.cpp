#include "ParticleSystem.h"

#include "Globals.h"
#include "GameObject.h"
#include "Transform.h"
#include "Application.h"

#include "imGUI\imgui.h"
#include "OpenGL.h"

//------------------------- Particle System -----------------------------------------------------------------------

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

	// -------------------
	particleForce = float3(0, -9.81f, 0);
	TIMER_START("ParticleSystemTimer");
}

ParticleSystem::~ParticleSystem()
{
	if (particleTextureID != 0)
	{
		glDeleteTextures(1, &particleTextureID);
		particleTextureID = 0;
	}
}

void ParticleSystem::EmitParticles()
{
	// Generate new particule each millisecond depending of "rate" value,
	// but limit this to 16 ms (60 fps), or if you have 1 long frame (1sec),
	// newparticles will be huge and the next frame even longer.

	int newParticles = (int)(Time.dt*rate);
	if (newParticles > (int)(0.016f*rate))
		newParticles = (int)(0.016f*rate);

	if (numParticlesAlive + newParticles < maxParticles)
	{
		int tmpInt = 0;

		for (int i = 0; i < newParticles; i++)
		{
			tmpInt = FindUnusedParticle();
			EmitParticle(particlesContainer[tmpInt]);
		}
	}
}

void ParticleSystem::EmitParticle(Particle& particle)
{
	// Sphere Emitter --
	// Cone Emitter ----
	float polarAngle = angleShape - rand() % (int)angleShape*2;
	float azimuth = 180 - rand() % 360;
	float radius = (float)rand() / (float)RAND_MAX; // 0 to 1.

	float xVec = sinf(polarAngle) * cosf(azimuth);
	float yVec = sinf(polarAngle) * sinf(azimuth);
	float zVec = cosf(polarAngle);

	float3 unitVec = float3(xVec, yVec, zVec);

	Transform* trans = object->GetTransform();
	float3 origin = trans->GetGlobalPos();

	particle.position = origin;
	particle.speed = unitVec * startSpeed;
	particle.life = startLifetime;
}

void ParticleSystem::UpdateNow(const float3& point, const float3& _up)
{}

void ParticleSystem::RenderParticles()
{}

int ParticleSystem::FindUnusedParticle()
{
	for (int i = lastUsedParticle; i < maxParticles; i++)
	{
		if (particlesContainer[i].life <= 0)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < lastUsedParticle; i++)
	{
		if (particlesContainer[i].life <= 0)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one... I'm not sure about it.
}

bool ParticleSystem::LoadParticleTexture(const std::string& fileName)
{
	if (particleTextureID != 0)
	{
		glDeleteTextures(1, &particleTextureID);
	}

	//particleTextureID = SOIL_load_OGL_texture(fileName.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	return (particleTextureID != 0);
}

void ParticleSystem::Resize(unsigned int numParticles)
{}

void ParticleSystem::BuildBuffer()
{}

//------------------------- Randomize methods ---------------------------------------------------------------------

// Iterates through the particle buffer and randomly distributes the particle using the ParticleSystem::RandomizeParticle().
void ParticleSystem::RandomizeParticles()
{
	for (std::vector<Particle>::iterator it = particlesContainer.begin(); it != particlesContainer.end(); it++)
	{
		RandomizeParticle(*it);
	}
}

// This method will randomly distribute a single particle on a unit circle centered at the origin of the particle effect.
void ParticleSystem::RandomizeParticle(Particle& particle)
{
	particle.life = .0f;
	duration = rand() % 3 + 3; // in the range 3 to 6.

	float3 unitVec = float3((float) rand() / (float) RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX);

	particle.position = unitVec * 1.f;
	particle.speed = unitVec * (rand() % 10 + 10);
}

//------------------------- Editor --------------------------------------------------------------------------------

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
		// Angle -------------
		tmpFloat = angleShape;
		if (ImGui::DragFloat("Start Speed", &tmpFloat, .01f))
		{
			angleShape = tmpFloat;
		}
		// Radius ------------
		tmpFloat = radiusShape;
		if (ImGui::DragFloat("Start Speed", &tmpFloat, .01f))
		{
			radiusShape = tmpFloat;
		}
		ImGui::TreePop();
	}
}

void ParticleSystem::SaveSpecifics(pugi::xml_node& myNode)
{}

void ParticleSystem::LoadSpecifics(pugi::xml_node & myNode)
{}