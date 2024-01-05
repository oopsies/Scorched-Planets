/// \file TurnManager.cpp
/// \brief Code for the the turn manager class CTurnManager.

#include "TurnManager.h"
#include "ObjectManager.h"
#include "LevelManager.h"
#include "Random.h"

#include <fstream>

CTurnManager::CTurnManager() {
	m_iNumPlayers = 0;
	m_iCurrentTurn = 0;
	m_currentTank = nullptr;
	m_iTotalTurns = 0;
	m_iHumanPlayers = 1; //there will always be at least 1 human player?

}//default constructor

CTurnManager::CTurnManager(list<std::shared_ptr<CTankObject>> tankList) {
	//set all values to 0/null
	m_iCurrentTurn = 0;
	m_iTotalTurns = 0;

	//get list of tanks
	m_tanks_list = tankList;

	//set the number of players and first tank based off list
	m_iNumPlayers = (int)m_tanks_list.size();
	m_currentTank = m_tanks_list.front();
	m_currentTank->set_in_control(true);

}//constructor

/// Set tank list if it was not done in the constructor. This must be done in order for the manager to function.
/// \param tankList List of tanks

void CTurnManager::SetTankList(list<std::shared_ptr<CTankObject>> tankList) {
	m_tanks_list = tankList;

	//set the number of players and first tank based off list
	m_iNumPlayers = (int)m_tanks_list.size();
	m_currentTank = m_tanks_list.front();
	m_currentTank->set_in_control(true);

}

/// Increment the turn counters and updates the current tank.
/// Reaching the end of the list wraps to the beginning.
/// \return Pointer to the tank being controlled by input.

std::shared_ptr<CTankObject> CTurnManager::NextTurn() {
	m_iTotalTurns++;

	//set camera
	m_eCameraMode = CameraMode::PLAYER_LOCKED;

	//if there is only one tank left, set game over 
	if (aliveCount() == 1) {
		m_bGameOver = true;

		//if the last tank is a player, mark the current level as completed
		if (m_currentTank->get_is_player_character()) {
			string levelFileString = m_pLevelManager->getSelectedFolder() + "\\" + m_pLevelManager->get_level_name().substr(1) + ".txt";
			if (!m_pLevelManager->LevelCleared(levelFileString)) { //if the level has not yet been cleared
				//append "CLEARED" to the end of the level
				printf("epic\n");
				std::ofstream fout;

				fout.open("Levels\\" + levelFileString, std::ios::app);
				if (!fout) {
					printf("cannot open file %s\n", levelFileString.c_str());
				}
				fout << "\nCLEARED";
				fout.close();

			}
		}

		return m_currentTank;
	}

	//do wormhole stuff
	createWormholes();
	cullDeadWormholes();

	//unlock controls
	m_bControlLock = false;

	//restore the fuel of the current tank
	m_pPlayer->set_current_fuel(m_pPlayer->get_max_fuel());

	//make sure 'in_control' variable is not true anymore for old tank
	m_currentTank->set_in_control(false);

	//check if current tank is the last tank in the list
	if (m_currentTank == m_tanks_list.back()) {
		//wrap around to front
		m_currentTank = m_tanks_list.front();
		m_currentTank->set_in_control(true);
		m_iCurrentTurn = 0;

		return m_currentTank;
	}

	//if not, iterate through tanks list to find the next tank.
	list<std::shared_ptr<CTankObject>>::iterator it;

	m_iCurrentTurn = 0; //get correct turn #
	//go to next tank
	for (it = m_tanks_list.begin(); it != m_tanks_list.end(); it++) {
		if (m_currentTank == *it) {
			do {
				++it;
				if (it == m_tanks_list.end())
					it = m_tanks_list.begin();
			} while ((*it)->IsDead());
			m_currentTank = *it;
			m_currentTank->set_in_control(true);
			return m_currentTank;
		}
		m_iCurrentTurn++;
	}
	m_currentTank->set_in_control(true);

	return m_currentTank;
}

/// Gives control to sepcified tank and updates counters.
/// \param tankNo tank number to be controlled by input
/// \return Pointer to the tank being controlled by input.

