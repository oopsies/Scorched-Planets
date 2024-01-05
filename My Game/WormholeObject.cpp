/// Implementation fo CWormholeObject class
/// When a bullet collides with a wormhole, it disappears and re-appears at that wormhole's
/// corresponding wormhole. The bullet maintains its veclocity from when it enetered the first wormhole.

#include "WormholeObject.h"
#include "GameDefines.h"
#include "ObjectManager.h"


CWormholeObject::CWormholeObject(const Vector2& pos, int turnsToLive, CWormholeObject* nextWormhole) : CObject(BULLET2_SPRITE, pos) {
	correspondingWormhole = nextWormhole;
	m_fXScale = m_fYScale = m_Sphere.Radius = radius;
	turns_before_death = turnsToLive;
}

CWormholeObject::CWormholeObject(const Vector2& pos, int turnsToLive) : CObject(BULLET2_SPRITE, pos) {
	m_fXScale = m_fYScale = m_Sphere.Radius = radius;
	turns_before_death = turnsToLive;
}

void CWormholeObject::DrawWormhole() {
	
	//increment the angle of the wormhole sprite
	angle += M_PI / 500.0f;

	CSpriteDesc2D spr;
	spr.m_nSpriteIndex = sprite;
	spr.m_fXScale = 1.25f;
	spr.m_fYScale = 1.25f;
	spr.m_fRoll = angle;
	spr.m_vPos = GetPos();

	m_pRenderer->Draw(spr);

}

int CWormholeObject::DecrementLife() {

	turns_before_death -= 1;
	return turns_before_death;

}