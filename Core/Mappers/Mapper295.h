#pragma once
#include "JyCompany.h"

class Mapper295 : public JyCompany
{
protected:
    uint8_t PrgMaskDefault() override { return 0x0F; } // (PRG 128K, CHR 128K)
	uint8_t PrgShiftDefault() override { return 4; }
    uint16_t ChrMaskDefault() override { return 0x007F; }
	uint8_t ChrShiftDefault() override { return 7; }

    void D003Write(uint8_t value) override
    {
        _outerPrgReg = _outerChrReg = (value & 0x07);
    }
};