/*
    C-Dogs SDL
    A port of the legendary (and fun) action/arcade cdogs.
    Copyright (C) 1995 Ronny Wester
    Copyright (C) 2003 Jeremy Chin
    Copyright (C) 2003-2007 Lucas Martin-King

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    This file incorporates work covered by the following copyright and
    permission notice:

    Copyright (c) 2013, Cong Xu
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
    Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/
#include "input.h"

#include <assert.h>
#include <stdlib.h>

#include "joystick.h"
#include "keyboard.h"
#include "defs.h"
#include "sounds.h"
#include "gamedata.h"


static int SwapButtons(int cmd)
{
	int c = (cmd & ~(CMD_BUTTON1 | CMD_BUTTON2));
	if (cmd & CMD_BUTTON1)
		c |= CMD_BUTTON2;
	if (cmd & CMD_BUTTON2)
		c |= CMD_BUTTON1;
	return c;
}

int GetOnePlayerCmd(
	struct PlayerData *data,
	int (*keyFunc)(keyboard_t *, int),
	int (*joyFunc)(joystick_t *, int))
{
	int cmd = 0;
	if (data->inputDevice == INPUT_DEVICE_KEYBOARD)
	{
		if (keyFunc(&gKeyboard, data->keys.left))
		{
			cmd |= CMD_LEFT;
		}
		else if (keyFunc(&gKeyboard, data->keys.right))
		{
			cmd |= CMD_RIGHT;
		}

		if (keyFunc(&gKeyboard, data->keys.up))
		{
			cmd |= CMD_UP;
		}
		else if (keyFunc(&gKeyboard, data->keys.down))
		{
			cmd |= CMD_DOWN;
		}

		if (keyFunc(&gKeyboard, data->keys.button1))
		{
			cmd |= CMD_BUTTON1;
		}

		if (keyFunc(&gKeyboard, data->keys.button2))
		{
			cmd |= CMD_BUTTON2;
		}
	}
	else
	{
		int swapButtons = 0;
		joystick_t *joystick = &gJoysticks.joys[0];

		if (data->inputDevice == INPUT_DEVICE_JOYSTICK_1)
		{
			joystick = &gJoysticks.joys[0];
			swapButtons = gOptions.swapButtonsJoy1;
		}
		else if (data->inputDevice == INPUT_DEVICE_JOYSTICK_2)
		{
			joystick = &gJoysticks.joys[1];
			swapButtons = gOptions.swapButtonsJoy2;
		}

		if (joyFunc(joystick, CMD_LEFT))
		{
			cmd |= CMD_LEFT;
		}
		else if (joyFunc(joystick, CMD_RIGHT))
		{
			cmd |= CMD_RIGHT;
		}

		if (joyFunc(joystick, CMD_UP))
		{
			cmd |= CMD_UP;
		}
		else if (joyFunc(joystick, CMD_DOWN))
		{
			cmd |= CMD_DOWN;
		}

		if (joyFunc(joystick, CMD_BUTTON1))
		{
			cmd |= CMD_BUTTON1;
		}

		if (joyFunc(joystick, CMD_BUTTON2))
		{
			cmd |= CMD_BUTTON2;
		}

		if (swapButtons)
		{
			cmd = SwapButtons(cmd);
		}
	}
	return cmd;
}


char *InputDeviceStr(int d)
{
	switch (d)
	{
	case INPUT_DEVICE_KEYBOARD:
		return "Keyboard";
	case INPUT_DEVICE_JOYSTICK_1:
		return "Joystick 1";
	case INPUT_DEVICE_JOYSTICK_2:
		return "Joystick 2";
	default:
		return "";
	}
}

void GetPlayerCmd(int *cmd1, int *cmd2, int isPressed)
{
	int (*keyFunc)(keyboard_t *, int) = isPressed ? KeyIsPressed : KeyIsDown;
	int (*joyFunc)(joystick_t *, int) = isPressed ? JoyIsPressed : JoyIsDown;

	if (cmd1 != NULL)
	{
		*cmd1 = GetOnePlayerCmd(&gPlayer1Data, keyFunc, joyFunc);
	}
	if (cmd2 != NULL)
	{
		*cmd2 = GetOnePlayerCmd(&gPlayer2Data, keyFunc, joyFunc);
	}
}

int GetMenuCmd(void)
{
	int cmd = 0;
	if (KeyIsPressed(&gKeyboard, keyEsc))
	{
		return CMD_ESC;
	}
	if (KeyIsPressed(&gKeyboard, keyF9))
	{
		gPlayer1Data.inputDevice = INPUT_DEVICE_KEYBOARD;
		gPlayer2Data.inputDevice = INPUT_DEVICE_KEYBOARD;
	}

	GetPlayerCmd(&cmd, NULL, 1);
	if (!cmd)
	{
		if (KeyIsPressed(&gKeyboard, keyArrowLeft))
		{
			cmd |= CMD_LEFT;
		}
		else if (KeyIsPressed(&gKeyboard, keyArrowRight))
		{
			cmd |= CMD_RIGHT;
		}
		if (KeyIsPressed(&gKeyboard, keyArrowUp))
		{
			cmd |= CMD_UP;
		}
		else if (KeyIsPressed(&gKeyboard, keyArrowDown))
		{
			cmd |= CMD_DOWN;
		}
		if (KeyIsPressed(&gKeyboard, keyEnter))
		{
			cmd |= CMD_BUTTON1;
		}
		if (KeyIsPressed(&gKeyboard, keyBackspace))
		{
			cmd |= CMD_BUTTON2;
		} 
	}

	return cmd;
}

int InputGetKey(input_keys_t *keys, key_code_e keyCode)
{
	switch (keyCode)
	{
	case KEY_CODE_LEFT:
		return keys->left;
		break;
	case KEY_CODE_RIGHT:
		return keys->right;
		break;
	case KEY_CODE_UP:
		return keys->up;
		break;
	case KEY_CODE_DOWN:
		return keys->down;
		break;
	case KEY_CODE_BUTTON1:
		return keys->button1;
		break;
	case KEY_CODE_BUTTON2:
		return keys->button2;
		break;
	default:
		printf("Error unhandled key code %d\n", keyCode);
		assert(0);
		return 0;
	}
}

void InputSetKey(input_keys_t *keys, int key, key_code_e keyCode)
{
	switch (keyCode)
	{
	case KEY_CODE_LEFT:
		keys->left = key;
		break;
	case KEY_CODE_RIGHT:
		keys->right = key;
		break;
	case KEY_CODE_UP:
		keys->up = key;
		break;
	case KEY_CODE_DOWN:
		keys->down = key;
		break;
	case KEY_CODE_BUTTON1:
		keys->button1 = key;
		break;
	case KEY_CODE_BUTTON2:
		keys->button2 = key;
		break;
	default:
		assert(0);
		break;
	}
}
