#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Super40in1Ws : public BaseMapper
{
private:
	uint8_t _regs[3];

protected:
	uint16_t GetPRGPageSize() override { return 0x4000; }
	uint16_t GetCHRPageSize() override { return 0x2000; }
	uint16_t RegisterStartAddress() override { return 0x6000; }
	uint16_t RegisterEndAddress() override { return 0xFFFF; }

	void InitMapper() override
	{
		RemoveRegisterRange(0x6000, 0x7fff, MemoryOperation::Read);
		for(uint8_t i = 0; i < 3; ++i) _regs[i] = 0;
		UpdateState();
	}

	void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);
		Stream(_regs[0], _regs[1], _regs[2]);
	}

	void Reset(bool softReset) override
	{
		BaseMapper::Reset(softReset);
		InitMapper();
	}

	void UpdateState()
	{
		if(_regs[0] & 0x08) {
			// NROM-128
			SelectPRGPage(0, (_regs[0] & 0x07) | ((_regs[0] & 0x40) >> 3));
			SelectPRGPage(1, (_regs[0] & 0x07) | ((_regs[0] & 0x40) >> 3));
		} else {
			// NROM-256
			SelectPrgPage2x(0, (_regs[0] & 0x06) | ((_regs[0] & 0x40) >> 3));
		}

		if(_regs[1] & 0x30) {
			// NROM
			SelectCHRPage(0, (_regs[1] & 0x07) | ((_regs[0] & 0x40) >> 3));
		} else {
			// CNROM
			SelectCHRPage(0, (_regs[2] & 0x03) | (_regs[1] & 0x04) | ((_regs[0] & 0x40) >> 3));
		}
		
		SetMirroringType((_regs[0] & 0x10) ? MirroringType::Horizontal : MirroringType::Vertical);
	}

	uint8_t ReadRegister(uint16_t addr) override
	{
		if(_regs[1] & 0xC0) {
			return _console->GetMemoryManager()->GetOpenBus(0xFF);
		} else {
			return InternalReadRam(addr);
		}
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if(addr >= 0x8000) {
			_regs[2] = value;
			UpdateState();
		} else if(!(_regs[0] & 0x20) && ((addr & 0xE000) == 0x6000)) {
			_regs[addr & 1] = value;
			UpdateState();
		}
	}
};