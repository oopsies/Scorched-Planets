/// \file Button.h
/// \brief Interface for the CButton class.

#pragma once

#include "GameDefines.h"
#include "Common.h"
#include "Component.h"
#include "StepTimer.h"
#include "Renderer.h"


/// \brief A small class for moving the camera smoothly for transitions.
/// Contains a number of easing functions to move the camera asap (as smooth as possible) from point a to point b.

class CSmoothCamera : public CCommon {

	private:

		float startingTime; ///< Set when setting up the smooth camera (in seconds)
		float totalTime; ///< Total desired time for the camera transition to take place over
		Vector2 startPos; ///< The starting position of the camera when setting up
		Vector2 endPos; ///< Where the camera should end up at the end of the transition
		CStepTimer* timer; ///< Pointer to timer
		CRenderer* renderer; ///< Pointer to renderer

		

	public:

		enum class EaseFunctionType { //functions referenced from http://gizma.com/easing/
			EASE_OUT_CIRC,
			EASE_IN_OUT_CIRC,
			EASE_OUT_BACK, //was planning to add these, but couldn't get them to work
			EASE_IN_OUT_BACK
		};

		//easing functions
		Vector2 circle_ease_out(float currentTime, Vector2 b, Vector2 c, float d);
		Vector2 circle_ease_in(float currentTime, Vector2 b, Vector2 c, float d);
		Vector2 circle_ease_in_out(float currentTime, Vector2 b, Vector2 c, float d);

		CSmoothCamera(CStepTimer* t, CRenderer* r);
		void Setup(float time, Vector2 endPos); ///< Set up the smooth camera variables
		void MoveCamera(EaseFunctionType easeFunc); ///< Moves the camera using the easing function.
		float GetCurrentCamTime() { return timer->GetTotalSeconds() - startingTime; };
		float GetTotalCamTime() { return totalTime; };

		

};