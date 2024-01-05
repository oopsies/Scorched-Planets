#include "PlanetObject.h"
#include "math.h"
#include "TankObject.h"
#include "Random.h"
#include "directxcollision.h"

#include "Particle.h"
#include "ParticleEngineScaling.h"
#include "StepTimer.h"

#define PI XM_PI


/// <summary>
/// 
/// </summary>
/// <param name="p">Vector2 representing the position of the planet center.</param>
/// <param name="radius">int representing the "sea-level radius" of the planet in pixels</param>
/// <param name="step_size">float used in the random generation of the planet.</param>
CPlanetObject::CPlanetObject(const Vector2& p, int radius, float step_size) : CObject(PLANET_SPRITE, p), altitudes(number_of_altitudes) {
	sealevel_radius = radius;

	//TODO: Make this procedural generation much better....
	//generate_noise_fractal_naive(9, step_size);
	CPlanetObject::generate_noise_planetary_method(2000, 2, 0);

	//set the minimum/maximum altitudes for faster collision detection.
	for (int altitude: altitudes){
		if (altitude > maximum_altitude) maximum_altitude = altitude; //Set the maximum altitude, if needed.
		if (altitude < minimum_altitude) minimum_altitude = altitude; //Set the minimum altitude, if needed.
	}
	core_radius = static_cast<int>(sealevel_radius * .3);

	//Set the "core" collision. This shouldn't account for terrain differentials
	m_Sphere.Radius = (float) core_radius;
	m_Sphere.Center = Vector3((float) m_vPos.x, (float) m_vPos.y, 0);
	//Set the maximum altitude boundary sphere. This will allow us to save some cycles on collision. We only need to check surface collisions if they intersect the maximum alitude.
	maximum_altitude_sphere.Radius = (float) maximum_altitude;
	maximum_altitude_sphere.Center = Vector3((float) m_vPos.x, (float) m_vPos.y, 0);
}

void CPlanetObject::draw_planet() {
	//int number_of_altitudes = *(&altitudes + 1) - altitudes; // Calculate the number of altitudes in the altitudes array.
	Vector2 center = GetPos();
	Vector2 endpoint;
	Vector2 midpoint;
	Vector2 previousendpoint;
	Vector2 waterendpoint;
	float angle;
	int interpolation_constant = 3; //How many lines we want to interpolate to smooth out the surface
	int num_waves = 10; //Number of waves to have on each planet.

	//Draw Oceans
	CSpriteDesc2D sd;
	sd.m_fXScale = sealevel_radius/(m_pRenderer->GetWidth(WATER_SPRITE)/2);
	sd.m_fYScale = sd.m_fXScale*.92f; //Make this slightly oblate, so we can rotate it and get waves.
	sd.m_fAlpha = 0.5f/(float)num_waves;
	sd.m_nSpriteIndex = WATER_SPRITE;
	sd.m_vPos = center;
	for (int i = 0; i < num_waves; i++) {
		sd.m_fRoll = (float)XM_2PI * sinf((float)i + .07f*m_pStepTimer->GetTotalSeconds());
		m_pRenderer->Draw(sd);
	}

	//Draw Atmosphere
	sd.m_fXScale = (float)maximum_altitude*1.1f/ (m_pRenderer->GetWidth(ATMOSPHERE_SPRITE) / 2);
	sd.m_fYScale = sd.m_fXScale;
	sd.m_fAlpha = .8f;
	sd.m_fRoll = 0.f;
	sd.m_f4Tint = XMFLOAT4(Colors::SkyBlue);
	sd.m_nSpriteIndex = ATMOSPHERE_SPRITE;
	m_pRenderer->Draw(sd);

	// Draw a line of each distance from the center.
	// It will have an angle that is 2pi/index, where index is which distance it is.
	CSpriteDesc2D planet_sprite;
	planet_sprite.m_fXScale = 10 / (m_pRenderer->GetWidth(PLANETLAYER_SPRITE));
	planet_sprite.m_nSpriteIndex = PLANETLAYER_SPRITE;
	planet_sprite.m_fAlpha = 1.f;

	for (int i = 0; i < number_of_altitudes; i++) {
		if (i != 0) {
			previousendpoint = endpoint;
		}
		angle = 2 * (float) PI * (float) i / number_of_altitudes;
		endpoint = center + (float) altitudes[i] * Vector2((float) cos(angle), (float) sin(angle)); // Start at the center, and move in the correct angle the correct distance
		midpoint = center + (float)altitudes[i] / 2 * Vector2((float)cos(angle), (float)sin(angle));// Start at the center, and move in the correct angle the correct distance
		planet_sprite.m_vPos = midpoint;
		planet_sprite.m_fRoll = -PI/2 + angle;
		planet_sprite.m_fYScale = altitudes[i]/ (m_pRenderer->GetHeight(PLANETLAYER_SPRITE));
		m_pRenderer->Draw(planet_sprite);
		//Linear interpolation.
		//An attempt to make the planet curves much smoother. But it did not go well. GPU was not amused.
		//TODO: Make the planet rendering smoother.
		/*for (int j = 0; j < interpolation_constant; j++) {
			float partial_distance = i + (float)j / (float)interpolation_constant;
			float height = altitudes[i] * (i - partial_distance) + altitudes[modulo(i + 1, number_of_altitudes)] * (partial_distance - (i + 1)); //Linear Interpolation
			angle = 2 * (float)PI * (float)partial_distance / number_of_altitudes;
			//endpoint = center + (float)height * Vector2((float)cos(angle), (float)sin(angle)); // Start at the center, and move in the correct angle the correct distance
			midpoint = center + (float)height / 2 * Vector2((float)cos(angle), (float)sin(angle));// Start at the center, and move in the correct angle the correct distance
			sd.m_vPos = midpoint;
			sd.m_fRoll = -PI / 2 + angle;
			sd.m_fYScale = height / (m_pRenderer->GetHeight(PLANETLAYER_SPRITE));
			m_pRenderer->Draw(sd);
		}*/

		/*if (altitudes[i] < sealevel_radius) {
			//If the altitude is less than sealevel, then we know we must be below water. So we render some crude "ocean".
			waterendpoint = center + sealevel_radius * Vector2(cos(angle), sin(angle));
			m_pRenderer->DrawLine(BULLET2_SPRITE, endpoint, waterendpoint);
		}*/
		if (i != 0) {
			//m_pRenderer->draw_triangle(center, previousendpoint, endpoint);
		}


	}
}

