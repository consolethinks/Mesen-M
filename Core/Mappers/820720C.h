#pragma once
#include "stdafx.h"
#include "MMC3.h"

class _820720C : public MMC3 // Mapper 393
{
private:
    uint8_t unromLatch;
    uint16_t addrLatch;

protected:
    uint32_t GetChrRamSize() override { return 0x2000; }
    uint16_t RegisterStartAddress() override { return 0x6000; }
    uint32_t GetSaveRamSize() override { return 0x0000; }
    uint32_t GetWorkRamSize() override { return 0x0000; }

    void SelectPRGPage(uint16_t slot, uint16_t page, PrgMemoryType memoryType = PrgMemoryType::PrgRom) override {
        BaseMapper::SelectPRGPage(slot, ((addrLatch & 0x07) << 4) | (page & 0x0F), memoryType);
    }

    void SelectCHRPage(uint16_t slot, uint16_t page, ChrMemoryType memoryType = ChrMemoryType::ChrRom) override {
        BaseMapper::SelectCHRPage(slot, ((addrLatch & 0x07) << 8) | (page & 0xFF), memoryType);
    }

    void UpdatePrgMapping() override
    {
        switch((addrLatch >> 4) & 0x0003) {
            case 0:
            case 1: // MMC3
                MMC3::UpdatePrgMapping();
                break;
            case 2: // BNROM
                SelectPRGPage(0, (_registers[6] & 0x0C));
                SelectPRGPage(1, (_registers[6] & 0x0C) | 0x01);
                SelectPRGPage(2, (_registers[6] & 0x0C) | 0x02);
                SelectPRGPage(3, (_registers[6] & 0x0C) | 0x03);
                break;
            case 3: // UNROM
                SelectPRGPage(0, ((unromLatch & 0x07) << 1));
                SelectPRGPage(1, ((unromLatch & 0x07) << 1) | 0x01);
                SelectPRGPage(2, 0x0E);
                SelectPRGPage(3, 0x0F);
                break;
        }
    }

    void UpdateChrMapping() override
    {
        if(addrLatch & 0x08) { // Unbanked 8K CHR-RAM
            SetPpuMemoryMapping(0x0000, 0x1FFF, 0, ChrMemoryType::ChrRam, MemoryAccessType::ReadWrite);
        } else { // MMC3 CHR-ROM
            MMC3::UpdateChrMapping();
        }
    }

    void StreamState(bool saving) override
    {
        MMC3::StreamState(saving);
        Stream(addrLatch, unromLatch);
        if(!saving) {
            UpdateState();
        }
    }

    void InitMapper() override
	{
        addrLatch = 0;
		unromLatch = 0;
        MMC3::InitMapper();
	}

    void Reset(bool softReset) override
    {
        if(softReset) {
            addrLatch = 0;
            unromLatch = 0;
            UpdateState();
        }
    }

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
        if(addr < 0x8000) {
            if(_state.RegA001 & 0x80) {
                addrLatch = addr;
                UpdateState();
            }
        } else {
            unromLatch = value;
            MMC3::WriteRegister(addr, value);
            UpdateState();
        }
	}
};
