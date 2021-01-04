#pragma once

#include "stdafx.h"
#include "MMC3.h"

class MMC3_134 : public MMC3
{
private:
	uint8_t _exRegs[4];

protected:
	uint32_t GetDipSwitchCount() override { return 2; }
	bool AllowRegisterRead() override { return true; }

	void UpdatePrgMapping() override
	{
		if(_exRegs[1] & 0x80) {
			// NROM mode
			//uint8_t A14 = ((_exRegs[1] & 0x08) >> 2);
			//SelectPRGPage(0, (_registers[6] & (~A14)) & 0xFE);
			//SelectPRGPage(1, (_registers[6] & (~A14)) | 0x01);
			//SelectPRGPage(2, (_registers[6] | A14) & 0xFE);
			//SelectPRGPage(3, (_registers[6] | A14) | 0x01);
			if (_exRegs[1] & 0x08) {
				SelectPRGPage(0, _registers[6] & 0xFE);
				SelectPRGPage(1, _registers[6] | 0x01);
				SelectPRGPage(2, _registers[6] & 0xFE);
				SelectPRGPage(3, _registers[6] | 0x01);
			} else {
				SelectPRGPage(0, (_registers[6] & 0xFC));
				SelectPRGPage(1, (_registers[6] & 0xFC) | 0x01);
				SelectPRGPage(2, (_registers[6] & 0xFC) | 0x02);
				SelectPRGPage(3, (_registers[6] & 0xFC) | 0x03);
			}
		} else {
			// MMC3 mode
			MMC3::UpdatePrgMapping();
		}
	}

	void UpdateChrMapping() override {
		if(_exRegs[0] & 0x08) {
			// CNROM mode
			SelectCHRPage(0, (_exRegs[2] << 3));
			SelectCHRPage(1, (_exRegs[2] << 3) + 1);
			SelectCHRPage(2, (_exRegs[2] << 3) + 2);
			SelectCHRPage(3, (_exRegs[2] << 3) + 3);
			SelectCHRPage(4, (_exRegs[2] << 3) + 4);
			SelectCHRPage(5, (_exRegs[2] << 3) + 5);
			SelectCHRPage(6, (_exRegs[2] << 3) + 6);
			SelectCHRPage(7, (_exRegs[2] << 3) + 7);
		} else {
			// MMC3 mode
			MMC3::UpdateChrMapping();
		}
	}

	void SelectCHRPage(uint16_t slot, uint16_t page, ChrMemoryType memoryType = ChrMemoryType::Default) override
	{
		if(_exRegs[1] & 0x40) {
			BaseMapper::SelectCHRPage(slot, (page & 0x7F) | ((_exRegs[1] & 0x30) << 3), memoryType);
		} else {
			BaseMapper::SelectCHRPage(slot, (page & 0xFF) | ((_exRegs[1] & 0x20) << 3), memoryType);
		}
	}

	void SelectPRGPage(uint16_t slot, uint16_t page, PrgMemoryType memoryType = PrgMemoryType::PrgRom) override
	{
		if(_exRegs[1] & 0x04) {
			BaseMapper::SelectPRGPage(slot, (page & 0x0F) | ((_exRegs[1] & 0x03) << 4), memoryType);
		} else {
			BaseMapper::SelectPRGPage(slot, (page & 0x1F) | ((_exRegs[1] & 0x02) << 4), memoryType);
		}
	}

	void InitMapper() override 
	{
		for(uint8_t i = 0; i < 4; ++i) _exRegs[i] = 0;
		AddRegisterRange(0x6000, 0x7fff, MemoryOperation::Write);
		MMC3::InitMapper();
	}

	void Reset(bool softReset) override 
	{
		if(softReset) {
			for(uint8_t i = 0; i < 4; ++i) _exRegs[i] = 0;
			MMC3::UpdateState();
		}
	}
	
	void StreamState(bool saving) override
	{
		MMC3::StreamState(saving);
		Stream(_exRegs[0], _exRegs[1], _exRegs[2], _exRegs[3]);
	}
	
	uint8_t ReadRegister(uint16_t addr) override
	{
		if(_exRegs[0] & 0x40) {
			return GetDipSwitches();
		} else {
			return BaseMapper::InternalReadRam(addr);
		}
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if((addr & 0xE000) == 0x6000) {
			if (!(_exRegs[0] & 0x80)) {
				_exRegs[addr & 0x0003] = value;
				MMC3::UpdateState();
			}
		} else {
			MMC3::WriteRegister(addr, value);
		}
	}
};