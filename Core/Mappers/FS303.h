#pragma once
#include "MMC3.h"

class FS303 : public MMC3
{
private: // supposed to be Mapper 195 (roms?)
    bool disableChrRam;
    bool smallChrRam;
    uint8_t chrBankToRam;

protected:
    uint16_t GetChrRamPageSize() override { return 0x400; }
	uint32_t GetChrRamSize() override { return 4 * 0x400; }

    void SetPpuMemoryMapping(uint16_t startAddr, uint16_t endAddr, uint16_t pageNumber, ChrMemoryType type = ChrMemoryType::Default, int8_t accessType = -1) override
    {
        ChrMemoryType memType = ChrMemoryType::ChrRom;
        if(!disableChrRam) {
            if((pageNumber & (smallChrRam ? 0xFE : 0xFC)) == chrBankToRam) {
                memType = ChrMemoryType::ChrRam;
            }
        }
        BaseMapper::SetPpuMemoryMapping(startAddr, endAddr, pageNumber, memType, 
            memType == ChrMemoryType::ChrRam ? MemoryAccessType::ReadWrite : MemoryAccessType::Read);
    }

    void InitMapper() override
    {
        disableChrRam = false;
        smallChrRam = false;
        chrBankToRam = 0x28;

        MMC3::InitMapper();
    }

public:
    void WriteVRAM(uint16_t addr, uint8_t value) override
    {
        BaseMapper::WriteVRAM(addr, value);
        MessageManager::DisplayMessage("CHRW", std::to_string(addr));

        if(value & 0x80) {
            uint8_t bank = (value & 0x40) | (value & 0x04) | (value & 0x02);
            switch(bank) {
                case 0x00: chrBankToRam = 0x28; break;
                case 0x02: chrBankToRam = 0x00; break;
                case 0x08: chrBankToRam = 0x4C; break;
                case 0x0A: chrBankToRam = 0x64; break;
                case 0x40: chrBankToRam = 0x46; break;
                case 0x42: chrBankToRam = 0x7C; break;
                case 0x48: chrBankToRam = 0x0A; break;
                case 0x4A: chrBankToRam = 0xFF; break;
            }

            if(chrBankToRam == 0xFF || (value & 0x10)) {
                disableChrRam = true;
            } else {
                disableChrRam = false;
            }

            smallChrRam = (value & 0x40) ? true : false;
        }
        UpdateState();
    }
};