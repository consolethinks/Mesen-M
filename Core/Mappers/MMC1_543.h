#pragma once
#include "MMC1.h"

class MMC1_543 : public MMC1
{
private:
    uint8_t _outerPrgReg;
    uint8_t _tempOuterPrgReg;
    uint8_t _outerShiftReg;
protected :
    void InitMapper() override
    {
        _outerPrgReg = 0;
        _tempOuterPrgReg = 0;
        _outerShiftReg = 0;
        AddRegisterRange(0x5000, 0x5FFF, MemoryOperation::Write);

        MMC1::InitMapper();
    }

    void Reset(bool softReset) override
    {
        if(softReset) {
            _outerPrgReg = 0;
            _outerShiftReg = 0;
            MMC1::InitMapper();
        }
    }

    void StreamState(bool saving) override
    {
        Stream(_outerPrgReg, _tempOuterPrgReg, _outerPrgReg);
        MMC1::StreamState(saving);
    }

	void SetCpuMemoryMapping(uint16_t startAddr, uint16_t endAddr, int16_t pageNumber, PrgMemoryType type, int8_t accessType = -1) override
    {
        if(type == PrgMemoryType::PrgRom) {
            BaseMapper::SetCpuMemoryMapping(startAddr, endAddr, ((_outerPrgReg & 0x07) << 4) | (pageNumber & 0x0F), PrgMemoryType::PrgRom, accessType);
        } else if(!(_outerPrgReg & 0x02)) {
            BaseMapper::SetCpuMemoryMapping(startAddr, endAddr, ((_outerPrgReg & 0x01) << 1) | (pageNumber & 0x01), PrgMemoryType::SaveRam, accessType);
        } else {
            BaseMapper::SetCpuMemoryMapping(startAddr, endAddr, 0x04 | ((_outerPrgReg & 0x04) >> 1) | (_outerPrgReg & 0x01), PrgMemoryType::SaveRam, accessType);
        }
    }

    void WriteRegister(uint16_t addr, uint8_t value) override
    {
        if((addr & 0xF000) == 0x5000) {
            _tempOuterPrgReg = (value & 0x08) | (_tempOuterPrgReg >> 1);
            
            if(++_outerShiftReg == 4) {
                _outerPrgReg = _tempOuterPrgReg;
                _outerShiftReg = 0;

                MMC1::UpdateState();
            }
        } else {
            MMC1::WriteRegister(addr, value);
        }
    }
};