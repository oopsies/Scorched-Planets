/// \file Game.h
/// \brief Interface for the game class CGame.

#pragma once

#include "Component.h"
#include "Common.h"
#include "ObjectManager.h"
#include "Settings.h"

/// \brief The game class.

class CGame: 
  public CComponent, 
  public CSettings,
  public CCommon{ 

  private:
    //int m_nCurrentLevel = 0;

    void BeginGame(); ///< Begin playing the game.
    void KeyboardHandler(); ///< The keyboard handler.
    void MouseHandler(); ///< The mouse handler.
    void ButtonHandler(); ///< The handler for buttons.
    void ControllerHandler(); ///< The controller handler.
    void RenderFrame(); ///< Render an animation frame.
    void CreateObjects(); ///< Create game objects.
    void FollowCamera(); ///< Make camera follow player character.
    void DrawInventory(); ///< Inventory for bullets.
    void DrawTrajectory(); ///< Draws the trajectory for the next fired bullet.
    void DrawButtons(); ///< Draws all of the buttons
    void DrawHPBar(); ///< Draws the HP bar and how much HP the player has
    void DrawBorder(); ///< Draws border around the world edges
    void DrawStarfield(); ///< Draws the starfield sprite
    void DrawInstructions(); ///< Draws the instructions when on the instructions page

    void StateBasedText(); ///< Draws text based on the game state (playing, level editor,)

    void NextLevel(); //< Start next level

    float m_fLevelTime = 0; ///< Time when the current level started.
    bool m_bControlLockBeginLevel = false; //< Should the controls be locked since we just started a level?


  public:
    ~CGame(); ///< Destructor.

    void Initialize(); ///< Initialize the game.
    void ProcessFrame(); ///< Process an animation frame.
    void Release(); ///< Release the renderer.
}; //CGame
