/// \file Button.h
/// \brief Interface for the CButton class.
#pragma once

#include "GameDefines.h"
#include "Common.h"


/// \brief A button object. It is an image with a bounding box that the player can click on.
/// It can contain functionality for when the user clicks, releases, or hovers.

class CButton : public CCommon {

	private:
		
		eSpriteType sprite; ///< Sprite that is displayed as the button
		int m_iSpriteWidth; ///< Width of the sprite
		int m_iSpriteHeight; ///< Height of the sprite
		Vector2 m_vSpritePosition; ///< Position of the button
		RECT BBox; ///< Bounding box of the button in screen coordinates
		CRenderer* renderer; //Renderer for drawing
		float xScale = 1.0f; //Scales of the sprite
		float yScale = 1.0f; 

		string custom = ""; //< Used for custom functions
		string color = ""; ///< Tint of button

		string folderName = ""; ///< Level name (custom for level select buttons
		int levelNum;

		bool deleteAfter = false; ///< delete after click

		bool customFunction(string func); ///< Executes a function not tied to the sprite

	public:

		CButton(eSpriteType sprite, Vector2 position, CRenderer* renderer); ///< Constructor
		bool OnPress();   ///< Actions to perform when the user presses the button. Returns false if the butotn is deleted
		void OnRelease(); ///< Actions to perform when the user releases the button
		void OnHover();   ///< Actions to perform when the user overs the mouse over the button
		bool PositionInSprite(Vector2 pos); ///< If the position is within the sprite boundaries of the button
		void SetScale(float xScale, float yScale); ///< Sets the scale of the button and resizes the bounding box
		void Draw(); ///< Draws the button
		eSpriteType GetSprite() { return sprite; };

		void SetCustomFunc(string func) { custom = func; }; ///< Set a custom function string
		void SetColor(string color) { this->color = color; }; ///< Set color of button

		void SetFolderName(string name) { folderName = name; };
		void SetLevelNumber(int num) { levelNum = num; };

		

};