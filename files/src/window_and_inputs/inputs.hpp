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
			Space = 32,
			Apostrophe = 39,
			Comma = 44,
			Minus = 45,
			Period = 46,
			Slash = 47,
			N0 = 48,
			N1 = 49,
			N2 = 50,
			N3 = 51,
			N4 = 52,
			N5 = 53,
			N6 = 54,
			N7 = 55,
			N8 = 56,
			N9 = 57,
			Semicolon = 59,
			Equal = 61,
			A = 65,
			B = 66,
			C = 67,
			D = 68,
			E = 69,
			F = 70,
			G = 71,
			H = 72,
			I = 73,
			J = 74,
			K = 75,
			L = 76,
			M = 77,
			N = 78,
			O = 79,
			P = 80,
			Q = 81,
			R = 82,
			S = 83,
			T = 84,
			U = 85,
			V = 86,
			W = 87,
			X = 88,
			Y = 89,
			Z = 90,
			Left_bracket = 91,
			Backslash = 92,
			Right_bracket = 93,
			Grave_accent = 96,
			World_1 = 161,
			World_2 = 162
		};

		enum FunctionKeys
			: std::uint16_t
		{
			Escape = 256,
			Enter = 257,
			Tab = 258,
			Backspace = 259,
			Insert = 260,
			Delete = 261,
			Right = 262,
			Left = 263,
			Down = 264,
			Up = 265,
			Page_up = 266,
			Page_down = 267,
			Home = 268,
			End = 269,
			Caps_lock = 280,
			Scroll_lock = 281,
			Num_lock = 282,
			Print_screen = 283,
			Pause = 284,
			F1 = 290,
			F2 = 291,
			F3 = 292,
			F4 = 293,
			F5 = 294,
			F6 = 295,
			F7 = 296,
			F8 = 297,
			F9 = 298,
			F10 = 299,
			F11 = 300,
			F12 = 301,
			F13 = 302,
			F14 = 303,
			F15 = 304,
			F16 = 305,
			F17 = 306,
			F18 = 307,
			F19 = 308,
			F20 = 309,
			F21 = 310,
			F22 = 311,
			F23 = 312,
			F24 = 313,
			F25 = 314,
			Kp_0 = 320,
			Kp_1 = 321,
			Kp_2 = 322,
			Kp_3 = 323,
			Kp_4 = 324,
			Kp_5 = 325,
			Kp_6 = 326,
			Kp_7 = 327,
			Kp_8 = 328,
			Kp_9 = 329,
			Kp_decimal = 330,
			Kp_divide = 331,
			Kp_multiply = 332,
			Kp_subtract = 333,
			Kp_add = 334,
			Kp_enter = 335,
			Kp_equal = 336,
			Left_shift = 340,
			Left_control = 341,
			Left_alt = 342,
			Left_super = 343,
			Right_shift = 344,
			Right_control = 345,
			Right_alt = 346,
			Right_super = 347,
			Menu = 348,

			Last = Menu
		};

		/* Modifier States: mod & mod & mod...*/
		enum KeyBoardMods
			: std::uint32_t
		{
			Shift = 0x0001,
			Control = 0x0002,
			Alt = 0x0004,
			Super = 0x0008,
			Caps_Lock = 0x0010,
			Num_Lock = 0x0020
		};

		enum BoutonEvent
			: std::uint8_t
		{
			Released = 0,
			Pressed = 1,
			Repeated = 2
		};
	};

} // WAI
