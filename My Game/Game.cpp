/// \file Game.cpp
/// \brief Code for the game class CGame.

#include "Game.h"

#include "GameDefines.h"
#include "Renderer.h"
#include "ComponentIncludes.h"
#include "ParticleEngineScaling.h"
#include "Object.h"
#include "Random.h"
#include "TurnManager.h"
#include "Button.h"
#include "SmoothCamera.h"
#include "LevelManager.h"
#include "LevelEditor.h"

#include <conio.h>
#include <Random.h>

/// Delete the renderer and the object manager.

CGame::~CGame(){
  delete m_pParticleEngine;
  delete m_pRenderer;
  delete m_pObjectManager;
  delete m_pTurnManager;
  delete m_pMouse;
  delete m_pSmoothCam;
  delete m_pLevelManager;
  //for (auto const& b : m_lButtonList)
  //    b = nullptr;
  m_lButtonList.clear();
  delete m_pLevelEditor;
  remove("Levels\\Your Levels\\user-level-test.txt");
} //destructor

/// Initialize the renderer and the object manager, load 
/// images and sounds, and begin the game.

void CGame::Initialize(){
  m_pRenderer = new CRenderer; 
  m_pRenderer->Initialize(NUM_SPRITES); 
  m_pRenderer->LoadImages(); //load images from xml file list
  m_pRenderer->SetBgColor(Colors::Black);

  m_pObjectManager = new CObjectManager; //set up the object manager 
  m_pAudio->Load(); //load the sounds for this game

  //m_pParticleEngine = new CParticleEngine2D((CSpriteRenderer*)m_pRenderer);
  m_pParticleEngine = new CParticleEngineScaling();

  m_pTurnManager = new CTurnManager();
  m_pLevelManager = new CLevelManager();

  m_pMouse = new CMouse(m_pRenderer->GetWindowHandler(), Vector2(static_cast<float>(m_nWinWidth), static_cast<float>(m_nWinHeight)), m_pRenderer);

  m_pSmoothCam = new CSmoothCamera(m_pStepTimer, m_pRenderer);

  m_pLevelEditor = new CLevelEditor();

  //test for getting level file names, will be removed later
  /*vector<string> n = m_pLevelManager->getLevelFilenames();
  for (int i = 0; i < n.size(); i++) {
      printf("%s\n", n[i].c_str());
  }*/

  BeginGame();
} //Initialize

/// Release all of the DirectX12 objects by deleting the renderer.

void CGame::Release(){
  delete m_pRenderer;
  m_pRenderer = nullptr; //for safety
} //Release

/// Ask the object manager to create a player object
/// and a turret.

void CGame::CreateObjects(){
  m_pLevelManager->LoadMap(m_nCurrentLevel);
  //m_pLevelManager->LoadMap(0);

  //Turns (based on tanks)
  if (m_eGameState == GameState::PLAYING) {
      m_pTurnManager->SetTankList(m_pObjectManager->get_tanks_list());
      m_pObjectManager->calculate_total_energy_for_all_objects();
  }

  

} //CreateObjects

/// Call this function to start a new game. This
/// should be re-entrant so that you can start a
/// new game without having to shut down and restart the
/// program.

void CGame::BeginGame(){  
  m_pParticleEngine->clear(); //clear old particles
  m_pObjectManager->clear(); //clear old objects
  //m_pLevelEditor->Clear();
  //for (auto const& b : m_lButtonList)
  //    delete b;
  m_lButtonList.clear();
  m_fLevelTime = m_pStepTimer->GetTotalSeconds(); // Set the level start time, so we can keep track of how long we've been in the level (also so we can make sure we don't accidentally carry over old key/mouse presses)
  m_bControlLockBeginLevel = true; //Don't accept any inputs from the previous level.
  CreateObjects(); //create new objects 
} //BeginGame

/// Poll the keyboard state and respond to the
/// key presses that happened since the last frame.

