#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Kaiser7058 : public BaseMapper
{
protected:
	virtual uint16_t GetPRGPageSize() override { return 0x8000; }
	virtual uint16_t GetCHRPageSize() override { return 0x1000; }
	virtual uint16_t RegisterStartAddress() override { return 0x8000; }
	virtual uint16_t RegisterEndAddress() override { return 0xFFFF; }

	void InitMapper() override
	{
		SelectPRGPage(0, 0);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if(addr & 0x01) {
			SelectCHRPage(1, value);
		} else {
			SelectCHRPage(0, value);
		}
	}
};