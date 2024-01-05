#include "LevelManager.h"
#include "PlanetObject.h"
#include "ObjectManager.h"
#include "LevelEditor.h"
#include "TurnManager.h"
#include "Button.h"
#include <fstream>
#include <sstream>
#include <memory>
#include <Windows.h>


vector<XMFLOAT4> colors_vector = { XMFLOAT4(Colors::Red), XMFLOAT4(Colors::Blue), XMFLOAT4(Colors::White), XMFLOAT4(Colors::LimeGreen),XMFLOAT4(Colors::Gold), XMFLOAT4(Colors::Green), XMFLOAT4(Colors::Purple), XMFLOAT4(Colors::Orange) };

CLevelManager::CLevelManager() {} //Constructor is blank for now. Once we introduce file reading, this will be more important.
CLevelManager::~CLevelManager(){} //Destructor is blank for now. Once we introduce file reading, this will be more important.

void CLevelManager::LoadMap(string filename){

    string f = "Levels\\" + filename;
    std::ifstream infile(f);
    if (!infile) {
        printf("cannot open file %s\n", f.c_str());
        m_eGameState = GameState::TITLE_SCREEN;
        LoadMap(0);
        return;
    }

    int hPlayers = m_pTurnManager->getNumHumanPlayers();
    int tPlayers = m_pLevelManager->getCurrentTankCount();
    if (!m_bTurnsEnabled) {
        hPlayers = 1; //can only be 1 human player in blitz mode
        tPlayers = m_pLevelManager->getTankCount(filename);
    }
    bool playerSet = false;

    int totalHumans = hPlayers;
    int totalAI = tPlayers - totalHumans;
    int totalPlayers = tPlayers;

    int AICounted = 0;

    vector<CPlanetObject*> planets; //vector of planets to reference for the tanks



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

        }
        else if (line.substr(0, 4) == "TANK") { //line is tank info
            //create planets before tanks!
            //format: TANK <angle> <planet no.> <color> <player?>

            if (tPlayers > 0)
                tPlayers--;
            else
                break;

            float angle;
            int planetNo;
            string color;
            int isPlayer;
            string s; //placeholder for "TANK"

            std::istringstream iss(line); //create string stream
            if (!(iss >> s >> angle >> planetNo >> color >> isPlayer)) { break; }

            XMFLOAT4 pColor = stringToColor(color); //actual color type of player

            if (planetNo >= planets.size())
                planetNo = 0;

            //set is player based on number of human players
            if (hPlayers) {
                isPlayer = 1;
                hPlayers--;
            }
           
            if (isPlayer != 0) {
                if (!playerSet) {
                    m_pPlayer = m_pObjectManager->create_tank(angle, planets[planetNo], pColor);
                    m_pPlayer->set_is_player_character(true);
                    m_pPlayer->set_player_number(totalHumans - hPlayers);
                    playerSet = true;
                }
                else {
                    std::shared_ptr<CTankObject> t = m_pObjectManager->create_tank(angle, planets[planetNo], pColor);
                    t->set_is_player_character(true);
                    t->set_player_number(totalHumans - hPlayers);
                }
            }
            else {
                std::shared_ptr<CTankObject> t = m_pObjectManager->create_tank(angle, planets[planetNo], pColor);
                AICounted++;
                t->set_player_number(AICounted);
            }

        }
        else if (line.substr(0, 4) == "NAME") { //line is level name
          //Set the level name.
          //format: NAME <name>
          string s; //placeholder for "NAME"
          char name[256];
          std::istringstream iss(line); //create string stream
          if (!(iss >> s)) { break; }
          iss.getline(name, 256);
          set_level_name(string(name));
        }
    
    }

} //LoadMap (string overload)

