/// \file LevelEditor.cpp
/// \brief Code for the level editor class CLevelEditor

#include "LevelEditor.h"
#include "LevelManager.h"
#include "Button.h"
#include <fstream>
#include <sstream>
#include <string>

CLevelEditor::CLevelEditor() {
	worldSize = Vector2(25000, 25000);
	m_vWorldSize = worldSize;
	selectedColor = XMFLOAT4(Colors::Red);
	editMode = EditMode::PLANET_CREATE;

	planetRadius = 200.0f;
	planetMass = 5.0f;

	filename = "";

}

CLevelEditor::~CLevelEditor() {
	//Clear();
}

void CLevelEditor::SaveLevel(string filename) {
	ofstream fout;
	fout.open("Levels\\Your Levels\\" + filename);

	string levelName = filename;
	levelName.erase(levelName.length() - 4);
	m_pLevelManager->set_level_name(string(levelName));

	//Write name
	fout << "NAME " + levelName << endl;

	//world dimensions
	fout << "WORLD " << (int)worldSize.x << " " << (int)worldSize.y << endl;

	//planets
	for (auto const& p : planets)
		fout << "PLANET " << (int)p->GetPos().x << " " << (int)p->GetPos().y << " " << p->GetMass() << " " << p->get_radius() << endl;

	//tanks
	for (std::shared_ptr<CTankObject>& t : tanks) {
		int planetNo = 0;

		//get planet number
		int i = 0;
		for (auto const& p : planets) {
			if (p == t->get_planet_index()){
				planetNo = i;
				break;
			}
			i++;
		}

		string color = "red";
		for (i = 0; i < colors_vector.size(); i++) {
			if (t->GetColor().x == colors_vector[i].x && t->GetColor().y == colors_vector[i].y && t->GetColor().z == colors_vector[i].z) { //can't compare XMFLOAT4's??
				color = colors_strings_vector[i];
			}
		}

		int player = t->get_is_player_character();

		fout << "TANK " << t->get_angle_relative_to_planet() << " " << planetNo << " " << color << " " << player << endl;
	}

	fout.close();
	
}

void CLevelEditor::LoadLevel(string filename) {
	
	//Clear();
	//printf("cleared this\n");
	m_pObjectManager->clear();
	planets.clear();
	tanks.clear();

	std::ifstream infile("Levels\\Your Levels\\" + filename);
	if (!infile) {
		printf("cannot open file\n");
		return;
	}

	vector<CPlanetObject*> pV; //vector of planets to reference for the tanks

	string line;
	while (std::getline(infile, line)) {
		if (line.substr(0, 5) == "WORLD") { //line is world info
			//format: WORLD <width> <height>

			int w, h;
			string s; //placeholder for "WORLD"

			std::istringstream iss(line); //create string stream
			if (!(iss >> s >> w >> h)) { break; }

			m_vWorldSize = Vector2(w, h);

		}
		else if (line.substr(0, 6) == "PLANET") { //line is planet info
			//create planets before tanks!
			//format: PLANET <xpos> <ypos> <mass> <radius>

			int xPos, yPos;
			float mass, radius;
			string s; //placeholder for "PLANET"

			std::istringstream iss(line); //create string stream
			if (!(iss >> s >> xPos >> yPos >> mass >> radius)) { break; }

			CPlanetObject* p = m_pObjectManager->create_planet(Vector2(xPos, yPos), mass, radius);

			planets.push_back(p);
			pV.push_back(p);

		}
		else if (line.substr(0, 4) == "TANK") { //line is tank info
			//create planets before tanks!
			//format: TANK <angle> <planet no.> <color> <player?>

			float angle;
			int planetNo;
			string color;
			int isPlayer;
			string s; //placeholder for "TANK"

			std::istringstream iss(line); //create string stream
			if (!(iss >> s >> angle >> planetNo >> color >> isPlayer)) { break; }

			XMFLOAT4 pColor = m_pLevelManager->stringToColor(color); //actual color type of player

			if (planetNo >= planets.size())
				planetNo = 0;

			std::shared_ptr<CTankObject> t = m_pObjectManager->create_tank(angle, pV[planetNo], pColor);

			if (isPlayer != 0)
				t->set_is_player_character(true);
			
			tanks.push_back(t);
			t->move();

		}
		else if (line.substr(0, 4) == "NAME") { //line is level name
		  //Set the level name.
		  //format: NAME <name>
			string s; //placeholder for "NAME"
			char name[256];
			std::istringstream iss(line); //create string stream
			if (!(iss >> s)) { break; }
			iss.getline(name, 256);
			m_pLevelManager->set_level_name(string(name));
		}
		
	}
	infile.close();

	//if the level is the temp level for testing, remove file
	remove("Levels\\Your Levels\\user-level-test.txt");
}

void CLevelEditor::DrawObject(Vector2 pos) {
	switch (editMode) {
		case EditMode::PLANET_CREATE:
			break;
		case EditMode::TANK_CREATE:
			break;
	}
}

