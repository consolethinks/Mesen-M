#pragma once
#include "stdafx.h"
#include "MMC3.h"

class MMC3_45 : public MMC3
{
private:
	uint8_t _regIndex;
	uint8_t _reg[4];

protected:
	bool AllowRegisterRead() override { return true; }
	uint32_t GetDipSwitchCount() override { return 3; }

	virtual void InitMapper() override
	{
		MMC3::InitMapper();
		RemoveRegisterRange(0x8000, 0xFFFF, MemoryOperation::Read);
		AddRegisterRange(0x5000, 0x5FFF, MemoryOperation::Read);
		AddRegisterRange(0x6000, 0x7FFF, MemoryOperation::Write);

		//Needed by Famicom Yarou Vol 1 - Game apparently writes to CHR RAM before initializing the registers
		_registers[0] = 0;
		_registers[1] = 2;
		_registers[2] = 4;
		_registers[3] = 5;
		_registers[4] = 6;
		_registers[5] = 7;
		UpdateChrMapping();
	}

	virtual void StreamState(bool saving) override
	{
		MMC3::StreamState(saving);
		ArrayInfo<uint8_t> reg = { _reg, 4 };
		Stream(_regIndex, reg);
	}

	virtual void Reset(bool softReset) override
	{
		_regIndex = 0;
		memset(_reg, 0, sizeof(_reg));
		_reg[2] = 0x0F;
		ResetMmc3();

		UpdateState();
	}

	virtual void SelectCHRPage(uint16_t slot, uint16_t page, ChrMemoryType memoryType = ChrMemoryType::Default) override
	{
		if(!HasChrRam()) {
			page &= 0xFF >> (0x0F - (_reg[2] & 0x0F));
			page |= _reg[0] | ((_reg[2] & 0xF0) << 4);
		}
		MMC3::SelectCHRPage(slot, page, memoryType);
	}

	virtual void SelectPRGPage(uint16_t slot, uint16_t page, PrgMemoryType memoryType = PrgMemoryType::PrgRom) override
	{
		page &= 0x3F ^ (_reg[3] & 0x3F);
		page |= _reg[1];
		MMC3::SelectPRGPage(slot, page, memoryType);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if(addr < 0x8000) {
			switch(addr & 0xF001) {
				case 0x6000:
					if(!(_reg[3] & 0x40)) {
						_reg[_regIndex] = value;
						_regIndex = (_regIndex + 1) & 0x03;
					}
					UpdateState();
					break;
				case 0x6001:
					Reset(true);
					break;
			}
		} else {
			MMC3::WriteRegister(addr, value);
		}
	}

	uint8_t ReadRegister(uint16_t addr) override
	{
		if(addr == (0x5000 | (0x0010 << GetDipSwitches()))) {
			return 0x00;
		} else {
			return 0x01;
		}
	}
};