void CLevelManager::LoadMap(int level_number) {
    //printf("loading map: %d (/%d)\n", level_number, filenames.size());

    //buttons (should be in every level except title screen)
    std::shared_ptr<CButton> b = std::make_shared<CButton>(BUTTON_CAMLOCK_SPRITE, Vector2(50.0f, 50.0f), m_pRenderer); //camera lock
    b->SetScale(0.75f, 0.75f);
    m_lButtonList.push_back(b);

    b = std::make_shared<CButton>(BUTTON_ZOOMIN_SPRITE, Vector2(110.0f, 50.0f), m_pRenderer); ///zoom in
    b->SetScale(1.75f, 1.75f);
    m_lButtonList.push_back(b);

    b = std::make_shared<CButton>(BUTTON_ZOOMOUT_SPRITE, Vector2(150.0f, 50.0f), m_pRenderer); //zoom out
    b->SetScale(1.75f, 1.75f);
    m_lButtonList.push_back(b);

   

    m_eCameraMode = CameraMode::PLAYER_LOCKED;
    m_pRenderer->set_scale_factor(0.3f);


    //normal level
    if (level_number > 0) {
        m_eGameState = GameState::PLAYING;
        if (level_number > filenames.size())
            LoadMap(selectedFolder + "\\" + filenames[0]);
        else
            LoadMap(selectedFolder + "\\" + filenames[level_number - 1]);

        b = std::make_shared<CButton>(BUTTON_BACK_SPRITE, Vector2((float)m_nWinWidth - 50.0f, 50.0f), m_pRenderer); //return to main menu
        b->SetCustomFunc("return_main_menu");
        b->SetScale(0.25f, 0.25f);
        m_lButtonList.push_back(b);

        if (song != SongPlaying::GAMEPLAY) {
            m_pAudio->stop();
            m_pAudio->loop(SCIFI_MUSIC);
            song = SongPlaying::GAMEPLAY;
        }

        return;
    }

  switch (level_number) {
    case 0: { ///title screen
        m_vWorldSize = Vector2((float)(m_nWinWidth), (float)(m_nWinHeight));
        Vector2 center = m_vWorldSize / 2;

        m_eCameraMode = CameraMode::FIXED;
        m_fixedCameraPos = center;
        m_pRenderer->set_scale_factor(1.0f);
        
        //m_eGameState = GameState::TITLE_SCREEN;

        //clear buttons that do not belong, create title screen buttons
        //for (auto const& b : m_lButtonList)
        //    delete b;
        m_lButtonList.clear();

        if (song != SongPlaying::MENU) {
            m_pAudio->stop();
            m_pAudio->loop(NEWDAWN_MUSIC);
            song = SongPlaying::MENU;
        }

        if (m_eGameState == GameState::TITLE_SCREEN) {
            b = std::make_shared<CButton>(BUTTON_STARTGAME_SPRITE, Vector2(center.x, center.y + 100.0f), m_pRenderer); //start button
            m_lButtonList.push_back(b);

            b = std::make_shared<CButton>(BUTTON_LEVELEDITOR_SPRITE, Vector2(center.x, center.y - 200.0f), m_pRenderer); //level editor button 
            m_lButtonList.push_back(b);

            b = std::make_shared<CButton>(BUTTON_INSTRUCTIONS_SPRITE, Vector2(m_vWorldSize.x - 50.0f, 50.0f), m_pRenderer);
            m_lButtonList.push_back(b);
        }
        else if (m_eGameState == GameState::MODE_SELECT) {
            b = std::make_shared<CButton>(BUTTON_CLASSICMODE_SPRITE, Vector2(center.x - 200.0f, center.y + 100.0f), m_pRenderer); //start button
            m_lButtonList.push_back(b);

            b = std::make_shared<CButton>(BUTTON_BLITZMODE_SPRITE, Vector2(center.x + 200.0f, center.y + 100.0f), m_pRenderer);
            m_lButtonList.push_back(b);

            std::shared_ptr<CButton> b = std::make_shared<CButton>(BUTTON_BACK_SPRITE, Vector2((float)m_nWinWidth / 2.0f, 70.0f), m_pRenderer); //return to main menu
            b->SetCustomFunc("return_main_menu");
            b->SetScale(0.4f, 0.4f);
            m_lButtonList.push_back(b);

        }
        else if (m_eGameState == GameState::LEVEL_SELECT) {
            vector<string> folders = getFolderNames();
            filenames = folders;
            for (int i = 0; i < folders.size(); i++) { //create a button for every folder
                int posX = (i % 2 == 0) ? center.x - 225 : center.x + 225;
                int posY = m_nWinHeight - (i / 2) * 150 - 150;
                b = std::make_shared<CButton>(BUTTON_LEVELNAME_SPRITE, Vector2(posX, posY), m_pRenderer); //start button
                b->SetFolderName(folders[i]);
                m_lButtonList.push_back(b);

                //create 1 more button: randomized level
                if (i == folders.size() - 1) {
                    i += 1;
                    int posX = (i % 2 == 0) ? center.x - 225 : center.x + 225;
                    int posY = m_nWinHeight - (i / 2) * 150 - 150;
                    b = std::make_shared<CButton>(BUTTON_LEVELNAME_SPRITE, Vector2(posX, posY), m_pRenderer); //start button
                    b->SetLevelNumber(-999);
                    b->SetCustomFunc("play_level");
                    m_lButtonList.push_back(b);
                }
            }

            std::shared_ptr<CButton> b = std::make_shared<CButton>(BUTTON_BACK_SPRITE, Vector2((float)m_nWinWidth / 2.0f, 70.0f), m_pRenderer); //return to main menu
            b->SetCustomFunc("return_main_menu");
            b->SetScale(0.4f, 0.4f);
            m_lButtonList.push_back(b);
            
        }
        else if (m_eGameState == GameState::PLAYER_SELECT) {
 
            //select the amount of human players
            tankCount = getTankCount(getSelectedFolder() + "\\" + getFilenames()[getSelectedLevel() - 1]);

            //increase human player count
            b = std::make_shared<CButton>(BUTTON_UP_SPRITE, Vector2(center.x - 160.0f, center.y + 90.0f), m_pRenderer);
            b->SetCustomFunc("tanks_up");
            m_lButtonList.push_back(b);

            //decrease human player count
            b = std::make_shared<CButton>(BUTTON_DOWN_SPRITE, Vector2(center.x - 160.0f, center.y + 15.0f), m_pRenderer);
            b->SetCustomFunc("tanks_down");
            m_lButtonList.push_back(b);

            //increase human player count
            b = std::make_shared<CButton>(BUTTON_UP_SPRITE, Vector2(center.x - 160.0f, center.y - 75.0f), m_pRenderer);
            b->SetCustomFunc("humans_up");
            m_lButtonList.push_back(b);

            //decrease human player count
            b = std::make_shared<CButton>(BUTTON_DOWN_SPRITE, Vector2(center.x - 160.0f, center.y - 150.0f), m_pRenderer);
            b->SetCustomFunc("humans_down");
            m_lButtonList.push_back(b);

            //start the game
            b = std::make_shared<CButton>(BUTTON_STARTGAME_SPRITE, Vector2((float)m_nWinWidth / 2.0f + 100.0f, 70.0f), m_pRenderer);
            b->SetCustomFunc("play_level_classic");
            b->SetScale(0.5f, 0.5f);
            m_lButtonList.push_back(b);
            
            //go back
            std::shared_ptr<CButton> b = std::make_shared<CButton>(BUTTON_BACK_SPRITE, Vector2((float)m_nWinWidth / 2.0f - 100.0f, 70.0f), m_pRenderer); //return to main menu
            b->SetCustomFunc("return_main_menu");
            b->SetScale(0.5f, 0.5f);
            m_lButtonList.push_back(b);
        }
        else if (m_eGameState == GameState::INSTRUCTIONS) {

            //next instructions page
            b = std::make_shared<CButton>(BUTTON_UP_SPRITE, Vector2((float)m_nWinWidth / 2.0f + 200.0f, 70.0f), m_pRenderer);
            b->SetCustomFunc("page_up");
            b->SetScale(2.0f, 2.0f);
            m_lButtonList.push_back(b);

            //next instructions page
            b = std::make_shared<CButton>(BUTTON_DOWN_SPRITE, Vector2((float)m_nWinWidth / 2.0f - 200.0f, 70.0f), m_pRenderer);
            b->SetCustomFunc("page_down");
            b->SetScale(2.0f, 2.0f);
            m_lButtonList.push_back(b);

            std::shared_ptr<CButton> b = std::make_shared<CButton>(BUTTON_BACK_SPRITE, Vector2((float)m_nWinWidth / 2.0f, 70.0f), m_pRenderer); //return to main menu
            b->SetCustomFunc("return_main_menu");
            b->SetScale(0.4f, 0.4f);
            m_lButtonList.push_back(b);
        }

      }
      break;
    case 1: {
      m_eGameState = GameState::PLAYING;
      set_level_name(to_string(level_number));
      m_vWorldSize = Vector2((float)(15000), (float)(15000));
      Vector2 center = m_vWorldSize / 2;

      m_pRenderer->set_scale_factor(0.3f);

      //Test turret
      //CObject* turret = m_pObjectManager->create(TURRET_SPRITE, center + Vector2(0.0f, 1000.0f), 0, TRUE);
      //turret->SetVelocity(Vector2(-100.0f, 0));

      //Planets
      CPlanetObject* planet = m_pObjectManager->create_planet(center, 100, 900);
      CPlanetObject* planet2 = m_pObjectManager->create_planet(center + Vector2(1500.0f, 1500.0f), 10, 500);


      //Players
      m_pPlayer = m_pObjectManager->create_tank(90.0f, planet, XMFLOAT4(Colors::Red));
      m_pPlayer->set_is_player_character(true); //We want to make Red a player character
      m_pObjectManager->create_tank(270.0f, planet2, XMFLOAT4(Colors::Blue));
      m_pObjectManager->create_tank(180.0f, planet2, XMFLOAT4(Colors::White));
      m_pObjectManager->create_tank(90.0f, planet2, XMFLOAT4(Colors::LimeGreen));
      m_pObjectManager->create_tank(0.0f, planet2, XMFLOAT4(Colors::Gold));
      m_pObjectManager->create_tank(180.0f, planet, XMFLOAT4(Colors::Green));
      m_pObjectManager->create_tank(270.0f, planet, XMFLOAT4(Colors::Purple));
      m_pObjectManager->create_tank(0.0f, planet, XMFLOAT4(Colors::Orange));
    } 
        break;
    case 2:{
      m_eGameState = GameState::PLAYING;
      set_level_name(to_string(level_number));
      m_vWorldSize = Vector2(15000, 15000);
      Vector2 center = m_vWorldSize / 2;

      //Test turret
      //CObject* turret = m_pObjectManager->create(TURRET_SPRITE, center + Vector2(0.0f, 1000.0f), 0, TRUE);
      //turret->SetVelocity(Vector2(-100.0f, 0));

      //Planets
      CPlanetObject* planet = m_pObjectManager->create_planet(center, 100, 700);
      CPlanetObject* planet2 = m_pObjectManager->create_planet(center + Vector2(1200.0f, 1200.0f), 100, 700);
      CPlanetObject* planet3 = m_pObjectManager->create_planet(center + Vector2(-1200.0f, 1200.0f), 100, 700);


      //Players
      m_pPlayer = m_pObjectManager->create_tank(90.0f, planet, XMFLOAT4(Colors::Red));
      m_pPlayer->set_is_player_character(true); //We want to make Red a player character
      m_pObjectManager->create_tank(270.0f, planet2, XMFLOAT4(Colors::Blue));
      m_pObjectManager->create_tank(180.0f, planet3, XMFLOAT4(Colors::White));
      m_pObjectManager->create_tank(90.0f, planet2, XMFLOAT4(Colors::LimeGreen));
      m_pObjectManager->create_tank(0.0f, planet2, XMFLOAT4(Colors::Gold));
      m_pObjectManager->create_tank(180.0f, planet, XMFLOAT4(Colors::Green));
      m_pObjectManager->create_tank(270.0f, planet, XMFLOAT4(Colors::Purple));
      m_pObjectManager->create_tank(0.0f, planet3, XMFLOAT4(Colors::Orange));
    }
        break;
    case 3:
      m_eGameState = GameState::PLAYING;
      set_level_name(to_string(level_number));
      LoadMap("exampleLevel.txt");
      break;
    case -1: { //level editor

        m_vWorldSize = Vector2(25000.0f, 25000.0f);
        Vector2 center = m_vWorldSize / 2;

        m_eCameraMode = CameraMode::PLAYER_UNLOCKED;
        m_unlockedCameraPos = center;
        m_fixedCameraPos = center;
        m_pRenderer->set_scale_factor(0.3f);

        m_eGameState = GameState::LEVEL_EDITOR;

        //clear buttons that do not belong
        //for (auto const& b : m_lButtonList)
        //    delete b;
        //m_lButtonList.clear();

        m_pLevelEditor->SetEditMode(EditMode::TANK_CREATE);
        m_pLevelEditor->SetEditMode(EditMode::PLANET_CREATE);

        //load level if returning from testing   
        if (m_pLevelEditor->GetTesting()) {
            m_pLevelEditor->LoadLevel("user-level-test.txt");
            m_pLevelEditor->SetTesting(false);
        }

        b = std::make_shared<CButton>(BUTTON_ZOOMIN_SPRITE, Vector2(110.0f, 50.0f), m_pRenderer); ///zoom in
        b->SetScale(1.75f, 1.75f);
        m_lButtonList.push_back(b);

        b = std::make_shared<CButton>(BUTTON_ZOOMOUT_SPRITE, Vector2(150.0f, 50.0f), m_pRenderer); //zoom out
        b->SetScale(1.75f, 1.75f);
        m_lButtonList.push_back(b);

        b = std::make_shared<CButton>(BUTTON_CREATEPLANET_SPRITE, Vector2(50.0f, (float)m_nWinHeight - 50.0f), m_pRenderer); //add planet
        m_lButtonList.push_back(b);

        b = std::make_shared<CButton>(BUTTON_CREATETANK_SPRITE, Vector2(150.0f, (float)m_nWinHeight - 50.0f), m_pRenderer); //add tank
        m_lButtonList.push_back(b);

        b = std::make_shared<CButton>(BUTTON_SAVELEVEL_SPRITE, Vector2((float)m_nWinWidth - 150.0f, (float)m_nWinHeight - 50.0f), m_pRenderer); //save level
        m_lButtonList.push_back(b);

        b = std::make_shared<CButton>(BUTTON_LOADLEVEL_SPRITE, Vector2((float)m_nWinWidth - 50.0f, (float)m_nWinHeight - 50.0f), m_pRenderer); //load level
        m_lButtonList.push_back(b);

        b = std::make_shared<CButton>(BUTTON_STARTGAME_SPRITE, Vector2((float)m_nWinWidth - 50.0f, 50.0f), m_pRenderer); //test level
        b->SetCustomFunc("start_level_test");
        b->SetScale(0.3f, 0.3f);
        m_lButtonList.push_back(b);

        b = std::make_shared<CButton>(BUTTON_BACK_SPRITE, Vector2((float)m_nWinWidth - 50.0f, 150.0f), m_pRenderer); //return to main menu
        b->SetCustomFunc("return_main_menu");
        b->SetScale(0.3f, 0.3f);
        m_lButtonList.push_back(b);

        b = std::make_shared<CButton>(BUTTON_UP_SPRITE, Vector2(50.0f, 150.0f), m_pRenderer); //increase world size
        b->SetCustomFunc("world_size_up");
        m_lButtonList.push_back(b);

        b = std::make_shared<CButton>(BUTTON_DOWN_SPRITE, Vector2(50.0f, 120.0f), m_pRenderer); //decrease world size
        b->SetCustomFunc("world_size_down");
        m_lButtonList.push_back(b);

        }
        break;
    case -2: { //user test level

        m_eGameState = GameState::PLAYING;

        b = std::make_shared<CButton>(BUTTON_BACK_SPRITE, Vector2((float)m_nWinWidth - 50.0f, 50.0f), m_pRenderer); //go back to editor
        b->SetScale(0.5f, 0.5f);
        m_lButtonList.push_back(b);

        m_pRenderer->set_scale_factor(0.3f);
        m_eCameraMode = CameraMode::PLAYER_LOCKED;
        LoadMap("Your Levels\\user-level-test.txt");
        set_level_name("Custom Level");
    }
        break;
    default: {
      m_eGameState = GameState::PLAYING;
      //Default to randomly generated map.
      set_level_name("Randomly Generated Level");
      int num_planets = m_pRandom->randn(1, 5); //Between 1 and 5 planets
      int num_tanks = m_pRandom->randn(max(3, num_planets), 8); //Between 3 and 8 tanks. If there are more than 3 planets, there should be more than 3 tanks.
      float world_radius = (float)m_pRandom->randn(15000, 30000); // The "radius" of the world. i.e. distance from center to an edge.
      m_vWorldSize = world_radius * Vector2(1.f, 1.f); // Make the world size a large square
      Vector2 center = m_vWorldSize / 2;
      Vector2 location;
      float mass;
      int radius;
      float angle;
      CPlanetObject* planet_pointer;
      XMFLOAT4 color;
      BoundingSphere bs;
      bool intersects = false;

      //back button
      b = std::make_shared<CButton>(BUTTON_BACK_SPRITE, Vector2((float)m_nWinWidth - 50.0f, 50.0f), m_pRenderer); //return to main menu
      b->SetCustomFunc("return_main_menu");
      b->SetScale(0.25f, 0.25f);
      m_lButtonList.push_back(b);

      if (song != SongPlaying::GAMEPLAY) {
          m_pAudio->stop();
          m_pAudio->loop(SCIFI_MUSIC);
          song = SongPlaying::GAMEPLAY;
      }

      //Create Random Planets
      vector<CPlanetObject*> planets;
      for (int i = 0; i < num_planets; i++) {
        //Find planet parameters
        do {
          intersects = false; //Trying new coordinates. We don't know if they intersect or not
          location = center + m_pRandom->randn(world_radius/12, world_radius / 8) * m_pRandom->randv(); //We want planets clustered near the center. So start at center, choose a random angle, go at most 1/4 of the worldsize away.
          radius = m_pRandom->randn(500, 1500);
          bs.Center = Vector3(location.x, location.y, 0);
          bs.Radius = radius;
          //Make sure that this does not intersect an already existing planet.
          for (auto planet : planets) {
            if (planet->Intersects(bs)) {
              intersects = true;
              break;
            }
          }
        } while (intersects);

        mass = powf((float)radius, 3.f) * 100.f / powf(900.f, 3.f); // Mass is proportional to radius cubed. This constant is so that a radius 900 planet has a mass of 100. In play testing, this is a good size.
        planets.push_back(m_pObjectManager->create_planet(location, (double)mass, radius));
      }

      //Players
      vector<std::shared_ptr<CTankObject>> players;
      for (int i = 0; i < num_tanks; i++) {
        angle = (float)m_pRandom->randn(0, 359); //Choose a random angle
        planet_pointer = planets[m_pRandom->randn(0, num_planets-1)]; // Choose a random planet
        color = colors_vector[m_pRandom->randn(0, colors_vector.size()-1)]; //Choose a random color
        players.push_back( m_pObjectManager->create_tank(angle, planet_pointer, color)); // Create the player
        //Set the first tank to be a human player
        if (i == 0) {
          m_pPlayer = players[i];
          m_pPlayer->set_is_player_character(true);
        }
      }
    }
        break;
  }
}

