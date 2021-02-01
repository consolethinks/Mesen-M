#pragma once
#include "JyCompany.h"

class Mapper281 : public JyCompany
{
protected:
    uint8_t PrgMaskDefault() override { return 0x1F; } // (PRG 256K, CHR 256K)
	uint8_t PrgShiftDefault() override { return 5; }

    void D003Write(uint8_t value) override
    {
        _outerPrgReg = _outerChrReg = value & 0x03;
    }
};