void CGame::KeyboardHandler(){
  m_pKeyboard->GetState(); //get current keyboard state 

  //if conrols are locked, don't do anything with input
  if (m_bControlLock) {
      if (m_bGameOver && (m_pKeyboard->TriggerDown() || m_pMouse->LeftMouseDown() || m_pMouse->RightMouseDown())) { //If it's game over and the player hits any key or clicks the mouse, start over.
          m_bGameOver = false;
          m_bControlLock = false;
          BeginGame();
      }
      else
        return;
  }
  else if (m_bControlLockBeginLevel && m_pStepTimer->GetTotalSeconds() - m_fLevelTime > 1) { //If we haven't reached game over and it's more than 1 second into the new map, turn on the controls.
    m_bControlLockBeginLevel = false;
    return;
  }

  
  else if (m_eGameState == GameState::LEVEL_EDITOR) {
      if (m_pLevelEditor->GetEditMode() == EditMode::SAVING || m_pLevelEditor->GetEditMode() == EditMode::LOADING) {
          char c = m_pMouse->GetCharacterPressed();
          std::string s = m_pLevelEditor->GetUserFilename();
          if (c != '.' && s.length() < 20) {
              s.push_back(c);
              m_pLevelEditor->SetUserFilename(s);
              //printf("%c", c);
          }
          if (m_pKeyboard->TriggerDown(VK_SPACE) && s.length() < 20 && s.length() != 0) {
              m_pLevelEditor->SetUserFilename(s + " ");
          }
          if (m_pKeyboard->TriggerDown(VK_BACK) && s.length() != 0) {
              s.pop_back();
              m_pLevelEditor->SetUserFilename(s);
          }

      }
      return;
  }
  if (m_eGameState != GameState::PLAYING)
      return;

  if (m_pKeyboard->TriggerDown(VK_F2)) //Debug text on/off
      m_bDebugText = !m_bDebugText;

  //if (m_pKeyboard->TriggerDown(VK_F3))  //turns on/off for debug mode
  //    m_bTurnsEnabled = (m_bTurnsEnabled) ? false : true;

  if (m_pKeyboard->TriggerDown(VK_F4))
      NextLevel();

  //make sure the player cannot move ai
  if (!m_pPlayer->get_is_player_character())
      return;
      
  /*if(m_pKeyboard->TriggerDown(VK_UP))
    m_pPlayer->set_power_speed(100.0f);

  if(m_pKeyboard->TriggerUp(VK_UP))
    m_pPlayer->set_power_speed(0.0f);

  if (m_pKeyboard->TriggerDown(VK_DOWN))
    m_pPlayer->set_power_speed(-100.0f);

  if (m_pKeyboard->TriggerUp(VK_DOWN))
    m_pPlayer->set_power_speed(0.0f);*/

  if (m_pKeyboard->Down(VK_UP)) {
      m_pPlayer->set_power(m_pPlayer->get_power() + 2.0f);
      if (m_pPlayer->get_power() > m_pPlayer->get_base_power() + m_pPlayer->get_current_fuel())
          m_pPlayer->set_power(m_pPlayer->get_base_power() + m_pPlayer->get_current_fuel());
  }

  if (m_pKeyboard->Down(VK_DOWN)) {
      m_pPlayer->set_power(m_pPlayer->get_power() - 2.0f);
      if (m_pPlayer->get_power() < 1.0f)
          m_pPlayer->set_power(1.0f);
  }

  if(m_pKeyboard->TriggerDown(VK_RIGHT))
    m_pPlayer->SetRotSpeed(-1.0f);

  if(m_pKeyboard->TriggerUp(VK_RIGHT))
    m_pPlayer->SetRotSpeed(0.0f);

  if(m_pKeyboard->TriggerDown(VK_LEFT))
    m_pPlayer->SetRotSpeed(1.0f);

  if(m_pKeyboard->TriggerUp(VK_LEFT))
    m_pPlayer->SetRotSpeed(0.0f);

  if (m_pKeyboard->TriggerDown(VK_TAB)) {
      m_pPlayer->next_bullet_type();
  }

  if (m_pKeyboard->TriggerDown(VK_SPACE)) {
      //m_pObjectManager->FireGun(m_pPlayer, BULLET_SPRITE);
      if ((!m_bTurnsEnabled && m_pPlayer->get_time_since_last_fired() >= 1.5f) || m_bTurnsEnabled) {
          while (!m_pPlayer->get_bullet_counts()[m_pPlayer->get_selected_bullet()]) { //If we're out of that bullet, switch to the next one.
              m_pPlayer->next_bullet_type();
          }
          eSpriteType bulletSpr = m_pPlayer->get_bullet_types()[m_pPlayer->get_selected_bullet()]; //get correct selected bullet
          m_pPlayer->FireGun(bulletSpr);
          m_pPlayer->set_fired_shot_time();
          if (m_bTurnsEnabled)
              m_pPlayer->set_state(TankState::PostFire); //Set it to wait until all the bullets clear before moving to next turn
      }
  }

  if (m_pKeyboard->Down('D')) {
      if (m_bTurnsEnabled && !m_pKeyboard->Down('A') && m_pPlayer->get_current_fuel() > 0) { //decrease fuel in classic mode
          m_pPlayer->StrafeRight();
          m_pPlayer->set_current_fuel(m_pPlayer->get_current_fuel() - 5.0f * m_pStepTimer->GetElapsedSeconds());
          if (m_pPlayer->get_power() > m_pPlayer->get_base_power() + m_pPlayer->get_current_fuel())
              m_pPlayer->set_power(m_pPlayer->get_base_power() + m_pPlayer->get_current_fuel());
      }
      else if(!m_bTurnsEnabled)
          m_pPlayer->StrafeRight();
  }
  
  if (m_pKeyboard->Down('A')) {
      if (m_bTurnsEnabled && !m_pKeyboard->Down('D') && m_pPlayer->get_current_fuel() > 0) { //decrease fuel in classic mode
          m_pPlayer->StrafeLeft();
          m_pPlayer->set_current_fuel(m_pPlayer->get_current_fuel() - 5.0f * m_pStepTimer->GetElapsedSeconds());
          if (m_pPlayer->get_power() > m_pPlayer->get_base_power() + m_pPlayer->get_current_fuel())
              m_pPlayer->set_power(m_pPlayer->get_base_power() + m_pPlayer->get_current_fuel());
      }
      else if (!m_bTurnsEnabled)
          m_pPlayer->StrafeLeft();
  }

  if(m_pKeyboard->Down(VK_DOWN))
    m_pPlayer->StrafeBack();

  if (m_pKeyboard->TriggerDown(VK_F1)) {//Switch camera modes by pressing F1
      switch (m_eCameraMode) {
          case CameraMode::PLAYER_LOCKED:
              m_eCameraMode = CameraMode::PLAYER_UNLOCKED;
              m_unlockedCameraPos = m_pPlayer->GetPos();
              break;
          case CameraMode::PLAYER_UNLOCKED:
              m_eCameraMode = CameraMode::BULLET_LOCKED;
              break;
          case CameraMode::BULLET_LOCKED:   
              m_eCameraMode = CameraMode::PLAYER_LOCKED;
              break;
      }
  }

  if (m_pKeyboard->Down(VK_NUMPAD4) && m_eCameraMode == CameraMode::PLAYER_UNLOCKED) //camera left
      m_unlockedCameraPos.x -= m_fCameraSpeed;

  if (m_pKeyboard->Down(VK_NUMPAD6) && m_eCameraMode == CameraMode::PLAYER_UNLOCKED) //camera right
      m_unlockedCameraPos.x += m_fCameraSpeed;

  if (m_pKeyboard->Down(VK_NUMPAD8) && m_eCameraMode == CameraMode::PLAYER_UNLOCKED) //camera up
      m_unlockedCameraPos.y += m_fCameraSpeed;

  if (m_pKeyboard->Down(VK_NUMPAD2) && m_eCameraMode == CameraMode::PLAYER_UNLOCKED) //camera down
      m_unlockedCameraPos.y -= m_fCameraSpeed;

} //KeyboardHandler