void CLevelManager::createLevelButtons() {

    //delete all buttons
    //for (auto const& b : m_lButtonList)
    //    delete b;
    m_lButtonList.clear();

    Vector2 center = m_vWorldSize / 2;
    vector<string> lvNames = getFilenames();
    for (int i = 0; i < lvNames.size(); i++) { //create a button for every folder
        int posX = (i % 2 == 0) ? center.x - 225 : center.x + 225;
        int posY = m_nWinHeight - (i / 2) * 150 - 150;
        std::shared_ptr<CButton> b = std::make_shared<CButton>(BUTTON_LEVELNAME_SPRITE, Vector2(posX, posY), m_pRenderer); //start button
        b->SetLevelNumber(i + 1);
        b->SetCustomFunc("play_level");
        m_lButtonList.push_back(b);
    }

    std::shared_ptr<CButton> b = std::make_shared<CButton>(BUTTON_BACK_SPRITE, Vector2((float)m_nWinWidth / 2.0f, 150.0f), m_pRenderer); //return to main menu
    b->SetCustomFunc("return_main_menu");
    b->SetScale(0.5f, 0.5f);
    m_lButtonList.push_back(b);
    
}

XMFLOAT4 CLevelManager::stringToColor(string color) {
    if (color == "blue")
        return XMFLOAT4(Colors::Blue);
    else if (color == "white")
        return XMFLOAT4(Colors::White);
    else if (color == "lime")
        return XMFLOAT4(Colors::LimeGreen);
    else if (color == "gold")
        return XMFLOAT4(Colors::Gold);
    else if (color == "green")
        return XMFLOAT4(Colors::Green);
    else if (color == "purple")
        return XMFLOAT4(Colors::Purple);
    else if (color == "orange")
        return XMFLOAT4(Colors::Orange);
    else if (color == "brown")
        return XMFLOAT4(Colors::Brown);
    else if (color == "red")
        return XMFLOAT4(Colors::Red);
    else if (color == "yellow")
        return XMFLOAT4(Colors::Yellow);
    else if (color == "black")
        return XMFLOAT4(Colors::Black);
    else if (color == "pink")
        return XMFLOAT4(Colors::Pink);
    else if (color == "turquoise")
        return XMFLOAT4(Colors::Turquoise);
    else if (color == "violet")
        return XMFLOAT4(Colors::Violet);
    else
        return XMFLOAT4(Colors::White);
}

