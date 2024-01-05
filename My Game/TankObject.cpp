#include "TankObject.h"
#include "ComponentIncludes.h"
#include "math.h"
#include "ParticleEngineScaling.h"
#include "ComponentIncludes.h"
#include "ObjectManager.h"
#include "Random.h"
#include "SmoothCamera.h"
#include <algorithm>

#define PI XM_PI

template<class T>
const T& clamp(const T& x, const T& lower, const T& upper) {
  return min(upper, max(x, lower));
}


CTankObject::CTankObject(const Vector2& p, CPlanetObject* planet_pointer) : CObject(PLAYER_SPRITE, p) {
  this->home_planet_pointer = planet_pointer;
  Vector2 planet_center = home_planet_pointer->GetPos();
  Vector2 difference = m_vPos - planet_center;
  angle_relative_to_planet = (float) atan2(difference.y, difference.x) * 180 / PI;

  //Internal AI state parameters
  //desired_angle = angle;
  desired_power = power;
  desired_angle_relative_to_planet = this->angle_relative_to_planet;
}

CTankObject::CTankObject(float angle_relative_to_planet, CPlanetObject* planet_pointer) : CObject(GREY1_SPRITE, Vector2::Zero) {
  this->home_planet_pointer = planet_pointer;
  this->angle_relative_to_planet = angle_relative_to_planet;

  //Internal AI state parameters
  //desired_angle = angle;
  power = sqrtf(m_pObjectManager->get_gravitational_constant() * planet_pointer->get_mass() / (float)planet_pointer->get_altitude_at_angle(angle_relative_to_planet)); //Velocity of a circular orbit is sqrt(G*M/R)
  //power = planet_pointer->get_altitude_at_angle(angle_relative_to_planet);
  //power = 500;
  desired_power = power;
  desired_angle_relative_to_planet = this->angle_relative_to_planet;

  Vector2 planet_center = home_planet_pointer->GetPos();
  Vector2 direction_unit_vector = m_vPos - planet_center;
  direction_unit_vector.Normalize();
  int altitude = home_planet_pointer->get_altitude_at_angle(angle_relative_to_planet);
  m_vPos = planet_center + (altitude + m_vRadius.y) * direction_unit_vector; //No animation, just jump to proper spot
}

CTankObject::CTankObject(float angle_relative_to_planet, CPlanetObject* planet_pointer, XMFLOAT4 color) : CObject(GREY1_SPRITE, Vector2::Zero) {
  CTankObject::CTankObject(angle_relative_to_planet, planet_pointer);
  smoke_color = color;
  m_f4Tint = color;
}

//Draw tank
void CTankObject::draw_tank()
{
    Vector2 middle = GetPos();
    Vector2 difference = middle - home_planet_pointer->GetPos();
    difference.Normalize();

    float render_angle = PI + (angle_relative_to_planet) * PI / 180;
    //float render_angle = PI/2 + (home_planet_pointer->get_slope_at_longitude(angle_relative_to_planet)) * PI / 180; //Failed attempt to get the tank to angle when on a slope.

    CSpriteDesc2D sd1; //turret
    sd1.m_fXScale = 1.0f;
    sd1.m_fYScale = 1.0f;
    sd1.m_fAlpha = 1.0f;
    sd1.m_nSpriteIndex = TURRET1_SPRITE;
    sd1.m_vPos = middle; //placeholder value
    sd1.m_fRoll = PI + (angle + angle_relative_to_planet)*PI / 180;
    sd1.m_f4Tint = m_f4Tint;
    //sd1.m_fRoll = get_angle() * (M_PI / 180.0f) + (PI+angle_relative_to_planet*PI/180);

    m_pRenderer->Draw(sd1);

    CSpriteDesc2D sd3; //treads
    float height = m_pRenderer->GetHeight(TREADS1_SPRITE);
    sd3.m_fXScale = 1.0f;
    sd3.m_fYScale = 1.0f;
    sd3.m_fAlpha = 1.0f;
    sd3.m_nSpriteIndex = TREADS1_SPRITE;
    sd3.m_vPos = middle - 0.3*height*difference; //placeholder value
    sd3.m_fRoll = render_angle;
    sd3.m_f4Tint = m_f4Tint;

    m_pRenderer->Draw(sd3);

    CSpriteDesc2D sd2; //tank body
    sd2.m_fXScale = 1.0f;
    sd2.m_fYScale = 1.0f;
    sd2.m_fAlpha = 1.0f;
    sd2.m_nSpriteIndex = GREYBODY1_SPRITE;
    sd2.m_vPos = middle; //placeholder value
    sd2.m_fRoll = render_angle;
    sd2.m_f4Tint = m_f4Tint;

    m_pRenderer->Draw(sd2);
}


