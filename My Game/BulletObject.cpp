#include "BulletObject.h"
#include "ParticleEngineScaling.h"
#include "Renderer.h"
#include "ObjectManager.h"
#include "ComponentIncludes.h"

CBulletObject::CBulletObject(eSpriteType t, const Vector2& p) : CObject(t, p) {
    is_bullet = true;
    affected_by_gravity = true; // For our purposes, all bullets should be affected by gravity.
    time_created = m_pStepTimer->GetTotalSeconds();

    time_to_live = 30.0f; //Previously 12second, but this was too short.

    //Set the damage, explosion radius, and/or other attributes based on the bullet type.
    switch (t) {
    case BULLET_SPRITE: //black standard
        damage = 10;
        explosion_radius = 50;
        break;
    case BULLET2_SPRITE: //red - dirt
        damage = 10;
        explosion_radius = 50;
        break;
    case BULLET4_SPRITE: //grey egg shaped - splits in to 3
        damage = 7.5;
        explosion_radius = 50;
        m_fXScale = m_fYScale = m_Sphere.Radius = .75;
        time_to_live = 1.25f;
        break;
    case BULLET5_SPRITE: //green/red missle - large explosion radius
        damage = 12.5;
        explosion_radius = 160;
        m_fXScale = m_fYScale = m_Sphere.Radius = .25;
        break;
    case BULLET6_SPRITE: //green/grey missle- timed explosion
        damage = 10;
        explosion_radius = 100;
        m_fXScale = m_fYScale = m_Sphere.Radius = .25;
        time_to_live = 6.0f;
        break;
    case BULLET7_SPRITE: //yellow rounded square - bounces off planets
        damage = 7.5;
        explosion_radius = 50;
        m_fXScale = m_fYScale = m_Sphere.Radius = .75;
        break;
    case BULLET3_SPRITE: //grey rounded square - teleportation bullet
        damage = 0;
        explosion_radius = 0;
        m_fXScale = m_fYScale = m_Sphere.Radius = .75;
        break;
    case BULLET8_SPRITE: //yellow egg - shoots bullets when right click
        damage = 7.5;
        explosion_radius = 50;
        m_fXScale = m_fYScale = m_Sphere.Radius = .75;
        break;
    case BULLET9_SPRITE: //pointy grey bullet - accelerates until it hits something
        damage = 28;
        explosion_radius = 25;
        m_fXScale = m_fYScale = m_Sphere.Radius = .1;
        m_vAcceleration = m_vVelocity;
        break;
    case BULLET10_SPRITE: //long grey&brown bullet - explode in air when right click
        damage = 7.5;
        explosion_radius = 100;
        m_fXScale = m_fYScale = m_Sphere.Radius = .1;
        break;
    case BULLET11_SPRITE: //short red & grey bullet - "fatman"
        damage = 35;
        explosion_radius = 225;
        m_fXScale = m_fYScale = m_Sphere.Radius = .1;
        explosion_sound = EXPLOSION3_SOUND;
        break;
    case BULLET12_SPRITE: //black cannon ball - creates a pair of wormholes
        damage = 2.5;
        explosion_radius = 5;
        m_fXScale = m_fYScale = m_Sphere.Radius = .1;
        break;
    case MINE_SPRITE:
        damage = 12.5;
        explosion_radius = 120;
        m_fXScale = m_fYScale = m_Sphere.Radius = .75;
        break;
    default:
        break;
    }

}

CBulletObject::CBulletObject(eSpriteType t, const Vector2& p, XMFLOAT4 trail_color) : CBulletObject(t, p) {
    smoke_color = trail_color;
}

