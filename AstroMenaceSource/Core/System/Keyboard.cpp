/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Keyboard.cpp

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.0

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


#include "System.h"
#include "../../Defines.h"




int MaxKeyCount = 0;


//------------------------------------------------------------------------------------
// установка-получение статуса нажатых кнопок на клавиатуре
//------------------------------------------------------------------------------------
bool vw_GetKeys(int Num)
{
	Uint8 *keystate = SDL_GetKeyState(&MaxKeyCount);
	if (keystate[Num]) return true;
	return false;
}
void vw_SetKeys(int Num, bool NewKeyStatus)
{
	Uint8 *keystate = SDL_GetKeyState(&MaxKeyCount);
	keystate[Num] = NewKeyStatus;
}

int GetMaxKeys()
{
	return MaxKeyCount;
}








// название кнопок
char * vw_VirtualCodeNameEN(int Num);
char * vw_VirtualCodeNameDE(int Num);
char * vw_VirtualCodeNameRU(int Num);


char * vw_VirtualCodeName(int KeyboardLayout, int Num)
{
	switch (KeyboardLayout)
	{
		case 1: return vw_VirtualCodeNameEN(Num);
		case 2: return vw_VirtualCodeNameDE(Num);
		case 3: return vw_VirtualCodeNameRU(Num);
	}
}










// значения кнопок клавиатуры
int VirtualCodeChar1(int Num);
int VirtualCodeChar2(int Num);
int VirtualCodeChar3(int Num);
int VirtualCodeChar4(int Num);

// значения кнопок клавиатуры
int VirtualCodeCharDE1(int Num);
int VirtualCodeCharDE2(int Num);
int VirtualCodeCharDE3(int Num);
int VirtualCodeCharDE4(int Num);

// значения кнопок клавиатуры
int VirtualCodeCharRU1(int Num);
int VirtualCodeCharRU2(int Num);
int VirtualCodeCharRU3(int Num);
int VirtualCodeCharRU4(int Num);









int vw_VirtualCodeChar(int KeyboardLayout, int Num)
{
	// нажат CapsLock
	bool CapsLockOn = false;
	// делаем немного "сложно", чтобы избежать перфоменс варнинга на MSVC
	if((SDL_GetModState() & KMOD_CAPS) > 0) CapsLockOn = true;


	switch (KeyboardLayout)
	{
		case 1:
		{
			// нажат LeftShift или LeftShift
			if (SDL_GetModState() & KMOD_SHIFT)
			{
				if (VirtualCodeChar1(Num) != 0)
				{
					if (!CapsLockOn)
						return VirtualCodeChar1(Num);
					else
						return VirtualCodeChar4(Num);
				}
			}
			else
			{
				if (VirtualCodeChar2(Num) != 0)
				{
					if (!CapsLockOn)
						return VirtualCodeChar2(Num);
					else
						return VirtualCodeChar3(Num);
				}
			}
		}
		break;
		case 2:
		{
			// нажат LeftShift или LeftShift
			if (SDL_GetModState() & KMOD_SHIFT)
			{
				if (VirtualCodeCharDE1(Num) != 0)
				{
					if (!CapsLockOn)
						return VirtualCodeCharDE1(Num);
					else
						return VirtualCodeCharDE4(Num);
				}
			}
			else
			{
				if (VirtualCodeCharDE2(Num) != 0)
				{
					if (!CapsLockOn)
						return VirtualCodeCharDE2(Num);
					else
						return VirtualCodeCharDE3(Num);
				}
			}
		}
		break;
		case 3:
		{
			// нажат LeftShift или LeftShift
			if (SDL_GetModState() & KMOD_SHIFT)
			{
				if (VirtualCodeCharRU1(Num) != 0)
				{
					if (!CapsLockOn)
						return VirtualCodeCharRU1(Num);
					else
						return VirtualCodeCharRU4(Num);
				}
			}
			else
			{
				if (VirtualCodeCharRU2(Num) != 0)
				{
					if (!CapsLockOn)
						return VirtualCodeCharRU2(Num);
					else
						return VirtualCodeCharRU3(Num);
				}
			}
		}
		break;
	}

	return 0;
}












struct sKeyboardCodeName
{
	int Code;
	char Name[32];
};