///< Decrease the health points by damage. Returns the current health point after damage is taken. Kills the tank if damage is less than 0.
/// \param damage is the number of hp to reduce.
/// \returns the number of health_points after taking damage.

int CTankObject::take_damage(int damage) {
  float volume = (float) damage / (float) health_points; //The volume with which we will play the "OW" sound. It'll be loud if it does more relative damage.
  health_points -= damage;
  if (health_points <= 0) {
    current_state = TankState::Dead;
    this->kill();
  }
  m_pAudio->play(damage_sound, volume);
  return health_points;
}

void CTankObject::teleport(Vector2& bpos, CPlanetObject* planet) {
    home_planet_pointer = planet;
    m_vPos=bpos;
    //Vector2 dif = m_vPos - planet_center;
    //dif.Normalize();
    //angle_relative_to_planet = (float)atan2(dif.y, dif.x) * 180 / PI;
}

void CTankObject::move() {
  //Check if I'm dead
  if (health_points <= 0) {
    kill();
  }

  const float t = m_pStepTimer->GetElapsedSeconds();
  power += m_fPowerSpeed * t; // Affect power if keys are pressed down.
  if (power < 0) { power = 0; }; // It's totally possible to bring the power to the negatives, meaning the projectile starts shooting backwards. Let's not do that.
  angle += 30* m_fRotSpeed * t; //We want to move 30 degrees every second
  angle = static_cast<float>(fmod(angle, 360)); //Make sure our angle is less than 360.
  if (angle < 0) //C++ has this annoying "feature" that negative numbers modulo can still be negative.
    angle += 360;
  m_fRoll = PI + (angle + angle_relative_to_planet) * PI/180; // Set the roll so that it compensates for the inclination on the planet
  const Vector2 viewvec = GetViewVector();
  const float delta = 40.0f * t;

  if (m_bStrafeRight)
    angle_relative_to_planet -= delta;
  

  else if (m_bStrafeLeft)
    angle_relative_to_planet += delta;
  angle_relative_to_planet = modulo(angle_relative_to_planet, 360.0f);

  Vector2 planet_center = home_planet_pointer->GetPos();
  Vector2 direction_unit_vector = Vector2(cosf(angle_relative_to_planet * PI / 180), sinf(angle_relative_to_planet * PI / 180));
  int altitude = home_planet_pointer->get_altitude_at_angle(angle_relative_to_planet);
  //m_vPos = planet_center + (altitude + m_vRadius.y)* direction_unit_vector; //No animation, just jump to proper spot
  Vector2 desired_pos = planet_center + (altitude + m_vRadius.y) * direction_unit_vector;
  float current_altitude = (m_vPos - planet_center).Length();
  if ( current_altitude - (float)altitude - m_vRadius.y > 10 && !m_bStrafeLeft && !m_bStrafeRight) { //We are currently more than 10 units higher than where we should be, and we're not currently moving
    animation_state = TankAnimationState::Falling;
    m_vPos = planet_center + (current_altitude - 3) * direction_unit_vector; //Move 1 unit down
  }
  else {
    animation_state = TankAnimationState::Normal;
    m_vPos = planet_center + (altitude + m_vRadius.y) * direction_unit_vector; //No animation, just jump to proper spot
  }
  
  m_bStrafeLeft = m_bStrafeRight = m_bStrafeBack = false;
  m_Sphere.Center = (Vector3)m_vPos; //update bounding sphere
}