vector<string> CLevelManager::getLevelFilenames(string folder) {
    vector<string> names;

    std::string pattern = ".\\Levels\\" + folder;
    pattern.append("\\*");
    WIN32_FIND_DATA data;
    HANDLE hFind;
    if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
        do {
            names.push_back(data.cFileName);
        } while (FindNextFile(hFind, &data) != 0);
        FindClose(hFind);
    }

    names.erase(names.begin()); 
    names.erase(names.begin());

    return names;
}

vector<string> CLevelManager::getFolderNames() {
    
    vector<string> names = getLevelFilenames("");
    for (int i = 0; i < names.size(); i++) {
        if (names[i].size() < 5) {
            names.erase(names.begin() + i);
            i = 0;
        }
        else if (names[i].substr(names[i].length() - 4) == ".txt") {
            names.erase(names.begin() + i);
            i = 0;
        }
    }

    //names.erase(names.begin());
    return names;

}

bool CLevelManager::LevelCleared(string filename) {
    string f = "Levels\\" + filename;
    std::ifstream infile(f);
    if (!infile) {
        printf("cannot open file %s\n", f.c_str());
        return false;
    }


    string line;
    while (std::getline(infile, line)) {
        if (line.substr(0, 7) == "CLEARED")
            return true;
    }

    infile.close();

    return false;
}//LevelCleared

int CLevelManager::getTankCount(string filename) {
    int count = 0;
    string f = "Levels\\" + filename;
    std::ifstream infile(f);
    if (!infile) {
        printf("cannot open file %s\n", f.c_str());
        return count;
    }


    string line;
    while (std::getline(infile, line)) {
        if (line.substr(0, 4) == "TANK")
            count++;
    }

    infile.close();

    return count;
}