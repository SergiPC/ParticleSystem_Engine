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
	looping = false;
	startLifetime = 5.f;
	startSpeed = 5.f;
	startSize = 1.f;
	endSize = 1.f;
	gravityModifier = .0f;
	alpha = .0f;
	maxParticles = 1000;

	// Emission ----------
	rate = 100;
	angleShape = 15.0f;
	radiusShape = 1.0f;

	// Shape -------------
	shape = EmitterShape::Cone;

	// -------------------
	lastUsedParticle = 0;
	numParticlesAlive = 0;
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

	if (numParticlesAlive + newParticles > maxParticles)
		newParticles = maxParticles - numParticlesAlive;
	
	int tmpInt = 0;

	for (int i = 0; i < newParticles; i++)
	{
		tmpInt = FindUnusedParticle();
		EmitParticle(particlesContainer[tmpInt]);
	}
}

void ParticleSystem::EmitParticle(Particle& particle)
{
	// Sphere Emitter ----
	// Cone Emitter ------
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
	particle.pGravity = gravityModifier;
	particle.life = startLifetime;
	particle.pLifeTime = startLifetime;
	particle.pSize = startSize;
	particle.sSize = startSize;
	particle.eSize = endSize;

	numParticlesAlive++;
}

void ParticleSystem::UpdateNow(const float3& point, const float3& _up)
{
	// Emit particles ----
	EmitParticles();

	// Update particles --
	for (std::vector<Particle>::iterator it = particlesContainer.begin(); it != particlesContainer.end(); it++)
	{
		if (it->life > .0f)
		{
			it->life -= Time.dt;
			if (it->life > .0f)
			{
				// Gravity -----------
				float3 gravityEffect = float3(.0f, it->pGravity, .0f);
				it->speed += gravityEffect * Time.dt * 0.5f;
				it->position += it->speed * Time.dt;

				// Size --------------
				float incSize = ((it->eSize - it->sSize) / it->pLifeTime) * Time.dt;
				it->pSize += incSize;

				// GPU Buffer --------
			}

			else
			{
				numParticlesAlive--;
			}
		}
	}
}

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

//------------------------- Editor --------------------------------------------------------------------------------

void ParticleSystem::EditorContent()
{
	// Looping -----------
	ImGui::Checkbox("Looping", &looping);
	// Start Lifetime ----
	float tmpFloat = startLifetime;
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
	static float vecSize[2] = { startSize, endSize };
	if (ImGui::DragFloat2("start/end Size", vecSize, .01f))
	{
		startSize = vecSize[1];
		endSize = vecSize[2];
	}
	// Gravity Modifier --
	tmpFloat = gravityModifier;
	if (ImGui::DragFloat("Gravity Modifier", &tmpFloat, .01f))
	{
		gravityModifier = tmpFloat;
	}
	// Alpha -------------
	tmpFloat = alpha;
	if (ImGui::DragFloat("Alpha Particle", &tmpFloat, .01f, 0, 1))
	{
		alpha = tmpFloat;
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
		if (ImGui::DragFloat("Angle Shape", &tmpFloat, .01f, .0f, 90.0f))
		{
			angleShape = tmpFloat;
		}
		// Radius ------------
		tmpFloat = radiusShape;
		if (ImGui::DragFloat("Radius Shape", &tmpFloat, .01f, 0.01f, 10000.0f))
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