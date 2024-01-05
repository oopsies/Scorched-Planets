/// \file Mouse.h
/// \brief Interface for the CMouse class.

#pragma once

#include <Keyboard.h>
#include "Renderer.h"
#include <string>

class CRenderer;

/// \brief The Mouse handler. It is implemented as a wrapper for the CKeyboard class.
///
/// This is a polled handler. Its GetState() function
/// must be called once per frame. The state from
/// the previous frame is retained so that queries can determine
/// whether a mouse button changed state.

class CMouse : public CKeyboard {

	private:
		int oldX; //< old mouse positions for calculating distance
		int oldY; 
		int currentX; //< Current mouse positions
		int currentY;
		int winW; //< Size of window
		int winH;
		HWND handle; //< Handle for window for calculating mouse pos
		CRenderer* renderer; //Renderer for getting camera pos
		RECT window_corners; ///< Struct to store the window corner coordinates

		bool is_window_active(); ///< Check if the game window is active or not.
		bool inactive;

	public:
		CMouse(HWND handle, Vector2 winSize, CRenderer* renderer);

		void GetState(); ///< Override to get position only once

		//buttons down/up
		bool LeftMouseDown();
		bool LeftMouseUp();
		bool RightMouseDown();
		bool RightMouseUp();

		//buttons pressed/released
		bool LeftMousePressed();
		bool LeftMouseReleased();
		bool RightMousePressed();
		bool RightMouseReleased();

		//scrollwheel up/down
		bool ScrollUp();
		bool ScrollDown();

		//mouse position
		void UpdatePos(); ///< Updates the position of the mouse
		Vector2 GetPos(); ///< Gets position of the mouse in window coordinates
		Vector2 GetPosWorld(); ///< Gets position of the mouse in world coordinates
		int GetX();
		int GetY();
		Vector2 GetDistance(); ///< Calculates the distance of the mouse from the last frame

		char GetCharacterPressed(); ///< Returns the character that is pressed on the keyboard as a string. (ugly, but i can't find another way to do it)

};