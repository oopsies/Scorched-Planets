/// \file SmoothCamera.cpp
/// \brief Implementation for the CSmoothCamera class.

#include "SmoothCamera.h"
#include "Component.h"

CSmoothCamera::CSmoothCamera(CStepTimer* t, CRenderer* r) {
	timer = t;
	renderer = r;
}

void CSmoothCamera::Setup(float time, Vector2 endPos) {
	startingTime = timer->GetTotalSeconds();
	totalTime = time; //duration
	startPos = Vector2(renderer->GetCameraPos() / renderer->get_scale_factor());
	this->endPos =  endPos; 
}

void CSmoothCamera::MoveCamera(EaseFunctionType easeFunc) {

	float t = timer->GetTotalSeconds() - startingTime;
	Vector2 c = endPos - startPos;
	Vector2 p; //new position of camera
	//printf("%f, %f     %f, %f     %f, %f\n", startPos.x, startPos.y, endPos.x, endPos.y, c.x, c.y);

	switch (easeFunc) {
	case EaseFunctionType::EASE_OUT_CIRC:
		p = circle_ease_out(t, startPos, c, totalTime);
		//printf("%f, %f -> %f, %f - now at %f, %f\n", startPos.x, startPos.y, endPos.x, endPos.y, p.x, p.y);
		renderer->SetCameraPos(Vector3(p.x, p.y, 0.0));
		break;
	case EaseFunctionType::EASE_IN_OUT_CIRC:
		p = circle_ease_in_out(t, startPos, c, totalTime);
		renderer->SetCameraPos(Vector3(p.x, p.y, 0.0));
		break;
	case EaseFunctionType::EASE_OUT_BACK:
		break;
	case EaseFunctionType::EASE_IN_OUT_BACK:
		break;
	}
}

/// <summary>
/// Returns the position of the camera given starting and ending parameters.
/// Simulates "smooth" camera movement.
///retrieved from http://gizma.com/easing/
///QUICK MAFS
/// </summary>
/// <param name="currentTime">The current time out of the total time the camera will move</param>
/// <param name="b">The starting position</param>
/// <param name="c">The change in position</param>
/// <param name="d">The total duration the camera will move</param>
/// <returns>The position of where the camera should be</returns>

Vector2 CSmoothCamera::circle_ease_out(float currentTime, Vector2 b, Vector2 c, float d) {
	float t = currentTime;
	t /= d;
	t--;
	return Vector2(c.x * sqrtf(1 - t * t) + b.x, c.y * sqrtf(1 - t * t) + b.y);
}

Vector2 CSmoothCamera::circle_ease_in(float currentTime, Vector2 b, Vector2 c, float d) {
	float t = currentTime;
	t /= d;
	return Vector2(-c.x * (sqrtf(1 - t * t) - 1) + b.x, -c.y * (sqrtf(1 - t * t) - 1) + b.y);
}

Vector2 CSmoothCamera::circle_ease_in_out(float currentTime, Vector2 b, Vector2 c, float d) {
	float t = currentTime;
	t /= d / 2;
	if (t < 1)
		return Vector2(-c.x / 2 * (sqrtf(1 - t * t) - 1) + b.x, -c.y / 2 * (sqrtf(1 - t * t) - 1) + b.y);
	t -= 2;
	return Vector2(c.x / 2 * (sqrtf(1 - t * t) + 1) + b.x, c.y / 2 * (sqrtf(1 - t * t) + 1) + b.y);
}