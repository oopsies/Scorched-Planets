/// \file Button.cpp
/// \brief Implementation for the CButton class.

#include "Button.h"
#include "Renderer.h"
#include "LevelManager.h"
#include "LevelEditor.h"

CButton::CButton(eSpriteType sprite, Vector2 position, CRenderer* renderer) {
	this->sprite = sprite;
	this->m_vSpritePosition = position;
	this->renderer = renderer;
	m_iSpriteWidth = static_cast<int>(renderer->GetWidth(sprite));
	m_iSpriteHeight = static_cast<int>(renderer->GetHeight(sprite));

	//set bounding box (screen coordinates
	BBox.left = (long) (m_vSpritePosition.x - (m_iSpriteWidth / 2));
	BBox.right = (long) (m_vSpritePosition.x + (m_iSpriteWidth / 2));
	BBox.bottom = (long) (m_vSpritePosition.y - (m_iSpriteHeight / 2));
	BBox.top = (long) (m_vSpritePosition.y + (m_iSpriteHeight / 2));
}

//What happens for each button when the user presses it
bool CButton::OnPress() {

	if (customFunction(custom)) {//check/execute custom function 
		return deleteAfter;
	}


	switch (sprite) {

		case BUTTON_CAMLOCK_SPRITE:
			sprite = BUTTON_CAMUNLOCK_SPRITE;
			m_eCameraMode = CameraMode::PLAYER_UNLOCKED;
			m_unlockedCameraPos = Vector2(renderer->GetCameraPos() / renderer->get_scale_factor());
			break;

		case BUTTON_CAMUNLOCK_SPRITE:
			sprite = BUTTON_CAMLOCK_SPRITE;
			m_eCameraMode = CameraMode::PLAYER_LOCKED;
			break;

		case BUTTON_ZOOMIN_SPRITE:
			if (renderer->get_scale_factor() < 1.5f)
				renderer->set_scale_factor(renderer->get_scale_factor() + .05f);
			break;

		case BUTTON_ZOOMOUT_SPRITE:
			if (renderer->get_scale_factor() > 0.1f)
				renderer->set_scale_factor(renderer->get_scale_factor() - .05f);
			break;

		case BUTTON_STARTGAME_SPRITE:
			//load first level
			m_eGameState = GameState::MODE_SELECT;
			m_nCurrentLevel = 0;
			break;

		case BUTTON_LEVELEDITOR_SPRITE:
			m_nCurrentLevel = -1;
			break;

		case BUTTON_CREATEPLANET_SPRITE:
			m_pLevelEditor->SetEditMode(EditMode::PLANET_CREATE);
			break;

		case BUTTON_CREATETANK_SPRITE:
			m_pLevelEditor->SetEditMode(EditMode::TANK_CREATE);
			break;

		case BUTTON_SAVELEVEL_SPRITE:
			//m_pLevelEditor->SaveLevel("testsave.txt");
			m_pLevelEditor->SetEditMode(EditMode::SAVING);
			break;

		case BUTTON_LOADLEVEL_SPRITE:
			//m_pLevelEditor->LoadLevel("testsave.txt");
			m_pLevelEditor->SetEditMode(EditMode::LOADING);
			break;
		
		case BUTTON_COLORSELECT_SPRITE:
			m_pLevelEditor->SetSelectedColor(m_pLevelManager->stringToColor(color));
			break;

		case BUTTON_CANCEL_SPRITE:
			m_pLevelEditor->SetEditMode(EditMode::PLANET_CREATE);
			m_pLevelEditor->SetUserFilename("");
			return true;
			break;

		case BUTTON_BACK_SPRITE:
			m_nCurrentLevel = -1;
			m_pTurnManager->SetTurn(0);
			m_pLevelManager->set_level_name("Custom Level");
			//m_pLevelManager->LoadMap(-1);
			//printf("map loaded\n");
			break;

		case BUTTON_LEVELNAME_SPRITE:
			m_pLevelManager->setFilenames(m_pLevelManager->getLevelFilenames(folderName));
			m_pLevelManager->setSelectedFolder(folderName);
			m_pLevelManager->createLevelButtons();
			return true;
			break;
		case BUTTON_CLASSICMODE_SPRITE:
			m_eGameState = GameState::LEVEL_SELECT;
			m_bTurnsEnabled = true;
			m_nCurrentLevel = 0;
			break;
		case BUTTON_BLITZMODE_SPRITE:
			m_eGameState = GameState::LEVEL_SELECT;
			m_bTurnsEnabled = false;
			m_nCurrentLevel = 0;
			break;
		case BUTTON_INSTRUCTIONS_SPRITE:
			m_eGameState = GameState::INSTRUCTIONS;
			m_nCurrentLevel = 0;
			break;

	};
	return false;
};

