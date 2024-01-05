/// \file Renderer.cpp
/// \brief Code for the renderer class CRenderer.

#include "Renderer.h"
#include "ComponentIncludes.h"
#include "Abort.h"
#include <iostream>
#include <string>
#include <exception>
#include <stdexcept>

CRenderer::CRenderer():
  CSpriteRenderer(Batched2D){
} //constructor

/// Load the specific images needed for this game.
/// This is where eSpriteType values from GameDefines.h get
/// tied to the names of sprite tags in gamesettings.xml. Those
/// sprite tags contain the name of the corresponding image file.
/// If the image tag or the image file are missing, then
/// the game should abort from deeper in the Engine code,
/// leaving you with a dialog box that tells you what
/// went wrong.

void CRenderer::LoadImages(){  
  BeginResourceUpload();

  Load(FLOOR_SPRITE, "floor"); 
  Load(PLAYER_SPRITE, "player");
  Load(BULLET_SPRITE, "bullet");
  Load(BULLET2_SPRITE, "bullet2");
  Load(BULLET3_SPRITE, "bullet3");
  Load(BULLET4_SPRITE, "bullet4");
  Load(BULLET5_SPRITE, "bullet5");
  Load(BULLET6_SPRITE, "bullet6");
  Load(BULLET7_SPRITE, "bullet7");
  Load(BULLET8_SPRITE, "bullet8");
  Load(BULLET9_SPRITE, "bullet9");
  Load(BULLET10_SPRITE, "bullet10");
  Load(BULLET11_SPRITE, "bullet11");
  Load(BULLET12_SPRITE, "bullet12");
  Load(MINE_SPRITE, "mine");
  Load(SMOKE_SPRITE, "smoke");
  Load(WHITESMOKE_SPRITE, "whitesmoke");
  Load(SPARK_SPRITE, "spark");
  Load(TURRET_SPRITE, "turret");
  Load(PLANET_SPRITE, "bullet");
  Load(WATER_SPRITE, "water");
  Load(CORE_SPRITE, "core");
  Load(PLANETLAYER_SPRITE, "planet_layers");
  Load(ATMOSPHERE_SPRITE, "atmosphere");

  Load(DESERT1_SPRITE, "tankDesert1");
  Load(DESERT2_SPRITE, "tankDesert2");
  Load(DESERT3_SPRITE, "tankDesert3");
  Load(DESERT4_SPRITE, "tankDesert4");
  Load(DESERT5_SPRITE, "tankDesert5");

  Load(GREEN1_SPRITE, "tankGreen1");
  Load(GREEN2_SPRITE, "tankGreen2");
  Load(GREEN3_SPRITE, "tankGreen3");
  Load(GREEN4_SPRITE, "tankGreen4");
  Load(GREEN5_SPRITE, "tankGreen5");

  Load(GREY1_SPRITE, "tankGrey1");
  Load(GREY2_SPRITE, "tankGrey2");
  Load(GREY3_SPRITE, "tankGrey3");
  Load(GREY4_SPRITE, "tankGrey4");
  Load(GREY5_SPRITE, "tankGrey5");

  Load(NAVY1_SPRITE, "tankNavy1");
  Load(NAVY2_SPRITE, "tankNavy2");
  Load(NAVY3_SPRITE, "tankNavy3");
  Load(NAVY4_SPRITE, "tankNavy4");
  Load(NAVY5_SPRITE, "tankNavy5");

  Load(GREYBODY1_SPRITE, "tanks_tankGrey_body1");
  Load(TURRET1_SPRITE, "tanks_turret1");
  Load(TREADS1_SPRITE, "tanks_tankTracks1");

  Load(EXPLOSION1_SPRITE, "explosion1");
  Load(EXPLOSION2_SPRITE, "explosion2");
  Load(EXPLOSION3_SPRITE, "explosion3");

  Load(WORMHOLE_SPRITE, "wormhole");

  Load(HPBAR_SPRITE, "hp_bar");
  Load(HPSEGMENT_SPRITE, "hp_segment");
  Load(FUELBAR_SPRITE, "fuel_bar");
  Load(FUELSEGMENT_SPRITE, "fuel_segment");
  Load(BORDEREDGE_SPRITE, "border_edge");
  Load(BORDERCORNER_SPRITE, "border_corner");
  Load(INPUT_TEXTBOX_SPRITE, "userinput_textbox");

  Load(YELLOW_STAR_SPRITE, "yellow_star");

  Load(LMB_SPRITE, "lmb");
  Load(RMB_SPRITE, "rmb");
  Load(ARROWKEY_SPRITE, "arrowkey");
  Load(AKEY_SPRITE, "akey");
  Load(DKEY_SPRITE, "dkey");
  Load(TABKEY_SPRITE, "tabkey");

  Load(BUTTON_CAMLOCK_SPRITE, "button_camLocked");
  Load(BUTTON_CAMUNLOCK_SPRITE, "button_camUnlocked");
  Load(BUTTON_ZOOMIN_SPRITE, "button_zoomIn");
  Load(BUTTON_ZOOMOUT_SPRITE, "button_zoomOut");
  Load(BUTTON_STARTGAME_SPRITE, "button_startGame");
  Load(BUTTON_LEVELEDITOR_SPRITE, "button_levelEditor");
  Load(BUTTON_CREATEPLANET_SPRITE, "button_createPlanet");
  Load(BUTTON_CREATETANK_SPRITE, "button_createTank");
  Load(BUTTON_SAVELEVEL_SPRITE, "button_saveLevel");
  Load(BUTTON_LOADLEVEL_SPRITE, "button_loadLevel");
  Load(BUTTON_UP_SPRITE, "button_up");
  Load(BUTTON_DOWN_SPRITE, "button_down");
  Load(BUTTON_COLORSELECT_SPRITE, "button_colorSelect");
  Load(BUTTON_CONFIRM_SPRITE, "button_confirm");
  Load(BUTTON_CANCEL_SPRITE, "button_cancel");
  Load(BUTTON_BACK_SPRITE, "button_back");
  Load(BUTTON_LEVELNAME_SPRITE, "button_levelName");
  Load(BUTTON_CLASSICMODE_SPRITE, "button_classicMode");
  Load(BUTTON_BLITZMODE_SPRITE, "button_blitzMode");
  Load(BUTTON_INSTRUCTIONS_SPRITE, "button_instructions");

  Load(STARFIELD1_SPRITE, "starfield1");
  Load(STARFIELD2_SPRITE, "starfield2");

  EndResourceUpload();
} //LoadImages



