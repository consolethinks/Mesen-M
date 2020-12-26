#pragma once

#include "stdafx.h"
#include "MMC3.h"

class MMC3_134 : public MMC3
{
private:
	uint8_t modeReg;
	uint8_t _exReg;

protected:
	uint32_t GetDipSwitchCount() override { return 2; }
	bool AllowRegisterRead() override { return true; }

	void UpdatePrgMapping() override
	{
		if (modeReg & 0x08) {
			// NROM mode
			uint8_t A14 = ((_exReg & 0x08) >> 2);
			SelectPRGPage(0, _registers[6] & (~A14));
			SelectPRGPage(1, _registers[7] & (~A14));
			SelectPRGPage(2, _registers[6] | A14);
			SelectPRGPage(3, _registers[7] | A14);
		} else {
			// MMC3 mode
			MMC3::UpdatePrgMapping();
		}
	}

	void SelectCHRPage(uint16_t slot, uint16_t page, ChrMemoryType memoryType = ChrMemoryType::Default) override
	{
		if (_exReg & 0x40) {
			BaseMapper::SelectCHRPage(slot, (page & 0x7F) | ((_exReg & 0x30) << 3), memoryType);
		} else {
			BaseMapper::SelectCHRPage(slot, (page & 0xFF) | ((_exReg & 0x20) << 3), memoryType);
		}
	}

	void SelectPRGPage(uint16_t slot, uint16_t page, PrgMemoryType memoryType = PrgMemoryType::PrgRom) override
	{
		if (_exReg & 0x04) {
			BaseMapper::SelectPRGPage(slot, (page & 0x0F) | ((_exReg & 0x03) << 4), memoryType);
		} else {
			BaseMapper::SelectPRGPage(slot, (page & 0x1F) | ((_exReg & 0x02) << 4), memoryType);
		}
	}

	void InitMapper() override 
	{
		modeReg = 0;
		_exReg = 0;
		AddRegisterRange(0x6000, 0x7fff, MemoryOperation::Write);
		MMC3::InitMapper();
	}

	void Reset(bool softReset) override 
	{
		if(softReset) {
			modeReg = 0;
			_exReg = 0;
			MMC3::UpdateState();
		}
	}
	
	void StreamState(bool saving) override
	{
		MMC3::StreamState(saving);
		Stream(modeReg, _exReg);
	}
	
	uint8_t ReadRegister(uint16_t addr) override
	{
		if (modeReg & 0x40) {
			return GetDipSwitches();
		} else {
			return BaseMapper::InternalReadRam(addr);
		}
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		switch(addr & 0xE003) {
			case 0x6000:
				if(!(modeReg & 0x80))
				{
					modeReg = value;
					MMC3::UpdateState();
				}
				break;
			case 0x6001:
				if (!(modeReg & 0x80)) // register lock bit
				{
					_exReg = value;
					MMC3::UpdateState();
				}
				break;
			default:
				MMC3::WriteRegister(addr, value);
				break;
		}
	}
};