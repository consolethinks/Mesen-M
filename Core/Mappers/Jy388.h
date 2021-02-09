#pragma once
#include "JyCompany.h"

class Jy388 : public JyCompany
{
protected:
    uint8_t PrgMaskDefault() override { return 0x1F; } // (PRG 256K, CHR 256K)
	uint8_t PrgShiftDefault() override { return 5; }
    bool InhibitRomNtExtMir() override { return true; }

    void D003Write(uint8_t value) override
    {
        _outerPrgReg = ((value & 0x0C) >> 2);
        _outerChrReg = (value & 0x03);
		_chrMask = (value & 0x20) ? 0x01FF : 0x00FF; // switches CHR outer bank to 512K
    }
};