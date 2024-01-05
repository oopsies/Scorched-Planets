#pragma once

#include <vector>
#include <string>

#include "Common.h"
#include "ComponentIncludes.h"

class CLevelManager : public CComponent, public CCommon, public CSettings {
private:
  int number_of_levels = 5; // The number of levels currently implemented
  string current_level_name = "";

  string selectedFolder; // Selected folder to view levels in
  vector<string> filenames; //filenames to display
  int selectedLevel; //selected level in the list of filenames
  int tankCount = 2; //maximum amount of tanks for the selected level

  enum class SongPlaying { //there's probably a better way to do this, but this is fast lol
	  DEFAULT,
	  MENU,
	  GAMEPLAY
  };

  SongPlaying song;


public:
  CLevelManager();
  ~CLevelManager();

  void LoadMap(string filename); //Load a map from filename.
  void LoadMap(int level_number); //Load a map corresponding to a specified number

  int get_number_of_levels() { return number_of_levels; };
  string get_level_name() { return current_level_name; };
  void set_level_name(string name) { current_level_name = name; };

  vector<string> getFilenames() { return filenames; };
  void setFilenames(vector<string> fnames) { filenames = fnames; };

  void createLevelButtons(); ///< Creates buttons for selecting a level

  XMFLOAT4 stringToColor(string color); //converts a string color to a XMFLOAT4 color

  vector<string> getLevelFilenames(string folder); ///< Retuns an array containing the filenames of all levels
  vector<string> getFolderNames(); ///< Returns an array of folders in the level folder
  void setSelectedFolder(string n) { selectedFolder = n; };
  string getSelectedFolder() { return selectedFolder; };
  int getSelectedLevel() { return selectedLevel; };
  void setSelectedLevel(int n) { selectedLevel = n; };
  int getCurrentTankCount() { return tankCount; };
  void setCurrentTankCount(int n) { tankCount = n; };

  bool LevelCleared(string filename); ///< Returns true if the player has cleared this level.

  int getTankCount(string filename); ///< Returns the number of tanks in a level file.

};