/// Poll the mouse state and respond to the
/// mouse presses and movements that happened since the last frame.
void CGame::MouseHandler() {
    m_pMouse->GetState();

    int mX = m_pMouse->GetX();
    int mY = m_pMouse->GetY();
    Vector2 cursorWPos = m_pMouse->GetPosWorld() / m_pRenderer->get_scale_factor();

    bool LEFT_DOWN, RIGHT_DOWN, LEFT_UP, RIGHT_UP, LEFT_PRESSED, RIGHT_PRESSED, LEFT_RELEASED, RIGHT_RELEASED;

    LEFT_DOWN = m_pMouse->LeftMouseDown();
    LEFT_UP = m_pMouse->LeftMouseUp();
    RIGHT_DOWN = m_pMouse->RightMouseDown();
    RIGHT_UP = m_pMouse->RightMouseUp();
    LEFT_PRESSED = m_pMouse->LeftMousePressed();
    LEFT_RELEASED = m_pMouse->LeftMouseReleased();
    RIGHT_PRESSED = m_pMouse->RightMousePressed();
    RIGHT_RELEASED = m_pMouse->RightMouseReleased();


    //printf("World: %d, %d    Camera: %f, %f\n", (int)cursorWPos.x, (int)cursorWPos.y, m_pRenderer->GetCameraPos().x / m_pRenderer->get_scale_factor(), m_pRenderer->GetCameraPos().y / m_pRenderer->get_scale_factor());
    //make sure the mouse is not over a button
    for (auto const b : m_lButtonList) {
        if (b->PositionInSprite(m_pMouse->GetPos()))
            return;
    }

    switch (m_eGameState) { //behave based on state

    case GameState::PLAYING:
        //Mouse controls for player character
        if (m_pPlayer->get_is_player_character() && LEFT_DOWN && !m_bControlLock) {
            if (m_pMouse->GetDistance() != Vector2::Zero) {
                //get angle and power based on mouse
                int deltaX = (int)cursorWPos.x - (int)m_pPlayer->GetPos().x;
                int deltaY = (int)cursorWPos.y - (int)m_pPlayer->GetPos().y;
                m_pPlayer->set_angle((float)atan2(deltaX, -deltaY) * (180.0f / (float)M_PI) - m_pPlayer->get_angle_relative_to_planet());

                float power = min(sqrtf((float)pow(deltaX, 2) + (float)pow(deltaY, 2)) * 1.5f, m_pPlayer->get_base_power() + (float)m_pPlayer->get_current_fuel());

                m_pPlayer->set_power(power);
                //printf("%d %d\n", (int)m_pPlayer->GetVelocity().x, (int)m_pPlayer->GetVelocity().y);
            }
        }

        //fire!
        if ((!m_bTurnsEnabled && m_pPlayer->get_time_since_last_fired() >= 1.5f) || m_bTurnsEnabled) {
            if (LEFT_RELEASED && m_pPlayer->get_is_player_character() && !m_bControlLock) {
                eSpriteType bulletSpr = m_pPlayer->get_bullet_types()[m_pPlayer->get_selected_bullet()]; //get correct selected bullet
                m_pPlayer->FireGun(bulletSpr);
                m_pPlayer->set_fired_shot_time();
                if (m_bTurnsEnabled) {
                    m_pPlayer->set_state(TankState::PostFire); //Set it to wait until all the bullets clear before moving to next turn
                    m_bControlLock = true;
                }
            }
        }

        //move camera if mouse is near edge of screen and camera is unlocked
        if (m_eCameraMode == CameraMode::PLAYER_UNLOCKED) {
            //near the edge of the screen (remember that in screen coords, lower is a higher number)
            int edge[4] = { 75, 50, 30, 15 }; //how close to the edge of the screen
            int intervals = 4;
            m_fCameraSpeed = 2.0f * (60.0f / m_pStepTimer->GetFramesPerSecond());
            //left
            for (int i = 0; i < intervals; i++) {
                if (mX < edge[i]) {
                    m_unlockedCameraPos.x -= m_fCameraSpeed;
                }
            }
            //right
            for (int i = 0; i < intervals; i++) {
                if (mX > m_nWinWidth - edge[i]) {
                    m_unlockedCameraPos.x += m_fCameraSpeed;
                }
            }
            //top
            for (int i = 0; i < intervals; i++) {
                if (mY < edge[i]) {
                    m_unlockedCameraPos.y += m_fCameraSpeed;
                }
            }
            //bottom
            for (int i = 0; i < intervals; i++) {
                if (mY > m_nWinHeight - edge[i]) {
                    m_unlockedCameraPos.y -= m_fCameraSpeed;
                }
            }
        }
        break;

    case GameState::LEVEL_EDITOR:

        //printf("%d planets\n", m_pLevelEditor->GetPlanets().size());
        if (LEFT_RELEASED) {
            m_pLevelEditor->ObjectCreate(cursorWPos);
        }
        else if (RIGHT_RELEASED) {
            m_pLevelEditor->ObjectDelete(cursorWPos);
        }
        //move camera if mouse is near edge of screen and camera is unlocked
        if (m_eCameraMode == CameraMode::PLAYER_UNLOCKED) {
            //near the edge of the screen (remember that in screen coords, lower is a higher number)
            int edge[1] = { 100 }; //how close to the edge of the screen
            int intervals = 1;
            //left
            m_fCameraSpeed = 2.0f * (60.0f / m_pStepTimer->GetFramesPerSecond());
            for (int i = 0; i < intervals; i++) {
                if (mX < edge[i]) {
                    m_unlockedCameraPos.x -= m_fCameraSpeed;
                }
            }
            //right
            for (int i = 0; i < intervals; i++) {
                if (mX > m_nWinWidth - edge[i]) {
                    m_unlockedCameraPos.x += m_fCameraSpeed;
                }
            }
            //top
            for (int i = 0; i < intervals; i++) {
                if (mY < edge[i]) {
                    m_unlockedCameraPos.y += m_fCameraSpeed;
                }
            }
            //bottom
            for (int i = 0; i < intervals; i++) {
                if (mY > m_nWinHeight - edge[i]) {
                    m_unlockedCameraPos.y -= m_fCameraSpeed;
                }
            }
        }

        break;
    }


}

///Runs events based on the mouse and buttons on the screen
void CGame::ButtonHandler() {

    for (auto const b : m_lButtonList) {
        if (b->PositionInSprite(m_pMouse->GetPos())) {
            if (m_pMouse->LeftMouseReleased()) {
                bool d = b->OnPress();
                if (d) 
                    break;
                eSpriteType spr = b->GetSprite();
                //this can't be called from the class itself, so need to check for it here
                if (spr == BUTTON_STARTGAME_SPRITE || spr == BUTTON_LEVELEDITOR_SPRITE || spr == BUTTON_BACK_SPRITE || spr == BUTTON_LEVELNAME_SPRITE ||
                    spr == BUTTON_CLASSICMODE_SPRITE || spr == BUTTON_BLITZMODE_SPRITE || spr == BUTTON_INSTRUCTIONS_SPRITE) { 
                    BeginGame();
                    return;
                }
            }
            else if (m_pMouse->LeftMouseReleased()) {
                b->OnRelease();
            }
        }//if
    }//for
}//ButtonHandler

///Draws all of the buttons.
void CGame::DrawButtons() {
    for (auto const b : m_lButtonList) {
        b->Draw();
    }
}

/// Poll the XBox controller state and respond to the
/// controls there.

void CGame::ControllerHandler(){
  if(!m_pController->IsConnected())return;

  m_pController->GetState(); //get state of controller's controls 

  m_pPlayer->SetSpeed(100*m_pController->GetRTrigger());
  m_pPlayer->SetRotSpeed(-2.0f*m_pController->GetRThumb().x);

  if(m_pController->GetButtonRSToggle()){
    m_pPlayer->FireGun(BULLET_SPRITE);
  }
  if(m_pController->GetDPadRight())
    m_pPlayer->StrafeRight();
  
  if(m_pController->GetDPadLeft())
    m_pPlayer->StrafeLeft();

  if(m_pController->GetDPadDown())
    m_pPlayer->StrafeBack();
} //ControllerHandler

/// Ask the object manager to draw the game objects. RenderWorld
/// is notified of the start and end of the frame so
/// that it can let Direct3D do its pipelining jiggery-pokery.