void CTankObject::SetSmokeColor(XMFLOAT4 color) {
  CObject::SetSmokeColor(color);
  m_f4Tint = color;
}///< Set smoke color


/// Set the object's power-up speed in velocity units per second. (Power == initial velocity of projectiles)
/// \param speed Power-up  speed in velocity units per second.
/// 
void CTankObject::set_power_speed(float speed) {
  m_fPowerSpeed = speed;
} //Rotate


/// Perform a death particle effect to mark the death of an object.
void CTankObject::DeathFX() {
  CObject::DeathFX();
  CParticleDesc2D d;
  //d.m_nSpriteIndex = SPARK_SPRITE;
  d.m_nSpriteIndex = EXPLOSION1_SPRITE;
  d.m_vPos = m_vPos;

  d.m_fLifeSpan = 0.5f;
  d.m_fMaxScale = 0.5f;
  d.m_fScaleInFrac = 0.2f;
  d.m_fFadeOutFrac = 0.8f;
  d.m_fScaleOutFrac = d.m_fFadeOutFrac;
  d.m_f4Tint = XMFLOAT4(Colors::Red);

  m_pParticleEngine->create(d);
} //DeathFX

/// Create a bullet object and a flash particle effect.
/// It is assumed that the object is round and that the bullet
/// appears at the edge of the object in the direction
/// that it is facing and continues moving in that direction.
/// \param pObj Pointer to an object.
/// \param bullet Sprite type of bullet.

void CTankObject::FireGun(eSpriteType bullet) {
  CTankObject* pObj = this;
  if (!pObj) return; // Occasionally, if you're trying to fire your gun and your character is dead, the game will crash because you derefence a nullptr.
  if (m_bDead) return; // We don't want to shoot if we're supposed to be dead.
  if (m_pObjectManager->get_bullets_list().size() && m_bTurnsEnabled) return; //We don't want to shoot if there are already bullets.
  m_pAudio->play(SHOOT_SOUND);

  const Vector2 view = pObj->GetViewVector();
  Vector2 pos = pObj->GetPos(); //+
    //0.5f * m_pRenderer->GetWidth(pObj->m_nSpriteIndex) * view;

  //set camera and control lock
  if (m_bTurnsEnabled) {
      m_bControlLock = true;
      m_eCameraMode = CameraMode::BULLET_LOCKED;
  }

  //create bullet object

  CBulletObject* pBullet = m_pObjectManager->create_bullet(bullet, pos, smoke_color); //create bullet
  pBullet->SetOwner(this);

  const Vector2 norm(view.y, -view.x); //normal to direction
  const float m = 2.0f * m_pRandom->randf() - 1.0f;
  const Vector2 deflection = Vector2::Zero; //0.01f * m * norm;

  pBullet->SetVelocity(power * (view + deflection)); // Power is the starting velocity.
  pBullet->SetOrientation(GetOrientation());

  //Decrease bullet count
  if (selected_bullet != 0 && selected_bullet != 1)
    bullet_counts[selected_bullet]--;
  while (!bullet_counts[selected_bullet]) //If we ran out of bullets
    next_bullet_type();

  //Set guntimer
  m_fGunTimer = m_pStepTimer->GetTotalSeconds();

  //particle effect for gun fire

  CParticleDesc2D d;

  d.m_nSpriteIndex = SPARK_SPRITE;
  d.m_vPos = pos;
  d.m_vVel = pObj->GetSpeed() * view;
  d.m_fLifeSpan = 0.25f;
  d.m_fScaleInFrac = 0.4f;
  d.m_fFadeOutFrac = 0.5f;
  d.m_fMaxScale = 0.5f;
  d.m_f4Tint = XMFLOAT4(Colors::Yellow);

  m_pParticleEngine->create(d);
} //FireGun