int CPlanetObject::get_altitude_at_angle(float angle) {
	float degrees_per_altitude_change = 360.0f / number_of_altitudes; // The distances are sample of the height of the planet from the core as we walk around the planet. If we have num distances, then each step is 360deg/num
	int altitude_index = (int)((int) angle / degrees_per_altitude_change);
	altitude_index = modulo(altitude_index, number_of_altitudes); // Avoids a weird error where sometimes the index is calculated as negative
	return altitudes[altitude_index];
}

int CPlanetObject::get_altitude_index_under_point(Vector2 p) {
	float degrees_per_altitude_change = 360.0f / number_of_altitudes;

	Vector2 direction = p - m_vPos; // Vector pointing from the center to the outside point.
	float angle = (float) atan2(direction.y, direction.x);
	angle *= (float) (180 / PI); //Convert to degrees

	int altitude_index = (int)((int)angle / degrees_per_altitude_change);
	altitude_index = modulo(altitude_index, number_of_altitudes); // Avoids a weird error where sometimes the index is calculated as negative
	return altitude_index;
}

Vector2 CPlanetObject::get_surface_vector_at_index(int altitude_index) {
	float angle = (float) (2 * PI * (float) altitude_index / number_of_altitudes);
	altitude_index = modulo(altitude_index, number_of_altitudes);
	int altitude = altitudes[altitude_index];
	return m_vPos + (float) altitude * Vector2((float) cos(angle), (float) sin(angle));
}

/// <summary>
/// Used for the naive fractal noise generator. This lets us use more "spicy" functions for our base function at each octave.
/// </summary>
/// <param name="x"></param>
/// <returns></returns>
float func(float x) {
	return (float) sin(4 * PI * x); //Chosen with trial and error to get the kind of shape I want
}

