/// \file ObjectManager.h
/// \brief Interface for the object manager CObjectManager.

#pragma once

#include <list>

#include "Component.h"
#include "Common.h"
#include "Settings.h"

#include "Object.h"
#include "PlanetObject.h"
#include "TankObject.h"
#include "BulletObject.h"
#include "WormholeObject.h"

using namespace std;

/// \brief The object manager.
///
/// A collection of all of the game objects.

class CObjectManager: 
  public CComponent, 
  public CCommon, 
  public CSettings{

  private:
    list<CObject*> m_stdObjectList; ///< Object list.
    list<CObject*> m_massive_objects; ///< Objects that have mass
    list<CObject*> m_objects_affected_by_gravity; ///< Objects that are affected by gravity
    list<CPlanetObject*> m_planets_list; ///< Planet list
    list<std::shared_ptr<CTankObject>> m_tanks_list; ///< List of all tanks. This is a list of shared pointers to avoid crashes where tanks fire guns while dying.
    list<CBulletObject*> m_bullets_list; ///< List of all bullets. Used to keep track of what's flying around.
    list<CWormholeObject*> m_wormholes_list; ///< List of all wormholes

    void BroadPhase(); ///< Broad phase collision detection and response.
    void NarrowPhase(CObject* p0, CObject* p1); ///< Narrow phase collision detection and response.
    void NarrowPhase(CPlanetObject* p0, CObject* p1); ///< Narrow phase collision detection and response where the first object is a planet.
    void NarrowPhase(std::shared_ptr<CTankObject> p0, CObject* p1); ///< Narrow phase collision detection and response where the first object is a tank.
    void NarrowPhase(CWormholeObject* p0, CObject* p1); ///< Narrow phase collision detection and response where first object is a wormhole
    bool AtWorldEdge(CObject* p); ///< Test whether at the edge of the world.
    bool AtWorldEdge(Vector2& pos);
    void CullDeadObjects(); ///< Cull dead objects.



    double gravitational_constant = 5000000;
    double softening_parameter = 0;

  public:
    CObjectManager(); ///< Constructor.
    ~CObjectManager(); ///< Destructor.

    CObject* create(eSpriteType t, const Vector2& v, double mass = 0, bool affected_by_gravity = FALSE); ///< Create new object.
    CPlanetObject* create_planet(const Vector2& p, double mass = 0, int radius = 500, bool affected_by_gravity = FALSE); ///< Create new planet.    
    std::shared_ptr<CTankObject> create_tank(float angle_relative_to_planet, CPlanetObject* home_planet); ///< Create new Tank
    std::shared_ptr<CTankObject> create_tank(float angle_relative_to_planet, CPlanetObject* home_planet, XMFLOAT4 color); ///< Create new Tank with a given color.
    CBulletObject* create_bullet(eSpriteType t, const Vector2& v); ///< Create new bullet
    CBulletObject* create_bullet(eSpriteType t, const Vector2& v, XMFLOAT4 trail_color); ///< Create new bullet with the color trail
    CWormholeObject* create_wormhole(const Vector2& pos, int ttl); ///< Create wormhole
    CWormholeObject* create_wormhole(const Vector2& pos, int ttl, CWormholeObject* next); ///< Create wormhole with next wormhole already in mind

    void clear(); ///< Reset to initial conditions.
    void move(); ///< Move all objects.
    void draw(); ///< Draw all objects.

    void FireGun(CObject* p, eSpriteType bullet); ///< Fire object's gun.

    void DamagePlayersInSphere(BoundingSphere sphere, int damage); ///< Damage all players that intersect with sphere. Used to implement damaging explosions.

    //Physics simulation functions
    Vector2 calculate_gravity(Vector2 position); ///< Calculates the gravitational field at the position
    float calculate_gravitational_potential(Vector2 position); ///< Calculates the gravitational potential at a position
    float calculate_total_mechanical_energy(Vector2 position, Vector2 velocity); ///< Calculates the total mechanical energy of a particle with velocity at position.
    void calculate_total_energy_for_all_objects();
    CPlanetObject* calculate_closest_planet(Vector2 position); ///< Calculates the closest planet at a position.
    float get_gravitational_constant() { return (float)gravitational_constant; };

    //AI functions
    std::shared_ptr<CTankObject> get_nearest_tank(Vector2 position); ///< Returns the nearest tank to a location.
    float get_nearest_tank_location(Vector2 position, CTankObject* origin_tank=nullptr); ///< Returns the nearest tank to a location.
    float create_phantom_bullet(eSpriteType t, const Vector2& position, const Vector2& velocity, CTankObject* owner); ///< Create a phantom bullet, which moves "instantly". Returns the distance to the nearest tank.
    void draw_trajectory(eSpriteType t, const Vector2& position, const Vector2& velocity, CTankObject* owner); ///< Draws the trajectory based on power

    list<std::shared_ptr<CTankObject>> get_tanks_list() { return m_tanks_list; };
    list<std::shared_ptr<CTankObject>>* get_tanks_list_pointer() { return &m_tanks_list; };
    list<CPlanetObject*> get_planets_list() { return m_planets_list; };
    list<CPlanetObject*>* get_planets_list_pointer() { return &m_planets_list; };
    list<CBulletObject*> get_bullets_list() { return m_bullets_list; };
    list<CWormholeObject*>* get_wormholes_list_pointer() { return &m_wormholes_list; };


}; //CObjectManager