float CTankObject::FirePhantomGun(eSpriteType bullet, Vector2 orientation, float power, Vector2 position) {
  CTankObject* pObj = this;
  Vector2 pos;
  
  if (position == Vector2::Zero) { //I.e. we go with the default, just use the current position
    pos = pObj->GetPos() + 0.5f * m_pRenderer->GetWidth(pObj->m_nSpriteIndex) * orientation;
  }
  else { //Otherwise, we use the specially designated position to fire from.
    pos = position + 0.5f * m_pRenderer->GetWidth(pObj->m_nSpriteIndex) * orientation;
  }

  //To get better results, we should average this over a couple shots with adjusted angles/power. The physics simulations will mess us up quite frequently.
  int num_simulations = 3;
  float running_distance_sum = 0;

  for (int i = 0; i < num_simulations; i++) {
    if (i)
      power /= 1.01f;
    Vector2 velocity = GetVelocity() + power * orientation; // Power is the starting velocity.
    running_distance_sum += m_pObjectManager->create_phantom_bullet(bullet, pos, velocity, pObj);
  }
  return running_distance_sum / num_simulations;
}//FirePhantomGun



void CTankObject::ThinkDumb() {
  // Dumb AI
  // This "Dumb" AI is implemented as a glorified state machine.
  // States: Manual, Move, MoveLeft, MoveRight, Aim, AimLeft, AimRight, PowerUp, PowerDown, Fire, Wait

  //Switch over the current state. 
  switch (current_state) {
  case TankState::Wait:
    if (m_pStepTimer->GetTotalSeconds() > m_fGunTimer + 1) { //Wait until timer runs out
      current_state = TankState::Move;
    }
    break;


    //All the move states
  case TankState::Move:
    if (m_pRandom->randn(0, 1)) 
      current_state = TankState::MoveLeft; 
    else 
      current_state = TankState::MoveRight;
    break;
  case TankState::MoveLeft:
    StrafeLeft();
    if (m_pRandom->randf() < 0.3f) //30% chance of switching to aiming.
      current_state = TankState::Aim;
    break;
  case TankState::MoveRight:
    StrafeRight();
    if (m_pRandom->randf() < 0.3f) //30% chance of switching to aiming.
      current_state = TankState::Aim;
    break;


    //All the aim states
  case TankState::Aim:
    //Decide whether to move the turret gun or to adjust the power.
    if (m_pRandom->randn(0, 1)) { //Move the turret gun
      switch (m_pRandom->randn(0, 1)) { //Decide to move left or right
      case 1: current_state = TankState::AimLeft; break;
      case 0: current_state = TankState::AimRight; break;
      }
    }
    else { //Adjust the power
      switch (m_pRandom->randn(0, 1)) { //Decide to power up or down
      case 0:
        current_state = TankState::PowerDown;
        if (power < 500)
          current_state = TankState::PowerUp;
        break;
      case 1: 
        current_state = TankState::PowerUp; 
        break;
      }
    }
    break;
  case TankState::AimLeft:
    SetRotSpeed(-1);
    if (m_pRandom->randf() < 0.7f) //90% chance of firing.
      current_state = TankState::Fire;
    break;
  case TankState::AimRight:
    SetRotSpeed(1);
    if (m_pRandom->randf() < 0.7f) //90% chance of firing.
      current_state = TankState::Fire;
    break;
  case TankState::PowerUp:
    set_power_speed(50.0f);
    if (m_pRandom->randf() < 0.7f) //90% chance of firing.
      current_state = TankState::Fire;
    break;
  case TankState::PowerDown:
    set_power_speed(-50.0f);
    if (m_pRandom->randf() < 0.7f) //90% chance of firing.
      current_state = TankState::Fire;
    break;

    //All the fire states
  case TankState::Fire:
    if (angle > 15 && angle < 165) { //For now, only shoot if we're aiming up
      FireGun(BULLET2_SPRITE);
      current_state = TankState::Wait;
    }
    else {
      current_state = TankState::Aim;
    }
    break;



    //If the case is manual, do nothing
  case TankState::Manual:
    break;

    //If unknown case, switch to waiting
  default:
    current_state = TankState::Wait;
    break;
  }
}

