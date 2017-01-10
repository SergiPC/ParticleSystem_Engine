#ifndef __PARTICLE_SYSTEM__
#define __PARTICLE_SYSTEM__

#include "Component.h"

// PS: ParticleSystem.h ---- 

enum EmitterShape
{
	Cone = 0,
	Sphere
};

class ParticleSystem : public Component
{
public:
	ParticleSystem(GameObject* linkedTo);
	
	void UpdateNow();

	void EditorContent();

	void SaveSpecifics(pugi::xml_node& myNode);
	void LoadSpecifics(pugi::xml_node & myNode);

	static Type GetType() { return Type::C_ParticleSystem; }

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
};

#endif