void CLevelEditor::ObjectCreate(Vector2 pos) {
	switch (editMode) {
		case EditMode::PLANET_CREATE: { //create a planet
			if (pos.x > 0 && pos.x < worldSize.x && pos.y > 0 && pos.y < worldSize.y) { //make sure not out of bounds
				CPlanetObject* p = m_pObjectManager->create_planet(Vector2(pos.x, pos.y), planetMass, planetRadius);
				planets.push_back(p);
				//printf("%d, %d\n", (int)p->GetPos().x, (int)(p->GetPos().y));
			}
		}
			break;
		case EditMode::TANK_CREATE: { //create a tank
			//if there are no planets, do not create anything
			if (planets.size() == 0)
				return;

			//find planet with smallest distance from point
			float smallestDist = 100000.0f;
			float angle = 90.0f;
			CPlanetObject* bestPlanet = nullptr;
			for (auto const& p : planets) {
				float d = getDistance(pos, p->GetPos());
				if (d < smallestDist) {
					angle = getAngle(pos, p->GetPos());
					smallestDist = d;
					bestPlanet = p;
				}
			}

			if (bestPlanet == nullptr)
				return;

			//create tank and add to list
			std::shared_ptr<CTankObject> t = m_pObjectManager->create_tank(angle - 90.0f, bestPlanet, selectedColor);
			if (tanks.size() == 0)
				t->set_is_player_character(true);
			tanks.push_back(t);
			t->move(); //move to have it appear on planet

			}
			break;
	}
}

void CLevelEditor::ObjectDelete(Vector2 pos) {
	switch (editMode) {
		case EditMode::PLANET_CREATE: {
			if (planets.size() == 0)
				return;

			//go through every planet and find the one that is closest to the click position

			float smallestDist = 100000.0f;
			CPlanetObject* bestPlanet;
			for (auto& p : planets) {
				float d = getDistance(pos, p->GetPos());
				if (d < smallestDist) {
					smallestDist = d;
					bestPlanet = p;
				}
			}

			//remove any tanks that are on the planet
			for (std::shared_ptr<CTankObject>& t : m_pObjectManager->get_tanks_list()) {
				if (t->get_planet_index() == bestPlanet) { //if this planet is t's home planet: remove
					m_pObjectManager->get_tanks_list_pointer()->remove(t);
					//remove from local list too
					for (auto& t2 : tanks) {
						if (t == t2) {
							tanks.remove(t2);
							break;
						}
					}
					if (tanks.size() == 0)
						break;
				}
			}
			
			//remove planet from planets objects list and local planet list
			for (auto& p : m_pObjectManager->get_planets_list()) {
				if (p == bestPlanet) {
					m_pObjectManager->get_planets_list_pointer()->remove(p);
					break;
				}
			}
			
			for (auto& p : planets) {
				if (p == bestPlanet) {
					delete p;
					planets.remove(p);
					break;
				}
			}
			
		}
		break;

		case EditMode::TANK_CREATE: {
			if (tanks.size() == 0)
				return;
			//go through every tank and find the one that is closest to the click position

			float smallestDist = 100000.0f;
			std::shared_ptr<CTankObject> bestTank;
			for (std::shared_ptr<CTankObject>& t : tanks) {
				float d = getDistance(pos, t->GetPos());
				if (d < smallestDist) {
					smallestDist = d;
					bestTank = t;
				}
			}

			//remove tank from local list and object manager list
			
			for (auto& t : m_pObjectManager->get_tanks_list()) {
				if (t == bestTank) {
					m_pObjectManager->get_tanks_list_pointer()->remove(t);
					break;
				}
			}

			for (auto& t : tanks) {
				if (t == bestTank) {
					tanks.remove(t);
					break;
				}
			}
			
		}
		break;
	}

}

bool CLevelEditor::StartTest() {

	if (planets.size() == 0 || tanks.size() == 0) //can't start if there are no tanks or planets
		return false;

	testing = true; //we are testing a level

	SaveLevel("user-level-test.txt"); //save level in temporary file

	//clear lists
	planets.clear();
	tanks.clear();

	m_eGameState = GameState::PLAYING; //playing game
	
	//load and begin game will be called from the button

	return true;

}

