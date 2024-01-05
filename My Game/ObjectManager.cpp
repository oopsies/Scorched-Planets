/// \file ObjectManager.cpp
/// \brief Code for the the object manager class CObjectManager.

#include "ObjectManager.h"
#include "ComponentIncludes.h"
#include "ParticleEngineScaling.h"
#include <memory>


CObjectManager::CObjectManager(){
} //constructor

/// Destruct all of the objects in the object list.

CObjectManager::~CObjectManager(){
  for(auto const& p: m_stdObjectList) //for each object
    delete p; //delete object
  for (auto const& p : m_planets_list) //delete all the planets
    delete p;
  for (auto const& p : m_wormholes_list) //delete all the planets
      delete p;
  for (std::shared_ptr<CTankObject>& p : m_tanks_list) //delete all the tanks
    p = nullptr;
} //destructor

/// Create an object and put a pointer to it on the object list.
/// \param t Sprite type.
/// \param v Initial position.
/// \return Pointer to the object created.

CObject* CObjectManager::create(eSpriteType t, const Vector2& v, double mass, bool affected_by_gravity){
  CObject* p = new CObject(t, v); 
  // Add to the list of massive objects if it has mass. I.E. it should affect the gravitational field
  if (mass) {
    p->mass = mass;
    m_massive_objects.push_back(p);
  }

  // Add to the list of objects affected by gravity, if appropriate.
  p->affected_by_gravity = affected_by_gravity;
  if (affected_by_gravity) {
    m_objects_affected_by_gravity.push_back(p);
  }
  m_stdObjectList.push_back(p); 
  return p;
} //create

CPlanetObject* CObjectManager::create_planet(const Vector2& p, double mass, int radius, bool affected_by_gravity) {
  CPlanetObject* planet = new CPlanetObject(p, radius);
  if (mass) {
    planet->mass = mass;
    m_massive_objects.push_back(planet);
  }
  planet->affected_by_gravity = affected_by_gravity;
  if (affected_by_gravity) {
    m_objects_affected_by_gravity.push_back(planet);
  }

  m_planets_list.push_back(planet);
  return planet;
}

std::shared_ptr<CTankObject> CObjectManager::create_tank(float angle_relative_to_planet, CPlanetObject* home_planet) {
  std::shared_ptr<CTankObject> tank(new CTankObject(angle_relative_to_planet, home_planet));

  m_tanks_list.push_back(tank);
  return tank;
}

std::shared_ptr<CTankObject> CObjectManager::create_tank(float angle_relative_to_planet, CPlanetObject* home_planet, XMFLOAT4 color) {
  std::shared_ptr<CTankObject> tank = create_tank(angle_relative_to_planet, home_planet);
  tank->SetSmokeColor(color);
  return tank;
}

CBulletObject* CObjectManager::create_bullet(eSpriteType t, const Vector2& v) {
  if (t > NUM_SPRITES) //Somehow, we created a bullet that has an invalid sprite index.
    t = BULLET_SPRITE;
  CBulletObject* p = new CBulletObject(t, v);
  m_objects_affected_by_gravity.push_back(p); //Bullets are affected by gravity
  m_bullets_list.push_back(p); //Bullets are bullets. We want to keep track of what's flying around.
  m_stdObjectList.push_back(p); //Bullets are objects.


  //p->m_vOldEnergy = calculate_total_mechanical_energy(p->m_vPos, p->GetVelocity());
  return p;
} //create_bullet
CBulletObject* CObjectManager::create_bullet(eSpriteType t, const Vector2& v, XMFLOAT4 trail_color) {
  CBulletObject* p = create_bullet(t, v);
  p->SetSmokeColor(trail_color);
  return p;
} //create

CWormholeObject* CObjectManager::create_wormhole(const Vector2& pos, int ttl) {
    CWormholeObject* w = new CWormholeObject(pos, ttl);
    m_wormholes_list.push_back(w);
    //m_stdObjectList.push_back(w);

    return w;
}
CWormholeObject* CObjectManager::create_wormhole(const Vector2& pos, int ttl, CWormholeObject* next) {
    CWormholeObject* w = new CWormholeObject(pos, ttl, next);
    m_wormholes_list.push_back(w);
    //m_stdObjectList.push_back(w);

    return w;
}