void CGame::RenderFrame(){
    m_pRenderer->BeginFrame();
    DrawStarfield();
    m_pParticleEngine->Draw();
    m_pObjectManager->draw();  
    DrawButtons();
    switch (m_eGameState) {
        case GameState::TITLE_SCREEN:
            break;
        case GameState::PLAYING:
            DrawInventory();
            DrawTrajectory();
            DrawHPBar();
            //DrawBorder();
            break;
        case GameState::LEVEL_EDITOR:
            DrawBorder();
            if (m_pLevelEditor->GetEditMode() == EditMode::SAVING || m_pLevelEditor->GetEditMode() == EditMode::LOADING) { //draw user input box
                //draw text
                m_pRenderer->DrawCenteredText(m_pLevelEditor->GetUserFilename().c_str(), Colors::Black);
            }
            break;
    }
    
    
    //draw text based on state of the game
    StateBasedText();

    string s;

    //Game Over text
    if (m_bGameOver) {
        s = "Game Over - Press Any Key To Restart";
        m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_vWinCenter.x - 250.0f, m_vWinCenter.y), Colors::White);
    }

    //Level name
    //TODO: Improve this placeholder
    if (m_nCurrentLevel != 0) {
      s = "Level: " + m_pLevelManager->get_level_name();
      m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_vWinCenter.x - 100.0f, 30.0f), Colors::White);
    }
  m_pRenderer->EndFrame();
} //RenderFrame

/// Draw different text depending on the state of the game.

