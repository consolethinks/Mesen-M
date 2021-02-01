#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Mapper57 : public BaseMapper
{
private:
	uint8_t _registers[2];

protected:
	virtual uint16_t GetPRGPageSize() override { return 0x4000; }
	virtual uint16_t GetCHRPageSize() override { return 0x2000; }
	uint16_t RegisterStartAddress() override { return 0x6000; }
	bool AllowRegisterRead() override { return true; }
	uint32_t GetDipSwitchCount() override { return 1; }

	void InitMapper() override
	{
		RemoveRegisterRange(0x6000, 0x7fff, MemoryOperation::Write);
		RemoveRegisterRange(0x7000, 0xffff, MemoryOperation::Read);
		_registers[0] = 0;
		_registers[1] = 0;

		UpdateState();
	}

	void StreamState(bool saving) override
	{		
		BaseMapper::StreamState(saving);
		Stream(_registers[0], _registers[1]);
	}

	void UpdateState()
	{
		SetMirroringType(_registers[1] & 0x08 ? MirroringType::Horizontal : MirroringType::Vertical);

		SelectCHRPage(0, ((_registers[0] & 0x40) >> 3) | ((_registers[0] | _registers[1]) & 0x07));

		if(_registers[1] & 0x10) {
			SelectPRGPage(0, (_registers[1] >> 5) & 0x06);
			SelectPRGPage(1, ((_registers[1] >> 5) & 0x06) + 1);
		} else {
			SelectPRGPage(0, (_registers[1] >> 5) & 0x07);
			SelectPRGPage(1, (_registers[1] >> 5) & 0x07);
		}
	}

	uint8_t ReadRegister(uint16_t addr) override
	{
		return GetDipSwitches();
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		switch(addr & 0x8800) {
			case 0x8000: _registers[0] = value; break;
			case 0x8800: _registers[1] = value; break;
		}

		UpdateState();
	}

	void Reset(bool softReset) override {
		if(softReset) {
			memset(_registers, 0, 2);
			UpdateState();
		}
	}
};
