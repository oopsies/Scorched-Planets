/// \file GameDefines.h
/// \brief Game specific defines.

#pragma once

#include "Defines.h"

/// \brief Sprite type.
///
/// Note: NUM_SPRITES must be last.

enum eSpriteType{
  PLAYER_SPRITE, TURRET_SPRITE, 
  BULLET_SPRITE, BULLET2_SPRITE, BULLET3_SPRITE, BULLET4_SPRITE, BULLET5_SPRITE, BULLET6_SPRITE, BULLET7_SPRITE, BULLET8_SPRITE,
  BULLET9_SPRITE, BULLET10_SPRITE, BULLET11_SPRITE, BULLET12_SPRITE,
  MINE_SPRITE,
  SMOKE_SPRITE, WHITESMOKE_SPRITE, SPARK_SPRITE,
  FLOOR_SPRITE,
  PLANET_SPRITE, WATER_SPRITE, CORE_SPRITE, PLANETLAYER_SPRITE, ATMOSPHERE_SPRITE,
  DESERT1_SPRITE, DESERT2_SPRITE, DESERT3_SPRITE, DESERT4_SPRITE, DESERT5_SPRITE, 
  GREEN1_SPRITE, GREEN2_SPRITE, GREEN3_SPRITE, GREEN4_SPRITE, GREEN5_SPRITE,
  GREY1_SPRITE, GREY2_SPRITE, GREY3_SPRITE, GREY4_SPRITE, GREY5_SPRITE,
  NAVY1_SPRITE, NAVY2_SPRITE, NAVY3_SPRITE, NAVY4_SPRITE, NAVY5_SPRITE,
  GREYBODY1_SPRITE, TREADS1_SPRITE, TURRET1_SPRITE,
  EXPLOSION1_SPRITE, EXPLOSION2_SPRITE, EXPLOSION3_SPRITE,
  WORMHOLE_SPRITE,
  BUTTON_CAMLOCK_SPRITE, BUTTON_CAMUNLOCK_SPRITE, BUTTON_ZOOMIN_SPRITE, BUTTON_ZOOMOUT_SPRITE, BUTTON_STARTGAME_SPRITE, BUTTON_LEVELEDITOR_SPRITE,
  BUTTON_CREATEPLANET_SPRITE, BUTTON_CREATETANK_SPRITE, BUTTON_SAVELEVEL_SPRITE, BUTTON_LOADLEVEL_SPRITE, BUTTON_UP_SPRITE, BUTTON_DOWN_SPRITE, BUTTON_COLORSELECT_SPRITE,
  BUTTON_CONFIRM_SPRITE, BUTTON_CANCEL_SPRITE, BUTTON_BACK_SPRITE, BUTTON_LEVELNAME_SPRITE, BUTTON_CLASSICMODE_SPRITE, BUTTON_BLITZMODE_SPRITE, BUTTON_INSTRUCTIONS_SPRITE,
  HPBAR_SPRITE, HPSEGMENT_SPRITE, FUELBAR_SPRITE, FUELSEGMENT_SPRITE, BORDEREDGE_SPRITE, BORDERCORNER_SPRITE, INPUT_TEXTBOX_SPRITE,
  STARFIELD1_SPRITE, STARFIELD2_SPRITE, YELLOW_STAR_SPRITE, LMB_SPRITE, RMB_SPRITE, ARROWKEY_SPRITE, AKEY_SPRITE, DKEY_SPRITE, TABKEY_SPRITE,
  NUM_SPRITES //MUST BE LAST
}; //eSpriteType
