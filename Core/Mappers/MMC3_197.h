#pragma once

#include "stdafx.h"
#include "MMC3.h"

class MMC3_197 : public MMC3
{
private:
	uint8_t sub3OuterBankReg;
protected:
	uint16_t RegisterStartAddress() override 
	{ 
		if(_romInfo.SubMapperID == 3)
		{
			return 0x6000; 
		} else {
			return 0x8000;
		}
	}

	void SelectPRGPage(uint16_t slot, uint16_t page, PrgMemoryType memoryType = PrgMemoryType::PrgRom) override {
        if(_romInfo.SubMapperID == 3) {
			if((sub3OuterBankReg & 0x08)) {
				BaseMapper::SelectPRGPage(slot, ((sub3OuterBankReg & 0x01) << 4) | (page & 0x0F), memoryType);
			} else {
				BaseMapper::SelectPRGPage(slot, page & 0x1F, memoryType);
			}
		} else {
			BaseMapper::SelectPRGPage(slot, page, memoryType);
		}
    }

	void UpdateChrMapping() override
	{
		switch(_romInfo.SubMapperID) {
			case 0:
			case 3:
			default:
				SelectChrPage4x(0, _registers[0] << 1);
				SelectChrPage2x(2, _registers[2] << 1);
				SelectChrPage2x(3, _registers[3] << 1);
				break;
			case 1: // NOTE: couldn't test this submapper (need ROM: Super Fighter III UNL-FC-28-5027)
				SelectChrPage4x(0, _registers[1] << 1);
				SelectChrPage2x(2, _registers[4] << 1);
				SelectChrPage2x(3, _registers[5] << 1);
				break;
			case 2:
				SelectChrPage2x(0, (_registers[0] & 0xFE) << 1);
				SelectChrPage2x(1, (_registers[1] | 0x01) << 1);
				SelectChrPage2x(2, _registers[2] << 1);
				SelectChrPage2x(3, _registers[5] << 1);
				break;
		}
	}

	void WriteRegister(uint16_t addr, uint8_t value) override {
		if(((addr & 0xE000) == 0x6000) && (_state.RegA001 & 0x80)) {
			sub3OuterBankReg = value;
			UpdateState();
		} else {
			MMC3::WriteRegister(addr, value);
		}
	}

	void InitMapper() override
	{
		sub3OuterBankReg = 0;
		MMC3::InitMapper();
	}

	void Reset(bool softReset) override
	{
		if(softReset) {
			sub3OuterBankReg = 0;
		}
	}

	void StreamState(bool saving) override
	{
		Stream(sub3OuterBankReg);
		MMC3::StreamState(saving);
	}
	
};