std::shared_ptr<CTankObject> CTurnManager::SetTurn(int tankNo) {
	m_iTotalTurns++;
	m_iCurrentTurn = tankNo;

	//set camera
	m_eCameraMode = CameraMode::PLAYER_LOCKED;

	//if there is only one tank left, set game over 
	if (aliveCount() == 1) {
		m_bGameOver = true;
		return m_currentTank;
	}

	//unlock controls
	m_bControlLock = false;

	//make sure 'in_control' variable is not true anymore for old tank
	m_currentTank->set_in_control(false);

	//create iterator to advance through list
	list<std::shared_ptr<CTankObject>>::iterator it = m_tanks_list.begin();

	//advance by tankNo spots
	std::advance(it, tankNo);

	//set new tank and return it
	m_currentTank = *it;
	m_currentTank->set_in_control(true);
	return m_currentTank;
}

std::shared_ptr<CTankObject> CTurnManager::GetNextTank() {
	//check if current tank is the last tank in the list
	if (m_currentTank == m_tanks_list.back()) 
		return m_tanks_list.front();

	//if not, iterate through tanks list to find the next tank.
	list<std::shared_ptr<CTankObject>>::iterator it;

	//go to next tank
	for (it = m_tanks_list.begin(); it != m_tanks_list.end(); it++) {
		if (m_currentTank == *it) {
			do {
				++it;
				if (it == m_tanks_list.end())
					it = m_tanks_list.begin();
			} while ((*it)->IsDead());
			
			return *it;
		}
	}
	return m_currentTank;
}

int CTurnManager::aliveCount() {
	list<std::shared_ptr<CTankObject>>::iterator it;

	int count = 0;

	//iterate through tanks
	for (it = m_tanks_list.begin(); it != m_tanks_list.end(); it++) {
		//increase count if tank not dead
		if (!(*it)->IsDead())
			count++;
	}

	return count;
}

