/// \file Common.cpp
/// \brief Code for the class CCommon.
///
/// This file contains declarations and initial values
/// for CCommon's static member variables.

#include "Common.h"

CRenderer* CCommon::m_pRenderer = nullptr;
CObjectManager* CCommon::m_pObjectManager = nullptr;
CParticleEngineScaling* CCommon::m_pParticleEngine = nullptr;
CLevelManager* CCommon::m_pLevelManager = nullptr;
CLevelEditor* CCommon::m_pLevelEditor = nullptr;
int CCommon::m_nCurrentLevel = 0;
string CCommon::m_sCurrentLevel = "";

CMouse* CCommon::m_pMouse = nullptr;

CTurnManager* CCommon::m_pTurnManager = nullptr;
bool CCommon::m_bTurnsEnabled = true; //set false for debug
bool CCommon::m_bControlLock = false;

CSmoothCamera* CCommon::m_pSmoothCam = nullptr;

Vector2 CCommon::m_vWorldSize = Vector2::Zero;

std::shared_ptr<CTankObject> CCommon::m_pPlayer;
int CCommon::m_iNumPlayers = 2;

CameraMode CCommon::m_eCameraMode = CameraMode::PLAYER_LOCKED;
Vector2 CCommon::m_unlockedCameraPos = Vector2::Zero;
Vector2 CCommon::m_lastBulletCameraPos = Vector2::Zero;
float CCommon::m_fCameraSpeed = 10.0f;
Vector2 CCommon::m_fixedCameraPos = Vector2::Zero;

std::list<std::shared_ptr<CButton>> CCommon::m_lButtonList;

bool CCommon::m_bGameOver = false;

bool CCommon::m_bDebugText = false;

GameState CCommon::m_eGameState = GameState::TITLE_SCREEN;

float CCommon::starfieldRotation = 0.0f;
float CCommon::bulletRotation = 0.0f;

int CCommon::instructionPage = 0;