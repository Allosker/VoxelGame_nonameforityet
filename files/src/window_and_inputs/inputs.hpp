#pragma once // input.hpp
// MIT
// Allosker ------------------------------
// =========+
// Define new names for key mods/inputs to make my life easier
// ---------------------------------------

#include "uHeaders/opengl.hpp"

namespace Wai
{

	struct Buttons
	{
		enum PrintableKeys
			: std::uint16_t
		{
			Space =			GLFW_KEY_SPACE        ,
			Apostrophe =	GLFW_KEY_APOSTROPHE   ,
			Comma =			GLFW_KEY_COMMA        ,
			Minus =			GLFW_KEY_MINUS        ,
			Period =		GLFW_KEY_PERIOD       ,
			Slash =			GLFW_KEY_SLASH        ,
			N0 =			GLFW_KEY_0            ,
			N1 =			GLFW_KEY_1            ,
			N2 =			GLFW_KEY_2            ,
			N3 =			GLFW_KEY_3            ,
			N4 =			GLFW_KEY_4            ,
			N5 =			GLFW_KEY_5            ,
			N6 =			GLFW_KEY_6            ,
			N7 =			GLFW_KEY_7            ,
			N8 =			GLFW_KEY_8            ,
			N9 =			GLFW_KEY_9            ,
			Semicolon =		GLFW_KEY_SEMICOLON    ,
			Equal =			GLFW_KEY_EQUAL        ,
			A =				GLFW_KEY_A            ,
			B =				GLFW_KEY_B            ,
			C =				GLFW_KEY_C            ,
			D =				GLFW_KEY_D            ,
			E =				GLFW_KEY_E            ,
			F =				GLFW_KEY_F            ,
			G =				GLFW_KEY_G            ,
			H =				GLFW_KEY_H            ,
			I =				GLFW_KEY_I            ,
			J =				GLFW_KEY_J            ,
			K =				GLFW_KEY_K            ,
			L =				GLFW_KEY_L            ,
			M =				GLFW_KEY_M            ,
			N =				GLFW_KEY_N            ,
			O =				GLFW_KEY_O            ,
			P =				GLFW_KEY_P            ,
			Q =				GLFW_KEY_Q            ,
			R =				GLFW_KEY_R            ,
			S =				GLFW_KEY_S            ,
			T =				GLFW_KEY_T            ,
			U =				GLFW_KEY_U            ,
			V =				GLFW_KEY_V            ,
			W =				GLFW_KEY_W            ,
			X =				GLFW_KEY_X            ,
			Y =				GLFW_KEY_Y            ,
			Z =				GLFW_KEY_Z            ,
			Left_bracket =	GLFW_KEY_LEFT_BRACKET ,
			Backslash =		GLFW_KEY_BACKSLASH    ,
			Right_bracket = GLFW_KEY_RIGHT_BRACKET,
			Grave_accent =	GLFW_KEY_GRAVE_ACCENT ,
			World_1 =		GLFW_KEY_WORLD_1      ,
			World_2 =		GLFW_KEY_WORLD_2      
		};

