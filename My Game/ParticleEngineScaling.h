#pragma once
#include "Common.h"
#include "Renderer.h"
#include "ParticleEngine.h"

class CParticleEngineScaling: public CParticleEngine2D, public CCommon
{
private:
  CRenderer* m_pRendererInherited = CCommon::m_pRenderer;

public:
  CParticleEngineScaling(); ///< Constructor.
  void Draw();  /// Draw all particles using the 2D renderer supplied in the constructor. This is overriden, so we can use special draw function in our inherited class Renderer
};

