#pragma once

#ifndef INPUT_H
#define INPUT_H

enum KeyState : uint8_t {
	Pressed = 0,
	Down,
	Up
};

struct Key {
	KeyState state = KeyState::Up;
	double invokeTime = 0;
};

struct InputManager {
	Key keyCode[254];
};

void PollInput(InputManager&IM) {
	for (int i = 0; i < 255; i++) {
		
		if (GetAsyncKeyState(i)) 
		{
			switch (IM.keyCode[i].state) {
			case Pressed:
				IM.keyCode[i].state = KeyState::Down;
				break;

			case Down:
				break;

			case Up:
				IM.keyCode[i].state = KeyState::Pressed;
				IM.keyCode[i].invokeTime = (double)clock();
				break;
			}
		}
		else 
		{
			IM.keyCode[i].state = KeyState::Up;
		}
	}
}

#endif