/// Delete all of the objects managed by the object manager. 
/// This involves deleting all of the CObject instances pointed
/// to by the object list, then clearing the object list itself.

void CObjectManager::clear(){
  for (auto const& p : m_planets_list)
    delete p;
  for(auto const& p: m_stdObjectList) //for each object
    delete p; //delete object
  for (auto const& p : m_wormholes_list)
      delete p;
  for (std::shared_ptr<CTankObject>& p : m_tanks_list) //for each object
    p.reset(); //delete object

  m_stdObjectList.clear(); //clear the object list
  m_planets_list.clear();
  m_tanks_list.clear();
  m_bullets_list.clear();
  m_massive_objects.clear();
  m_objects_affected_by_gravity.clear();
  m_wormholes_list.clear();
} //clear

/// Draw the objects in the object list.

void CObjectManager::draw(){
  for (auto const& p : m_stdObjectList) //for each object
    m_pRenderer->Draw(*(CSpriteDesc2D*)p);
 
  for (auto const& p : m_planets_list)
    p->draw_planet();

  for (auto const& p : m_tanks_list)  //for each object
      p->draw_tank();

  for (auto const& p : m_wormholes_list)
      p->DrawWormhole();
    //m_pRenderer->Draw(*(CSpriteDesc2D*)p.get());
} //draw

/// Test whether an object's left, right, top or bottom
/// edge has crossed the left, right, top, bottom edge of
/// the world, respectively. This function assumes that the
/// bottom left corner of the world is at the origin.
/// \param p Pointer to an object.
/// \return true if the object is at the edge of the world.

bool CObjectManager::AtWorldEdge(CObject* p){   
  const Vector2 pos = p->m_vPos; //position of center of sprite
  float w, h; //sprite width and height
  m_pRenderer->GetSize(p->m_nSpriteIndex, w, h);
        
  if(pos.x - w/2 < 0 || pos.x + w/2 > m_vWorldSize.x ||
     pos.y - h/2 < 0 || pos.y + h/2 > m_vWorldSize.y)
    return true;

  return false; //default
} //AtWorldEdge

bool CObjectManager::AtWorldEdge(Vector2& pos) {
  if (pos.x < m_vWorldSize.x*.5 || pos.x > m_vWorldSize.x*.95 ||
    pos.y < m_vWorldSize.x*0.5 || pos.y  > m_vWorldSize.y*.95)
    OutputDebugStringA("found edge\n");
    return true;

  return false; //default
} //AtWorldEdge

/// Move all of the objects and perform 
/// broad phase collision detection and response.

