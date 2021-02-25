#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Eh8813A : public BaseMapper
{
private:
	bool _readDIP;
	bool _lock;
	uint8_t _chrPage;

protected:
	uint32_t GetDipSwitchCount() override { return 4; }
	uint16_t GetPRGPageSize() override { return 0x4000; }
	uint16_t GetCHRPageSize() override { return 0x2000; }
	bool AllowRegisterRead() override {	return true; }

	void InitMapper() override
	{
		SetMirroringType(MirroringType::Vertical);
	}

	void Reset(bool softReset) override
	{
		_lock = false;
		_chrPage = 0;
		WriteRegister(0x8000, 0);
	}

	void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);
		Stream(_readDIP);
	}

	uint8_t ReadRegister(uint16_t addr) override
	{
		if(_readDIP) {
			addr = (addr & 0xFFF0) + GetDipSwitches();
		}
		return InternalReadRam(addr);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if(!_lock) {
			_lock = (addr & 0x100) == 0x100;
			_readDIP = (addr & 0x40) == 0x40;

			if(addr & 0x80) {
				SelectPRGPage(0, addr & 0x3F);
				SelectPRGPage(1, addr & 0x3F);
			} else {
				SelectPrgPage2x(0, addr & 0x3E);
			}

			_chrPage = value & 0xEF;
			SetMirroringType(value & 0x80 ? MirroringType::Horizontal : MirroringType::Vertical);
		} else {
			_chrPage = (_chrPage & 0xEC) | (value & 0x03);
		}

		SelectCHRPage(0, _chrPage);
	}
};