void CBulletObject::move() {
    //check if the time is past the ttl
    if (m_pStepTimer->GetTotalSeconds() - time_created >= time_to_live && time_to_live != -1.0f) {
        CBulletObject::kill();
        return;
    }

    //do special stuff on move
    switch (m_nSpriteIndex) {
        case BULLET8_SPRITE: {
            if (m_pMouse->RightMouseReleased() && timesShot < 3) {
                timesShot++;
                float rads = 45.0f * ((float)M_PI / 180.0f); //convert degrees to radians
                for (int i = 0; i < 2; i++) {
                    CBulletObject* pBullet = m_pObjectManager->create_bullet(BULLET4_SPRITE, m_vPos, smoke_color); //create bullet
                    pBullet->SetOwner(owner);
                    pBullet->SetVelocity(GetVelocity());
                    pBullet->onDeathCompleted = true;
                    if (i == 0)
                        pBullet->SetVelocity(Vector2(cosf(rads) * GetVelocity().x - sinf(rads) * GetVelocity().y, sinf(rads) * GetVelocity().x + cosf(rads) * GetVelocity().y));
                    else if (i == 1)
                        pBullet->SetVelocity(Vector2(cosf(-rads) * GetVelocity().x - sinf(-rads) * GetVelocity().y, sinf(-rads) * GetVelocity().x + cosf(-rads) * GetVelocity().y));
                    pBullet->SetOrientation(GetOrientation());
                    pBullet->time_to_live = 15.0f;
                    pBullet->m_fXScale = pBullet->m_fYScale = pBullet->m_Sphere.Radius = .4f;
                }
                m_pAudio->play(SHOOT_SOUND);
            }

        }
            break;
        case BULLET9_SPRITE:
            m_vAcceleration = m_vVelocity;
            break;
        case BULLET10_SPRITE: {
            if (m_pMouse->RightMouseReleased() && timesShot < 3) {
                timesShot++;
                Explode();
            }
        }
            break;
    }

    CObject::move();
    m_fRoll = (float)atan2(m_vVelocity.y, m_vVelocity.x);
    if (!is_phantom)
        EmitSmoke();
}

/// <summary>
/// Kills the bullet like a normal object, but additionally damages any players in the explosion radius.
/// </summary>
void CBulletObject::kill() {
    Explode();

    //Tell the owner, if there is one, how close we were to hitting a target.
    if (owner) owner->adjust_aim(m_pObjectManager->get_nearest_tank_location(m_vPos, owner)); //Tell the owner the distance from the nearest tank at the moment the bullet explodes.

    //do ondeath actions
    OnDeath();

    //Normal object kill
    CObject::kill();
}

void CBulletObject::Explode(){
    if (explosion_radius && !is_phantom) { //We might as well skip this if the radius is 0. Save some cycles. Additionally, we don't want to do all the extra stuff if the bullet is phantom.
      //Create bounding sphere of explosion radius
        BoundingSphere sphere;
        sphere.Center = Vector3(m_vPos.x, m_vPos.y, 0);
        sphere.Radius = explosion_radius;

        //Damage players that intersect bounding sphere
        m_pObjectManager->DamagePlayersInSphere(sphere, damage);

        //Create spark to show that there was an explosion
        CParticleDesc2D d;

        float scale = 2 * explosion_radius / m_pRenderer->GetHeight(explosion_sprite);
        d.m_nSpriteIndex = explosion_sprite;
        d.m_vPos = m_vPos;
        d.m_vVel = Vector2::Zero;
        d.m_fLifeSpan = 0.4f;
        d.m_fScaleInFrac = 0.4f;
        d.m_fFadeOutFrac = 0.5f;
        d.m_fMaxScale = scale;
        d.m_f4Tint = XMFLOAT4(Colors::Red);

        m_pParticleEngine->create(d);

        //Create sound to make explosion audible
        //TODO: I'm not sure how to model the pitch/volume of an explosion...
        float volume = 3.0f;
        float pitch = 0.0f;
        m_pAudio->play(explosion_sound, m_vPos, volume, pitch);
    }
}


/// <summary>
/// Kills the bullet like normal, but since it collided with a planet, additionally do whatever changes it needs to do the planet terrain.
/// </summary>
/// <param name="planet">pointer to the planet whose terrain we need to affect. </param>
void CBulletObject::kill(CPlanetObject* planet) {
    //TODO: Implemenet terrain damage. I'm not sure where to start.
    //Do whatever terrain damage that needs to happen, defined by the bullet type.

    BoundingSphere sphere;
    sphere.Center = Vector3(m_vPos.x, m_vPos.y, 0);
    sphere.Radius = explosion_radius;

    switch (m_nSpriteIndex) {
        //Bullets that destroy terrain
    case BULLET_SPRITE:
    case BULLET4_SPRITE:
    case BULLET6_SPRITE:
    case BULLET8_SPRITE:
    case BULLET9_SPRITE:
    case BULLET10_SPRITE:
    case BULLET11_SPRITE:
    case BULLET12_SPRITE:
    case MINE_SPRITE:
    case BULLET5_SPRITE:
        planet->destroy_terrain(sphere);
    case BULLET7_SPRITE:
        if (bounces >= 2)
            planet->destroy_terrain(sphere);
        break;
        //Bullets that generate terrain
    case BULLET2_SPRITE:
        planet->generate_terrain(sphere);
    case BULLET3_SPRITE:
    default:
        break;
    }

    CBulletObject::OnPlanetHit(planet);

    //Normal bullet kill
    CBulletObject::kill();
}

