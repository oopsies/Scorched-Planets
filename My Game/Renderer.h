/// \file Renderer.h 
/// \brief Interface for the renderer class CRenderer.

#pragma once

#include "GameDefines.h"
#include "SpriteRenderer.h"

/// \brief The renderer.
///
/// CRenderer handles the game-specific rendering tasks, relying on
/// the base class to do all of the actual API-specific rendering.

class CRenderer: public CSpriteRenderer{
private:
  float m_fScalingFactor = .6f;

  public:
    CRenderer(); ///< Constructor.

    void LoadImages(); ///< Load images.

    float GetCameraYaw(); ///< Get camera yaw.
    void SetCameraYaw(float a); ///< Set camera yaw.

    const Vector3& GetCameraPos(); ///< Get camera position.
    void SetCameraPos(const Vector3& pos); ///< Set camera position.

    void draw_triangle(const Vector2& v1, const Vector2& v2, const Vector2& v3);

    void BeginFrame();
    void EndFrame();

    void Draw(const CSpriteDesc2D& sd); //Overload, so we can do cool scaling!
    void DrawUnscaled(CSpriteDesc2D sd); //Draw unscaled for UI elements

    HWND GetWindowHandler() { return m_Hwnd; };
    Vector2 GetWindowSize();

    void set_scale_factor(float scale_factor) { m_fScalingFactor = scale_factor; };
    float get_scale_factor() { return m_fScalingFactor; };

}; //CRenderer