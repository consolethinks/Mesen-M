#pragma once
#include "stdafx.h"
#include "BaseControlDevice.h"

class AsderKeyboard : public BaseControlDevice
{
private:
    uint8_t _keySet = 0;
	//bool _enabled = false;

protected:
	string GetKeyNames() override
	{
		return "ABCDEFGHIJKLMNOPQRSTUVWXYZ01234567891234567890120123456789edpmdmncdsasbemglrcpcsasbteeehidududlr123";
	}

	enum Buttons
	{
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		Numpad0, Numpad1, Numpad2, Numpad3, Numpad4, Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
		NumpadEnter, NumpadDot, NumpadPlus, NumpadMultiply, NumpadDivide, NumpadMinus, NumLock,
		Comma, Dot, SemiColon, Apostrophe,
		Slash, Backslash,
		Equal, Minus, Grave,
		LeftBracket, RightBracket, 		
		CapsLock, Pause,
		Ctrl, Shift, Alt,
		Space, Backspace, Tab, Esc, Enter,
		End, Home,
		Ins, Delete,
		PageUp, PageDown,
		Up, Down, Left, Right,
		Unknown1, Unknown2, Unknown3,
	};

	// NOTE: keyboard matrix needs adjustment for NumPad
	Buttons _keyboardMatrix[104] = {
		Num4, G, F, C, F2, E, Num5, V,
		Num2, D, S, End, F1, W, Num3, X,
		Ins, Backspace, PageDown, Right, F8, PageUp, Delete, Home,
		Num9, I, L, Comma, F5, O, Num0, Dot,
		RightBracket, Enter, Up, Left, F7, LeftBracket, Backslash, Down,
		Q, CapsLock, Z, Alt, Esc, A, Num1, Ctrl, // Alt = ę
		Num7, Y, K, M, F4, U, Num8, J,
		Minus, SemiColon, Apostrophe, Slash, F6, P, Equal, Shift,
		T, H, N, Space, F3, R, Num6, B,
		Backslash, Numpad4, Numpad7, NumpadPlus, NumpadEnter, Numpad1, Numpad2, Numpad8, // Columns 4 and 5 do something in the composer app (not numpad keys though)
		NumpadMinus, Numpad7, NumpadMultiply, Numpad9, F9, Numpad5, NumpadDivide, NumLock,
		Grave, Numpad6, F12, Tab, F10, Numpad3, NumpadDot, Numpad0,
		Alt, Unknown3, Pause, Space, Unknown3, F11, Unknown3, Unknown3 // this row is completely random since the only known game doesnt use it
	};  // Alt, Ctrl are pretty much never used in game, Numpad enter == regular enter? since calculator accepts it, NumpadPlus??

	void InternalSetStateFromInput() override
	{
		for(KeyMapping keyMapping : _keyMappings) {
			for(int i = 0; i < 99; i++) {
				SetPressedState(i, keyMapping.SuborKeyboardButtons[i]); // introduce separate keyset config for Asder at some point
			}
		}
	}

	uint8_t GetActiveKeys(uint8_t keySet)
	{
		uint8_t result = 0;
		uint32_t baseIndex = keySet * 8;
		for(int i = 0; i < 8; i++) {
            result >>= 1;
			if(IsPressed(_keyboardMatrix[baseIndex + i])) {
				result |= 0x80;
			}
		}
		return result;
	}

	void StreamState(bool saving) override
	{
		BaseControlDevice::StreamState(saving);
		//Stream(_row, _column, _enabled);
		Stream(_keySet);
	}

	void RefreshStateBuffer() override
	{
        _keySet = 0;
	}

public:
	AsderKeyboard(shared_ptr<Console> console, KeyMappingSet keyMappings) : BaseControlDevice(console, BaseControlDevice::MapperInputPort, keyMappings)
	{
	}

	bool IsKeyboard() override
	{
		return true;
	}

	uint8_t ReadRAM(uint16_t addr) override
	{
		if(addr == 0x4906 && _keySet < 13) {
			return (~GetActiveKeys(_keySet));
		}

		return 0;
	}

	void WriteRAM(uint16_t addr, uint8_t value) override
	{
        if(addr == 0x4904){
            //StrobeProcessWrite(value);

		    _keySet = value;
        }
	}
};