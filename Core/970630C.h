#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class _970630C : public BaseMapper // Mapper 380
{
private:
    bool _returnDipSwitch;

protected:
	uint16_t GetPRGPageSize() override { return 0x4000; }
	uint16_t GetCHRPageSize() override { return 0x2000; }
    bool AllowRegisterRead() override { return true; }
    uint32_t GetDipSwitchCount() override { return 4; }

    void StreamState(bool saving) override
    {
        BaseMapper::StreamState(saving);
        Stream(_returnDipSwitch);
    }

    void InitMapper() override
	{
        WriteRegister(0, 0);
	}

    uint8_t ReadRegister(uint16_t addr) override
    {
        if (_returnDipSwitch) {
            return (uint8_t)BaseMapper::GetDipSwitches();
        } else {
            return BaseMapper::InternalReadRam(addr);
        }
    }

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
        uint8_t prgMode = ((addr >> 8) & 0x0002) | (addr & 0x0001);
        uint8_t prgAddr = ((addr >> 2) & 0x001F);
        switch(prgMode)
        {
            // UNROM-like
            case 0b00:
            case 0b01:
                SelectPRGPage(0, prgAddr);
                SelectPRGPage(1, prgAddr | 0x07);
                break;
            // NROM-256
            case 0b10:
                SelectPrgPage2x(0, prgAddr >> 1);
                break;
            // NROM-128
            case 0b11:
                SelectPRGPage(0, prgAddr);
                SelectPRGPage(1, prgAddr);
                break;
        }
        SetMirroringType((addr & 0x0002) ? MirroringType::Horizontal : MirroringType::Vertical);
        _returnDipSwitch = (bool)(addr & 0x0100);
	}
};