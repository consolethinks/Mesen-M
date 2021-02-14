#pragma once
#include "JyCompany.h"

class Jy358 : public JyCompany
{
protected:
    uint8_t PrgMaskDefault() override { return 0x1F; } // (PRG 256K, CHR 256/512K)
	uint8_t PrgShiftDefault() override { return 4; }
    uint16_t ChrMaskDefault() override { return 0x00FF; }
	uint8_t ChrShiftDefault() override { return 8; }

    void D003Write(uint8_t value) override
    {
        _outerPrgReg = (value & 0x07);
        _outerChrReg = ((value & 0x04) >> 1) | (value & 0x01);
        _chrMask = (value & 0x20) ? 0x01FF : 0x00FF;
    }
};