void CGame::StateBasedText() {
    string s;
    float yOffset = sin(m_pStepTimer->GetTotalSeconds()) * 10.0f;
    Vector2 winSize = m_pRenderer->GetWindowSize();
    switch (m_eGameState) {

        case GameState::TITLE_SCREEN: { //when on title screen
            s = "Scorched Planets";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2((float)m_nWinWidth / 2.0f - 120.0f, 80.0f + yOffset), Colors::Orange);
            s = "Level Editor";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2((float)m_nWinWidth / 2.0f - 100.0f, (float)m_nWinHeight - 200.0f), Colors::Orange);
        }
            break;
        case GameState::PLAYING: { //when playing the game
            if (m_bDebugText) { //turn on/off with F2
                s = "Angle: " + to_string(m_pPlayer->get_angle()) + "\n";
                s += "Power: " + to_string(m_pPlayer->get_power()) + "\n";
                s += "Health: " + to_string(m_pPlayer->get_health_points()) + "\n";
                m_pRenderer->DrawScreenText(s.c_str(), Vector2(30.0f, 30.0f), Colors::White);

                //Frame Count
                string s = to_string(m_pStepTimer->GetFramesPerSecond()) + " fps";
                const Vector2 pos(m_nWinWidth - 128.0f, 30.0f);
                m_pRenderer->DrawScreenText(s.c_str(), pos, Colors::White);
                //printf("Level: %s\n", m_sCurrentLevel.c_str());
            }
            if (m_eCameraMode == CameraMode::TRANSITION) { //smooth transitions for players

                Vector2 startPos;
                Vector2 deltaPos;
                Vector2 curPos;
                float totalTime = m_pSmoothCam->GetTotalCamTime() / 2.0f;

                if (m_pSmoothCam->GetCurrentCamTime() < totalTime) {
                    startPos = Vector2(-100.0f, winSize.y / 4.0f);
                    deltaPos = Vector2(winSize.x / 2.0f, 0.0f);
                    float curTime = m_pSmoothCam->GetCurrentCamTime();
                    curPos = m_pSmoothCam->circle_ease_out(curTime, startPos, deltaPos, totalTime);
                }
                else {
                    startPos = Vector2(winSize.x / 2.0f - 100.0f, winSize.y / 4.0f);
                    deltaPos = Vector2(winSize.x / 2.0f + 150.0f, 0.0f);
                    float curTime = m_pSmoothCam->GetCurrentCamTime() - totalTime;
                    curPos = m_pSmoothCam->circle_ease_out(curTime, startPos, deltaPos, totalTime);
                }

                std::shared_ptr<CTankObject> p = m_pTurnManager->GetNextTank();
                if (p->get_is_player_character()) {
                    s = "Player " + to_string(p->get_player_number()) + "'s turn";
                    XMFLOAT4 pColor = p->GetColor();
                    XMVECTORF32 color = { { { pColor.x, pColor.y, pColor.z, 1.000000000f } } };
                    m_pRenderer->DrawScreenText(s.c_str(), curPos, color);
                }
                else {
                    s = "AI " + to_string(p->get_player_number()) + "'s turn";
                    XMFLOAT4 pColor = p->GetColor();
                    XMVECTORF32 color = { { { pColor.x, pColor.y, pColor.z, 1.000000000f } } };
                    m_pRenderer->DrawScreenText(s.c_str(), curPos, color);
                }
            }
        }
            break;
        case GameState::LEVEL_EDITOR: { //when in level editor

            EditMode e = m_pLevelEditor->GetEditMode();
            if (e == EditMode::PLANET_CREATE) { //show mass and radius
                s = "Radius: " + to_string((int)m_pLevelEditor->GetRadius());
                m_pRenderer->DrawScreenText(s.c_str(), Vector2(70.0f, 165.0f), Colors::Orange);
                s = "Mass: " + to_string((int)m_pLevelEditor->GetMass());
                m_pRenderer->DrawScreenText(s.c_str(), Vector2(70.0f, 265.0f), Colors::Orange);
            }
            else if (e == EditMode::TANK_CREATE) {
                s = "Selected color: " + m_pLevelEditor->GetSelectedColor();
                m_pRenderer->DrawScreenText(s.c_str(), Vector2(30.0f, 165.0f), Colors::Orange);
            }
            if (e != EditMode::SAVING && e != EditMode::LOADING) {
                s = "Edit mode: " + m_pLevelEditor->ModeToString(m_pLevelEditor->GetEditMode());
                m_pRenderer->DrawScreenText(s.c_str(), Vector2(30.0f, 100.0f), Colors::Orange);
                s = "LMB: Create\nRMB: Delete";
                m_pRenderer->DrawScreenText(s.c_str(), Vector2(30.0f, 450.0f), Colors::Orange);
            }
            else if (e == EditMode::SAVING) {
                s = "Save";
                m_pRenderer->DrawScreenText(s.c_str(), Vector2((float)m_nWinWidth / 2 - 300.0f, (float)m_nWinHeight / 2 - 190.0f), Colors::Orange);
                s = "Level name:";
                m_pRenderer->DrawScreenText(s.c_str(), Vector2((float)m_nWinWidth / 2 - 325.0f, (float)m_nWinHeight / 2 - 110.0f), Colors::Orange);
            }
            else if (e == EditMode::LOADING) {
                s = "Load";
                m_pRenderer->DrawScreenText(s.c_str(), Vector2((float)m_nWinWidth / 2 - 300.0f, (float)m_nWinHeight / 2 - 190.0f), Colors::Orange);
                s = "Level name:";
                m_pRenderer->DrawScreenText(s.c_str(), Vector2((float)m_nWinWidth / 2 - 325.0f, (float)m_nWinHeight / 2 - 110.0f), Colors::Orange);
            }
            s = "World Size: " + to_string((int)m_pLevelEditor->GetWorldSize().x);
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(70.0f, m_nWinHeight - 160.0f), Colors::Orange);
        }
            break;
        case GameState::LEVEL_SELECT: {
            
            m_pRenderer->SetCameraPos(Vector3(m_nWinWidth / 2.0f, m_nWinHeight / 2.0f, 0.0f));
            s = "Level Select";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(winSize.x / 2 - 70.0f, 40.0f + yOffset), Colors::Orange);
            vector<string> names = m_pLevelManager->getFilenames(); //list of filenames
            if (names.size() == 0) {
                s = "No Levels";
                m_pRenderer->DrawCenteredText(s.c_str(), Colors::Orange);
            }
            for (int i = 0; i < names.size(); i++) { //create a button for every folder/level
                int posX = (i % 2 == 0) ? winSize.x / 2 - 400 : winSize.x / 2 + 50.0f;
                int posY = (i / 2) * 150 + 125;
                string name = names[i];

                if (names[i].substr(names[i].size() - 4) == ".txt") {
                //draw star next to level if it has been completed
                    if (m_pLevelManager->LevelCleared(m_pLevelManager->getSelectedFolder() + "\\" + names[i])) {
                        CSpriteDesc2D spr;
                        spr.m_nSpriteIndex = YELLOW_STAR_SPRITE;
                        spr.m_fXScale = spr.m_fYScale = 0.075f;
                        spr.m_vPos = Vector2(posX + 305.0f, winSize.y - posY - 20.0f);
                        m_pRenderer->DrawUnscaled(spr);
                    }
                    //printf(m_pLevelManager->LevelCleared(m_pLevelManager->getSelectedFolder() + "\\" + names[i]) ? "Cleared" : "Not Cleared");
                    
                    name = names[i].substr(0, names[i].size() - 4); //cut off .txt extension
                }
                
                m_pRenderer->DrawScreenText(name.c_str(), Vector2((float)posX, (float)posY), Colors::Black);

                if (i == names.size() - 1 && names[i].substr(names[i].size() - 4) != ".txt") {
                    i += 1;
                    name = "Randomized Level";
                    int posX = (i % 2 == 0) ? m_nWinWidth / 2 - 400 : m_nWinWidth / 2 + 50.0f;
                    int posY = (i / 2) * 150 + 125;
                    m_pRenderer->DrawScreenText(name.c_str(), Vector2((float)posX, (float)posY), Colors::Black);
                }
            }
        }
            break;
        case GameState::MODE_SELECT: {
            s = "Choose a Game Mode";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2((float)m_nWinWidth / 2.0f - 150.0f, 60.0f + yOffset), Colors::Orange);

            s = "Classic";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2((float)m_nWinWidth / 2.0f - 250.0f, 450.0f), Colors::Orange);

            s = "Blitz";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2((float)m_nWinWidth / 2.0f + 175.0f, 450.0f), Colors::Orange);

            for (auto const b : m_lButtonList) {
                if (b->PositionInSprite(m_pMouse->GetPos())) {
                    if (b->GetSprite() == BUTTON_CLASSICMODE_SPRITE || b->GetSprite() == BUTTON_BLITZMODE_SPRITE)
                        b->OnHover();
                }//if
            }//for
        } //case
            break;
        case GameState::PLAYER_SELECT: {
            //draw the number of human players vs AI players
            string s = "Select Amount of Total and Human Players";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(winSize.x / 2 - 260.0f, 40.0f + yOffset), Colors::Orange);

            int hP = m_pTurnManager->getNumHumanPlayers(); //number of human players;
            int tP = m_pLevelManager->getCurrentTankCount();
            int MtP = m_pLevelManager->getTankCount(m_pLevelManager->getSelectedFolder() + "\\" + m_pLevelManager->getFilenames()[m_pLevelManager->getSelectedLevel() - 1]); //total number of players

            CSpriteDesc2D spr;
            spr.m_nSpriteIndex = BUTTON_LEVELNAME_SPRITE;
            spr.m_vPos = Vector2(Vector2(winSize.x / 2, winSize.y / 2 + 205.0f));
            spr.m_fXScale = spr.m_fYScale = 0.75f;
            m_pRenderer->DrawUnscaled(spr);

            string name = m_pLevelManager->getFilenames()[m_pLevelManager->getSelectedLevel() - 1];
            s = name.substr(0, name.size() - 4);
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(winSize.x / 2 - 130.0f, winSize.y / 2 - 225.0f), Colors::Black);

            s = to_string(tP) + "/" + to_string(MtP) + " Total Players";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(winSize.x / 2 - 120.0f, winSize.y / 2 - 70.0f), Colors::Orange);

            s = to_string(hP) + "/" + to_string(tP) + " Human Players";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(winSize.x / 2 - 120.0f, winSize.y / 2 + 90.0f), Colors::Orange);

            s = "(" + to_string(hP) + " human players, " + to_string(tP - hP) + " AI players)";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(winSize.x / 2 - 220.0f, winSize.y / 2 + 200.0f), Colors::Orange);

        }//case
            break;
        case GameState::INSTRUCTIONS:
            m_pRenderer->SetCameraPos(Vector3(m_nWinWidth / 2.0f, m_nWinHeight /2.0f, 0.0f));
            DrawInstructions();
            break;
    }
}

/// Make the camera follow the player, but don't let it get
/// too close to the edge. Unless the world is smaller than
/// the window, in which case we center everything.

