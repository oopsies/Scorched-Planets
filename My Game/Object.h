/// \file Object.h
/// \brief Interface for the game object class CObject.

#pragma once
#ifndef OBJECT_H
#define OBJECT_H



#include "GameDefines.h"
#include "Renderer.h"
#include "Common.h"
#include "Component.h"
#include "SpriteDesc.h"

/// \brief The game object. 
///
/// CObject is the abstract representation of an object.

class CObject:
  public CCommon,
  public CComponent,
  public CSpriteDesc2D
{
  friend class CObjectManager;

  private:

protected:

    float m_fGunTimer = 0; ///< Gun fire timer.
    double mass = 0;
    bool affected_by_gravity = FALSE;
    float m_fSmokeTimer = 0.0f; ///< Last time smoke was emitted.
    XMFLOAT4 smoke_color;

    Vector2 m_vRadius; ///< Half width and height of object sprite.
    float m_fSpeed = 0; ///< Speed.
    float m_fRotSpeed = 0; ///< Rotational speed.
    Vector2 m_vOldPos; ///< Last position.
    Vector2 m_vVelocity; ///< Velocity.
    Vector2 m_vAcceleration;
    bool m_bDead = false; ///< Is dead or not.

    bool m_bStrafeLeft = false; ///< Strafe left.
    bool m_bStrafeRight = false; ///< Strafe right.
    bool m_bStrafeBack = false; ///< Strafe back.

    BoundingSphere m_Sphere; ///< Bounding sphere.

    bool is_bullet = false;

    float m_vOldEnergy = -1; ///< Initial total mechanical energy of the object. -1 means that the energy was never calculated. Used for debugging physics precision.

    //C++ has this annoying "feature" where the modulus of a negative number can still be negative. Adding the quotient once then taking the modulus fixes this.
    //This is a member function because the linker keeps yelling at me. I don't like it when the linker is upset.
    int modulo(int a, int b) { return (a % b + b) % b; };
    float modulo(float a, float b) { return (float) fmod( (float) fmod(a, b) + b, b); };
    float modulo(int a, float b) { return (float) modulo((float)a, b); };



  public:
    CObject(eSpriteType t, const Vector2& p); ///< Constructor.

    virtual void move(); ///< Move object.

    virtual void kill(); ///< Kill me.
    bool IsDead(); ///< Query whether dead.
    void DeathFX(); ///< Death special effects.
    void CollisionResponse(); ///< Collision response.
    void CollisionReflectionResponse(Vector2 unit_normal); ///< Instead of stopping the object, it reflects the object following Snell's law.
    
    void SetSpeed(float speed); ///< Set speed.
    float GetSpeed(); ///< Set speed.
    void SetRotSpeed(float speed); ///< Set rotational velocity.

    Vector2 GetViewVector(); //Get view vector.

    float GetOrientation(); ///< Get orientation.
    void SetOrientation(float angle); ///< Set orientation.
    
    const Vector2& GetVelocity(); ///< Get orientation.
    void SetVelocity(const Vector2& v); ///< Set orientation.
    const Vector2& GetAcceleration(); ///< Get orientation.
    void SetAcceleration(const Vector2& v); ///< Set orientation.

    virtual void SetSmokeColor(XMFLOAT4 color); ///< Set smoke color
    
    void StrafeLeft(); ///< Strafe left.
    void StrafeRight(); ///< Strafe right.
    void StrafeBack(); ///< Strafe back.
    
    const BoundingSphere& GetBoundingSphere(); ///< Get bounding sphere.
    const Vector2& GetPos(); ///< Get position.

    double GetMass() { return mass; }; ///< Get the mass

    void EmitSmoke();

    bool GetIsBullet();

    float get_mass() { return (float)mass; };
}; //CObject



#endif // !OBJECT_H