void CObjectManager::move(){
    const float dt = m_pStepTimer->GetElapsedSeconds();
    for (auto const& p : m_stdObjectList) { //for each object
        const Vector2 oldpos = p->m_vPos; //its old position
        
        /*
        #ifdef _DEBUG
            //It can be useful to keep track of the total energy in a system; this gives us some idea of the accuracy of the simulation.
            //For now, we might as well just keep this in debug mode, since our attempts thus far to use this to improve accuracy have dramatically failed.
            //I'm going to keep this skeleton here for now, since I want to come back to it before the end of the semester.

            const float old_gravity_potential = calculate_gravitational_potential(p->m_vPos);
            float old_energy = p->m_vOldEnergy;

            if (p->m_vOldEnergy == -1) { //Object's initial energy is not calculated yet, so do so.
              p->m_vOldEnergy = calculate_total_mechanical_energy(p->m_vPos, p->GetVelocity());
              old_energy = p->m_vOldEnergy;
            }
            else { //The object's initial energy is already calculated
              // Correction to ensure energy is conserved
              // This correction was a good idea, but it damps systems WAY too fast. All projectiles will collapse to the center of the nearest planet before the completion of a single orbit.
              // As soon as they collide with the core, they immediately shoot off with an energy of -nan(ind).
              // I think this can be fixed by instead of simply scaling the velocity (and consequently the kinetic energy) to keep total energy constant,
              // it would be better to rotate the velocity vector so that it's Total energy in the next time step will be the same as it should be.
              // Doing this requires a bit more differential geometry and numerical analysis than I'm willing to dedicate this weekend.
              // TODO: Improve energy conservation corrections
              // TODO: If energy conservation corrections do not seem to work, then get rid of this section.


              //const float old_velocity = p->m_vVelocity.Length();
              //p->m_vVelocity.Normalize();
              //p->m_vVelocity *= sqrt(2 * (old_energy - old_gravity_potential));


              //const float new_energy = calculate_total_mechanical_energy(p->m_vPos, p->GetVelocity());
              //OutputDebugStringA(("old:\t" + to_string(old_energy) + "\tnew:\t" + to_string(new_energy) + "\ndifference:\t" + to_string(new_energy - old_energy) + "\n").c_str());
            }
        #endif // _DEBUG
        */



        switch (p->m_nSpriteIndex) {
        case TURRET_SPRITE: {
            const Vector2 v = m_pPlayer->m_vPos - p->m_vPos;
            bool bVisible = v.Length() < 256.0f;

            if (bVisible && m_pStepTimer->GetTotalSeconds() > p->m_fGunTimer + 1) {
                p->m_fGunTimer = m_pStepTimer->GetTotalSeconds();
                const Vector2 v = m_pPlayer->m_vPos - p->m_vPos;
                p->m_fRoll = atan2f(v.y, v.x) - XM_PI / 2.0f;
                FireGun(p, BULLET2_SPRITE);
            } //if
        } //case
                          break;

        case BULLET_SPRITE:
        case BULLET2_SPRITE:
            if (AtWorldEdge(p)) {
                p->kill();
                m_pAudio->play(RICOCHET_SOUND);
            } //if
            break;

        case PLAYER_SPRITE:
            if (AtWorldEdge(p))
                p->CollisionResponse();
            break;
        } //switch

    }

    // Calculate effect of gravity for all the gravationally affected objects
    for (auto const& p : m_objects_affected_by_gravity) {
      if (p->GetIsBullet() && AtWorldEdge(p)) {
        p->kill();
        m_pAudio->play(RICOCHET_SOUND);
      } //if
      p->SetAcceleration(calculate_gravity(p->GetPos()));

      

      OutputDebugStringA((to_string(p->GetAcceleration().Length()) + "\n").c_str());
      p->SetVelocity(p->GetVelocity() + p->GetAcceleration()*dt);

      Vector2 pos = p->GetPos();
      Vector2 acc = p->GetAcceleration();
      Vector2 vel = p->GetVelocity();
      int c = 0;

      // Having the objects move AFTER calculating what their velocity should be on the next time step seems to improve numerical stability of simulations
      // This makes a certain amount of sense. This effectively is a crude Semi-implicit Euler integration, as opposed to the naive Euler integration it was doing before.
      // It's still not 100% accurate, but at least in 2 body systems, elliptical orbits stay elliptical and do not precess (rotate) as quickly as they were before!
      // I'll take the appearance of accuracy over blatant inaccuracy.
      p->move(); //move it

    }

    



  if (m_bTurnsEnabled)
    m_pPlayer->Think();
  for (auto const& p : m_tanks_list) {
    if (!m_bTurnsEnabled)
      p->Think();
    p->move(); //move it
  }

  //now do object-object collision detection and response and
  //remove any dead objects from the object list.

  BroadPhase(); //broad phase collision detection and response
  CullDeadObjects(); //remove dead objects from object list
} //move

/// Create a bullet object and a flash particle effect.
/// It is assumed that the object is round and that the bullet
/// appears at the edge of the object in the direction
/// that it is facing and continues moving in that direction.
/// \param pObj Pointer to an object.
/// \param bullet Sprite type of bullet.