		enum FunctionKeys
			: std::uint16_t
		{
			Escape =		GLFW_KEY_ESCAPE       ,
			Enter =			GLFW_KEY_ENTER        ,
			Tab =			GLFW_KEY_TAB          ,
			Backspace =		GLFW_KEY_BACKSPACE    ,
			Insert =		GLFW_KEY_INSERT       ,
			Delete =		GLFW_KEY_DELETE       ,
			Right =			GLFW_KEY_RIGHT        ,
			Left =			GLFW_KEY_LEFT         ,
			Down =			GLFW_KEY_DOWN         ,
			Up =			GLFW_KEY_UP           ,
			Page_up =		GLFW_KEY_PAGE_UP      ,
			Page_down =		GLFW_KEY_PAGE_DOWN    ,
			Home =			GLFW_KEY_HOME         ,
			End =			GLFW_KEY_END          ,
			Caps_lock =		GLFW_KEY_CAPS_LOCK    ,
			Scroll_lock =	GLFW_KEY_SCROLL_LOCK  ,
			Num_lock =		GLFW_KEY_NUM_LOCK     ,
			Print_screen =	GLFW_KEY_PRINT_SCREEN ,
			Pause =			GLFW_KEY_PAUSE        ,
			F1 =			GLFW_KEY_F1           ,
			F2 =			GLFW_KEY_F2           ,
			F3 =			GLFW_KEY_F3           ,
			F4 =			GLFW_KEY_F4           ,
			F5 =			GLFW_KEY_F5           ,
			F6 =			GLFW_KEY_F6           ,
			F7 =			GLFW_KEY_F7           ,
			F8 =			GLFW_KEY_F8           ,
			F9 =			GLFW_KEY_F9           ,
			F10 =			GLFW_KEY_F10          ,
			F11 =			GLFW_KEY_F11          ,
			F12 =			GLFW_KEY_F12          ,
			F13 =			GLFW_KEY_F13          ,
			F14 =			GLFW_KEY_F14          ,
			F15 =			GLFW_KEY_F15          ,
			F16 =			GLFW_KEY_F16          ,
			F17 =			GLFW_KEY_F17          ,
			F18 =			GLFW_KEY_F18          ,
			F19 =			GLFW_KEY_F19          ,
			F20 =			GLFW_KEY_F20          ,
			F21 =			GLFW_KEY_F21          ,
			F22 =			GLFW_KEY_F22          ,
			F23 =			GLFW_KEY_F23          ,
			F24 =			GLFW_KEY_F24          ,
			F25 =			GLFW_KEY_F25          ,
			Kp_0 =			GLFW_KEY_KP_0         ,
			Kp_1 =			GLFW_KEY_KP_1         ,
			Kp_2 =			GLFW_KEY_KP_2         ,
			Kp_3 =			GLFW_KEY_KP_3         ,
			Kp_4 =			GLFW_KEY_KP_4         ,
			Kp_5 =			GLFW_KEY_KP_5         ,
			Kp_6 =			GLFW_KEY_KP_6         ,
			Kp_7 =			GLFW_KEY_KP_7         ,
			Kp_8 =			GLFW_KEY_KP_8         ,
			Kp_9 =			GLFW_KEY_KP_9         ,
			Kp_decimal =	GLFW_KEY_KP_DECIMAL   ,
			Kp_divide =		GLFW_KEY_KP_DIVIDE    ,
			Kp_multiply =	GLFW_KEY_KP_MULTIPLY  ,
			Kp_subtract =	GLFW_KEY_KP_SUBTRACT  ,
			Kp_add =		GLFW_KEY_KP_ADD       ,
			Kp_enter =		GLFW_KEY_KP_ENTER     ,
			Kp_equal =		GLFW_KEY_KP_EQUAL     ,
			Left_shift =	GLFW_KEY_LEFT_SHIFT   ,
			Left_control =	GLFW_KEY_LEFT_CONTROL ,
			Left_alt =		GLFW_KEY_LEFT_ALT     ,
			Left_super =	GLFW_KEY_LEFT_SUPER   ,
			Right_shift =	GLFW_KEY_RIGHT_SHIFT  ,
			Right_control = GLFW_KEY_RIGHT_CONTROL,
			Right_alt =		GLFW_KEY_RIGHT_ALT    ,
			Right_super =	GLFW_KEY_RIGHT_SUPER  ,
			Menu =			GLFW_KEY_MENU         ,

			Last = Menu
		};

		struct Mouse
		{
			enum
			{
				B1 =        GLFW_MOUSE_BUTTON_1     ,
				B2 =        GLFW_MOUSE_BUTTON_2     ,
				B3 =        GLFW_MOUSE_BUTTON_3     ,
				B4 =        GLFW_MOUSE_BUTTON_4     ,
				B5 =        GLFW_MOUSE_BUTTON_5     ,
				B6 =        GLFW_MOUSE_BUTTON_6     ,
				B7 =        GLFW_MOUSE_BUTTON_7     ,
				B8 =        GLFW_MOUSE_BUTTON_8     ,
				Last =		GLFW_MOUSE_BUTTON_LAST  ,
				Left =		GLFW_MOUSE_BUTTON_LEFT  ,
				Right =		GLFW_MOUSE_BUTTON_RIGHT ,
				Middle =	GLFW_MOUSE_BUTTON_MIDDLE
			};
		};

		/* Modifier States: mod & mod & mod...*/
		enum KeyBoardMods
			: std::uint32_t
		{
			Shift =		GLFW_MOD_SHIFT,
			Control =	GLFW_MOD_CONTROL,
			Alt =		GLFW_MOD_ALT,
			Super =		GLFW_MOD_SUPER,
			Caps_Lock =	GLFW_MOD_CAPS_LOCK,
			Num_Lock =	GLFW_MOD_NUM_LOCK
		};

		enum BoutonEvent
			: std::uint8_t
		{
			Released =	GLFW_RELEASE,
			Pressed =	GLFW_PRESS,
			Repeated =	GLFW_REPEAT
		};
	};

} // WAI
