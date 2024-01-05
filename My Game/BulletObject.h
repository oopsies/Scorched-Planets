#pragma once
#include "Object.h"
#include "PlanetObject.h"
#include "TankObject.h"

class CBulletObject : public CObject {
protected:
  int damage = 10;
  float explosion_radius = 0;
  eSpriteType explosion_sprite = WATER_SPRITE;
  eSoundType explosion_sound = EXPLOSION7_SOUND;

  CTankObject* owner = nullptr; ///< The tank that fired the bullet.

  bool is_phantom = false;

  float time_to_live = -1.0f; ///< Kills itself after this many seconds. -1 means it lives until it collides with something
  float time_created; ///< Compare current time with this value to see how many seconds the bullet has been alive for
  bool onDeathCompleted = false; ///< Has completed ondeath event, and will not occur again

  float bounces = 0; ///< Bounces for bouncing bullet (BULLET7)
  int timesShot = 0; ///< Times shot manually by player after shooting initial bullet (BULLET8)


public:
  CBulletObject(eSpriteType t, const Vector2& p); ///< Constructor.
  CBulletObject(eSpriteType t, const Vector2& p, XMFLOAT4 trail_color);

  int GetDamage() { return damage; };
  void move(); ///< Move the bullet object
  void kill(); ///< Kill the object like default, but additionally damage all players in the explosion radius.
  void kill(CPlanetObject* planet); ///< Kill the object like default, but additionally do whatever it needs to do to the planet terrain.
  void OnDeath(); ///< Do some action on bullet death, depending on the sprite
  void OnPlanetHit(CPlanetObject* planet); ///< Do some action on planet collision, depeneding on the sprite

  void Explode(); ///creates an explosion around the bullet

  void SetOwner(CTankObject* owner) { this->owner = owner; }
  CTankObject* GetOwner() { return owner; };

  void set_is_phantom(bool phantom) { is_phantom = phantom; };
  bool get_is_phantom() { return is_phantom; };
};