void CObjectManager::FireGun(CObject* pObj, eSpriteType bullet){
  m_pAudio->play(SHOOT_SOUND);

  const Vector2 view = pObj->GetViewVector();
  Vector2 pos = pObj->GetPos() + 
    0.5f*m_pRenderer->GetWidth(pObj->m_nSpriteIndex)*view;

  //create bullet object
  CBulletObject* pBullet = m_pObjectManager->create_bullet(bullet, pos); //create bullet

  const Vector2 norm(view.y, -view.x); //normal to direction
  const float m = 2.0f*m_pRandom->randf() - 1.0f;
  const Vector2 deflection = Vector2::Zero; //0.01f * m * norm;

  pBullet->SetVelocity(m_pPlayer->GetVelocity() + 500.0f*(view + deflection));
  pBullet->SetOrientation(m_pPlayer->GetOrientation()); 

  //particle effect for gun fire
  
  CParticleDesc2D d;

  d.m_nSpriteIndex = SPARK_SPRITE;
  d.m_vPos = pos;
  d.m_vVel = pObj->GetSpeed()*view;
  d.m_fLifeSpan = 0.25f;
  d.m_fScaleInFrac = 0.4f;
  d.m_fFadeOutFrac = 0.5f;
  d.m_fMaxScale = 0.5f;
  d.m_f4Tint = XMFLOAT4(Colors::Yellow);
  
  m_pParticleEngine->create(d);
} //FireGun


/// Iterate over all the players and damage them if they are inside the explosion sphere
/// \param sphere BoundingSphere representing the explosion
/// \param damage how much to damage players.
void CObjectManager::DamagePlayersInSphere(BoundingSphere sphere, int damage) {
  for (auto tank : m_tanks_list) {
    if (sphere.Intersects(tank->m_Sphere)) {
      tank->take_damage(damage);
    }
  }
}//DamagePlayersInSphere

/// This is a "bring out yer dead" Monty Python type of thing.
/// Iterate through the objects and check whether their "is dead"
/// flag has been set. If so, then delete its pointer from
/// the object list and destruct the object.

void CObjectManager::CullDeadObjects(){
  //Erase references to the dead objects in the gravity/mass lists
  for (auto i = m_objects_affected_by_gravity.begin(); i != m_objects_affected_by_gravity.end();) {
    if ((*i)->IsDead()) { //"He's dead, Dave." --- Holly, Red Dwarf
      i = m_objects_affected_by_gravity.erase(i); //remove from object list and advance to next object
    } //if

    else ++i; //advance to next object
  } //for
  for (auto i = m_massive_objects.begin(); i != m_massive_objects.end();) {
    if ((*i)->IsDead()) { //"He's dead, Dave." --- Holly, Red Dwarf
      i = m_massive_objects.erase(i); //remove from object list and advance to next object
    } //if

    else ++i; //advance to next object
  } //for

  for (auto i = m_tanks_list.begin(); i != m_tanks_list.end();) {
    if ((*i)->IsDead()) { //"He's dead, Dave." --- Holly, Red Dwarf
      i->reset(); //delete object
      i = m_tanks_list.erase(i); //remove from object list and advance to next object
    } //if
    else ++i; //advance to next object
  } //for

  for (auto i = m_bullets_list.begin(); i != m_bullets_list.end();) {
      if ((*i)->IsDead()) { //"He's dead, Dave." --- Holly, Red Dwarf
          i = m_bullets_list.erase(i); //remove from object list and advance to next object
      } //if
      else ++i; //advance to next object
  } //for

  for(auto i=m_stdObjectList.begin(); i!=m_stdObjectList.end();){
    if((*i)->IsDead()){ //"He's dead, Dave." --- Holly, Red Dwarf
      delete *i; //delete object
      i = m_stdObjectList.erase(i); //remove from object list and advance to next object
    } //if
      
    else ++i; //advance to next object
  } //for
} //CullDeadObjects

/// Perform collision detection and response for all pairs
/// of objects in the object list, making sure that each
/// pair is processed only once.

