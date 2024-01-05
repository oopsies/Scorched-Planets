/// \file Mouse.cpp
/// \brief Code for the mouse handler class CMouse.

#include "Mouse.h"

///The constructor starts the initial mouse positions at 0.

CMouse::CMouse(HWND handle, Vector2 winSize, CRenderer* renderer) {
	oldX = 0; //start at 0
	oldY = 0;
	currentX = 0;
	currentY = 0;
	this->handle = handle;
	this->renderer = renderer;
	winW = static_cast<int>(winSize.x);
	winH = static_cast<int>(winSize.y);
	inactive = is_window_active();
	CKeyboard();
}

void CMouse::GetState() {
	CKeyboard::GetState();
	GetWindowRect(handle, &window_corners);
	winW = static_cast<int>(window_corners.right - window_corners.left);
	winH = static_cast<int>(window_corners.bottom - window_corners.top);
	UpdatePos();
	if (!is_window_active())
		inactive = true;
	/*if (inactive)
		printf("inactive\n");
	else
		printf("active\n");*/
}

///Buttons down/up
///Returns true when a button is down or up
///depending on the function

bool CMouse::LeftMouseDown() {
	return Down(VK_LBUTTON) && is_window_active() && !inactive;
}
bool CMouse::LeftMouseUp() {
	return !Down(VK_LBUTTON) && is_window_active() && !inactive;
}
bool CMouse::RightMouseDown() {
	return Down(VK_RBUTTON) && is_window_active() && !inactive;
}
bool CMouse::RightMouseUp() {
	return !Down(VK_RBUTTON) && is_window_active() && !inactive;
}

///Buttons pressed released
///Returns true once when a mouse button has changed state

bool CMouse::LeftMousePressed() {
	if (is_window_active() && inactive && TriggerDown(VK_LBUTTON)) {
		//printf("inactive\n");
		return false;
	}
	return TriggerDown(VK_LBUTTON) && is_window_active();
}
bool CMouse::LeftMouseReleased() {
	if (is_window_active() && inactive && TriggerUp(VK_LBUTTON)) {
		inactive = false;
		return false;
	}
	return TriggerUp(VK_LBUTTON) && is_window_active();
}
bool CMouse::RightMousePressed() {
	if (is_window_active() && inactive && TriggerDown(VK_RBUTTON)) {
		//printf("inactive\n");
		return false;
	}
	return TriggerDown(VK_RBUTTON) && is_window_active();
}
bool CMouse::RightMouseReleased() {
	if (is_window_active() && inactive && TriggerUp(VK_RBUTTON)) {
		//printf("inactive\n");
		inactive = false;
		return false;
	}
	return TriggerUp(VK_RBUTTON) && is_window_active();
}

//not working becuase i can't modify the engine :((
bool CMouse::ScrollUp() {
	return false;
}
bool CMouse::ScrollDown() {
	return false;
}

///Mouse position functions
///Returns the position of the mouse when called

void CMouse::UpdatePos() {
	oldX = currentX;
	oldY = currentY;
	POINT mousePos;
	GetCursorPos(&mousePos); //get cursor position for screen
	ScreenToClient(handle, &mousePos);
	currentX = mousePos.x;
	currentY = mousePos.y;
}

Vector2 CMouse::GetPos() {
	return Vector2((float)currentX, (float)currentY);
}

///Returns the mouse position of where it would be in the world as opposed to the window
///\return Position of the mouse in world coords
Vector2 CMouse::GetPosWorld() {
	/*RECT r;
	GetWindowRect(handle, &r);*/
	Vector2 m = GetPos();
	m = Vector2(m.x - winW / 2, m.y - winH / 2);
	//printf("Mouse (screen): (%f, %f) Mouse (world): (%f, %f) Camera (world): (%f, %f)\n", GetPos().x, GetPos().y, renderer->GetCameraPos().x + m.x, renderer->GetCameraPos().y + m.y, renderer->GetCameraPos().x, renderer->GetCameraPos().y);
	return renderer->GetCameraPos() + Vector2(m.x, -m.y);
}

int CMouse::GetX() {
	return (int)GetPos().x;
}
int CMouse::GetY() {
	return (int)GetPos().y;
}

//returns the distance in mouse position between the last frame and this fram
Vector2 CMouse::GetDistance() {
	return Vector2((float)currentX, (float)currentY) - Vector2((float)oldX, (float)oldY);;
}

//Returns the character that is pressed as a string.
char CMouse::GetCharacterPressed() {

	char characters[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
	const int charArraySize = 26;

	for (int i = 0; i < charArraySize; i++) {
		if (Down(VK_SHIFT) && TriggerDown(characters[i]))
			return characters[i];
		else if (!Down(VK_SHIFT) && TriggerDown(characters[i]))
			return tolower(characters[i]);
	}
	return '.';
	
}

bool CMouse::is_window_active() {
	return (GetActiveWindow() == handle);
}