///< Generates a procedurally generated planet surface using a simple 1D fractal noise algorithm
/// Based on this DESMOS activity: https://www.desmos.com/calculator/u5znu9c6ny
//TODO: Write a more sophisticated procedurally generated noise algorithm, potentially based on perlin noise?
void CPlanetObject::generate_noise_fractal_naive(int num_iterations, float step_size) {
	float step_size_power;
	for (int i = 0; i < number_of_altitudes; i++) {
		altitudes[i] = (int) 1.05 * sealevel_radius; //When the planet terrain starts at sealevel, about half of it ends up underwater (as expected), but I want more land than water, to maximize play area.
		for (int j = 0; j < num_iterations; j++) {
			step_size_power = (float) pow(step_size, j);
			//First term (radius/(pow * 10)) is the scaling. Basically, how "tall" mountains are likely to be. Chosen arbitrarily. step_size_power make it so each octave is exponentially less extreme
			//Second term (sin(PI....)) makes sure that the two endpoints on the interval are 0 and smooth, so we don't get weird cliffs at longitude=0
			//Third term (func(...)) is the magic. step_size_power make it so each octave is exponentially higher frequency.
			altitudes[i] += (int) ((sealevel_radius/(10 * step_size_power)) * sin((float) PI* (float) i / (number_of_altitudes)) * func(step_size_power * (float) i / (number_of_altitudes)));
		}
		if (altitudes[i] > maximum_altitude) maximum_altitude = altitudes[i]; //Set the maximum altitude, if needed.
		if (altitudes[i] < minimum_altitude) minimum_altitude = altitudes[i]; //Set the maximum altitude, if needed.
	}
}//generate_noise

///< Generates a procedurally generated planet surface using a simple circle version of the planetary method
/// Based on the article "Modelling Fake Planets" at this URL: http://paulbourke.net/fractals/noise/
//TODO: Write a more sophisticated procedurally generated noise algorithm, potentially based on perlin noise?
void CPlanetObject::generate_noise_planetary_method(int num_iterations, int height_step, int indices_to_move) {
	CRandom random = CRandom();
	int random_index, up_down;
	//The tallest mountain in the solar system (relative to planet size) is Caloris Montes on Mercury, which is .12% of the radius.
	//.12% however, is /way/ too small for dramatic effect in our game. So we'll multiply it by 100. Sometimes, these hills are a tad /too/ dramatic. But we can work with that.
	const float tallest_mountain_altitude = (float) sealevel_radius * .12; 
	if (!indices_to_move) //If we don't specify how much to move this round, we'll adjust half the planet.
		indices_to_move = number_of_altitudes / 2;

	//Initialize all heights as 0
	for (int i = 0; i < number_of_altitudes; i++) {
		altitudes[i] = 0;
	}
	maximum_altitude = minimum_altitude = 0;


	for (int i = 0; i < num_iterations; i++) {
		random_index = random.randn(0, number_of_altitudes);
		up_down = random.randn(0, 1); // The probability of going up is equal to the $f(upper bound)/(upper bound + 1) $f
		for (int j = 0; j < indices_to_move; j++) {
			int index = modulo(random_index + j, number_of_altitudes);
			if ((bool) up_down) {
				altitudes[index] += height_step;
				if (altitudes[index] > maximum_altitude) maximum_altitude = altitudes[index]; //Set the maximum altitude, if needed.
			} else {
				altitudes[index] -= height_step;
				if (altitudes[index] < minimum_altitude) minimum_altitude = altitudes[index]; //Set the maximum altitude, if needed.
			}
		}
	}
	//Scale the heights so that mountains are in the right range. Then add sealevel_radius.
	for (int i = 0; i < number_of_altitudes; i++) {
		altitudes[i] = static_cast<int>((float)altitudes[i] * tallest_mountain_altitude/((float)maximum_altitude-minimum_altitude)); //Scale the current altitude so that the tallest peak is at most tallest_mountain_altitude % of the radius
		altitudes[i] += sealevel_radius;
	}
	maximum_altitude = minimum_altitude = 0;
}//generate_noise



bool CPlanetObject::Intersects(BoundingSphere &object_boundary) {
	if (m_Sphere.Intersects(object_boundary)){
		return TRUE;
	}
	else if (maximum_altitude_sphere.Intersects(object_boundary)){
		Vector2 center = Vector2(object_boundary.Center.x, object_boundary.Center.y);
		int altitude_index = get_altitude_index_under_point(center);


		Vector2 v0, v1;

		//Now, we need to check if it intersects the triangles below the point.
		//A triangle has vertices of the planet center and the surface positions at two adjacent altitudes indices
		//We check two triangles back and two triangles forward.
		for (int i = -2; i < 2; ++i) {
			int current_index = modulo((altitude_index + i), number_of_altitudes); // We want to make sure we get indices that are within the proper range.
			v0 = get_surface_vector_at_index(current_index);
			v1 = get_surface_vector_at_index(current_index + 1);
			if (m_vPos != v0 && m_vPos != v1 && v0 != v1) { //Things crash if the triangle is degenerate (i.e. two points are the same).
				if (object_boundary.Intersects(m_vPos, v0, v1)) {
					return TRUE;
				}
			}
		}
	}
	
	//Didn't intersect any thing, so we can return FALSE.
	return FALSE;
}//Intersects

