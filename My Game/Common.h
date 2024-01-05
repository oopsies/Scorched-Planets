/// \file Common.h
/// \brief Interface for the class CCommon.

#pragma once

#include "SndList.h"
#include <list>

//forward declarations to make the compiler less stroppy

class CObjectManager; 
class CRenderer;
class CParticleEngineScaling;
class CObject;
class CTankObject;
class CTurnManager;
class CMouse;
class CButton;
class CSmoothCamera;
class CLevelManager;
class CLevelEditor;

/// \brief The common variables class.
///
/// CCommon is a singleton class that encapsulates 
/// things that are common to different game components,
/// including game state variables.
/// Making it a singleton class means
/// that we can avoid passing its member variables
/// around as parameters, which makes the code
/// minisculely faster, and more importantly, reduces
/// function clutter.

enum class CameraMode {
    PLAYER_LOCKED, //camera is always centered around tank
    PLAYER_UNLOCKED, //player moves camera manually and does not follow tank
    BULLET_LOCKED, //camera is centered around fired bullets
    TRANSITION, //camera is moving in transition
    FIXED //put camera in fixed position, locked onto a point instead of an object
};

enum class GameState { //state of the game, used to control scenes during game
    PLAYING, //normal game
    TITLE_SCREEN, //before the game starts
    LEVEL_EDITOR, //making a level
    LEVEL_SELECT, //selecting a level
    MODE_SELECT, //selecting classic or blitz mode
    PLAYER_SELECT, //select how many human players in classic mode
    INSTRUCTIONS
};

class CCommon{
  protected:  
    static CRenderer* m_pRenderer; ///< Pointer to the renderer.
    static CObjectManager* m_pObjectManager; ///< Pointer to the object manager.
    static CParticleEngineScaling* m_pParticleEngine; ///< Pointer to particle engine.
    static CLevelManager* m_pLevelManager; ///< Pointer to the Level Manager.
    static CLevelEditor* m_pLevelEditor; ///< Pointer to the level editor
    static int m_nCurrentLevel; ///< Current level
    static string m_sCurrentLevel; ///< Current level as a string

    static CMouse* m_pMouse; ///< Pointer to keyboard handler

    static CTurnManager* m_pTurnManager; ///< Pointer to turn manager
    static bool m_bTurnsEnabled; ///< Turns disabled for debug purposes
    static bool m_bControlLock; ///< Control lock for post fire

    static CSmoothCamera* m_pSmoothCam; ///< Smooth camera functions for smooth transitions

    static Vector2 m_vWorldSize; ///< World height and width.

    static std::shared_ptr<CTankObject> m_pPlayer; ///< Pointer to first player character.
    static int m_iNumPlayers; ///< Number of players in the game

    static CameraMode m_eCameraMode; ///< Mode of camera (player locked/unlocked, bullet)
    static Vector2 m_unlockedCameraPos; ///< Player's unlocked camera position
    static Vector2 m_lastBulletCameraPos; ///< Last position of bullet
    static Vector2 m_fixedCameraPos; ///< Position of fixed camera
    static float m_fCameraSpeed; ///< Speed of manual camera movement

    static std::list<std::shared_ptr<CButton>> m_lButtonList; ///< List of all buttons

    static bool m_bGameOver; ///< Player wins, game is over

    static bool m_bDebugText; ///< Turn debug text on/off

    static GameState m_eGameState; ///< State of game

    static float starfieldRotation; ///< Angle to rotate the background image
    static float bulletRotation; ///< Angle to rotate the bullet sprite in inventory

    static int instructionPage;

}; //CCommon
