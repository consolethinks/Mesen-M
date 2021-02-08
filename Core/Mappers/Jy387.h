#pragma once
#include "JyCompany.h"

class Jy387 : public JyCompany
{
protected:
    uint8_t PrgMaskDefault() override { return 0x0F; } // (PRG 128K, CHR 256K)
	uint8_t PrgShiftDefault() override { return 4; }

    void D003Write(uint8_t value) override
    {
        _outerPrgReg = (((value & 0x08) >> 2) | ((value & 0x02) >> 1));
        _outerChrReg = (((value & 0x04) >> 1) | (value & 0x01));
		_chrMask = (value & 0x20) ? 0x01FF : 0x00FF; // switches CHR outer bank to 512K
    }
};