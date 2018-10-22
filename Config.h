#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

inline double stod(const std::string&s)
{
	std::istringstream i(s);
	double x;
	if (!(i >> x))
		return 0;
	return x;
}

inline bool stob(std::string&s)
{
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);
	return (s == "1" || s == "on" || s == "true") || false;
}

struct UserConfig {
	double requestedMinFrameTime = 0;
	double requestedMinFrameTimeMenus = (double)1LL / (double)60LL;
	double requestedMinFrameTimeNoFocus = (double)1LL / (double)24LL;
	double mainThreadUpdateCoef = 2;
	float fov = 83;
	float gamma = 45;
	float igmState0Override = 1;
	float igmState1Override = 2;
	float igmState2Override = 4;
	float lockedMouseMulti = 1;
	bool bEnableFullSpeedMode = false;
	bool bEnableAdaptiveIGM = false;
	bool bEnableAdaptiveMouse = true;
	bool bEnableLockedMouseMulti = true;
	bool bEnableTransitionSpeedRamp = true;
};

namespace Config {
	void UpdateUserConfig(UserConfig&uConfig)
	{
		std::fstream config;
		config.open("fpsconfig.ini", std::fstream::in | std::fstream::out);
		if (!config)
		{
			std::cout << "Config file not found! Writing a new one...\n\n";
			config.open("fpsconfig.ini", std::fstream::trunc | std::fstream::out);
			config.close();
			config.open("fpsconfig.ini", std::fstream::in | std::fstream::out);

			config << "#In game frametime target. [Reccomended: A limit that your PC can hit]\n"
				<< "requestedMinFrameTime=90\n\n"
				<< "#Don't want superfast menus? [Reccomended: Keep at 60]\n"
				<< "requestedMinFrameTimeMenus=60\n\n"
				<< "#The frametime target when you are not tabbed into the game.\n"
				<< "requestedMinFrameTimeNoFocus=24\n\n"
				<< "#The in-game field of view. [Reccomended: User Preference]\n"
				<< "Fov=83\n\n"
				<< "#The in game gamma (brightness). [Reccomended: User Preference]\n"
				<< "gamma=45\n\n"
				<< "#How many times per frame should the main loop run? Raising this will increase the CPU usage of the program. If you're experiencing performance problems, consider lowering this.\n"
				<< "#I suggest not going below one, because that will make the program much less accurate. 0 will crash the program. [Reccomended: User Preference]\n"
				<< "mainThreadUpdateCoef=2\n\n"
				<< "#IGMState overrides change the base multiplier for each state. Default: 0=1, 1=2, 2=4 [Reccomended: User Preference]\n"
				<< "igmState0Override=1\n"
				<< "igmState1Override=2\n"
				<< "igmState2Override=4\n\n"
				<< "#This works in conjunction with bEnableLockedMouseMulti and bEnableAdaptiveMouse. At least one of them must be enabled for this to be used. [Reccomended: User Preference]\n"
				<< "lockedMouseMulti=1\n\n"
				<< "#If you want to completely disable the speed limiter, enable this. Might have some bad side effects. Setting the minimum frametime to 0 has the same effect, but is theoretically safer. [Reccomended: keep off]\n"
				<< "bEnableFullSpeedMode=false\n\n"
				<< "#If your in game animations are running at an incorrect speed, enabling this option may help. [Reccomended: keep on]\n"
				<< "bEnableAdaptiveIGM=true\n\n"
				<< "#If you like the game's default sensitivity scaling, disable this. Enables my relative mouse speed algorithm. [Reccomended: User Preference]\n"
				<< "bEnableAdaptiveMouse=true\n\n"
				<< "#Enabling this will disable the in game mouse sensitivity controller, and lock the relative sensitivity to a desired variable. [Reccomended: User Preference]\n"
				<< "bEnableLockedMouseMulti=true\n\n"
				<< "#This attempts to fix the slowdowns that happen when changing scenes and entering/exiting some menus. Not perfect. [Reccomended: keep on]\n"
				<< "bEnableTransitionSpeedRamp=true\n\n";

			config.close();
		}
		else
		{
			std::string refLine;
			std::string inLine;
			while (std::getline(config, inLine))
			{
				if (inLine.find("#") == inLine.npos)
				{
					refLine = inLine.substr(0, inLine.find("=") + 1);

					if (refLine == "requestedMinFrameTime=")
					{
						std::string val = inLine.substr(inLine.find("=") + 1);
						uConfig.requestedMinFrameTime = stod(val.c_str());
					}
					else if (refLine == "requestedMinFrameTimeMenus=")
					{
						std::string val = inLine.substr(inLine.find("=") + 1);
						uConfig.requestedMinFrameTimeMenus = stod(val.c_str());
					}
					else if (refLine == "requestedMinFrameTimeNoFocus=")
					{
						std::string val = inLine.substr(inLine.find("=") + 1);
						uConfig.requestedMinFrameTimeNoFocus = stod(val.c_str());
					}
					else if (refLine == "Fov=")
					{
						std::string val = inLine.substr(inLine.find("=") + 1);
						uConfig.fov = ::atof(val.c_str());
					}
					else if (refLine == "gamma=")
					{
						std::string val = inLine.substr(inLine.find("=") + 1);
						uConfig.gamma = ::atof(val.c_str());
					}
					else if (refLine == "mainThreadUpdateCoef=")
					{
						std::string val = inLine.substr(inLine.find("=") + 1);
						uConfig.mainThreadUpdateCoef = stod(val.c_str());
					}
					else if (refLine == "igmState0Override=")
					{
						std::string val = inLine.substr(inLine.find("=") + 1);
						uConfig.igmState0Override = ::atof(val.c_str());
					}
					else if (refLine == "igmState1Override=")
					{
						std::string val = inLine.substr(inLine.find("=") + 1);
						uConfig.igmState1Override = ::atof(val.c_str());
					}
					else if (refLine == "igmState2Override=")
					{
						std::string val = inLine.substr(inLine.find("=") + 1);
						uConfig.igmState2Override = ::atof(val.c_str());
					}
					else if (refLine == "lockedMouseMulti=")
					{
						std::string val = inLine.substr(inLine.find("=") + 1);
						uConfig.lockedMouseMulti = ::atof(val.c_str());
					}
					else if (refLine == "bEnableFullSpeedMode=")
					{
						std::string val = inLine.substr(inLine.find("=") + 1);
						uConfig.bEnableFullSpeedMode = stob(val);
					}
					else if (refLine == "bEnableAdaptiveIGM=")
					{
						std::string val = inLine.substr(inLine.find("=") + 1);
						uConfig.bEnableAdaptiveIGM = stob(val);
					}
					else if (refLine == "bEnableAdaptiveMouse=")
					{
						std::string val = inLine.substr(inLine.find("=") + 1);
						uConfig.bEnableAdaptiveMouse = stob(val);
					}
					else if (refLine == "bEnableLockedMouseMulti=")
					{
						std::string val = inLine.substr(inLine.find("=") + 1);
						uConfig.bEnableLockedMouseMulti = stob(val);
					}
					else if (refLine == "bEnableTransitionSpeedRamp=")
					{
						std::string val = inLine.substr(inLine.find("=") + 1);
						uConfig.bEnableTransitionSpeedRamp = stob(val);
					}

					refLine.clear();
					inLine.clear();
				}
			}
			config.close();

			std::cout << "Loaded config \nrequestedMinFrameTime=" << uConfig.requestedMinFrameTime
				<< "\nrequestedMinFrameTimeMenus=" << uConfig.requestedMinFrameTimeMenus
				<< "\nrequestedMinFrameTimeNoFocus=" << uConfig.requestedMinFrameTimeNoFocus
				<< "\nmainThreadUpdateCoef=" << uConfig.mainThreadUpdateCoef
				<< "\nfov=" << uConfig.fov
				<< "\ngamma=" << uConfig.gamma
				<< "\nigmState0Override=" << uConfig.igmState0Override
				<< "\nigmState1Override=" << uConfig.igmState1Override
				<< "\nigmState2Override=" << uConfig.igmState2Override
				<< "\nlockedMouseMulti=" << uConfig.lockedMouseMulti
				<< "\nbEnableFullSpeedMode=" << uConfig.bEnableFullSpeedMode
				<< "\nbEnableAdaptiveIGM=" << uConfig.bEnableAdaptiveIGM 
				<< "\nbEnableAdaptiveMouse=" << uConfig.bEnableAdaptiveMouse
				<< "\nbEnableLockedMouseMulti=" << uConfig.bEnableLockedMouseMulti
				<< "\nbEnableTransitionSpeedRamp=" << uConfig.bEnableTransitionSpeedRamp
				
				<< std::endl << std::endl;
		}
	}
}