//What happens for each button when the user releases it
void CButton::OnRelease() {
	switch (sprite) {

	case BUTTON_CAMLOCK_SPRITE:
		
		break;

	case BUTTON_CAMUNLOCK_SPRITE:
		break;

	};
}

//What happens for each button when the user hovers over it
void CButton::OnHover() {
	switch (sprite) {

	case BUTTON_CAMLOCK_SPRITE:
		break;

	case BUTTON_CAMUNLOCK_SPRITE:
		break;

	case BUTTON_CLASSICMODE_SPRITE: {
		string s = "The classic mode of Scorched Planets. Take turns shooting";
		renderer->DrawScreenText(s.c_str(), Vector2((float)renderer->GetWindowSize().x / 2.0f - 355.0f, 525.0f), Colors::Orange);
		s = "until the last man standing. (1+ Human Players)";
		renderer->DrawScreenText(s.c_str(), Vector2((float)renderer->GetWindowSize().x / 2.0f - 300.0f, 560.0f), Colors::Orange);
	}
		break;

	case BUTTON_BLITZMODE_SPRITE: {
		string s = "No turns. Move and shoot simultaneously with other";
		renderer->DrawScreenText(s.c_str(), Vector2((float)renderer->GetWindowSize().x / 2.0f - 325.0f, 525.0f), Colors::Orange);
		s = "tanks until one tank remains. (1 Human Player)";
		renderer->DrawScreenText(s.c_str(), Vector2((float)renderer->GetWindowSize().x / 2.0f - 300.0f, 560.0f), Colors::Orange);
	}
		break;

	};
}

//Returns true if pos is within the bounding box.
bool CButton::PositionInSprite(Vector2 pos) {
	int h = static_cast<int>(m_pRenderer->GetWindowSize().y);
	return pos.x > BBox.left && pos.x < BBox.right&& pos.y < h - BBox.bottom && pos.y > h - BBox.top;
}

//Sets the scale of the button and resizes the bounding box
void CButton::SetScale(float xScale, float yScale) {
	//set xscale & yscale
	this->xScale = xScale;
	this->yScale = yScale;

	//resize width and height
	m_iSpriteWidth = static_cast<int>(m_iSpriteWidth * xScale);
	m_iSpriteHeight = static_cast<int>(m_iSpriteHeight * yScale);

	//resize bounding box
	BBox.left = (long)(m_vSpritePosition.x - (m_iSpriteWidth / 2));
	BBox.right = (long)(m_vSpritePosition.x + (m_iSpriteWidth / 2));
	BBox.bottom = (long)(m_vSpritePosition.y - (m_iSpriteHeight / 2));
	BBox.top = (long)(m_vSpritePosition.y + (m_iSpriteHeight / 2));
}

//Draws the button to the screen in its position
void CButton::Draw() {
	Vector2 camPos = renderer->GetCameraPos() - (renderer->GetWindowSize() / 2);
	CSpriteDesc2D spr; //create new sprite
	spr.m_nSpriteIndex = sprite;
	spr.m_vPos = camPos + m_vSpritePosition;
	spr.m_fXScale = xScale;
	spr.m_fYScale = yScale;
	if (color != "")
		spr.m_f4Tint = m_pLevelManager->stringToColor(color);
	renderer->DrawUnscaled(spr); //draw the sprite
}

