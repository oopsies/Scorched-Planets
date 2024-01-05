///header file for class CWormholeObject
#pragma once

#include "Object.h"
#include "BulletObject.h"

class CWormholeObject : public CObject {

	protected:

		CWormholeObject* CWormholeObject::correspondingWormhole; ///< Wormhole that this wormhole will correspond with
		eSpriteType sprite = WORMHOLE_SPRITE;
		float angle = 0.0f; ///< angle of the wormhole sprite
		float radius = 80.0f; ///< the radius of the wormhole
		bool disabled = false;
		int turns_before_death; ///< Number of turns before destruction

	public:

		CWormholeObject(const Vector2& pos, int turnsToLive, CWormholeObject* nextWormhole);
		CWormholeObject(const Vector2& pos, int turnsToLive);

		bool IsDisabled() { return disabled; };
		void SetDisabled(bool d) { disabled = d; };
		CWormholeObject* GetNextWormhole() { return correspondingWormhole; };
		void SetNextWormhole(CWormholeObject* w) { correspondingWormhole = w; };

		void DrawWormhole();

		int DecrementLife(); ///< decrements the worm hole's turns to live,

};