void CTankObject::Think() {
  // "Smart" AI
  // This "Smart" AI is implemented as a glorified state machine.
  // States: Manual, Move, MoveLeft, MoveRight, Aim, AimLeft, AimRight, Power, PowerUp, PowerDown, Fire, Wait, Think
  //Switch over the current state. 
  switch (current_state) {
  case TankState::Wait:
    //Make sure I'm not dead.
    if (m_bDead)
      current_state = TankState::Dead;
    //Make sure I'm not a player character
    if (is_player_character)
      current_state = TankState::Manual;
    //Check if it's my turn. If so, switch to think. If not, do nothing
    if ((in_control || !m_bTurnsEnabled) && !is_player_character)
      current_state = TankState::Think;
   
    break;

    //Think==Make a plan for this turn
  case TankState::Think:
    adjust_aim();
    current_state = TankState::Move;
    break;

    //All the move states
  case TankState::Move: {
      //string test_string = "Desired Longitude: " + to_string(desired_angle_relative_to_planet) + "\tCurrent longitude: " + to_string(angle_relative_to_planet) + "\n";
      //OutputDebugStringA(test_string.c_str());
      float diff_angle = modulo(angle_relative_to_planet - desired_angle_relative_to_planet, 360.0f); //How far apart are the angles in positive degrees?
      if ( diff_angle < 180 && diff_angle > 2)  StrafeRight(); //If the difference is less than 180, it's faster to go left. Buffer of 5 degrees longitude to prevent spazzing.
      else if (diff_angle > 180 && diff_angle > 2) StrafeLeft(); //If the difference is greater than 180, it's faster to go right. Buffer of 5 degrees longitude to prevent spazzing.
      else current_state = TankState::Aim;
    }
    break;

    //All the aim states
  case TankState::Aim: {
      desired_angle = modulo(desired_angle, 360.0f);
      float diff_angle = modulo(angle - desired_angle, 360.0f); //How far apart are the angles in positive degrees?
      if (diff_angle > 180 && diff_angle > 1) SetRotSpeed(1); //The difference being greater than 1 gives us some buffer. It's unlikely that we'll ever get angle and desired_angle to be exactly correct with how the steptimer works.
      else if (diff_angle < 180 && diff_angle > 1) SetRotSpeed(-1);
      else {
        current_state = TankState::Power;
        SetRotSpeed(0);
      }
    }
    break;

    //All the Power States
  case TankState::Power:
    if (desired_power - power > 50) set_power_speed(50.0f);
    else if (desired_power - power < -50) set_power_speed(-50.0f);
    else {
      current_state = TankState::Fire;
      set_power_speed(0);
    }
    break;

    //All the fire states
  case TankState::Fire:
    if (m_pStepTimer->GetTotalSeconds() > m_fGunTimer + 3) {
      set_selected_bullet(m_pRandom->randn(0, bullet_type_count - 1)); //Choose a random bullet that we have access to.
      eSpriteType bulletSpr = get_bullet_types()[get_selected_bullet()]; //get correct selected bullet
      FireGun(bulletSpr);
      current_state = TankState::PostFire;
    }
    break;

    //All the Post-Fire states
  case TankState::PostFire:
    if (!m_bTurnsEnabled) {
        if (is_player_character)
            current_state = TankState::Manual;
        else
            current_state = TankState::Wait;

        
    }
    else if (m_pObjectManager->get_bullets_list().size() == 0) { //We don't want to switch to the next player until after all the bullets clear.

        float timeElapsed = m_pStepTimer->GetTotalSeconds() - time_hit;
        float pauseTime = 1.0f;
        float transTime = 1.5f;

        if (!paused_after_hit) {
            paused_after_hit = true;
            time_hit = m_pStepTimer->GetTotalSeconds();
        }
        //transition and pause is over, go to next tank
        else if (paused_after_hit && timeElapsed >= pauseTime + transTime) {
            if (m_bTurnsEnabled)
              m_pPlayer = m_pTurnManager->NextTurn();
            paused_after_hit = false;
            transStarted = false;
            if (is_player_character)
                current_state = TankState::Manual;
            else
                current_state = TankState::Wait;
        }
        //pause is over, start transition
        else if (paused_after_hit && timeElapsed >= pauseTime && !transStarted) {
            //smooth camera stuff
            if (m_bTurnsEnabled) {
                transStarted = true;
                m_eCameraMode = CameraMode::TRANSITION;
                m_pSmoothCam->Setup(transTime, m_pTurnManager->GetNextTank()->GetPos());
            }
        }
    }
    break;

  case TankState::Dead:
    if (m_bDead && m_bTurnsEnabled) {
      m_pPlayer = m_pTurnManager->NextTurn();
    }
    break;



    //If the case is manual, do nothing
  case TankState::Manual:
    break;

    //If unknown case, switch to waiting
  default:
    current_state = TankState::Wait;
    break;
  }
}


