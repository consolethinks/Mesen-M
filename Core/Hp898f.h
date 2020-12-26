#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Hp898f : public BaseMapper
{
private:
	uint8_t _regs[2];

protected:
	uint16_t GetPRGPageSize() override { return 0x4000; }
	uint16_t GetCHRPageSize() override { return 0x2000; }
	uint16_t RegisterStartAddress() override { return 0x6000; }
	uint16_t RegisterEndAddress() override { return 0xFFFF; }
	bool AllowRegisterRead() override { return true; }
	uint32_t GetDipSwitchCount() override { return 1; }

	void InitMapper() override
	{
		RemoveRegisterRange(0x8000, 0xFFFF, MemoryOperation::Read);
		AddRegisterRange(0x5000, 0x5FFF, MemoryOperation::Read);
		_regs[0] = _regs[1] = 0;
		UpdateState();
	}

	void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);
		Stream(_regs[0], _regs[1]);
	}

	void UpdateState()
	{
		uint8_t prgReg = ((_regs[1] >> 2) & 0x06) | ((_regs[1] >> 5) & 0x01);
		uint8_t prgMask = ((_regs[1] >> 6) & 0x01) ^ 0x01;
		SelectCHRPage(0, _regs[0] >> 4);
		SelectPRGPage(0, prgReg & (0xFE | prgMask));
		SelectPRGPage(1, prgReg | ((~prgMask) & 0x01));
		SetMirroringType(_regs[1] & 0x80 ? MirroringType::Vertical : MirroringType::Horizontal);
	}

	uint8_t ReadRegister(uint16_t addr) override
	{
		return (GetDipSwitches() << 6);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if((addr & 0x6000) == 0x6000) {
			_regs[(addr & 0x04) >> 2] = value;
			UpdateState();
		}
	}
};
