#include "ParticleEngineScaling.h"
/// Draw all particles using the 2D renderer supplied in the constructor.


CParticleEngineScaling::CParticleEngineScaling() : CParticleEngine2D((CSpriteRenderer*) m_pRendererInherited) {
} ///< Constructor.


void CParticleEngineScaling::Draw() {
  for (auto const& p : m_stdList) //for each object
    m_pRendererInherited->Draw((CSpriteDesc2D)*p); //append to render list
} //Draw