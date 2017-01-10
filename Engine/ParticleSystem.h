#ifndef __PARTICLE_SYSTEM__
#define __PARTICLE_SYSTEM__

#include "Component.h"

// PS: ParticleSystem.h ---- 

struct Particle
{
	Particle() : position(float3::zero), speed(float3::zero), color(float4::zero), angle(.0f), size(.0f), life(.0f)
	{}

	float3 position, speed;
	float4 color;
	float angle, size, life;
};

struct Vertex
{
	Vertex() : position(float3::zero), diffuseColor(float4::zero), textureCoor(float2::zero)
	{}
	
	float3 position;
	float4 diffuseColor;
	float2 textureCoor;
};

enum EmitterShape
{
	Cone = 0,
	Sphere
};

class ParticleSystem : public Component
{
public:
	ParticleSystem(GameObject* linkedTo);
	
	void EmitParticles();
	void UpdateNow();
	void RenderParticles();

	bool LoadParticleTexture(const std::string& fileName);
	void Resize(unsigned int numParticles);
	void BuildBuffer();

	void EditorContent();

	void SaveSpecifics(pugi::xml_node& myNode);
	void LoadSpecifics(pugi::xml_node & myNode);

	static Type GetType() { return Type::C_ParticleSystem; }
	
	std::vector<Particle> particlesContainer;
	std::vector<Vertex> vertexContainer;

private:
	// Global ------------
	float duration = .0f;
	bool looping = false;
	float startLifetime = .0f;
	float startSpeed = .0f;
	float startSize = .0f;
	float gravityModifier = .0f;
	int maxParticles = 0;
	// Emission ----------
	int rate = 0;
	// Shape -------------
	int shape = 0;
	// Texture -----------
	unsigned int particleTextureID = 0;

	// -------------------
	unsigned int particleBuffer = 0;
	unsigned int vertexBuffer = 0;
	float4x4 localToWorldMatrix;
	// Apply this force to every particle in the effect
	float3 particleForce = float3::zero;
};

#endif
