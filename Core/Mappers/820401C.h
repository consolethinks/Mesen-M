#pragma once

#include "stdafx.h"
#include "MMC3.h"

class _820401C : public MMC3 // Mapper 436 - 1993 Super HIK 6 in 1 (B-010)
{
private:
    uint8_t _latch;
protected:
    uint16_t RegisterStartAddress() override { return 0x6000; }

    void InitMapper() override
    {
        _latch = 0;
        MMC3::InitMapper();
    }

    void Reset(bool softReset) override
    {
        if(softReset) {
            _latch = 0;
            UpdateState();
        }
    }

    void StreamState(bool saving) override
    {
        Stream(_latch);
        MMC3::StreamState(saving);
    }

    void SelectPRGPage(uint16_t slot, uint16_t page, PrgMemoryType memoryType = PrgMemoryType::PrgRom) override
    {
        if(_latch & 0x08) {
            BaseMapper::SelectPRGPage(slot, ((_latch & 0xF0) >> 2) | (page & 0x03), memoryType); // 32K PRG bank
        } else {
            BaseMapper::SelectPRGPage(slot, ((_latch & 0xC0) >> 2) | (page & 0x0F), memoryType); // 128K PRG bank
        }
        
    }

    void SelectCHRPage(uint16_t slot, uint16_t page, ChrMemoryType memoryType = ChrMemoryType::Default) override
    {
        BaseMapper::SelectCHRPage(slot, ((_latch & 0xC0) << 1) | (page & 0x7F), memoryType);
    }

    void UpdatePrgMapping() override
    {
        if(_latch & 0x01) {
            // MMC3 with 128K PRG
            MMC3::UpdatePrgMapping();
        } else {
            // NROM-256
            for(int i = 0; i < 4; ++i) {
                SelectPRGPage(i, ((_latch & 0x30) >> 2) | i);
            }
        }
    }

    void WriteRegister(uint16_t addr, uint8_t value) override
    {
        if(addr < 0x8000) {
            if((_state.RegA001 & 0xC0) == 0x80) {
                _latch = value;
                MMC3::UpdateState();
            }
        } else {
            MMC3::WriteRegister(addr, value);
        }
    }
};