/// <summary>
/// Adjusts the internal aim parameters given a new distance. If the shot is better than the last one, it continues adjusting the aim in the same direction. 
/// If it's worse, then it reverses the direction of change.
/// </summary>
/// <param name="new_distance">float describing the new distance to the nearest player from the bullet's explosion.</param>
void CTankObject::adjust_aim(float new_distance) {
  /*
  //Figure out which way we need to move our parameters.
  if (previous_distance) { // We have shot before, so we can check if it's better or not.
    if (new_distance < previous_distance) { // Our new shot was better! WooHoo! Let's keep going in that direction.
      //Direction is fine, so we don't have to make any adjustments.
    }
    else { // Our shot was worse (or equally bad, which means we wasted a shot)! Better reverse course!
      desired_power_direction *= -1;
      desired_angle_direction *= -1;
      desired_angle_relative_to_planet_direction *= -1;
    }
  }
  else { // We have not shot before, so we have nothing to check it against. Just adjust in the pos direction for now.
      //Direction is fine, so we don't have to make any adjustments.
  }

  //Move all of the the internal aim parameters in the desired direction.
  //TODO: Make these linearly independent. These are fine being dependent for now, but not very realistic.
  //TODO: Make these have finer and finer adjustments, probably proportional to the distances.
  desired_power += 20 * desired_power_direction; //Adjust the power by 20 power units.
  desired_angle += 10 * desired_angle_direction; //Adjust the angle of the turret gun by 10deg.
  desired_angle = modulo(desired_angle, 360.0f);
  desired_angle_relative_to_planet += 5 * desired_angle_relative_to_planet_direction; //Adjust the longitude by 5 degrees.
  desired_angle_relative_to_planet = modulo(desired_angle_relative_to_planet, 360.0f);
  previous_distance = new_distance; //Adjust the previous_distance parameter. This is only a first-order minimizer. We don't care about previous attempts.
  */
}