void CTurnManager::createWormholes() {
	if (m_pObjectManager->get_wormholes_list_pointer()->size() == 0) {
		CRandom rand = CRandom();
		int randInt = rand.randn(0, 5);
		if (randInt == 1 || randInt == 0) { //1 in 6 chance

			Vector2 w1Pos;
			Vector2 w2Pos;
			//decide positions of wormholes
			if (m_pObjectManager->get_planets_list().size() == 1) { //if there's only 1 planet
				//determine angles relative to planet
				int angle1 = rand.randn(0, 359);
				int angle2 = rand.randn(angle1, angle1 + 240); //angles at least 120 degrees apart

				//get normailzed vectors
				CPlanetObject* p = m_pObjectManager->get_planets_list().front();
				Vector2 planetPos = p->GetPos();
				Vector2 dir1Vec = Vector2(cos(angle1 * (M_PI /180.0f)), sin(angle1 * (M_PI / 180.0f)));
				Vector2 dir2Vec = Vector2(cos(angle2 * (M_PI / 180.0f)), sin(angle2 * (M_PI / 180.0f)));

				//determine positions of wormholes
				Vector2 worm1Pos = planetPos + (dir1Vec * (p->get_radius() + 350.0f));
				Vector2 worm2Pos = planetPos + (dir2Vec * (p->get_radius() + 350.0f));

				//create wormholes
				CWormholeObject* w1 = m_pObjectManager->create_wormhole(worm1Pos, m_tanks_list.size() * 2);
				CWormholeObject* w2 = m_pObjectManager->create_wormhole(worm2Pos, m_tanks_list.size() * 2, w1);
				w1->SetNextWormhole(w2);

			}
			else {
				//pick two random planets
				int planet1Num = rand.randn(0, m_pObjectManager->get_planets_list().size() - 1);
				int planet2Num = rand.randn(0, m_pObjectManager->get_planets_list().size() - 1);

				if (planet1Num == planet2Num) { //if the two picked planets are the same
					CPlanetObject* p;
					int i = 0;
					for (auto& planet : m_pObjectManager->get_planets_list()) { //get planet
						if (i++ == planet1Num)
							p = planet;
					}

					int angle1 = rand.randn(0, 359);
					int angle2 = rand.randn(angle1, angle1 + 240); //angles at least 120 degrees apart

					//get normailzed vectors
					Vector2 planetPos = p->GetPos();
					Vector2 dir1Vec = Vector2(cos(angle1 * (M_PI / 180.0f)), sin(angle1 * (M_PI / 180.0f)));
					Vector2 dir2Vec = Vector2(cos(angle2 * (M_PI / 180.0f)), sin(angle2 * (M_PI / 180.0f)));

					//determine positions of wormholes
					Vector2 worm1Pos = planetPos + (dir1Vec * (p->get_radius() + 350.0f));
					Vector2 worm2Pos = planetPos + (dir2Vec * (p->get_radius() + 350.0f));

					//create wormholes
					CWormholeObject* w1 = m_pObjectManager->create_wormhole(worm1Pos, m_tanks_list.size() * 2);
					CWormholeObject* w2 = m_pObjectManager->create_wormhole(worm2Pos, m_tanks_list.size() * 2, w1);
					w1->SetNextWormhole(w2);

				}

				else {
					CPlanetObject* p1;
					CPlanetObject* p2;
					int i = 0;
					for (auto& planet : m_pObjectManager->get_planets_list()) { //get planet
						if (i == planet1Num)
							p1 = planet;
						if (i == planet2Num)
							p2 = planet;
						i++;
					}

					int angle1 = rand.randn(0, 359);
					int angle2 = rand.randn(0, 359); //angles at least 120 degrees apart

					//get normailzed vectors
					Vector2 planet1Pos = p1->GetPos();
					Vector2 planet2Pos = p2->GetPos();
					Vector2 dir1Vec = Vector2(cos(angle1 * (M_PI / 180.0f)), sin(angle1 * (M_PI / 180.0f)));
					Vector2 dir2Vec = Vector2(cos(angle2 * (M_PI / 180.0f)), sin(angle2 * (M_PI / 180.0f)));

					//determine positions of wormholes
					Vector2 worm1Pos = planet1Pos + (dir1Vec * (p1->get_radius() + 350.0f));
					Vector2 worm2Pos = planet2Pos + (dir2Vec * (p2->get_radius() + 350.0f));

					//create wormholes
					CWormholeObject* w1 = m_pObjectManager->create_wormhole(worm1Pos, m_tanks_list.size() * 2);
					CWormholeObject* w2 = m_pObjectManager->create_wormhole(worm2Pos, m_tanks_list.size() * 2, w1);
					w1->SetNextWormhole(w2);

				}

			}

			/*CWormholeObject* w1 = m_pObjectManager->create_wormhole(Vector2(7500.0f, 9000.0f), m_tanks_list.size() * 1.5);
			CWormholeObject* w2 = m_pObjectManager->create_wormhole(Vector2(9000.0f, 7500.0f), m_tanks_list.size() * 1.5, w1);
			w1->SetNextWormhole(w2);*/

		}
	}
}

void CTurnManager::cullDeadWormholes() {
	bool deleted = false;
	for (auto& w : *m_pObjectManager->get_wormholes_list_pointer()) {
		if (w->DecrementLife() == 0) {
			deleted = true;
			delete w;
		}
	}

	if (deleted)
		m_pObjectManager->get_wormholes_list_pointer()->clear();

}

void CTurnManager::Reset() {
	m_iCurrentTurn = 0;
	m_iTotalTurns = 0;
}

bool CTurnManager::CheckGameOver() {
	if (aliveCount() == 1) {
		m_bGameOver = true;
		m_bControlLock = true;

		//if the last tank is a player, mark the current level as completed
		if (m_currentTank->get_is_player_character()) {
			string levelFileString = m_pLevelManager->getSelectedFolder() + "\\" + m_pLevelManager->get_level_name().substr(1) + ".txt";
			if (!m_pLevelManager->LevelCleared(levelFileString)) { //if the level has not yet been cleared
				//append "CLEARED" to the end of the level
				printf("epic\n");
				std::ofstream fout;

				fout.open("Levels\\" + levelFileString, std::ios::app);
				if (!fout) {
					printf("cannot open file %s\n", levelFileString.c_str());
				}
				fout << "\nCLEARED";
				fout.close();

			}
		}

		return true;
	}
	return false;
}