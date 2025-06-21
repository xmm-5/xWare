#include "recoil.h"

void MoveMouse(int recoil)
{
	recoil = recoil * 2.5;

	mouse_event(MOUSEEVENTF_MOVE, 0, recoil, 0, 0);
}