void CLevelEditor::SetEditMode(EditMode mode) { //set edit mode and buttons

	if (editMode == mode) //if nothing is changed, dont do anything
		return;

	editMode = mode;

	//remove buttons from previous mode
	for (auto const& b1 : buttons) {
		for (auto const& b2 : m_lButtonList) {
			if (b1 == b2) {
				m_lButtonList.remove(b2);
				break;
			}
		}
	}
	buttons.clear();
	
	//buttons for creating planet
	if (editMode == EditMode::PLANET_CREATE) {
		std::shared_ptr<CButton> b = std::make_shared<CButton>(BUTTON_UP_SPRITE, Vector2(50.0f, (float)m_nWinHeight - 170.0f), m_pRenderer);
		b->SetCustomFunc("radius_up");
		m_lButtonList.push_back(b);
		buttons.push_back(b);

		b = std::make_shared<CButton>(BUTTON_DOWN_SPRITE, Vector2(50.0f, (float)m_nWinHeight - 200.0f), m_pRenderer); //radius down
		b->SetCustomFunc("radius_down");
		m_lButtonList.push_back(b);
		buttons.push_back(b);

		b = std::make_shared<CButton>(BUTTON_UP_SPRITE, Vector2(50.0f, (float)m_nWinHeight - 270.0f), m_pRenderer); //mass up
		b->SetCustomFunc("mass_up");
		m_lButtonList.push_back(b);
		buttons.push_back(b);

		b = std::make_shared<CButton>(BUTTON_DOWN_SPRITE, Vector2(50.0f, (float)m_nWinHeight - 300.0f), m_pRenderer); //mass down
		b->SetCustomFunc("mass_down");
		m_lButtonList.push_back(b);
		buttons.push_back(b);
	}
	else if (editMode == EditMode::TANK_CREATE) {
		float xPos = 0.0f;
		for (int i = 0; i < colors_vector.size(); i++) {
			xPos += 50.0f;
			std::shared_ptr<CButton> b = std::make_shared<CButton>(BUTTON_COLORSELECT_SPRITE, Vector2(xPos, (float)m_nWinHeight - 250.0f), m_pRenderer); //mass up
			b->SetColor(colors_strings_vector[i]);
			m_lButtonList.push_back(b);
			buttons.push_back(b);
		}
	}
	else if (editMode == EditMode::SAVING) {

		std::shared_ptr<CButton> b = std::make_shared<CButton>(INPUT_TEXTBOX_SPRITE, Vector2((float)m_nWinWidth / 2.0f, (float)m_nWinHeight / 2.0f), m_pRenderer); //textbox (no button functionality)
		b->SetScale(1.5f, 1.5);
		m_lButtonList.push_back(b);
		buttons.push_back(b);

		b = std::make_shared<CButton>(BUTTON_CANCEL_SPRITE, Vector2(520.0f, (float)m_nWinHeight - 505.0f), m_pRenderer); //cancel save
		b->SetScale(1.5f, 1.5);
		m_lButtonList.push_back(b);
		buttons.push_back(b);

		b = std::make_shared<CButton>(BUTTON_CONFIRM_SPRITE, Vector2(830.0f, (float)m_nWinHeight - 505.0f), m_pRenderer); //confirm save
		b->SetScale(1.5f, 1.5);
		b->SetCustomFunc("confirm_save");
		m_lButtonList.push_back(b);
		buttons.push_back(b);

	}
	else if (editMode == EditMode::LOADING) {

		std::shared_ptr<CButton> b = std::make_shared<CButton>(INPUT_TEXTBOX_SPRITE, Vector2((float)m_nWinWidth / 2.0f, (float)m_nWinHeight / 2.0f), m_pRenderer); //textbox
		b->SetScale(1.5f, 1.5);
		m_lButtonList.push_back(b);
		buttons.push_back(b);

		b = std::make_shared<CButton>(BUTTON_CANCEL_SPRITE, Vector2(520.0f, (float)m_nWinHeight - 505.0f), m_pRenderer); //cancel load
		b->SetScale(1.5f, 1.5);
		m_lButtonList.push_back(b);
		buttons.push_back(b);

		b = std::make_shared<CButton>(BUTTON_CONFIRM_SPRITE, Vector2(830.0f, (float)m_nWinHeight - 505.0f), m_pRenderer); //confirm save
		b->SetScale(1.5f, 1.5);
		b->SetCustomFunc("confirm_load");
		m_lButtonList.push_back(b);
		buttons.push_back(b);

	}

}

EditMode CLevelEditor::GetEditMode() {
	return editMode;
}

void CLevelEditor::Clear() {
	worldSize = Vector2(3000, 3000);
	selectedColor = XMFLOAT4(Colors::Red);

	//planets
	//for (auto const& p : planets)
	//	delete p;
	planets.clear();
		
	//tanks
	//for (std::shared_ptr<CTankObject>& t : tanks)
	//	t = nullptr;
	tanks.clear();

}

float CLevelEditor::getDistance(Vector2 p1, Vector2 p2) {
	float xdSq = pow(p2.x - p1.x, 2);
	float ydSq = pow(p2.y - p1.y, 2);
	return (float)sqrt(xdSq + ydSq);
}

float CLevelEditor::getAngle(Vector2 p1, Vector2 p2) {
	//get angle and power based on mouse
	int deltaX = (int)p1.x - (int)p2.x;
	int deltaY = (int)p1.y - (int)p2.y;
	return (float)atan2(deltaX, -deltaY) * (180.0f / (float)M_PI);
}

string CLevelEditor::ModeToString(EditMode mode) {
	switch (mode) {
	case EditMode::SAVING:
	case EditMode::LOADING:
	case EditMode::PLANET_CREATE:
		return "Create Planet";
		break;
	case EditMode::TANK_CREATE:
		return "Create Tank";
		break;
	}
	return "Unknown"; //this should never happen
}