bool CButton::customFunction(string func) { //perform a function NOT based on the button's sprite

	if (func == "mass_up") {
		if (m_pLevelEditor->GetMass() < 1000.0f)
			m_pLevelEditor->SetMass(m_pLevelEditor->GetMass() + 5.0f);
		return true;
	}
	else if (func == "mass_down") {
		if (m_pLevelEditor->GetMass() > 0.0f)
			m_pLevelEditor->SetMass(m_pLevelEditor->GetMass() - 5.0f);
		return true;
	}
	else if (func == "radius_up") {
		if (m_pLevelEditor->GetRadius() < 3000.0f)
			m_pLevelEditor->SetRadius(m_pLevelEditor->GetRadius() + 20.0f);
		return true;
	}
	else if (func == "radius_down") {
		if (m_pLevelEditor->GetRadius() > 60.0f)
			m_pLevelEditor->SetRadius(m_pLevelEditor->GetRadius() - 20.0f);
		return true;
	}
	else if (func == "world_size_up") {
		if (m_pLevelEditor->GetWorldSize().x < 45000) {
			m_pLevelEditor->SetWorldSize(m_pLevelEditor->GetWorldSize() + Vector2(250.0f, 250.0f));
		}
		return true;
	}
	else if (func == "world_size_down") {
		if (m_pLevelEditor->GetWorldSize().x > 5000) {
			m_pLevelEditor->SetWorldSize(m_pLevelEditor->GetWorldSize() - Vector2(250.0f, 250.0f));
		}
		return true;
	}
	else if (func == "confirm_save") {
		deleteAfter = true;
		m_pLevelEditor->SaveLevel(m_pLevelEditor->GetUserFilename() + ".txt");
		printf("Saved %s.txt\n", m_pLevelEditor->GetUserFilename().c_str());
		m_pLevelEditor->SetEditMode(EditMode::PLANET_CREATE);
		//printf("Switched\n");
		return true;
	}
	else if (func == "confirm_load") {
		deleteAfter = true;
		m_pLevelEditor->LoadLevel(m_pLevelEditor->GetUserFilename() + ".txt");
		printf("Loaded %s.txt\n", m_pLevelEditor->GetUserFilename().c_str());
		m_pLevelEditor->SetEditMode(EditMode::PLANET_CREATE);
		//printf("Switched\n");
		return true;
	}
	else if (func == "start_level_test") {

		if (m_pLevelEditor->StartTest()) {//save the current level
			deleteAfter = false;
			m_nCurrentLevel = -2;
			
		}
		else
			deleteAfter = true;
		return true;
	}
	else if (func == "return_main_menu") {
		if (m_eGameState == GameState::PLAYING)
			m_pTurnManager->SetTurn(0);

		switch (m_eGameState) {
			
			case GameState::LEVEL_SELECT:
				m_eGameState = GameState::MODE_SELECT;
				break;
			case GameState::PLAYER_SELECT:
				m_eGameState = GameState::LEVEL_SELECT;
				break;
			default:
				m_eGameState = GameState::TITLE_SCREEN;
				break;
		}

		m_nCurrentLevel = 0;
		return true;
	}
	else if (func == "play_level") {

		if (!m_bTurnsEnabled || levelNum == -999) {
			m_nCurrentLevel = levelNum;
		}
		else {
			m_nCurrentLevel = 0;
			m_pLevelManager->setSelectedLevel(levelNum);
			m_eGameState = GameState::PLAYER_SELECT;
		}
			
		return true;
	}
	else if (func == "humans_up") {
		int tCount = m_pLevelManager->getTankCount(m_pLevelManager->getSelectedFolder() + "\\" + m_pLevelManager->getFilenames()[m_pLevelManager->getSelectedLevel() - 1]);
		if (m_pTurnManager->getNumHumanPlayers() < m_pLevelManager->getCurrentTankCount())
			m_pTurnManager->setNumHumanPlayers(m_pTurnManager->getNumHumanPlayers() + 1);

		return true;
	}
	else if (func == "humans_down") {
		if (m_pTurnManager->getNumHumanPlayers() > 1)
			m_pTurnManager->setNumHumanPlayers(m_pTurnManager->getNumHumanPlayers() - 1);
		return true;
	}
	else if (func == "tanks_up") {
		int tCount = m_pLevelManager->getTankCount(m_pLevelManager->getSelectedFolder() + "\\" + m_pLevelManager->getFilenames()[m_pLevelManager->getSelectedLevel() - 1]);
		if (m_pLevelManager->getCurrentTankCount() < tCount)
			m_pLevelManager->setCurrentTankCount(m_pLevelManager->getCurrentTankCount() + 1);
		return true;
	}
	else if (func == "tanks_down") {
	if (m_pLevelManager->getCurrentTankCount() > 2)
		m_pLevelManager->setCurrentTankCount(m_pLevelManager->getCurrentTankCount() - 1);
	if (m_pTurnManager->getNumHumanPlayers() > m_pLevelManager->getCurrentTankCount())
		m_pTurnManager->setNumHumanPlayers(m_pLevelManager->getCurrentTankCount());
		return true;
	}
	else if (func == "play_level_classic") {
		m_nCurrentLevel = m_pLevelManager->getSelectedLevel();
		return true;
	}
	else if (func == "page_up") {
		if (instructionPage < 3)
			instructionPage++;
		return true;
	}
	else if (func == "page_down") {
		if (instructionPage > 0)
			instructionPage--;
		return true;
	}

	return false;
}