float CRenderer::GetCameraYaw() {
  return m_pCamera->GetYaw();
} //SetCameraYaw

/// Writer function for camera yaw.
/// \param a New camera yaw value.

void CRenderer::SetCameraYaw(float a) {
  m_pCamera->SetYaw(a);
} //SetCameraYaw

/// Reader function for camera position.
/// \return Camera position.

const Vector3& CRenderer::GetCameraPos() {
	return m_pCamera->GetPos();
} //GetCameraPos

/// Writer function for camera position.
/// \param pos New camera position.

void CRenderer::SetCameraPos(const Vector3& pos) {
  m_pCamera->MoveTo(pos * m_fScalingFactor);
  //m_pCamera->MoveTo(pos);
  m_pCamera->SetPerspective(1.333f, 1.14f, 1.0f, 1000.0f);
} //SetCameraPos

/// Reader function for toggle camera status.
/// \return true if camera is a follow camera.



void CRenderer::draw_triangle(const Vector2& v1, const Vector2& v2, const Vector2& v3) {
  VertexPositionColor vertex1 = VertexPositionColor(XMFLOAT3(v1.x, v1.y, 0.0f), XMFLOAT4(Colors::Red));
  VertexPositionColor vertex2 = VertexPositionColor(XMFLOAT3(v2.x, v2.y, 0.0f), XMFLOAT4(Colors::Red));
  VertexPositionColor vertex3 = VertexPositionColor(XMFLOAT3(v3.x, v3.y, 0.0f), XMFLOAT4(Colors::Red));
  m_pPrimitiveBatch->DrawTriangle(vertex1, vertex2, vertex3);
}



/// Initialize the render pipeline and the SpriteBatch.

void CRenderer::BeginFrame() {
  CSpriteRenderer::BeginFrame();

} //BeginFrame

/// End the SpriteBatch frame and present.

void CRenderer::EndFrame() {
  CSpriteRenderer::EndFrame();
} //EndFrame

void CRenderer::Draw(const CSpriteDesc2D& sd) {
  CSpriteDesc2D sd_scaled = sd;
  sd_scaled.m_fXScale *= m_fScalingFactor;
  sd_scaled.m_fYScale *= m_fScalingFactor;
  sd_scaled.m_vPos *= m_fScalingFactor;
  CSpriteRenderer::Draw(sd_scaled);
}//Draw

void CRenderer::DrawUnscaled(CSpriteDesc2D sd) {
	CSpriteRenderer::Draw(sd);
}//DrawUnscaled


Vector2 CRenderer::GetWindowSize() { 
  /*RECT window_corners;
  GetWindowRect(GetWindowHandler(), &window_corners);
  float winW = static_cast<float>(window_corners.right - window_corners.left);
  float winH = static_cast<float>(window_corners.bottom - window_corners.top);
  return Vector2(winW, winH);*/
  return Vector2((float)m_nWinWidth, (float)m_nWinHeight); 
}