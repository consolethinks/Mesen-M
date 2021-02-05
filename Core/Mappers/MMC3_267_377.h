#pragma once
#include "stdafx.h"
#include "MMC3.h"
// 267: JY-119 multicart specific mapper
// 377: JY-111/JY-112 multicart specific mapper
// Bits 6 and 7 in the outer bank reg has unknown functions
// Outer reg is $6000, unknown mask
class MMC3_267_377 : public MMC3 // Mapper 267/377
{
private:
    uint8_t outerBankReg;
protected:
    uint16_t RegisterStartAddress() override { return 0x6000; }

    void SelectPRGPage(uint16_t slot, uint16_t page, PrgMemoryType memoryType = PrgMemoryType::PrgRom) override {
        if(_romInfo.MapperID == 267) {
            BaseMapper::SelectPRGPage(slot, ((outerBankReg & 0x20) << 2) | ((outerBankReg & 0x06) << 4) | (page & 0x1F), memoryType);
        } else { // As Mapper 377
            BaseMapper::SelectPRGPage(slot, ((outerBankReg & 0x20) << 1) | ((outerBankReg & 0x06) << 3) | (page & 0x0F), memoryType);
        }
    }

    void SelectCHRPage(uint16_t slot, uint16_t page, ChrMemoryType memoryType = ChrMemoryType::ChrRom) override {
        BaseMapper::SelectCHRPage(slot, ((outerBankReg & 0x20) << 4) | ((outerBankReg & 0x06) << 6) | (page & 0x7F), memoryType);
    }

    void StreamState(bool saving) override
    {
        MMC3::StreamState(saving);
        Stream(outerBankReg);
        if(!saving) {
            UpdateState();
        }
    }

    void InitMapper() override
	{
        outerBankReg = 0;
        MMC3::InitMapper();
	}

    void Reset(bool softReset) override
    {
        if(softReset) {
            outerBankReg = 0;
            UpdateState();
        }
    }

    void WriteRegister(uint16_t addr, uint8_t value) override
	{
        if(addr < 0x8000) {
            if(!(outerBankReg & 0x80) && (_state.RegA001 & 0x80)) { // outerBankReg.7 = maybe lock bit ??
                outerBankReg = value;
                UpdateState();
            }
        } else {
            MMC3::WriteRegister(addr, value);
        }
    }
};