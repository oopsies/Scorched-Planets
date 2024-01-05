/// \file SndList.h
/// \brief Enumerated type for sounds.

#pragma once

#include "Sound.h"

/// \brief Game sound enumerated type. 
///
/// These are the sounds used in actual gameplay. 
/// The sounds must be listed here in the same order that
/// they are in the sound settings XML file.

enum eSoundType{ 
  GUN_SOUND, RICOCHET_SOUND, OW_SOUND, CLANG_SOUND, EXPLOSION1_SOUND, EXPLOSION3_SOUND, EXPLOSION7_SOUND, SHOOT_SOUND, SHOOT2_SOUND,
  SCIFI_MUSIC, NEWDAWN_MUSIC
}; //eSoundType
//Music: https://www.bensound.com/royalty-free-music
//Explosion Sounds: https://www.freesoundeffects.com/