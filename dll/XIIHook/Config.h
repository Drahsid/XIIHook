#pragma once

#ifndef CONFIG_H
#define CONFIG_H

enum TypeConversion 
{
	toInt,
	toFloat,
	toDouble,
	toBool,
	toStr
};

constexpr int cVERSION = 4;

//Might be a way to automate this
constexpr uint64_t NUM_VARS = 24;
#define DataConfigs \
sEnum(Version, "Version", "# File version. Changing this will cause the config to update.\n", TypeConversion::toInt) \
sEnum(Framerate, "Framerate", "# In game frametime target. [Default: 60, Recommended: A limit that your PC can hit (NOTE THAT YOU CAN PRESS TAB TO FORCE THIS)]\n", TypeConversion::toDouble)\
sEnum(FramerateMenus, "Framerate_Menus", "# In game frametime target while in menus. [Default: 60, Recommended: A limit that your PC can hit]\n", TypeConversion::toDouble)\
sEnum(FramerateMovies, "Framerate_Movies", "# In game frametime target while viewing the in game movies/fmvs. [Default: 59.97, Recommended: Default]\n", TypeConversion::toDouble)\
sEnum(FramerateNoFocus, "Framerate_NoFocus", "# In game frametime target while the game window is not in focus. [Default: 60, Recommended: A lower value if the game is hogging resources]\n", TypeConversion::toDouble)\
sEnum(mainThreadUpdateCoef, "Main_Thread_Update_Coef", "# The number of times the main loop will run per frame. A higher number results in slightly more accurate predictions, but also slightly increases cpu usage. [Recommended: 2]\n", TypeConversion::toDouble)\
sEnum(Fov, "Field_of_View", "# The in-game Field of View. If you feel claustrophic, or that the viewing angle is too small, consider raising this. [Default: 45, Recommended: User Preference]\n", TypeConversion::toFloat)\
sEnum(Gamma, "Gamma", "# The in-game gamma (brightness). [Default: 50, Recommended: User Preference]\n", TypeConversion::toFloat)\
sEnum(IgmState0Override, "IGM_State0_Override", "# The 1x speed state of the in-game-multiplier. [Default: 1, Recommended: 1]\n", TypeConversion::toFloat)\
sEnum(IgmState1Override, "IGM_State1_Override", "# The 2x speed state of the in-game-multiplier. [Default: 2, Recommended: User Preference]\n", TypeConversion::toFloat)\
sEnum(IgmState2Override, "IGM_State2_Override", "# The 4x speed state of the in-game-multiplier. [Default: 4, Recommended: User Preference]\n", TypeConversion::toFloat)\
sEnum(LockedMouseMulti, "Locked_Mouse_Multi", "# The mouse speed multiplier. This works in conjunction with Enable_Locked_Mouse_Multi and Enable_Adaptive_Mouse. At least one of them must be enabled for this to be used. [Default: 2, Recommended: User Preference]\n", TypeConversion::toFloat)\
sEnum(EnableFullSpeedMode, "Enable_Full_Speed_Mode", "# Disable the framerate limit completely by erasing related opcodes. [Default: Off, Recommended: Off]\n", TypeConversion::toBool)\
sEnum(EnableAdaptiveMouse, "Enable_Adaptive_Mouse", "# Enabling this will make the mouse speed scale relativley to your framerate AND to the in game multiplier. This fixes mouse speed issues found in the vanilla game. [Default: On, Recommended: On]\n", TypeConversion::toBool)\
sEnum(EnableLockedMouseMulti, "Enable_Locked_Mouse_Multi", "# Enabling this will stop the game from taking control over the mouse speed multiplier. This is ignored if Enable_Adaptive_Mouse is on. [Default: On, Recommended: On]\n", TypeConversion::toBool)\
sEnum(EnableEasing, "Enable_Easing", "# Enabling this will cause certain variables to ease when they are changed, such as the IGM. [Default: On, Recommended: User Preference]\n", TypeConversion::toBool)\
sEnum(EasingType, "EasingType", "# This value can be set to Linear, OnePointFive, Square, Cubic, Quad, Quint, Sine, SinSq, SinInvSq, PerlinFast, or Perlin. This changes the default equation used to smooth out igm changes, where linear is flat, and quint is the sharpest. [Default: SinSq, Recommended: User Preference]\n# You can find a visualization of the different easing types here: desmos.com/calculator/l1jiznfdof \n", TypeConversion::toStr)\
sEnum(EasingTime, "EasingTime", "# The time taken to ease into the next igm state. [Default: 0.5, Recommended: User Preference]\n", TypeConversion::toDouble)\
sEnum(BaseMoveSpeed, "Free_Camera_Base_Move_Speed", "# The base square speed in in game units of which the free camera will translate. [Default: 4, Recommended: User Preference (NOTE THAT YOU CAN USE X AND Z TO MODIFY THIS)]\n", TypeConversion::toFloat)\
sEnum(BaseLookSpeed, "Free_Camera_Base_Look_Speed", "# The number of degrees per 'Second / Main_Thread_Update_Coef' of which the free camera will rotate. This does NOT effect mouse input and only effects the arrow keys. [Default: 16, Recommended: User Preference (NOTE THAT YOU CAN USE V AND C TO MODIFY THIS)]\n", TypeConversion::toFloat)\
sEnum(SpeedMax, "Free_Camera_Speed_Max", "# The maximum speed while moving. This effects both look and move. [Default: 128, Recommended: User Preference (NOTE THAT YOU CAN HOLD SHIFT TO TEMPORARILY FREEZE YOUR CURRENT SPEED)]\n", TypeConversion::toFloat)\
sEnum(TimeUntilSpeedRamp, "Free_Camera_Speed_Ramping_Time", "# The time, in ms, for which you have to be moving before the speed begins to increase. [Default: 1000, Recommended: User Preference (NOTE THAT YOU CAN USE M AND N TO MODIFY THIS)]\n", TypeConversion::toFloat)\
sEnum(MPitch, "Free_Camera_M_Pitch", "# Default mouse sensitivity for vertical mouse movements. [Default: 0.01, Recommended: User Preference (NOTE THAT YOU CAN USE + AND - TO MODIFY THIS)]\n", TypeConversion::toFloat)\
sEnum(MYaw, "Free_Camera_M_Yaw", "# Default mouse sensitivity for horizontal mouse movements. [Default: 0.01, Recommended: User Preference (NOTE THAT YOU CAN USE + AND - TO MODIFY THIS)]\n", TypeConversion::toFloat)