void CPlanetObject::draw_smoke(int start_altitude_index, int final_altitude_index) {
	int interval = final_altitude_index - start_altitude_index;

	CParticleDesc2D d;
	d.m_nSpriteIndex = WHITESMOKE_SPRITE;
	d.m_fLifeSpan = 1.5f; //1.75f + m_pRandom->randf()/2.0f; //short lived
	d.m_fFadeInFrac = 0.1f; //fade in
	d.m_fFadeOutFrac = 1.f; //fade out
	d.m_fFriction = -1.0f;
	d.m_fRSpeed = .5f * (m_pRandom->randf() - 0.5f); //spin each puff of smoke
	d.m_f4Tint = XMFLOAT4(Colors::SlateGray);

	for (int i = start_altitude_index; i < final_altitude_index; i++) {

		d.m_fMaxScale = 3*exp(-pow(.1*abs((float)i - start_altitude_index - (float)interval / 2),2.f));
		d.m_fScaleInFrac = 0.5f+ (1-d.m_fMaxScale/3); //scale in

		d.m_vPos = get_surface_vector_at_index(i);
		d.m_vVel = GetPos() -d.m_vPos; // Point towards the center of the planet
		d.m_vVel.Normalize();		
		d.m_vVel *= 10.f*d.m_fMaxScale;

		m_pParticleEngine->create(d); //create smoke puff
	}
}


/// <summary>
/// Destroys all of the terrain of a planet that is within the explosion radius.
/// </summary>
/// <param name="object_boundary">BoundingSphere that represents the explosion radius.</param>
void CPlanetObject::destroy_terrain(BoundingSphere& object_boundary) {
	Vector2 origin = m_vPos; // use origin to represent the center of the planet.
	Vector2 center = Vector2(object_boundary.Center.x, object_boundary.Center.y); //Center of the offending object
	Vector2 difference = center - origin;
	float r = (float)object_boundary.Radius; // Radius of the offending object
	float h = (float)difference.Length(); //Distance between the planet core and offending object

	float angle; //Angle from the positive x-axis to the altitude we are adjusting.
	float delta_angle = (float)abs(asin(r / h)); //Angle in radians measuring how far we have to sweep (when centered at the planet core) from the offending object center to its radius. This can be found with a little bit of trigonometry.
	int delta_altitude_index = (int) ceil(delta_angle * (float) number_of_altitudes / (2 * PI)); //The number of altitude indices that that angle translates to
	Vector2 direction; //Direction from the planet center to surface at an angle

	float f_length; //We will store the a float version of length here.

	int altitude_index = get_altitude_index_under_point(center); //Altitude index under the center of the offending object
	draw_smoke(altitude_index - delta_altitude_index, altitude_index + delta_altitude_index);

	for (int i = altitude_index - delta_altitude_index; i < altitude_index + delta_altitude_index; i++) {
		angle = 2 * PI * i / number_of_altitudes; //Angle from the positive x-axis to the altitude we are adjusting.
		direction = Vector2((float)cos(angle), (float)sin(angle)); //Unit vector from the planet core to the offending object center
		int current_index = modulo(i, number_of_altitudes); // We want to make sure we get indices that are within the proper range.
		int& length = altitudes[current_index]; //Length from the planet center to the surface at an angle

		//OutputDebugStringA(("Index: " + to_string(current_index) + "\tHeight: " + to_string(length) + "\n").c_str());
		if (length > core_radius + 5 && object_boundary.Intersects(origin, direction, f_length)) {
			if (length < f_length) { //altitude does not intersect with boundingsphere
			}
			else if (length >= f_length) {
				//Find the second intersection point and store it in f_length2
				float f_length2;
				object_boundary.Intersects(origin + (f_length + 1) * direction, direction, f_length2); 
				//OutputDebugStringA(("Index: " + to_string(current_index) + "\tLength: " + to_string(length) +"\tf_Length: " + to_string(f_length)+ "\tf_Length2: " + to_string(f_length2) + "\n").c_str());
				length = static_cast<int>(max(f_length, length - f_length2));
			}
		}
		length = max(length, core_radius + 5);// Don't want to expose the core
	}
} //destroy_terrain