void CObjectManager::BroadPhase(){
  //Iterate over objects
  for(auto i=m_stdObjectList.begin(); i!=m_stdObjectList.end(); i++){
    for(auto j=next(i); j!=m_stdObjectList.end(); j++)
      NarrowPhase(*i, *j);
  } //for

  //Iterate over the tanks
  for (auto i = m_tanks_list.begin(); i != m_tanks_list.end(); i++) {
    for (auto j = m_stdObjectList.begin(); j != m_stdObjectList.end(); j++)
      NarrowPhase(*i, *j);
  } //for

  //Iterate over planets
  for (auto i = m_planets_list.begin(); i != m_planets_list.end(); i++) {
    for (auto j = m_stdObjectList.begin(); j != m_stdObjectList.end(); j++)
      NarrowPhase(*i, *j);
  } //for

  //Iterate over wormholes
  for (auto i = m_wormholes_list.begin(); i != m_wormholes_list.end(); i++) {
      for (auto j = m_stdObjectList.begin(); j != m_stdObjectList.end(); j++)
          NarrowPhase(*i, *j);
  } //for

} //BroadPhase

/// Perform collision detection and response for a pair of objects.
/// We are talking about bullets hitting the player and the
/// turrets here. When a collision is detected the response
/// is to delete the bullet (i.e. mark its "is dead" flag)
/// play a particle effect at the point of contact, and play one 
/// sound for the player and another for the turrets.
/// \param p0 Pointer to the first object.
/// \param p1 Pointer to the second object.

void CObjectManager::NarrowPhase(CObject* p0, CObject* p1){
  eSpriteType t0 = (eSpriteType)p0->m_nSpriteIndex;
  eSpriteType t1 = (eSpriteType)p1->m_nSpriteIndex;

  if(p0->m_Sphere.Intersects(p1->m_Sphere)){ //bounding spheres intersect
    if(t0 == PLAYER_SPRITE && t1 == TURRET_SPRITE) //player hits turret
      p0->CollisionResponse();

    else if(t1 == PLAYER_SPRITE && t0 == TURRET_SPRITE) //turret hit by player
      p1->CollisionResponse();
  } //if
} //NarrowPhase

void CObjectManager::NarrowPhase(CPlanetObject* p0, CObject* p1) {
  eSpriteType t0 = (eSpriteType)p0->m_nSpriteIndex;
  eSpriteType t1 = (eSpriteType)p1->m_nSpriteIndex;

  if (p0->Intersects(p1->m_Sphere)) { //bounding spheres intersect
    if (p1->GetIsBullet()) {
      CBulletObject* bullet = (CBulletObject*)p1;
      bullet->kill(p0);
    }//if
    else if (t1 == TURRET_SPRITE) {
      Vector2 unit_normal = p0->GetPos() - p1->GetPos();
      unit_normal.Normalize();
      p1->CollisionReflectionResponse(unit_normal);
    }//else if

    /*if (t0 == PLAYER_SPRITE && t1 == TURRET_SPRITE) //player hits turret
      p0->CollisionResponse();

    else if (t1 == PLAYER_SPRITE && t0 == TURRET_SPRITE) //turret hit by player
      p1->CollisionResponse();

    if (t0 == BULLET_SPRITE && t1 == TURRET_SPRITE) { //bullet hits turret
      p0->kill();
      m_pAudio->play(CLANG_SOUND);
    } //if

    else if (t1 == BULLET_SPRITE && t0 == TURRET_SPRITE) { //turret hit by bullet
      p1->kill();
      m_pAudio->play(CLANG_SOUND);
    } //else if

    else if (t0 == BULLET2_SPRITE && t1 == PLAYER_SPRITE) { //player hit by bullet
      m_pAudio->play(OW_SOUND);
      p0->kill();
    } //else if

    else if (t1 == BULLET2_SPRITE && t0 == PLAYER_SPRITE) { //bullet hits player
      m_pAudio->play(OW_SOUND);
      p1->kill();
    } //else if*/
  } //if
} //NarrowPhase

