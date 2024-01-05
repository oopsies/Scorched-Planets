#pragma once
#include "Object.h"
#include "PlanetObject.h"
#include "TurnManager.h"
#include "ComponentIncludes.h"

enum class TankState {Manual, Move, MoveLeft, MoveRight, Aim, AimLeft, AimRight, Power, PowerUp, PowerDown, Fire, Wait, Think, PostFire, Dead};
enum class TankAnimationState {Normal, Falling};

class CTankObject : public CObject {
	friend class CObjectManager;

private:
	CPlanetObject* home_planet_pointer;
	float angle_relative_to_planet; ///< The longitude at which the tank is situated on the planet.
	float angle = 0; ///< Angle at which the turret gun is pointing
	float power = 400; ///< Power with which to shoot the turret gun (power == initial velocity)
	int health_points = 100; ///< Health points

	float m_fPowerSpeed = 0; ///< Speed at which to increase or decrease power.

	bool in_control = false; ///< If input is currently controlling this object

	eSoundType damage_sound = EXPLOSION1_SOUND;

	TankAnimationState animation_state = TankAnimationState::Normal; ///< Tells the renderer which animation state we should render.

	// AI private variables
	bool is_player_character = false;
	float accuracy_multiplier = 15.0f;
	TankState current_state = TankState::Wait;
	float desired_angle = 45;
	int desired_angle_direction = 1; // +1 to keep adjusting angle in the pos direction, -1 to adjust in the neg direction, 0 to not change at all.
	float desired_power;
	int desired_power_direction = 1; // +1 to keep adjusting power in the pos direction, -1 to adjust in the neg direction, 0 to not change at all.
	float desired_angle_relative_to_planet;
	int desired_angle_relative_to_planet_direction = 0; // +1 to keep adjusting angle_relative_to_planet in the pos direction, -1 to adjust in the neg direction, 0 to not change at all.
	float previous_distance = 0;

	//variables for pausing after turn
	bool paused_after_hit = false;
	float time_hit = 0.0f;
	bool transStarted = false;
	int playerNumber = 0; ///< Number for player, used for showing whose turn it is during transitions

	//variables for bullet selection
	int selected_bullet = 0; //< Bullet # currently ready to fire
	eSpriteType bullet_types[13] = { BULLET_SPRITE, BULLET2_SPRITE, BULLET4_SPRITE, BULLET5_SPRITE, BULLET7_SPRITE, BULLET6_SPRITE, BULLET3_SPRITE, BULLET8_SPRITE, BULLET9_SPRITE, BULLET10_SPRITE, BULLET11_SPRITE, BULLET12_SPRITE, MINE_SPRITE }; //< Array of avaiable bullet types to choose from
	int bullet_counts[13] = { 9999, 9999, 5, 5, 5, 5, 5, 5, 5, 5, 1, 2, 2}; //< Amount of bullets of each type left to fire
	int bullet_type_count = 13; //< Total number of types of bullets this tank has access to

	//moving/shooting limits
	float maxFuel = 500; //maximum amount of fuel the tank has for moving
	float currentFuel = maxFuel; //when this reaches 0, the player cannot move anymore
	float basePower = 700.0f; //the maximum power the player can shoot at. leftover currentFuel is added onto this amount
	float lastFiredShot; //time of last fired shot


public:

	CTankObject(const Vector2& p, CPlanetObject* planet_pointer); ///< Constructor.
	CTankObject(float relative_angle, CPlanetObject* planet_pointer);
	CTankObject(float relative_angle, CPlanetObject* planet_pointer, XMFLOAT4 color);
	void set_planet_index(CPlanetObject* planet_pointer) { home_planet_pointer = planet_pointer; };
	CPlanetObject* get_planet_index() { return home_planet_pointer; };

	void draw_tank();

	void set_angle(float angle) { this->angle = angle; };
	float get_angle() { return angle; };
	float get_angle_relative_to_planet() { return angle_relative_to_planet; };

	void set_power(float power) { this->power = power; };
	float get_power() { return power; };

	void set_power_speed(float speed);

	void set_health_points(int hp) { health_points = hp; };
	int get_health_points() { return health_points; };
	int take_damage(int damage); ///< Decrease the health points by damage. Returns the current health point after damage is taken.

	void set_in_control(bool hasControl) { in_control = hasControl; } ///< Change whether the game will control
	bool get_in_control() { return in_control; }; ///< Returns whether the input will control object

	void teleport(Vector2& bpos, CPlanetObject* planet);

	void move();
	void SetSmokeColor(XMFLOAT4 color); ///< Set smoke color

	XMFLOAT4 GetColor() { return m_f4Tint; };


	void DeathFX(); ///< Death special effects. Overrides Object.h

	void FireGun(eSpriteType bullet); ///< Fire gun at Tank with bullet type bullet
	float FirePhantomGun(eSpriteType bullet, Vector2 orientation, float power, Vector2 position = Vector2::Zero); ///< Fires a phantom bullet with bullet type bullet. Returns the distance from the nearest tank when that bullet explodes.

	//AI public member functions
	void Think();
	void ThinkDumb();
	void adjust_aim(float new_distance); ///< Adjust the internal aim parameters given a new_distance.
	void adjust_aim(bool monte_carlo = false);
	void set_state(TankState state) { current_state = state; };
	void set_accuracy_multiplier(float acc) { accuracy_multiplier = acc; };
	float get_accuracy_multiplier() { return accuracy_multiplier; };
	void set_is_player_character(bool is_player) { is_player_character = is_player; if (is_player_character) current_state = TankState::Manual; };
	bool get_is_player_character() { return is_player_character; };

	//Bullet selection functions
	int get_selected_bullet() { return selected_bullet; };
	void set_selected_bullet(int n);
	eSpriteType* get_bullet_types() { return bullet_types; };
	int* get_bullet_counts() { return bullet_counts; };
	int get_bullet_type_count() { return bullet_type_count; };
	eSpriteType next_bullet_type(); //Goes to the next bullet type in the array

	//power and fuel setters/getters
	int get_max_fuel() { return maxFuel; };
	float get_current_fuel() { return currentFuel; };
	void set_current_fuel(float fuel) { currentFuel = fuel; };
	float get_base_power() { return basePower; };
	void set_fired_shot_time() { lastFiredShot = m_pStepTimer->GetTotalSeconds(); };
	float get_time_since_last_fired() { return m_pStepTimer->GetTotalSeconds() - lastFiredShot; };
	
	int get_player_number() { return playerNumber; };
	void set_player_number(int n) { playerNumber = n; };

};