/// <summary>
/// Generates terrain within the explosion radius, which immediately falls down to the planet's surface.
/// </summary>
/// <param name="object_boundary">BoundingSphere that represents the explosion radius.</param>
void CPlanetObject::generate_terrain(BoundingSphere& object_boundary) {
	Vector2 origin = m_vPos; // use origin to represent the center of the planet.
	Vector2 center = Vector2(object_boundary.Center.x, object_boundary.Center.y); //Center of the offending object
	Vector2 difference = center - origin;
	float r = (float)object_boundary.Radius; // Radius of the offending object
	float h = (float)difference.Length(); //Distance between the planet core and offending object

	float angle; //Angle from the positive x-axis to the altitude we are adjusting.
	float delta_angle = (float)abs(asin(r / h)); //Angle in radians measuring how far we have to sweep (when centered at the planet core) from the offending object center to its radius. This can be found with a little bit of trigonometry.
	int delta_altitude_index = (int)ceil(delta_angle * (float)number_of_altitudes / (2 * PI)); //The number of altitude indices that that angle translates to
	Vector2 direction; //Direction from the planet center to surface at an angle

	float f_length; //We will store the a float version of length here. The compiler doesn't like it when we do an implicit cast in the function call

	int altitude_index = get_altitude_index_under_point(center); //Altitude index under the center of the offending object
	for (int i = altitude_index - delta_altitude_index; i < altitude_index + delta_altitude_index; i++) {
		angle = 2 * PI * i / number_of_altitudes; //Angle from the positive x-axis to the altitude we are adjusting.
		direction = Vector2((float)cos(angle), (float)sin(angle)); //Unit vector from the planet core to the offending object center
		int current_index = modulo(i, number_of_altitudes); // We want to make sure we get indices that are within the proper range.
		int& length = altitudes[current_index]; //Length from the planet center to the surface at an angle
		f_length = static_cast<float>(length); //That altitude length as a float.

		if (object_boundary.Intersects(origin, direction, f_length)) {
			//f_length tells us the distance from the center of the planet to the first intersection with the boundingsphere along the ray pointing along the angle
			if (length > f_length) { // the bounding sphere's first intersection is underground
				if (object_boundary.Intersects(origin + static_cast<float>(length) * direction, direction, f_length)) { //Find the far intersection point so we know how much dirt to drop
					//The distance of the chord connecting the two intersection points is stored in f_length
					length += (int)f_length;
				}
			}
			else if (length < f_length) { //The bounding sphere is completely above the surface at this altitude
				if (object_boundary.Intersects(origin + f_length * direction, direction, f_length)) { //Find the far intersection point so we know how much dirt to drop
					//The distance of the chord connecting the two intersection points is stored in f_length
					length += (int)f_length;
				}
			}
		}
	}
} //generate_terrain


/// <summary>
/// Calculates the angular slope of the planet at a given longitude. Uses a basic difference quotient to calculate the linear slope of the terrain at that point, and then uses atan2 to get that as an angle.
/// </summary>
/// <param name="longitude">float representing the angle in degrees</param>
/// <returns></returns>
float CPlanetObject::get_slope_at_longitude(float longitude) {
	//TODO: Use a linear regression over the nearest 5 indices so that the angle changes more smoothly
	float degrees_per_altitude_change = 360.0f / number_of_altitudes; // The distances are sample of the height of the planet from the core as we walk around the planet. If we have num distances, then each step is 360deg/num
	int altitude_index = (int)((int)longitude / degrees_per_altitude_change);
	altitude_index = modulo(altitude_index, number_of_altitudes); // Avoids a weird error where sometimes the index is calculated as negative

	//Use a central difference quotient. This doesn't need to be perfect.
	Vector2 difference = get_surface_vector_at_index(altitude_index + 1) - get_surface_vector_at_index(altitude_index - 1);
	//Return atan2 of that difference vector to get the angle, then convert to degrees
	return atan2f(difference.y, difference.x)* 180.f/XM_PI;
}