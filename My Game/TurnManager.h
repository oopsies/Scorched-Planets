#pragma once
/// \file TurnManager.h
/// \brief Interface for the turn manager, which will control the flow of the game.

#pragma once

#include <list>

#include "Component.h"
#include "Common.h"
#include "Settings.h"

#include "Object.h"
#include "PlanetObject.h"
#include "TankObject.h"
#include "BulletObject.h"

using namespace std;

/// \brief The turn manager.
///
/// Will control the flow of the game and will determine which objects should move for input

class CTurnManager : CCommon {

private:
	int m_iNumPlayers; //< Total number of controllable tanks in the game
	int m_iCurrentTurn; //< The current turn (# tank that input is controlling)
	std::shared_ptr<CTankObject> m_currentTank; //< Current tank that the input is controlled by
	list<std::shared_ptr<CTankObject>> m_tanks_list; //< List of tanks to control

	int m_iTotalTurns; //< Total turns taken

	int m_iHumanPlayers; ///< Number of human players

public:
	CTurnManager(); //< Constructor
	CTurnManager(list<std::shared_ptr<CTankObject>> tankList);

	void SetTankList(list<std::shared_ptr<CTankObject>> tankList); //< Sets tank list if it was not set during constructor

	std::shared_ptr<CTankObject> NextTurn(); //< Gives turn to the next tank, wraps around to 0
	std::shared_ptr<CTankObject> SetTurn(int tankNo); //< Sets the turn to tankNo instead of incrementing by 1
	std::shared_ptr<CTankObject> GetNextTank(); //< Returns the tank who will be controlled next turn

	std::shared_ptr<CTankObject> GetTurnTank() { return m_currentTank; }; //< Returns the tank who is controlled by input
	int getTurnNo() { return m_iCurrentTurn; }; //< Returns the number of the tank being controlled
	int getRoundNo() { return m_iTotalTurns / m_iNumPlayers; }; //< Returns the round number (total_turns / num_players)
	int getNumPlayers() { return m_iNumPlayers; };

	int getNumHumanPlayers() { return m_iHumanPlayers; };
	void setNumHumanPlayers(int n) { m_iHumanPlayers = n; };

	int aliveCount(); //< Returns the amount of tanks that are alive

	void createWormholes(); ///< Has a chance of creating two linked wormholes, if there are no wormholes present
	void cullDeadWormholes(); ///< Destroys any wormholes who's turns to live count is 0

	void Reset(); //< Resets the object to the state it was created as

	bool CheckGameOver(); //< Checks if there is only one tank left. (for realtime mode)

}; //CTurnManager