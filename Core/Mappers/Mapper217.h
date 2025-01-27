#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Mapper217 : public BaseMapper // New Mapper 217 Definition
{
protected:
	virtual uint16_t GetPRGPageSize() override { return 0x8000; }
	virtual uint16_t GetCHRPageSize() override { return 0x2000; }

	void InitMapper() override
	{
		SelectPRGPage(0, 0);
		SelectCHRPage(0, 0);
	}
	
	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		SelectPRGPage(0, (addr >> 2) & 0x03);
		SelectCHRPage(0, addr & 0x07);
	}
}; 
