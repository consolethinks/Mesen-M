#pragma once

#include "stdafx.h"
#include "MMC3.h"

class MMC3_114 : public MMC3
{
private:
	const uint8_t _registerLUT0[8] = { 0,3,1,5,6,7,2,4 };
	const uint8_t _registerLUT1[8] = { 0,2,5,3,6,1,7,4 };

	uint8_t _exRegs[2];

protected:
	virtual uint16_t RegisterStartAddress() override { return 0x5000; }
	virtual bool ForceMmc3RevAIrqs() override { return true; }

	void InitMapper() override
	{
		MMC3::InitMapper();
		_exRegs[0] = _exRegs[1] = 0;
	}

	virtual void StreamState(bool saving) override
	{
		Stream(_exRegs[0], _exRegs[1]);
		MMC3::StreamState(saving);
	}

	virtual void UpdatePrgMapping() override
	{
		if(_exRegs[0] & 0x80) {
			if(_exRegs[0] & 0x20) {
				SelectPrgPage4x(0, (_exRegs[0] & 0x0E) << 1);
			} else {
				SelectPrgPage2x(0, (_exRegs[0] & 0x0F) << 1);
				SelectPrgPage2x(1, (_exRegs[0] & 0x0F) << 1);
			}
		} else {
			MMC3::UpdatePrgMapping();
		}
	}

	void SelectCHRPage(uint16_t slot, uint16_t page, ChrMemoryType memoryType = ChrMemoryType::Default) override
	{
		MMC3::SelectCHRPage(slot, ((_exRegs[1] & 0x01) << 8) | (page & 0xFF), memoryType);
	}

	virtual void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if(addr < 0x8000) {
			switch(addr & 0xE001) {
				case 0x6000: _exRegs[0] = value;
				case 0x6001: _exRegs[1] = value;
			}
			UpdateState();
		} else {
			if(_romInfo.SubMapperID != 1) {
				switch(addr & 0xE001) { // Submapper 0
					case 0x8000: MMC3::WriteRegister(0xA001, value); break;
					case 0x8001: MMC3::WriteRegister(0xA000, value); break;
					case 0xA000: MMC3::WriteRegister(0x8000, _registerLUT0[value & 0x07]); break;
					case 0xA001: MMC3::WriteRegister(0xC000, value); break;
					case 0xC000: MMC3::WriteRegister(0x8001, value); break;
					default: 	 MMC3::WriteRegister(addr, value);   break;
				}
			} else {
				switch(addr & 0xE001) { // Submapper 1 (Boogerman)
					case 0x8000: MMC3::WriteRegister(0xA001, value); break;
					case 0x8001: MMC3::WriteRegister(0x8001, value); break;
					case 0xA000: MMC3::WriteRegister(0x8000, _registerLUT1[value & 0x07]); break;
					case 0xA001: MMC3::WriteRegister(0xC001, value); break;
					case 0xC000: MMC3::WriteRegister(0xA000, value); break;
					case 0xC001: MMC3::WriteRegister(0xC000, value); break;
					default:     MMC3::WriteRegister(addr, value);   break;
				}
			}
		}
	}
};