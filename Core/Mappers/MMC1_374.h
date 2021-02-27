#pragma once
#include "MMC1.h"

class MMC1_374 : public MMC1
{
private:
    uint8_t gameSelect;
protected :
    void InitMapper() override
    {
        gameSelect = 0;

        MMC1::InitMapper();
    }

    void Reset(bool softReset) override
    {
        if(softReset) {
            gameSelect = (gameSelect + 1) % 4;
            MMC1::InitMapper();
        }
    }

    void StreamState(bool saving) override
    {
        Stream(gameSelect);
        MMC1::StreamState(saving);
    }

	void SetCpuMemoryMapping(uint16_t startAddr, uint16_t endAddr, int16_t pageNumber, PrgMemoryType type, int8_t accessType = -1) override
    {
        BaseMapper::SetCpuMemoryMapping(startAddr, endAddr, (gameSelect << 3) | (pageNumber & 0x07), type, accessType);
    }

    void SetPpuMemoryMapping(uint16_t startAddr, uint16_t endAddr, uint16_t pageNumber, ChrMemoryType type = ChrMemoryType::Default, int8_t accessType = -1) override
    {
        BaseMapper::SetPpuMemoryMapping(startAddr, endAddr, (gameSelect << 5) | (pageNumber & 0x1F), type, accessType);
    }
};