void CGame::FollowCamera(){
    Vector3 vCameraPos;
    
    switch (m_eCameraMode) {
    case CameraMode::PLAYER_LOCKED:
        vCameraPos = Vector3(m_pPlayer->GetPos()); //player position
        break;
    case CameraMode::PLAYER_UNLOCKED:
        vCameraPos = Vector3(m_unlockedCameraPos.x, m_unlockedCameraPos.y, 0.0f); //player moves camera
        break;
    case CameraMode::BULLET_LOCKED:
        //printf("%d\n", bullets.size());
        if (m_pObjectManager->get_bullets_list().size() == 0) {
            //vCameraPos = Vector3(m_pPlayer->GetPos()); //player position
            vCameraPos = Vector3(m_lastBulletCameraPos.x, m_lastBulletCameraPos.y, 0.0f);
        }
        else {
            vCameraPos = Vector3(m_pObjectManager->get_bullets_list().back()->GetPos());
            m_lastBulletCameraPos = Vector2(vCameraPos.x, vCameraPos.y);
        }
        break;
    case CameraMode::TRANSITION:
        m_pSmoothCam->MoveCamera(CSmoothCamera::EaseFunctionType::EASE_IN_OUT_CIRC);
        return;
        break;
    }

  if(m_vWorldSize.x > m_nWinWidth){ //world wider than screen
    vCameraPos.x = max(vCameraPos.x, m_nWinWidth/2.0f); //stay away from the left edge
    vCameraPos.x = min(vCameraPos.x, m_vWorldSize.x - m_nWinWidth/2.0f);  //stay away from the right edge
  } //if
  else vCameraPos.x = m_vWorldSize.x/2.0f; //center horizontally.
  
  if(m_vWorldSize.y > m_nWinHeight){ //world higher than screen
    vCameraPos.y = max(vCameraPos.y, m_nWinHeight/2.0f);  //stay away from the bottom edge
    vCameraPos.y = min(vCameraPos.y, m_vWorldSize.y - m_nWinHeight/2.0f); //stay away from the top edge
  } //if
  else vCameraPos.y = m_vWorldSize.y/2.0f; //center vertically

  
  m_unlockedCameraPos = Vector2(vCameraPos.x, vCameraPos.y); //prevent unlocked camera from going out
  m_pRenderer->SetCameraPos(vCameraPos); //camera to player
} //FollowCamera

/// Handle keyboard input, move the game objects and render 
/// them in their new positions and orientations. Notify the 
/// audio player at the start of each frame so that it can 
/// prevent multiple copies of a sound from starting on the
/// same frame. Nostify the timer of the start and end of the
/// frame so that it can calculate frame time. 

void CGame::ProcessFrame(){
  KeyboardHandler(); //handle keyboard input
  MouseHandler(); //handle mouse input
  ControllerHandler(); //handle controller input
  ButtonHandler();
  m_pAudio->BeginFrame(); //notify audio player that frame has begun
  
  m_pStepTimer->Tick([&](){ 
    if (m_eGameState == GameState::PLAYING) {
        m_pObjectManager->move(); //move all objects
        FollowCamera(); //make camera follow player
    }
    if (m_eGameState == GameState::LEVEL_EDITOR)
        FollowCamera();

    m_pParticleEngine->step(); //advance particle animation
  });

  if (!m_bTurnsEnabled) //cosnstantly check if the game is over when in blitz mode
    m_pTurnManager->CheckGameOver();

  //printf("Camera Position: (%d, %d)\n", (int)m_pRenderer->GetCameraPos().x, (int)m_pRenderer->GetCameraPos().y);
  RenderFrame(); //render a frame of animation
} //ProcessFrame

/// Draw the inventory of the current player on the left hand side of the screen.

void CGame::DrawInventory() {
    CSpriteDesc2D spr; //create new sprite desc

    Vector2 camPos = m_pRenderer->GetCameraPos();
    int selectedIndex = m_pPlayer->get_selected_bullet();

    //go through each available bullet for player
    for (int i = 0; i < 5; i++) { //only display 5 bullets at a time
        int bulletIndex = i + selectedIndex;
        if (bulletIndex >= m_pPlayer->get_bullet_type_count())
            bulletIndex -= m_pPlayer->get_bullet_type_count();

        //set sprite and draw on side of screen
        spr.m_nSpriteIndex = m_pPlayer->get_bullet_types()[bulletIndex];
        if (spr.m_nSpriteIndex > 9 && spr.m_nSpriteIndex <= 13)
        {
            spr.m_fXScale = 0.1f;
            spr.m_fYScale = 0.1f;
        }

        else
        {
            spr.m_fXScale = 0.75f;
            spr.m_fYScale = 0.75f;
        }
        spr.m_vPos = camPos - Vector2(static_cast<float>(m_nWinWidth / 2 - 30), static_cast<float>(-30 + (i * 40)));
        spr.m_fRoll = 0.0f;

        if (i == 0) {
            bulletRotation += (M_PI / 150) * m_pStepTimer->GetElapsedSeconds();
            spr.m_fRoll = bulletRotation;
            //spr.m_fYScale = (sin(bulletRotation) + 1.0f) / 2.0f;
        }

        m_pRenderer->DrawUnscaled(spr);

        //draw bullet count
        Vector2 textPos = Vector2(static_cast <float>(75), static_cast<float>((m_nWinHeight / 2) - 50 + (i * 40)));
        if (bulletIndex == 0 || bulletIndex == 1)
            m_pRenderer->DrawScreenText("Infinite", textPos, Colors::White);
        else
            m_pRenderer->DrawScreenText(to_string(m_pPlayer->get_bullet_counts()[bulletIndex]).c_str(), textPos, Colors::White);

        if (i == 0) {
            string s = "<-";
            m_pRenderer->DrawScreenText(s.c_str(), textPos + Vector2(100, 0), Colors::White);
        }

    }

}

/// Draw the trajectory that the bullet would take at the current power and angle.

void CGame::DrawTrajectory() {
    if (m_pPlayer->get_is_player_character())
        m_pObjectManager->draw_trajectory(eSpriteType::BULLET2_SPRITE, m_pPlayer->GetPos(), m_pPlayer->GetVelocity() + m_pPlayer->get_power() * m_pPlayer->GetViewVector(), &*m_pPlayer);
}

/// Draw the current player's current hit points at the top left of the screen.
/// Also draw the player's fuel bar because i was too lazy to make a separate function for it so i'll just do it in this one what's the harm in that

