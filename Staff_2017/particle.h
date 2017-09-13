#ifndef PARTICLE_H
#define PARTICLE_H
#include "effect.h"

// A small class to capture the behavior of a point moving along and accelerating/decelerating 
// With some limits like lifetime or destination
class particle : public effect
{
public:
	float location; // in frontbuffer
	float velocity; // in frontbuffer/second
	float acceleration; // in frontbuffer/s/s
	float lifetime; // in seconds
	float loclimit; // limit of location. If vel is positive,  then when loclimit is exceeded spark dies. if vel is neg, then when loc < loclimit, death
	bool active; // if yes, then animate and render are valid, otherwise not
	
	
  public:
  
	void Init()
	{
		location = 0.0f;
		loclimit = (float)NUM_LEDS;
		velocity = 0.0f;
		acceleration = 0.0f;
		lifetime = 0.0f;
		active = false;
	}

	String Identify()
	{
		return "particle";
	}

	void Reset()
	{
		active = false;
	}

	void Launch(float loc, float vel, float acc, float life)
	{
		location = loc;
		velocity = vel;
		acceleration = acc;
		lifetime = life;
		loclimit = (float)NUM_LEDS/2.0f;
		active = true;
	}

	// Basically just encapsulating the physics of linear motion
	void Animate(unsigned long mics)
	{
		if (!active) return;

		// Do all anims here in float seconds
		float dt = (float)mics/1000000.0f;

		 location += velocity * dt;
		 velocity += acceleration * dt;
		 lifetime -= dt;
		 if (lifetime <= 0.0f)
		 {
		 	active = false;
		 	return;
		 }
		 #if 0
		 if (velocity > 0.0f && location > loclimit)
		 {
		 	active = false;
		 	return;
		 }
		 if (velocity < 0.0f && location < loclimit)
		 {
		 	active = false;
		 	return;
		 }
		 #endif

	}

	void Render()
	{
		// Nothing to do here, we want the owners of the particles to decide how to render
	}

  
};
#endif