void CObjectManager::NarrowPhase(std::shared_ptr<CTankObject> p0, CObject* p1) {
  eSpriteType t0 = (eSpriteType)p0->m_nSpriteIndex;
  eSpriteType t1 = (eSpriteType)p1->m_nSpriteIndex;

  if (p0->m_Sphere.Intersects(p1->m_Sphere)) { //bounding spheres intersect
    if (p1->GetIsBullet()) {
      CBulletObject* bullet = (CBulletObject*)p1;
      if (p0.get() != bullet->GetOwner()) { //We don't want the bullets to "misfire" i.e. explode before leaving the tank that shot them.
        bullet->kill();
        p0->take_damage(bullet->GetDamage());
      }
    }
  } //if
} //NarrowPhase

void CObjectManager::NarrowPhase(CWormholeObject* p0, CObject* p1) {

    if (p0->m_Sphere.Intersects(p1->m_Sphere)) {
        if (p1->GetIsBullet()) {
            p1->m_vPos = p0->GetNextWormhole()->GetPos();
            Vector2 v = p1->m_vVelocity;
            v.Normalize();
            p1->m_vPos += (p0->m_Sphere.Radius + 1.0f) * v;
        }
    }

} //NarrowPhase

/// Calculates the gravitational field at the position
/// \param position Vector 2 representing some position at the board.
Vector2 CObjectManager::calculate_gravity(Vector2 position) {
  Vector2 current_gravity = Vector2(0, 0);
  Vector2 temp_direction;
  float magnitude_squared=0;
  int i = 0;
  for (auto const& p : m_massive_objects) {
    i++;
    temp_direction = p->GetPos() - position;
    magnitude_squared = temp_direction.LengthSquared();
    temp_direction.Normalize();
    temp_direction *= static_cast<float>(p->mass * (double) gravitational_constant / (magnitude_squared + softening_parameter));
    current_gravity += temp_direction;
  }

  //char msgbuf[128];
  //sprintf_s(msgbuf, "Gravity: (%f, %f, %f, %f)\tNumber of Massive objects: %d\t Number of calculated objects: %d\n", current_gravity.Length(), current_gravity.x, current_gravity.y, magnitude_squared, m_massive_objects.size(), i);
  //OutputDebugString(msgbuf);
  return current_gravity;
}

/// Calculates the gravitational potential at the position
/// \param position Vector 2 representing some position at the board.
float CObjectManager::calculate_gravitational_potential(Vector2 position) {
  float current_gravity_potential = 0;
  Vector2 temp_direction;
  float distance = 0;
  for (auto const& p : m_massive_objects) {
    temp_direction = p->GetPos() - position;
    distance = temp_direction.Length();
    current_gravity_potential += static_cast<float>(p->mass / distance);
  }

  current_gravity_potential *= static_cast<float>(gravitational_constant);
  return -current_gravity_potential;
}


float CObjectManager::calculate_total_mechanical_energy(Vector2 position, Vector2 velocity) {
  //Note: Technically, the dimensions of this quantity are NOT actually energy. It's closer to the "Gravitational Potential" i.e. Energy/mass.
  //Since we are mostly dealing with "massless" particles, it's more convenient to work in this quantity.
  float running_total_energy = 0;

  //Calculate the total gravitational potential "energy"
  running_total_energy += m_pObjectManager->calculate_gravitational_potential(position);
  //Calculate the total kinetic "energy"
  running_total_energy += 0.5f * velocity.LengthSquared();

  return running_total_energy;
}

void CObjectManager::calculate_total_energy_for_all_objects() {
  for (auto p : m_objects_affected_by_gravity) {
    p->m_vOldEnergy = calculate_total_mechanical_energy(p->m_vPos, p->GetVelocity());
  }
}

/// Calculates the closest planet at the given position.
/// \param position Vector2 representing some position in the world
/// \returns a pointer to the nearest planet

CPlanetObject* CObjectManager::calculate_closest_planet(Vector2 position) {
  CPlanetObject* current_closest_planet = nullptr;
  float current_closest_length = -1.0f;
  float distance;
  //Loop over all the planets and calculate distance.
  for (auto const& planet : m_planets_list) {
    distance = (position - planet->GetPos()).Length();
    if (current_closest_length == -1 || distance < current_closest_length) {
      current_closest_length = distance;
      current_closest_planet = planet;
    }
  }

  return current_closest_planet;
}