/// <summary>
/// Perform some action when the bullet dies, depending on what bullet it is.
/// For example, BULLET4 will split into 3 separate smaller bullets.
/// </summary>
void CBulletObject::OnDeath() {
    switch (m_nSpriteIndex) {
    case BULLET4_SPRITE:
        if (this->onDeathCompleted) //do not explode again
            return;
        //create 3 smaller bullets on death
        if (m_pStepTimer->GetTotalSeconds() - time_created >= time_to_live && time_to_live != -1.0f) { //do not run if the ttl is -1 (the bullets have already been created
            float rads = 20.0f * ((float) M_PI / 180.0f); //convert degrees to radians
            for (int i = 0; i < 3; i++) {
                CBulletObject* pBullet = m_pObjectManager->create_bullet(BULLET4_SPRITE, m_vPos, smoke_color); //create bullet
                pBullet->SetOwner(owner);
                pBullet->SetVelocity(GetVelocity() * 1.25);
                pBullet->onDeathCompleted = true;
                if (i == 0)
                    pBullet->SetVelocity(Vector2(cosf(rads) * GetVelocity().x - sinf(rads) * GetVelocity().y, sinf(rads) * GetVelocity().x + cosf(rads) * GetVelocity().y) * 1.25);
                else if (i == 1)
                    pBullet->SetVelocity(Vector2(cosf(-rads) * GetVelocity().x - sinf(-rads) * GetVelocity().y, sinf(-rads) * GetVelocity().x + cosf(-rads) * GetVelocity().y) * 1.25);
                pBullet->SetOrientation(GetOrientation());
                pBullet->time_to_live = 15.0f;
                pBullet->m_fXScale = pBullet->m_fYScale = pBullet->m_Sphere.Radius = .4f;
            }
            m_pAudio->play(SHOOT_SOUND);
        }
        break;
    }
}

/// <summary>
/// Perform some action when the bullet hits a planet, depending on what bullet it is.
/// For example, BULLET7 will bounce along the planet.
/// </summary>
void CBulletObject::OnPlanetHit(CPlanetObject* planet) {
    switch (m_nSpriteIndex) {
        case BULLET7_SPRITE: {
            if (bounces < 2) { //can bounce only 2 times
                //bounce along the surface of the planet
                Vector2 planetPos = planet->GetPos(); //position of planet
                Vector2 bulletPos = GetPos(); //position of bullet
                Vector2 bulletVel = GetVelocity(); //velocity of bullet

                Vector2 normalV = planetPos - bulletPos; //nomral vector of the surface
                normalV.Normalize();

                CBulletObject* pBullet = m_pObjectManager->create_bullet(BULLET7_SPRITE, Vector2(m_vPos.x + (-normalV.x * 10), m_vPos.y + (-normalV.y * 10)), smoke_color); //create bullet
                pBullet->SetOwner(owner);
                pBullet->SetVelocity(-(2 * (normalV * bulletVel) * normalV - bulletVel)); //set correct velocity
                pBullet->bounces = this->bounces + 1;
            }
        }
        break;
        case BULLET3_SPRITE: {
            Vector2 bulletPos = GetPos();
            Vector2 planetPos = planet->GetPos();
            owner->teleport(bulletPos, planet);
            break;
        }
        case BULLET12_SPRITE: { //create 2 wormholes
            Vector2 planetPos = planet->GetPos();
            Vector2 bulletPos = GetPos();

            int deltaX = (int)bulletPos.x - (int)planetPos.x;
            int deltaY = (int)bulletPos.y - (int)planetPos.y;
            float angle = (float)atan2(deltaX, -deltaY) * (180.0f / (float)M_PI) + 90.0f;

            Vector2 dir1Vec = Vector2(cos(angle * (M_PI / 180.0f)), sin(angle * (M_PI / 180.0f)));
            Vector2 dir2Vec = Vector2(cos((angle + 180.0f) * (M_PI / 180.0f)), sin(-angle * (M_PI / 180.0f)));

            Vector2 worm1Pos = planetPos + (dir1Vec * (planet->get_radius() + 350.0f));
            Vector2 worm2Pos = planetPos + (dir2Vec * (planet->get_radius() + 350.0f));

            //create wormholes
            CWormholeObject* w1 = m_pObjectManager->create_wormhole(worm1Pos, m_pTurnManager->getNumPlayers() * 2);
            CWormholeObject* w2 = m_pObjectManager->create_wormhole(worm2Pos, m_pTurnManager->getNumPlayers() * 2, w1);
            w1->SetNextWormhole(w2);


        }
            break;

    }
}