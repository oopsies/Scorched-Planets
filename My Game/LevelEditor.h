/// \file LevelEditor.h
/// \brief Interface for the CLevelEditor class.
#pragma once

#include "Common.h"
#include "PlanetObject.h"
#include "TankObject.h"
#include "ObjectManager.h"
#include "ComponentIncludes.h"

#include <list>

/// \brief Provides an interface for the user to create their own levels
///And save them to play later.

enum class EditMode { //possible modes the player can be in when editing
	PLANET_CREATE,
	TANK_CREATE,
	SAVING,
	LOADING
};

class CLevelEditor : public CCommon, public CSettings {

	private:

		list<CPlanetObject*> planets; ///< List of planets the player has created
		list<std::shared_ptr<CTankObject>> tanks; ///< List of tanks the player has created
		Vector2 worldSize; ///< Size of the level
		CLevelEditor::EditMode editMode; ///< Mode of editing the user is in

		list<std::shared_ptr<CButton>> buttons; ///< List of buttons that will change between edit modes
		
		///Vector of colors to choose from
		vector<XMFLOAT4> colors_vector = { XMFLOAT4(Colors::Red), XMFLOAT4(Colors::Blue), XMFLOAT4(Colors::White), XMFLOAT4(Colors::LimeGreen),XMFLOAT4(Colors::Gold), XMFLOAT4(Colors::Green), XMFLOAT4(Colors::Purple), XMFLOAT4(Colors::Orange) };
		vector<string> colors_strings_vector = { "red", "blue", "white", "lime", "gold", "green", "purple", "orange" };
		XMFLOAT4 selectedColor; ///< Player's selected tank color

		float planetRadius; ///< Radius of planet to create
		float planetMass; ///< Mass of planet to create

		float getDistance(Vector2 p1, Vector2 p2); ///< Gets distance from p1 to p2
		float getAngle(Vector2 p1, Vector2 p2); ///< Gets angle from p1 to p2 in degrees

		bool testing = false; ///< Whether the user is testing the current level

		std::string filename; ///< The user input filename to save/load

	public:

		CLevelEditor(); ///< Constructor
		~CLevelEditor(); ///< Destructor
		void SaveLevel(string filename); ///< Saves the current level
		void LoadLevel(string filename); ///< Loads a level file
		void DrawObject(Vector2 pos); ///< Draws opaque object over mouse
		void ObjectCreate(Vector2 pos); ///< Create an object at/near the position
		void ObjectDelete(Vector2 pos); ///< Delete an object at/near the position

		bool StartTest(); ///< Lets the user play the current level in the editor - returns true if save succeeded

		void SetEditMode(EditMode mode); ///< Sets the mode (planet, tank/add, delete)
		EditMode GetEditMode(); ///< Gets the edit mode

		list<CPlanetObject*> GetPlanets() { return planets; };
		list<std::shared_ptr<CTankObject>> GetTanks() { return tanks; };

		string ModeToString(EditMode mode); //< Outputs the edit mode as a string

		float GetMass() { return planetMass; };
		void SetMass(float m) { planetMass = m; };
		float GetRadius() { return planetRadius; };
		void SetRadius(float r) { planetRadius = r; };
		string GetSelectedColor() { //have to get color in annyoing way
			for (int i = 0; i < colors_vector.size(); i++) {
				if (selectedColor.x == colors_vector[i].x && selectedColor.y == colors_vector[i].y && selectedColor.z == colors_vector[i].z)
					return colors_strings_vector[i];
			}
		};
		void SetSelectedColor(XMFLOAT4 color) { selectedColor = color; };
		Vector2 GetWorldSize() { return worldSize; };
		void SetWorldSize(Vector2 s) { worldSize = s; m_vWorldSize = worldSize; };
		bool GetTesting() { return testing; };
		void SetTesting(bool testing) { this->testing = testing; };

		std::string GetUserFilename() { return filename; }
		void SetUserFilename(std::string s) { filename = s; };

		void Clear(); ///< Clear all objects and remove them from memory

};