#define sEnum(eName, sName, sDesc, iType) eName,
enum EnumConfigs : size_t
{
	DataConfigs
};
#undef sEnum

#define sEnum(eName, sName, sDesc, iType) sName,
char const *ENUM_CONFIGS_NAME[] =
{
	DataConfigs
};
#undef sEnum

#define sEnum(eName, sName, sDesc, iType) sDesc,
char const *ENUM_CONFIGS_DESC[] =
{
	DataConfigs
};
#undef sEnum

#define sEnum(eName, sName, sDesc, iType) iType,
int const ENUM_CONFIGS_TYPES[] =
{
	DataConfigs
};
#undef sEnum

#undef dConfigGroups
#undef DataConfigs


__forceinline double stringToDouble(const std::string& s)
{
	std::istringstream i(s);
	double x;
	if (!(i >> x))
		return 0;
	return x;
}

__forceinline bool stringToBool(std::string& s)
{
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);
	return (s == "1" || s == "on" || s == "true") || false;
}

class UserConfig 
{
public:
	int version = -1;
	double requestedMinFrametime = 60;
	double requestedMinFrametimeMenus = 60;
	double requestedMinFrametimeMovies = 59.978;
	double requestedMinFrametimeNoFocus = 30;
	double mainThreadUpdateCoef = 3;
	float fov = 45;
	float gamma = 50;
	float igmState0Override = 1;
	float igmState1Override = 2;
	float igmState2Override = 4;
	float lockedMouseMulti = 2;
	float baseMoveSpeed = 4;
	float baseLookSpeed = 16;
	float speedMax = 128;
	float timeUntilSpeedRamp = 1000;
	float mPitch = 0.01f;
	float mYaw = 0.01f;
	bool bEnableFullSpeedMode = false;
	bool bEnableAdaptiveMouse = true;
	bool bEnableLockedMouseMulti = true;
	bool bEnableEasing = true;
	const char* easingType = "SinSq";
	double easingTime = 0.5;
public:
	
	__forceinline std::string getVarByEnumID(int& Id)
	{
		switch (Id) 
		{
		case 0:
			return std::to_string(version);
			break;
		case 1:
			return std::to_string(requestedMinFrametime);
			break;
		case 2:
			return std::to_string(requestedMinFrametimeMenus);
			break;
		case 3:
			return std::to_string(requestedMinFrametimeMovies);
			break;
		case 4:
			return std::to_string(requestedMinFrametimeNoFocus);
			break;
		case 5:
			return std::to_string(mainThreadUpdateCoef);
			break;
		case 6:
			return std::to_string(fov);
			break;
		case 7:
			return std::to_string(gamma);
			break;
		case 8:
			return std::to_string(igmState0Override);
			break;
		case 9:
			return std::to_string(igmState1Override);
			break;
		case 10:
			return std::to_string(igmState2Override);
			break;
		case 11:
			return std::to_string(lockedMouseMulti);
			break;
		case 12:
			return std::to_string(bEnableFullSpeedMode);
			break;
		case 13:
			return std::to_string(bEnableAdaptiveMouse);
			break;
		case 14:
			return std::to_string(bEnableLockedMouseMulti);
			break;
		case 15:
			return std::to_string(bEnableEasing);
			break;
		case 16:
			return std::string(easingType, strlen(easingType));
			break;
		case 17:
			return std::to_string(easingTime);
			break;
		case 18:
			return std::to_string(baseMoveSpeed);
			break;
		case 19:
			return std::to_string(baseLookSpeed);
			break;
		case 20:
			return std::to_string(speedMax);
			break;
		case 21:
			return std::to_string(timeUntilSpeedRamp);
			break;
		case 22:
			return std::to_string(mPitch);
			break;
		case 23:
			return std::to_string(mYaw);
			break;
		}

		return "";
	}

