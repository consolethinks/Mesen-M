#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Realtec8031 : public BaseMapper // New Mapper 217 Definition
{
private:
    uint8_t _regs[2];

protected:
	uint16_t GetPRGPageSize() override { return 0x4000; }
	uint16_t GetCHRPageSize() override { return 0x2000; }
    bool AllowRegisterRead() override { return true; }
    uint32_t GetDipSwitchCount() override { return 4; }
	
    void UpdateState()
    {
        switch((_regs[1] >> 4) & 0x03) {
            case 0:
            case 1:
                // 0: UNROM
                // 1: UNROM with DIP OR'd into address at read
                SelectPRGPage(0, _regs[1] & 0x0F);
                SelectPRGPage(1, (_regs[1] & 0x08) | 0x07);
                break;
            case 2:
                // this mode was unused on original cart!
                // assumed to be NROM-256
                SelectPrgPage2x(0, (_regs[1] & 0x0F) >> 1);
                break;
            case 3:
                // NROM-128
                SelectPRGPage(0, _regs[1] & 0x0F);
                SelectPRGPage(1, _regs[1] & 0x0F);
                break;
        }
        SelectCHRPage(0, _regs[0] & 0x0F);
        SetMirroringType((_regs[0] & 0x20) ? MirroringType::Horizontal : MirroringType::Vertical);
    }

    void StreamState(bool saving) override
    {
        BaseMapper::StreamState(saving);
        Stream(_regs[0], _regs[1]);
    }

    void InitMapper() override
	{
        _regs[0] = 0;
        _regs[1] = 0;
        UpdateState();
	}

    uint8_t ReadRegister(uint16_t addr) override
    {
        if ((_regs[1] & 0x30) == 0x10) {
            return BaseMapper::InternalReadRam(addr | BaseMapper::GetDipSwitches());
        } else {
            return BaseMapper::InternalReadRam(addr);
        }
    }

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
        switch(addr & 0xC000) {
            case 0x8000:
                _regs[0] = addr & 0x2F;
                UpdateState();
                break;
            case 0xC000:
                _regs[1] = addr & 0x3F;
                UpdateState();
                break;
        }
	}
};