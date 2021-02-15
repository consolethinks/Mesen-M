#pragma once
#include "stdafx.h"
#include "MMC3.h"

class ResetTxrom : public MMC3
{
private:
	uint8_t _resetCounter;
	uint8_t _prgShiftCount;
	uint8_t _chrShiftCount;
	uint8_t _prgMask;
	uint8_t _chrMask;
	bool _switchPrgSize;

protected:
	void InitMapper() override
	{
		_switchPrgSize = false;
		switch(_romInfo.SubMapperID) {
			default: 
			case 0: _prgShiftCount = 4; _chrShiftCount = 7; break; // PRG 128K CHR 128K
			case 1: _prgShiftCount = 5; _chrShiftCount = 7; break; // PRG 256K CHR 128K
			case 2: _prgShiftCount = 4; _chrShiftCount = 8; break; // PRG 128K CHR 256K
			case 3: _prgShiftCount = 5; _chrShiftCount = 8; break; // PRG 256K CHR 256K
			case 4: _switchPrgSize = true; _chrShiftCount = 7; break; // PRG 256K only on first game, 128K on rest, CHR 128K
		}
		_prgMask = (1 << _prgShiftCount) - 1;
		_chrMask = (1 << _chrShiftCount) - 1;

		MMC3::InitMapper();
	}

	void Reset(bool softReset) override
	{
		MMC3::Reset(softReset);
		if(softReset) {
			_resetCounter = (_resetCounter + 1) & 0x03;
			
			if(_switchPrgSize) {
				if(_resetCounter != 0) {
					_prgShiftCount = 4;
				} else {
					_prgShiftCount = 5;
				}
				_prgMask = (1 << _prgShiftCount) - 1;
			}
			
			UpdateState();
		} else {
			_resetCounter = 0;
		}
	}

	void StreamState(bool saving) override
	{
		MMC3::StreamState(saving);
		Stream(_resetCounter, _prgShiftCount, _chrShiftCount, _prgMask, _chrMask, _switchPrgSize);
	}

	void SelectCHRPage(uint16_t slot, uint16_t page, ChrMemoryType memoryType) override
	{
		page = (_resetCounter << _chrShiftCount) | (page & _chrMask);
		MMC3::SelectCHRPage(slot, page, memoryType);
	}

	void SelectPRGPage(uint16_t slot, uint16_t page, PrgMemoryType memoryType) override
	{
		page = (_resetCounter << _prgShiftCount) | (page & _prgMask);
		MMC3::SelectPRGPage(slot, page, memoryType);
	}
};