void CGame::DrawHPBar() {
    //define sprite
    CSpriteDesc2D spr;
    //get camera position
    Vector2 camPos = m_pRenderer->GetCameraPos();

    //draw HP bar sprite
    spr.m_nSpriteIndex = HPBAR_SPRITE;
    spr.m_vPos = camPos - Vector2(static_cast<float>(m_nWinWidth / 2 - m_pRenderer->GetWidth(HPBAR_SPRITE) / 2), static_cast<float>(-m_nWinHeight / 2 + 72));
    m_pRenderer->DrawUnscaled(spr);

    //draw segments of HP bar
    spr.m_nSpriteIndex = HPSEGMENT_SPRITE;
    spr.m_vPos.x = camPos.x - static_cast<float>(m_nWinWidth / 2) + 12;

    //calculate tint based on HP
    int hp = m_pPlayer->get_health_points();
    if (hp >= 95)
        spr.m_f4Tint = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
    else if (hp >= 80)
        spr.m_f4Tint = XMFLOAT4(0.451f, 1.0f, 0.0f, 1.0f);
    else if (hp >= 65)
        spr.m_f4Tint = XMFLOAT4(0.741f, 1.0f, 0.0f, 1.0f);
    else if (hp >= 45)
        spr.m_f4Tint = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
    else if (hp >= 20)
        spr.m_f4Tint = XMFLOAT4(1.0f, 0.486f, 0.0f, 1.0f);
    else
        spr.m_f4Tint = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

    //draw health segments
    for (int i = 0; i < (int)hp/4; i++) {
        m_pRenderer->DrawUnscaled(spr);
        spr.m_vPos.x += m_pRenderer->GetWidth(spr.m_nSpriteIndex);
    }

    //draw text (hp%)
    string s = to_string(hp) + "%";
    m_pRenderer->DrawScreenText(s.c_str(), Vector2(195, 75), Colors::DarkOrange);


    //fuel bar
    if (m_pPlayer->get_current_fuel() == m_pPlayer->get_max_fuel() || !m_pPlayer->get_is_player_character())
        return;
    if (m_eCameraMode == CameraMode::BULLET_LOCKED || m_eCameraMode == CameraMode::TRANSITION)
        return;
    float alpha = (m_pPlayer->get_current_fuel() > 0.0f) ? 0.75f : 0.4f;

    spr.m_nSpriteIndex = FUELBAR_SPRITE;
    spr.m_vPos = camPos - Vector2(50.0f, -50.0f);
    spr.m_f4Tint = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    spr.m_fAlpha = alpha;
    m_pRenderer->DrawUnscaled(spr);

    spr.m_nSpriteIndex = FUELSEGMENT_SPRITE;
    spr.m_vPos = camPos - Vector2(50.0f, -50.0f);

    float fuelPercentage = (float)m_pPlayer->get_current_fuel() / (float)m_pPlayer->get_max_fuel();
    if (fuelPercentage >= 0.95f)
        spr.m_f4Tint = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
    else if (fuelPercentage >= 0.80f)
        spr.m_f4Tint = XMFLOAT4(0.451f, 1.0f, 0.0f, 1.0f);
    else if (fuelPercentage >= 0.65f)
        spr.m_f4Tint = XMFLOAT4(0.741f, 1.0f, 0.0f, 1.0f);
    else if (fuelPercentage >= 0.45f)
        spr.m_f4Tint = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
    else if (fuelPercentage >= 0.20f)
        spr.m_f4Tint = XMFLOAT4(1.0f, 0.486f, 0.0f, 1.0f);
    else
        spr.m_f4Tint = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
    spr.m_fAlpha = alpha;
    spr.m_fXScale = fuelPercentage;
    m_pRenderer->DrawUnscaled(spr);

}

/// Draws a border around the edges of the map.

void CGame::DrawBorder() {

    //draw 4 corners
    CSpriteDesc2D spr;
    spr.m_nSpriteIndex = BORDERCORNER_SPRITE;
    //bottom left
    spr.m_vPos = Vector2::Zero;
    m_pRenderer->Draw(spr);
    //top left
    spr.m_vPos.y = m_vWorldSize.y;
    m_pRenderer->Draw(spr);
    //top right
    spr.m_vPos.x = m_vWorldSize.x;
    m_pRenderer->Draw(spr);
    //bottom right
    spr.m_vPos.y = 0.0f;
    m_pRenderer->Draw(spr);

    //draw edges
    spr.m_nSpriteIndex = BORDEREDGE_SPRITE;

    //bottom
    spr.m_vPos.y = 0.0f;
    spr.m_vPos.x = 0.0f;
    int lines = m_vWorldSize.x / 120;
    for (int i = 0; i < lines - 1; i++) {
        spr.m_vPos.x += 120.0f;
        m_pRenderer->Draw(spr);
    }

    //top
    spr.m_vPos.y = m_vWorldSize.y;
    spr.m_vPos.x = 0.0f;
    for (int i = 0; i < lines - 1; i++) {
        spr.m_vPos.x += 120.0f;
        m_pRenderer->Draw(spr);
    }

    //left
    spr.m_vPos.y = 0.0f;
    spr.m_vPos.x = 0.0f;
    spr.m_fRoll = M_PI / 2.0f;
    lines = m_vWorldSize.y / 120;
    for (int i = 0; i < lines - 1; i++) {
        spr.m_vPos.y += 120.0f;
        m_pRenderer->Draw(spr);
    }

    //right
    spr.m_vPos.y = 0.0f;
    spr.m_vPos.x = m_vWorldSize.x;
    for (int i = 0; i < lines - 1; i++) {
        spr.m_vPos.y += 120.0f;
        m_pRenderer->Draw(spr);
    }

}

void CGame::NextLevel() {
  m_nCurrentLevel = (m_nCurrentLevel + 1) % m_pLevelManager->get_number_of_levels();
  if (m_nCurrentLevel == 0) //dont go back to the title screen
      m_nCurrentLevel = 1;
  BeginGame();
} //NextLevel

void CGame::DrawStarfield() {
  eSpriteType STARFIELD_SPRITE = STARFIELD2_SPRITE; // TODO: Make this choose a random starfield.
  Vector2 image_size, window_size;
  m_pRenderer->GetSize(STARFIELD_SPRITE, image_size.x, image_size.y);
  window_size = m_pRenderer->GetWindowSize();
  float win_max, img_min;
  win_max = (float)max(window_size.x, window_size.y);
  img_min = (float)min(image_size.x, image_size.y);
  CSpriteDesc2D sd;
  sd.m_nSpriteIndex = STARFIELD_SPRITE;
  sd.m_vPos = (Vector2)m_pRenderer->GetCameraPos();
  sd.m_fXScale = sd.m_fYScale = (win_max / img_min)*1.25;
  sd.m_fAlpha = 0.3f;
  starfieldRotation += M_PI * 2 / 7000 * m_pStepTimer->GetElapsedSeconds();;
  sd.m_fRoll = starfieldRotation;

  m_pRenderer->DrawUnscaled(sd);
} //DrawStarfield 

