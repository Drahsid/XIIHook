#pragma once

#ifndef INPUT_H
#define INPUT_H

#include <Windows.h>
#include <ctime>

enum KeyState : uint8_t {
	Pressed = 0,
	Down,
	Up
};

struct Key {
	KeyState State = KeyState::Up;
	double invokeTime = 0;
};

struct InputManager {
	Key keyCode[254];
};

void PollInput(InputManager&IM) {
	for (int i = 0; i < 255; i++) {
		
		if (GetAsyncKeyState(i)) 
		{
			switch (IM.keyCode[i].State) {
			case Pressed:
				IM.keyCode[i].State = KeyState::Down;
				break;

			case Down:
				break;

			case Up:
				IM.keyCode[i].State = KeyState::Pressed;
				IM.keyCode[i].invokeTime = (double)clock();
				break;
			}
		}
		else 
		{
			IM.keyCode[i].State = KeyState::Up;
		}
	}
}

#endif