/// Calculates the closest tank at the given position.
/// \param position Vector2 representing some position in the world
/// \returns a shared pointer to the nearest tank
std::shared_ptr<CTankObject> CObjectManager::get_nearest_tank(Vector2 position) {
  std::shared_ptr<CTankObject> current_closest_tank;
  float current_closest_length = -1.0f;
  float distance;

  //Loop over all the planets and calculate distance.
  for (auto const& tank : m_tanks_list) {
    distance = (position - tank->GetPos()).Length();
    if (current_closest_length == -1 || distance < current_closest_length) {
      current_closest_length = distance;
      current_closest_tank = tank;
    }
  }

  return current_closest_tank;
}

/// Calculates the distance to the closest tank at the given position.
/// \param position Vector2 representing some position in the world
/// \returns the distance as a float
float CObjectManager::get_nearest_tank_location(Vector2 position, CTankObject* origin_tank) {
  

  float current_closest_length = -1.0f;
  float distance;
  //Loop over all the planets and calculate distance.
  for (auto const& tank : m_tanks_list) {
    if (tank.get() != origin_tank && !(tank->IsDead())){ // We don't want tanks aiming at themselves or dead tanks!
      distance = (position - tank->GetPos()).Length(); //Plain old euclidean distance
      /*Vector2 object_to_core = position - tank->get_planet_index()->GetPos();
      float object_elevation = object_to_core.Length();
      Vector2 tank_location = tank->GetPos() - tank->get_planet_index()->GetPos();
      float tank_elevation = tank_location.Length();
      float angle_between_tank_object = acosf(tank_location.Dot(object_to_core) / (object_elevation * tank_elevation)); //Use the dot product definition of cosine to get the angle between the two points.
      float radius = (float) tank->get_planet_index()->sealevel_radius;
      //A custom distance function I wrote. This isn't rigorous from a differential geometry perspective.
      //sqrt((angle*radius)^2 + dz^2)
      //substitutes dx^2+dy^2 with the change in the angular displacement squared
      distance = sqrtf(powf(angle_between_tank_object , 2) + powf(radius, 2) + powf(object_elevation - tank_elevation, 2));
      */
      //penalize it by increasing the "distance" if it's close to a suicide shot.
      //OutputDebugStringA(("before: " + to_string(distance) + "\t").c_str());
      //OutputDebugStringA(("dis_origin: " + to_string((origin_tank->GetPos() - position).Length()) + "\t").c_str());
      distance /= 1- expf(-(origin_tank->GetPos() - position).LengthSquared()/250); //If the shot is close to the origin tank, then this makes it look far away. If it's far away, it doesn't affect it much
      //OutputDebugStringA(("after: " + to_string(distance)).c_str());
      //OutputDebugStringA(to_string((int)origin_tank).c_str());
      //OutputDebugStringA("\n");
      
      
      if (current_closest_length == -1 || distance < current_closest_length) {
        current_closest_length = distance;
      }
    }
  }
  //Deincentivize shots that hit the worldedge or that are suicide shots.
  if (AtWorldEdge(position))
    current_closest_length *= 5;
  if ((position - origin_tank->GetPos()).Length() < 35){
    OutputDebugStringA(("Suicide shot\t"+to_string((position-origin_tank->GetPos()).Length()) + "\n").c_str());
  }

  return current_closest_length;
}

