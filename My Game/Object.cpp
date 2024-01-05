/// \file Object.cpp
/// \brief Code for the game object class CObject.

#include "Object.h"
#include "ComponentIncludes.h"
#include "GameDefines.h"
#include "Particle.h"
#include "ParticleEngineScaling.h"

std::vector<XMFLOAT4> colors = { XMFLOAT4(Colors::Red),XMFLOAT4(Colors::Purple),XMFLOAT4(Colors::Blue),XMFLOAT4(Colors::Green),XMFLOAT4(Colors::Black) };


/// Create and initialize an object given its sprite type
/// and initial position.
/// \param t Type of sprite.
/// \param p Initial position of object.

CObject::CObject(eSpriteType t, const Vector2& p){ 
  m_nSpriteIndex = t;
  m_vPos = p; 

  m_pRenderer->GetSize(t, m_vRadius.x, m_vRadius.y);
  m_vRadius *= 0.5f;
  
  const Vector2 topleft(-m_vRadius.x, m_vRadius.y);
  const Vector2 bottomrt(m_vRadius.x, -m_vRadius.y);

  m_Sphere.Radius = max(m_vRadius.x, m_vRadius.y);
  m_Sphere.Center = (Vector3)m_vPos;
  
  m_fGunTimer = m_pStepTimer->GetTotalSeconds();
  //smoke_color = XMFLOAT4(Colors::Red);
  //smoke_color = XMFLOAT4(rand() % 255, rand() % 255, rand() % 255, 255);  //Random color
  smoke_color = colors[rand() % colors.size()];
} //constructor

/// Move and update all bounding shapes.
/// The player object gets moved by the controller, everything
/// else moves an amount that depends on its velocity and the
/// frame time.

void CObject::move(){
  m_vOldPos = m_vPos;
  m_pStepTimer->SetFixedTimeStep(true);
  const float t = m_pStepTimer->GetElapsedSeconds();
  

  if (affected_by_gravity) {
    // Calculate effect of gravity for all the gravationally affected objects
    // TODO: implement orbit calculation if only one source of gravity. Analytic astrodynamics solutions are more stable than numerical integration. Patched Conics may be good enough.
    // SetVelocity(GetVelocity() + calculate_gravity(p->GetPos()));
    m_vPos += m_vVelocity * t;// + m_vAcceleration * t * t;
  }
  else m_vPos += m_vVelocity * t;// + m_vAcceleration * t*t;

  m_Sphere.Center = (Vector3)m_vPos; //update bounding sphere
} //move

void CObject::CollisionResponse(){
  m_vPos = m_vOldPos;
} //CollisionResponse


/// Changes the object's velocity so that it matches what it would be after a reflection following Snell's law.
/// unit_normal is the unit vector normal the the surface of reflection.

void CObject::CollisionReflectionResponse(Vector2 unit_normal) {
  CollisionResponse();
  m_vVelocity = m_vVelocity - 2 * m_vVelocity.Dot(unit_normal) * unit_normal;
} //CollisionResponse

/// Set the strafe left flag.

void CObject::StrafeLeft(){
  m_bStrafeLeft = true;
} //StrafeLeft

/// Set the strafe right flag.

void CObject::StrafeRight(){
  m_bStrafeRight = true;
} //StrafeRight

/// Set the strafe back flag.

void CObject::StrafeBack(){
  m_bStrafeBack = true;
} //StrafeBack

/// Perform a death particle effect to mark the death of an object.

void CObject::DeathFX(){
  CParticleDesc2D d;
  d.m_nSpriteIndex = SMOKE_SPRITE;
  d.m_vPos = m_vPos;
      
  d.m_fLifeSpan = 0.5f;
  d.m_fMaxScale = 0.5f;
  d.m_fScaleInFrac = 0.2f;
  d.m_fFadeOutFrac = 0.8f;
  d.m_fScaleOutFrac = d.m_fFadeOutFrac;

  m_pParticleEngine->create(d);
} //DeathFX

/// Kill an object by marking its "is dead" flag. The object
/// will get deleted later at the appropriate time.

void CObject::kill(){
  m_bDead = true;
  DeathFX();
} //kill

/// Reader function for the "is dead" flag.
/// \return true if marked as being dead, that is, ready for disposal.