	template <typename T> void setByEnumID(int& id, T& var);
	template <> void setByEnumID<int>(int& id, int& var) 
	{
		switch (id)
		{
		case 0:
			version = (int)var;
			break;
		}
	}

	template <> void setByEnumID<double>(int& id, double& var)
	{
		switch (id)
		{
		case 1:
			requestedMinFrametime = (double)var;
			break;
		case 2:
			requestedMinFrametimeMenus = (double)var;
			break;
		case 3:
			requestedMinFrametimeMovies = (double)var;
			break;
		case 4:
			requestedMinFrametimeNoFocus = (double)var;
			break;
		case 5:
			mainThreadUpdateCoef = (double)var;
			break;
		case 17:
			easingTime = (double)var;
			break;
		}
	}

	template <> void setByEnumID<float>(int& id, float& var) 
	{
		switch (id) 
		{
		case 6:
			fov = (float)var;
			break;
		case 7:
			gamma = (float)var;
			break;
		case 8:
			igmState0Override = (float)var;
			break;
		case 9:
			igmState1Override = (float)var;
			break;
		case 10:
			igmState2Override = (float)var;
			break;
		case 11:
			lockedMouseMulti = (float)var;
			break;
		case 18:
			baseMoveSpeed = (float)var;
			break;
		case 19:
			baseLookSpeed = (float)var;
			break;
		case 20:
			speedMax = (float)var;
			break;
		case 21:
			timeUntilSpeedRamp = (float)var;
			break;
		case 22:
			mPitch = (float)var;
			break;
		case 23:
			mYaw = (float)var;
			break;
		}
	}

	template <> void setByEnumID<bool>(int& id, bool& var) 
	{
		switch (id) 
		{
		case 12:
			bEnableFullSpeedMode = (bool)var;
			break;
		case 13:
			bEnableAdaptiveMouse = (bool)var;
			break;
		case 14:
			bEnableLockedMouseMulti = (bool)var;
			break;
		case 15:
			bEnableEasing = (bool)var;
			break;
		}
	}

	template <> void setByEnumID<std::string>(int& id, std::string& var)
	{
		switch (id) 
		{
		case 16:
			easingType = var.c_str();
			break;
		}
	}
};

namespace Config 
{
	template <typename T>
	__forceinline void setVal(int& i, T val, UserConfig& uConfig) 
	{
		uConfig.setByEnumID<T>(i, val);
	}

	__forceinline void readConfigLine(std::string& inLine, UserConfig& uConfig)
	{
		if (inLine.find("#") == inLine.npos && inLine.find("[") == inLine.npos && inLine.find(",") == inLine.npos)
		{
			printf("%s\n", inLine.c_str());
			std::string refLine = inLine.substr(0, inLine.find("="));
			for (int i = 0; i < NUM_VARS; i++) 
			{
				if (ENUM_CONFIGS_NAME[i] == refLine) 
				{
					std::string val = inLine.substr(inLine.find("=") + 1);
					switch (ENUM_CONFIGS_TYPES[i])
					{
					case 0:
						setVal<int>(i, std::stoi(val.c_str()), uConfig);
						break;
					case 1:
						setVal<float>(i, ::atof(val.c_str()), uConfig);
						break;
					case 2:
						setVal<double>(i, stringToDouble(val.c_str()), uConfig);
						break;
					case 3:
						setVal<bool>(i, stringToBool(val), uConfig);
						break;
					case 4:
						setVal<std::string>(i, val, uConfig);
						break;
					}
				}
			}
		}
	}

	__forceinline void newUserConfig(UserConfig& uConfig, std::fstream& config, const bool usePriorValues)
	{
		config.open("fpsconfig.ini", std::fstream::trunc | std::fstream::out);
		config.close();
		config.open("fpsconfig.ini", std::fstream::in | std::fstream::out);

		uConfig.version = cVERSION;

		if (usePriorValues && uConfig.version < 2) 
		{
			remove("fpsconfig.ini");
			printf("Your config is not compatable with the new format... you must reconfigure!\n");
		}

		for (int i = 0; i < NUM_VARS; i++) 
		{
			config << ENUM_CONFIGS_DESC[i]
				<< ENUM_CONFIGS_NAME[i] << "=" << uConfig.getVarByEnumID(i) << "\n\n";
		}

		config.close();
	}

	void UpdateUserConfig(UserConfig& uConfig)
	{
		std::fstream config;
		config.open("fpsconfig.ini", std::fstream::in | std::fstream::out);
		if (!config)
		{
			printf("Config file not found! Writing a new one...\n\n");
			newUserConfig(uConfig, config, false);
		}
		else
		{
			std::string refLine;
			std::string inLine;
			while (std::getline(config, inLine))
			{
				readConfigLine(inLine, uConfig);
				refLine.clear();
				inLine.clear();
			}
			config.close();

			if (uConfig.version != cVERSION) 
			{
				printf("Config out of date! Updating...\n");
				newUserConfig(uConfig, config, true);
			}

		}
	}
}

#endif