void CTankObject::adjust_aim(bool monte_carlo) {
  float test_angle = desired_angle;
  float test_longitude = desired_angle_relative_to_planet;
  float test_power = desired_power;
  float test_roll = PI + (test_angle + test_longitude) * PI / 180;  // Set the roll so that it compensates for the inclination on the planet
  Vector2 view = Vector2(-sinf(test_roll), cosf(test_roll)); //Orientation of the phantom bullet.
  previous_distance = FirePhantomGun(WATER_SPRITE, view, test_power); //Initial guess, so we have something to compare to.
  float new_distance;
  float gradient_step_size = 1;
  float descent_step_size = 5;
  float d_angle, d_power, d_longitude; //The derivatives of angle, power, and longitude, evaluated at a point
  float start_time = m_pStepTimer->GetElapsedSeconds();
  if (monte_carlo) { //Monte Carlo methods are purely random. They're fast, but no promise of accuracy.
    for (int i = 0; i < 1000 * accuracy_multiplier; i++) { //Do a thousand test bullets.
    //Randomize the aim parameters
      test_angle = (float)m_pRandom->randn(-15, 195);
      test_angle = modulo(test_angle, 360.0f);
      test_longitude = test_longitude; //For now, let's not move the tank around.
      test_power = (float)m_pRandom->randn(50, 1000);
      test_roll = PI + (test_angle + test_longitude) * PI / 180;  // Set the roll so that it compensates for the inclination on the planet
      view = Vector2(-sinf(test_roll), cosf(test_roll)); //Orientation of the phantom bullet.
      new_distance = FirePhantomGun(WATER_SPRITE, view, test_power);
      //string test_string = "Previous: " + to_string(previous_distance) + "\tNew: " + to_string(new_distance) + "\n";
      //OutputDebugStringA(test_string.c_str());
      if (new_distance < previous_distance) { //Our test shot was better! WooHoo! Let's use that to refine our shots.
        previous_distance = new_distance;
        float deflection = 2.5f * (2.f * m_pRandom->randf() - 1.f);
        desired_angle = test_angle + deflection;
        desired_angle_relative_to_planet = test_longitude;
        desired_power = test_power;
      }
    }
  }
  else { // If we don't use a Monte Carlo method, then let's use gradient descent.
    //NOTE: This is not currently functioning great.
    //each iteration, one uses the formula a_(n+1)=a_n-lambda*DF(a_n), where a_n is the input vector, lambda is a step size, and DF is the gradient of the function (in this case taking in a set of parameters and outputting a distance to nearest tank)
    //We want to minimize this, and this sequence should converge to a (local) minimum (under certain conditions).
    //We can calculate this component-wise, to avoid having to do a bunch of vector calculus.

    // Adjust the longitude
    // Since I can never get the step size right for longitude, I'm going to just have it move a random amount left or right, then aim from there.
    float max_d_longitude = 20.f;
    float change_in_longitude = 2*max_d_longitude*m_pRandom->randf()-max_d_longitude; //Between +/- 10 deg
    desired_angle_relative_to_planet += change_in_longitude;
    test_longitude = desired_angle_relative_to_planet;
    test_longitude = modulo(test_longitude, 360.f);
    test_roll = PI + (test_angle + test_longitude) * PI / 180;  // Set the roll so that it compensates for the inclination on the planet

    int iterations = 100;
    if (!m_bTurnsEnabled)
        iterations = 5;

    //Gradient Descent Time
    for (int i = 0; i < iterations * accuracy_multiplier && m_pStepTimer->GetElapsedSeconds() - start_time < .5f; i++) {
      //Calculate DF (The gradient of the distance function calculated at the current 
      //Angle
      view = Vector2(-sinf(test_roll), cosf(test_roll)); //Orientation of the phantom bullet.
      float d_roll = PI + (test_angle + gradient_step_size + test_longitude) * PI / 180;
      Vector2 d_view = Vector2(-sinf(d_roll), cosf(d_roll)); //Orientation of the phantom bullet.
      d_angle = (FirePhantomGun(WATER_SPRITE, d_view, test_power) - FirePhantomGun(WATER_SPRITE, view, test_power)) / gradient_step_size; // Derivative of distance w.r.t. angle

      //Longitude
      //Find the new location to fire from with that longitude
      Vector2 planet_center = home_planet_pointer->GetPos();
      Vector2 direction_unit_vector = Vector2((float)cos((test_longitude + gradient_step_size )* PI / 180), (float)sinf((test_longitude + gradient_step_size) * PI / 180));
      int altitude = home_planet_pointer->get_altitude_at_angle((test_longitude + gradient_step_size));
      Vector2 temp_position = planet_center + (altitude + m_vRadius.y) * direction_unit_vector;

      d_roll = PI + (test_angle + test_longitude + gradient_step_size) * PI / 180;
      d_view = Vector2(-sinf(d_roll), cosf(d_roll)); //Orientation of the phantom bullet.
      //d_longitude = (FirePhantomGun(WATER_SPRITE, d_view, test_power, temp_position) - FirePhantomGun(WATER_SPRITE, view, test_power)) / gradient_step_size; // Derivative of distance w.r.t. longitude
      

      //Power
      //We use 10*gradient_step_size instead of gradient_step_size as our step, since power has a much wider range, and small tweaks to power may not overcome the noise
      d_power = (FirePhantomGun(WATER_SPRITE, view, test_power-10*gradient_step_size) - FirePhantomGun(WATER_SPRITE, view, test_power)) / (10*gradient_step_size); // Derivative of distance w.r.t. power

      //Calculate the next iteration of parameters in our gradient descent.
      test_angle = test_angle - descent_step_size * d_angle;
      test_angle = modulo(test_angle, 360.0f);
      test_angle = clamp(test_angle, 5.f, 175.f);
      float temp_longitude = test_longitude;
      //test_longitude = test_longitude - descent_step_size * d_longitude; //Adjust by 10, because it always overcompensated.
      //test_longitude = clamp(test_longitude, temp_longitude - 15.f, temp_longitude + 15.f);
     // test_longitude = modulo(test_longitude, 360.0f);
      test_power = test_power - 10*descent_step_size * d_power;
      //test_power = std::max(test_power, 50.f); //Don't let power get close to zero. Gradient Descent suffers from a vanishing gradient near 0.
      test_power = clamp(test_power, 50.f, 1000.f); //Don't let the power get close to zero or bigger than 1000. Gradient descent suffers from a vanishing gradient.
      test_roll = PI + (test_angle + test_longitude) * PI / 180;  // Set the roll so that it compensates for the inclination on the planet
      view = Vector2(-sinf(test_roll), cosf(test_roll)); //Orientation of the phantom bullet.
      new_distance = FirePhantomGun(WATER_SPRITE, view, test_power);
      if (new_distance < previous_distance) { //Our test shot was better! WooHoo! Let's use that to refine our shots.
        //string test_string = "Previous distance: " + to_string(previous_distance) + "\tNew distance: " + to_string(new_distance) + "\n";
        //test_string += "Angle: " + to_string(test_angle) + "\tLongitude: " + to_string(test_longitude) + " " + to_string(angle_relative_to_planet) + "\tPower: " + to_string(test_power) + "\n";
        //OutputDebugStringA(test_string.c_str());
        previous_distance = new_distance;
        float deflection = 0; //2.5 * (2 * m_pRandom->randf() - 1);
        desired_angle = test_angle + deflection;
        desired_angle_relative_to_planet = test_longitude;
        desired_power = test_power;
      }

    }//for
    
  }
  
}

//Go to next bullet type, go to first one if at end
//Returns the bullet sprite
eSpriteType CTankObject::next_bullet_type() {
    selected_bullet++;
    if (selected_bullet >= bullet_type_count)
        selected_bullet = 0;
    while (!bullet_counts[selected_bullet]) { //If that's empty, then switch to the next bullet type
      next_bullet_type();
    }

    return bullet_types[selected_bullet];
}

/// <summary>
/// Sets the selected bullet. Will continue moving forward to the next bullet if that bullet type ran out.
/// </summary>
/// <param name="n">the desired bullet id</param>
void CTankObject::set_selected_bullet(int n) { 
  selected_bullet = n; 
  while (!bullet_counts[selected_bullet]) { //If that's empty, then switch to the next bullet type
    next_bullet_type();
  }
};