const int KeyboardCodeNameDataCount = 99;
sKeyboardCodeName KeyboardCodeNameData[KeyboardCodeNameDataCount] =
{
{SDLK_F1, "SDLK_F1"},
{SDLK_F2, "SDLK_F2"},
{SDLK_F3, "SDLK_F3"},
{SDLK_F4, "SDLK_F4"},
{SDLK_F5, "SDLK_F5"},
{SDLK_F6, "SDLK_F6"},
{SDLK_F7, "SDLK_F7"},
{SDLK_F8, "SDLK_F8"},
{SDLK_F9, "SDLK_F9"},
{SDLK_F10, "SDLK_F10"},
{SDLK_F11, "SDLK_F11"},
{SDLK_F12, "SDLK_F12"},
{SDLK_1, "SDLK_1"},
{SDLK_2, "SDLK_2"},
{SDLK_3, "SDLK_3"},
{SDLK_4, "SDLK_4"},
{SDLK_5, "SDLK_5"},
{SDLK_6, "SDLK_6"},
{SDLK_7, "SDLK_7"},
{SDLK_8, "SDLK_8"},
{SDLK_9, "SDLK_9"},
{SDLK_MINUS, "SDLK_MINUS"},
{SDLK_EQUALS, "SDLK_EQUALS"},
{SDLK_BACKSPACE, "SDLK_BACKSPACE"},
{SDLK_TAB, "SDLK_TAB"},
{SDLK_q, "SDLK_q"},
{SDLK_w, "SDLK_w"},
{SDLK_e, "SDLK_e"},
{SDLK_r, "SDLK_r"},
{SDLK_t, "SDLK_t"},
{SDLK_y, "SDLK_y"},
{SDLK_u, "SDLK_u"},
{SDLK_i, "SDLK_i"},
{SDLK_o, "SDLK_o"},
{SDLK_p, "SDLK_p"},
{SDLK_LEFTBRACKET, "SDLK_LEFTBRACKET"},
{SDLK_RIGHTBRACKET, "SDLK_RIGHTBRACKET"},
{SDLK_RETURN, "SDLK_RETURN"},
{SDLK_LCTRL, "SDLK_LCTRL"},
{SDLK_a, "SDLK_a"},
{SDLK_s, "SDLK_s"},
{SDLK_d, "SDLK_d"},
{SDLK_f, "SDLK_f"},
{SDLK_g, "SDLK_g"},
{SDLK_h, "SDLK_h"},
{SDLK_j, "SDLK_j"},
{SDLK_k, "SDLK_k"},
{SDLK_l, "SDLK_l"},
{SDLK_SEMICOLON, "SDLK_SEMICOLON"},
{SDLK_QUOTE, "SDLK_QUOTE"},
{SDLK_QUOTEDBL, "SDLK_QUOTEDBL"},
{SDLK_BACKQUOTE, "SDLK_BACKQUOTE"},
{SDLK_LSHIFT, "SDLK_LSHIFT"},
{SDLK_BACKSLASH, "SDLK_BACKSLASH"},
{SDLK_z, "SDLK_z"},
{SDLK_x, "SDLK_x"},
{SDLK_c, "SDLK_c"},
{SDLK_v, "SDLK_v"},
{SDLK_b, "SDLK_b"},
{SDLK_n, "SDLK_n"},
{SDLK_m, "SDLK_m"},
{SDLK_COMMA, "SDLK_COMMA"},
{SDLK_PERIOD, "SDLK_PERIOD"},
{SDLK_SLASH, "SDLK_SLASH"},
{SDLK_RSHIFT, "SDLK_RSHIFT"},
{SDLK_ASTERISK, "SDLK_ASTERISK"},
{SDLK_LALT, "SDLK_LALT"},
{SDLK_SPACE, "SDLK_SPACE"},
{SDLK_KP7, "SDLK_KP7"},
{SDLK_KP8, "SDLK_KP8"},
{SDLK_KP9, "SDLK_KP9"},
{SDLK_KP_MINUS, "SDLK_KP_MINUS"},
{SDLK_KP_MULTIPLY, "SDLK_KP_MULTIPLY"},
{SDLK_KP4, "SDLK_KP4"},
{SDLK_KP5, "SDLK_KP5"},
{SDLK_KP6, "SDLK_KP6"},
{SDLK_KP_PLUS, "SDLK_KP_PLUS"},
{SDLK_KP1, "SDLK_KP1"},
{SDLK_KP2, "SDLK_KP2"},
{SDLK_KP3, "SDLK_KP3"},
{SDLK_KP0, "SDLK_KP0"},
{SDLK_KP_PERIOD, "SDLK_KP_PERIOD"},
{SDLK_KP_ENTER, "SDLK_KP_ENTER"},
{SDLK_RCTRL, "SDLK_RCTRL"},
{SDLK_KP_DIVIDE, "SDLK_KP_DIVIDE"},
{SDLK_RALT, "SDLK_RALT"},
{SDLK_PAUSE, "SDLK_PAUSE"},
{SDLK_HOME, "SDLK_HOME"},
{SDLK_UP, "SDLK_UP"},
{SDLK_PAGEUP, "SDLK_PAGEUP"},
{SDLK_LEFT, "SDLK_LEFT"},
{SDLK_RIGHT, "SDLK_RIGHT"},
{SDLK_END, "SDLK_END"},
{SDLK_DOWN, "SDLK_DOWN"},
{SDLK_PAGEDOWN, "SDLK_PAGEDOWN"},
{SDLK_INSERT, "SDLK_INSERT"},
{SDLK_DELETE, "SDLK_DELETE"},
{SDLK_LSUPER, "SDLK_LSUPER"},
{SDLK_RSUPER, "SDLK_RSUPER"},
};



// название кнопок
char * vw_KeyboardCodeName(int Num)
{
	for (int i=0; i<KeyboardCodeNameDataCount; i++)
		if (KeyboardCodeNameData[i].Code == Num) return KeyboardCodeNameData[i].Name;

	return "?";
}
// номер кнопок
int vw_KeyboardNameCode(const char * Name)
{
	for (int i=0; i<KeyboardCodeNameDataCount; i++)
		if (!strcmp(KeyboardCodeNameData[i].Name, Name)) return KeyboardCodeNameData[i].Code;

	return -1;
}