void CGame::DrawInstructions() {
    //draw page number
    string s = "Page: " + to_string(instructionPage + 1) + "/4";
    m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth - 200.0f, m_nWinHeight - 100.0f), Colors::White);

    CSpriteDesc2D spr;

    switch (instructionPage) {
        case 0: //controls

            s = "Controls";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 50.0f, 50.0f), Colors::Orange);

            //a/d: move
            spr.m_nSpriteIndex = AKEY_SPRITE;
            spr.m_vPos = Vector2(300.0f, m_nWinHeight - 250.0f);
            spr.m_fXScale = spr.m_fYScale = 0.4f;
            m_pRenderer->Draw(spr);

            spr.m_nSpriteIndex = DKEY_SPRITE;
            spr.m_vPos = Vector2(400.0f, m_nWinHeight - 250.0f);
            m_pRenderer->Draw(spr);

            s = "Move Left/Right";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(600.0f, 250.0f), Colors::White);

            //lmb(release)/space: shoot
            spr.m_nSpriteIndex = LMB_SPRITE;
            spr.m_vPos = Vector2(300.0f, m_nWinHeight - 350.0f);
            spr.m_fXScale = spr.m_fYScale = 0.2f;
            m_pRenderer->Draw(spr);

            s = "(Release) OR Spacebar:  Shoot";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(600.0f, 350.0f), Colors::White);

            //lmb(hold)/arrowkeys :adjust angle/power
            spr.m_nSpriteIndex = LMB_SPRITE;
            spr.m_vPos = Vector2(300.0f, m_nWinHeight - 450.0f);
            spr.m_fXScale = spr.m_fYScale = 0.2f;
            m_pRenderer->Draw(spr);

            s = "(Hold) OR Arrow Keys: Adjust Aim";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(600.0f, 450.0f), Colors::White);

            //tab: change weapon
            spr.m_nSpriteIndex = TABKEY_SPRITE;
            spr.m_vPos = Vector2(300.0f, m_nWinHeight - 550.0f);
            spr.m_fXScale = spr.m_fYScale = 0.2f;
            m_pRenderer->Draw(spr);

            s = "Switch weapon";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(600.0f, 550.0f), Colors::White);

            break;

        case 1: //how to play
            s = "How to Play";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 75.0f, 50.0f), Colors::Orange);

            s = "Objective: Move and shoot your way to victory by being the last man standing.";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 500.0f, 150.0f), Colors::White);

            s = "Shoot other tanks with your arsenal of bullets to take them out.";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 450.0f, 250.0f), Colors::White);
            s = "Each tank has 100 hit points. When a tank's hit points reaches 0, that tank is eliminated.";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 600.0f, 300.0f), Colors::White);

            s = "Fuel Bar (Classic Mode Only):";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 225.0f, 400.0f), Colors::White);
            s = "Moving reduces your fuel. When your fuel runs out, you can no longer move.";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 500.0f, 450.0f), Colors::White);
            s = "Be conservative with your fuel, as any left over fuel can be";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 400.0f, 500.0f), Colors::White);
            s = "converted into more distance for your shots.";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 300.0f, 550.0f), Colors::White);

            break;
        case 2: //bullet types, part 1
            s = "Bullet Types";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 60.0f, 50.0f), Colors::Orange);

            s = "Standard Bullet";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 300.0f, 150.0f), Colors::White);
            spr.m_nSpriteIndex = BULLET_SPRITE;
            spr.m_vPos = Vector2(m_nWinWidth / 2.0f - 400.0f, m_nWinHeight - 175.0f);
            m_pRenderer->Draw(spr);

            s = "Creates dirt wherever it lands.";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 300.0f, 225.0f), Colors::White);
            spr.m_nSpriteIndex = BULLET2_SPRITE;
            spr.m_vPos = Vector2(m_nWinWidth / 2.0f - 400.0f, m_nWinHeight - 250.0f);
            m_pRenderer->Draw(spr);

            s = "Splits into 3 bullets after a short period.";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 300.0f, 300.0f), Colors::White);
            spr.m_nSpriteIndex = BULLET4_SPRITE;
            spr.m_vPos = Vector2(m_nWinWidth / 2.0f - 400.0f, m_nWinHeight - 325.0f);
            m_pRenderer->Draw(spr);

            s = "Has a large explosion radius with decent damage.";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 300.0f, 375.0f), Colors::White);
            spr.m_nSpriteIndex = BULLET5_SPRITE;
            spr.m_vPos = Vector2(m_nWinWidth / 2.0f - 400.0f, m_nWinHeight - 400.0f);
            m_pRenderer->Draw(spr);

            s = "Explodes in the air after a certain time.";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 300.0f, 450.0f), Colors::White);
            spr.m_nSpriteIndex = BULLET6_SPRITE;
            spr.m_vPos = Vector2(m_nWinWidth / 2.0f - 400.0f, m_nWinHeight - 475.0f);
            m_pRenderer->Draw(spr);

            s = "Bounces up to 3 times after hitting a planet.";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 300.0f, 525.0f), Colors::White);
            spr.m_nSpriteIndex = BULLET7_SPRITE;
            spr.m_vPos = Vector2(m_nWinWidth / 2.0f - 400.0f, m_nWinHeight - 550.0f);
            m_pRenderer->Draw(spr);

            break;
        case 3: //bullet types, part 2
            s = "Bullet Types";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 60.0f, 50.0f), Colors::Orange);

            s = "Teleports the user to where ever it lands.";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 300.0f, 150.0f), Colors::White);
            spr.m_nSpriteIndex = BULLET3_SPRITE;
            spr.m_vPos = Vector2(m_nWinWidth / 2.0f - 400.0f, m_nWinHeight - 175.0f);
            m_pRenderer->Draw(spr);

            s = "Press RMB to shoot bullets in midair, up to 3 times.";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 300.0f, 225.0f), Colors::White);
            spr.m_nSpriteIndex = BULLET8_SPRITE;
            spr.m_vPos = Vector2(m_nWinWidth / 2.0f - 400.0f, m_nWinHeight - 250.0f);
            m_pRenderer->Draw(spr);

            s = "Accelerates until it hits something.";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 300.0f, 300.0f), Colors::White);
            spr.m_nSpriteIndex = BULLET9_SPRITE;
            spr.m_vPos = Vector2(m_nWinWidth / 2.0f - 400.0f, m_nWinHeight - 325.0f);
            spr.m_fXScale = spr.m_fYScale = .1f;
            m_pRenderer->Draw(spr);

            s = "Explode with RMB in midair, up to 3 times.";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 300.0f, 375.0f), Colors::White);
            spr.m_nSpriteIndex = BULLET10_SPRITE;
            spr.m_vPos = Vector2(m_nWinWidth / 2.0f - 400.0f, m_nWinHeight - 400.0f);
            spr.m_fXScale = spr.m_fYScale = .1f;
            m_pRenderer->Draw(spr);

            s = "Huge damage and explosion radius.";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 300.0f, 450.0f), Colors::White);
            spr.m_nSpriteIndex = BULLET11_SPRITE;
            spr.m_vPos = Vector2(m_nWinWidth / 2.0f - 400.0f, m_nWinHeight - 475.0f);
            spr.m_fXScale = spr.m_fYScale = .1f;
            m_pRenderer->Draw(spr);

            s = "Create a pair of wormholes above and below where it lands.";
            m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_nWinWidth / 2.0f - 300.0f, 525.0f), Colors::White);
            spr.m_nSpriteIndex = BULLET12_SPRITE;
            spr.m_vPos = Vector2(m_nWinWidth / 2.0f - 400.0f, m_nWinHeight - 550.0f);
            spr.m_fXScale = spr.m_fYScale = .1f;
            m_pRenderer->Draw(spr);

            break;
    }
}