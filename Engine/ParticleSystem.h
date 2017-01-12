#ifndef __PARTICLE_SYSTEM__
#define __PARTICLE_SYSTEM__

#include "Component.h"
#include "PerfTimer.h"

// PS: ParticleSystem.h ---- 

struct Particle
{
	Particle() : position(float3::zero), speed(float3::zero), color(float4::zero), pSize(.0f), sSize(.0f), eSize(.0f), life(.0f), pLifeTime(.0f), pGravity(.0f)
	{}

	float3 position, speed;
	float4 color;
	float pSize, sSize, eSize, life, pLifeTime, pGravity;
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
	~ParticleSystem();

	void EmitParticles();
	void EmitParticle(Particle& particle);
	void UpdateNow(const float3& point, const float3& up = float3::zero);
	void RenderParticles();

	int FindUnusedParticle();

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
	bool looping = false;
	float startLifetime = .0f;
	float startSpeed = .0f;
	float startSize = .0f;
	float endSize = .0f;
	float gravityModifier = .0f;
	float alpha = .0f;
	int maxParticles = 0;
	// Emission ----------
	int rate = 0;
	// Shape -------------
	int shape = 0;
	float angleShape = .0f;
	float radiusShape = .0f;
	// Texture -----------
	unsigned int particleTextureID = 0;

	// -------------------
	unsigned int particleBuffer = 0;
	unsigned int vertexBuffer = 0;
	float4x4 localToWorldMatrix;

	// -------------------
	int lastUsedParticle = 0;
	int numParticlesAlive = 0;
};

#endif