bool CObject::IsDead(){
  return m_bDead;
} //IsDead

/// Set the object's speed, assuming that the object
/// moves according to its speed and view vector.
/// \param speed Speed.

void CObject::SetSpeed(float speed){
  m_fSpeed = speed;
} //SetVelocity

/// Set the object's rotational speed in revolutions per second.
/// \param speed Rotational speed in RPS.

void CObject::SetRotSpeed(float speed){
  m_fRotSpeed = speed;
} //Rotate

/// Reader function for the bounding sphere.
/// \return The bounding sphere.

const BoundingSphere& CObject::GetBoundingSphere(){
  return m_Sphere;
} //GetBoundingSphere

/// Reader function for position.
/// \return Position.

const Vector2& CObject::GetPos(){
  return m_vPos;
} //GetPos

/// Reader function for speed.
/// \return Speed.

float CObject::GetSpeed(){
  return m_fSpeed;
} //GetSpeed

/// Get the view vector.
/// \return The view vector.

Vector2 CObject::GetViewVector(){
  return Vector2(-sinf(m_fRoll), cosf(m_fRoll));
} //GetViewVector

/// Reader function for the orientation. A 2D object's
/// orientation is its roll amount in 3D space.
/// \return The view vector.

float CObject::GetOrientation(){
  return m_fRoll;
} //GetOrientation

/// Writer function for the orientation. A 2D object's
/// orientation is its roll amount in 3D space.
/// \param angle The new orientation.

void CObject::SetOrientation(float angle){
  m_fRoll = angle;
} //SetOrientation

/// Writer function for the smoke color.
/// \param color The new smoke color.

void CObject::SetSmokeColor(XMFLOAT4 color) {
  smoke_color = color;
} //SetSmokeColor

/// Reader function for velocity.
/// \return Velocity.

const Vector2& CObject::GetVelocity(){
  return m_vVelocity;
} //GetVel

/// Writer function for velocity.
/// \param v Velocity.

void CObject::SetVelocity(const Vector2& v){
  m_vVelocity = v;
} //SetVel

/// Reader function for acceleration.
/// \return Acceleration.

const Vector2& CObject::GetAcceleration() {
  return m_vAcceleration;
} //GetAcceleration

/// Writer function for Acceleration.
/// \param v Acceleration.

void CObject::SetAcceleration(const Vector2& v) {
  m_vAcceleration = v;
} //SetAcceleration

void CObject::EmitSmoke() {
  if (m_pStepTimer->GetTotalSeconds() > m_fSmokeTimer) {
    m_fSmokeTimer = m_pStepTimer->GetTotalSeconds();

    CParticleDesc2D d;
    d.m_nSpriteIndex = SPARK_SPRITE;
    XMMATRIX M = XMMatrixRotationZ(m_fRoll);
    //const int n = (int)round(8 * m_fDamage);
    const int n = 1;

    for (int i = 0; i < n; i++) {
      Vector3 vOffset(0, 0, 0); //offset from plane center to engine
      vOffset = Vector3::Transform(vOffset, M);
      d.m_vPos = m_vPos + vOffset; //where smoke starts
      //d.m_vPos.z -= 0.1f; //in front of plane

      //d.m_vVel = m_vVel; //slower than the plane
      d.m_vVel = Vector2(0, 0);
      //d.m_vVel.y += 20 * (m_pRandom->randf() - 0.5f) * 1; //spread out vertically over time
      //d.m_fFriction = 0.8f; //slow down over time
      //d.m_fRSpeed = (m_pRandom->randf() - 0.5f); //spin each puff of smoke
      d.m_fLifeSpan = 10.0f; //1.75f + m_pRandom->randf()/2.0f; //short lived
      d.m_fMaxScale = 0.1f; //small
      d.m_fScaleInFrac = 0.0f; //scale in
      d.m_fFadeInFrac = 0.0f; //fade in
      d.m_fFadeOutFrac = 0.0f; //fade out
      d.m_f4Tint = smoke_color;

      m_pParticleEngine->create(d); //create smoke puff
    } //for
  } //if
} //EmitSmoke

bool CObject::GetIsBullet() {
  return is_bullet;
}