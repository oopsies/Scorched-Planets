#pragma once
#include "Object.h"
#include <vector>

enum class PlanetGenerationAlgo {FractalNoise, PlanetaryNoise};

class CPlanetObject :
  public CObject
{

  friend class CObjectManager;
private:
  //int altitudes[720];
  int number_of_altitudes = 360*2;
  std::vector<int> altitudes;
  int sealevel_radius=500;
  int maximum_altitude = sealevel_radius;
  int minimum_altitude = sealevel_radius;
  int core_radius = static_cast<int>(sealevel_radius * .3);

  BoundingSphere maximum_altitude_sphere;

  //TODO: Write a more sophisticated procedurally generated noise algorithm, potentially based on perlin noise?
  void generate_noise_fractal_naive(int num_iterations, float step_size); ///< Generates a procedurally generated planet surface using a simple 1D fractal noise algorithm
  void generate_noise_planetary_method(int num_iterations, int height_step, int indices_to_move=0);

  void draw_smoke(int start_altitude_index, int final_altitude_index);

public:
  //CPlanetObject(const Vector2& p); ///< Constructor.
  CPlanetObject(const Vector2& p, int radius = 500, float step_size=2.718); ///< Constructor with radius
  void draw_planet(); ///< Tells the renderer how to draw the planet

  int get_altitude_at_angle(float angle); ///< Get the distance at a given angle in degrees
  int get_altitude_index_under_point(Vector2 p);
  Vector2 get_surface_vector_at_index(int altitude_index);

  int get_radius() { return sealevel_radius; }; ///< Returns the radius of the planet

  bool Intersects(BoundingSphere &object_boundary); ///< Check if a Bounding Sphere intersects the planet.
  void destroy_terrain(BoundingSphere& object_boundary); ///< Destroys terrain within the bounding sphere
  void generate_terrain(BoundingSphere& object_boundary); ///< Adds terrain within the bounding sphere, which then immediately falls downward.

  float get_slope_at_longitude(float longitude); ///< Calculates the slope of the terrain at the longitude
};