/// <summary>
/// Creates a phantom bullet which moves "instantly". I.e. we repeatedly force it to move until it explodes before the end of the frame.
/// </summary>
/// <param name="t"></param>
/// <param name="position"></param>
/// <param name="velocity"></param>
/// <returns></returns>
float CObjectManager::create_phantom_bullet(eSpriteType t, const Vector2& position, const Vector2& velocity, CTankObject* owner) {
  CBulletObject* phantom_bullet = new CBulletObject(t, position);
  phantom_bullet->SetOwner(owner);
  phantom_bullet->set_is_phantom(true);
  phantom_bullet->SetVelocity(velocity);
  float start_time = m_pStepTimer->GetTotalSeconds();
  while (!phantom_bullet->IsDead()) { // Force it to the next place while it's not dead.
    //string test_string = "Bullet location: " + to_string(phantom_bullet->GetPos().x) + ", " + to_string(phantom_bullet->GetPos().y) + "\n";
    //OutputDebugStringA(test_string.c_str());
    const int dt = m_pStepTimer->GetElapsedSeconds();
    phantom_bullet->SetAcceleration(calculate_gravity(phantom_bullet->GetPos()));
    phantom_bullet->SetVelocity(phantom_bullet->GetVelocity() + phantom_bullet->GetAcceleration()*dt);
    phantom_bullet->move(); //move it
    //Check for collisions with planets
    for (auto i = m_planets_list.begin(); i != m_planets_list.end(); i++) {
      if ((*i)->Intersects(phantom_bullet->m_Sphere)) { //bounding spheres intersect
        phantom_bullet->m_bDead = true;
        break;
      }
    }
    //Check if off edge
    if (AtWorldEdge(phantom_bullet)) {
      phantom_bullet->m_bDead = true;
      break;
    } //if

    //Deal with wormholes
    for (auto p0 : m_wormholes_list) {
      if (p0->m_Sphere.Intersects(phantom_bullet->m_Sphere)) {
        if (phantom_bullet->GetIsBullet()) {
          phantom_bullet->m_vPos = p0->GetNextWormhole()->GetPos();
          Vector2 v = phantom_bullet->m_vVelocity;
          v.Normalize();
          phantom_bullet->m_vPos += (p0->m_Sphere.Radius + 1.0f) * v;
        }
      }
    }
  }
  Vector2 final_pos = phantom_bullet->GetPos();
  delete phantom_bullet;
  //TODO: Deincentivize suicide shots by figuring out how to maximize how far away it is from the player
  float self_distance = (owner->GetPos() - final_pos).Length();
  if (self_distance > 50)
    self_distance = 1;
  return get_nearest_tank_location(final_pos, owner);
} ///create_phantom_bullet

//draws the trajectory that the bullet will take.
void CObjectManager::draw_trajectory(eSpriteType t, const Vector2& position, const Vector2& velocity, CTankObject* owner) {
    CBulletObject* phantom_bullet = new CBulletObject(t, position);
    phantom_bullet->SetOwner(owner);
    phantom_bullet->set_is_phantom(true);
    phantom_bullet->SetVelocity(velocity);
    const float dt = m_pStepTimer->GetElapsedSeconds();
    for (int i = 0; i < 200; i++) {

        phantom_bullet->SetAcceleration(calculate_gravity(phantom_bullet->GetPos()));
        phantom_bullet->SetVelocity(phantom_bullet->GetVelocity() + phantom_bullet->GetAcceleration()*dt);
        phantom_bullet->move(); //move it

        //only need to check for collision as much as we draw the dots
        if (i % 10 == (int)(m_pStepTimer->GetTotalSeconds() * 30) % 10) {
            //Check for collisions with planets
            for (auto i = m_planets_list.begin(); i != m_planets_list.end(); i++) {
                if ((*i)->Intersects(phantom_bullet->m_Sphere)) { //bounding spheres intersect
                    phantom_bullet->m_bDead = true;
                    break;
                }
            }
            //Check if off edge
            if (AtWorldEdge(phantom_bullet)) {
                phantom_bullet->m_bDead = true;
                break;
            } //if

        //draw if not dead
            if (!phantom_bullet->IsDead()) {
                CSpriteDesc2D spr; //create new sprite desc
                spr.m_nSpriteIndex = t;
                spr.m_fXScale = 0.75f;
                spr.m_fYScale = 0.75f;
                spr.m_vPos = phantom_bullet->GetPos();
                m_pRenderer->Draw(spr);   
            }
            else
                break;
        }
        
    }
    delete phantom_bullet;
   
} ///draw_trajectory