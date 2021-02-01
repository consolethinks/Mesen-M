#pragma once

#include "stdafx.h"
#include "MMC3.h"

class MMC3_534 : public MMC3
{
private:
	uint8_t _extRegs[4];

protected:
    uint32_t GetDipSwitchCount() override { return 2; }
	bool AllowRegisterRead() override { return true; }

    void UpdateChrMapping() override
    {
        if(_extRegs[3] & 0x10) {
            // CNROM mode
            for(uint8_t i = 0; i < 8; ++i) SelectCHRPage(i, ((_extRegs[2] & 0x0F) << 3) | i);
        } else {
            // MMC3 mode
            MMC3::UpdateChrMapping();
        }
    }

	void UpdatePrgMapping() override
	{
		switch(_extRegs[3] & 0x03) {
            case 0:     // MMC3 mode
			    MMC3::UpdatePrgMapping();
                break;
            case 1:     // NROM-128 mode
            case 2:
			    SelectPRGPage(0, (_registers[6] & 0xFE));
                SelectPRGPage(1, (_registers[6] & 0xFE)+1);
                SelectPRGPage(2, (_registers[6] & 0xFE));
			    SelectPRGPage(3, (_registers[6] & 0xFE)+1);
                break;
            case 3:     // NROM-256 mode
                for (int i = 0; i < 4; ++i) SelectPRGPage(i, (_registers[6] & 0xFC)+i);
                break;
		}
	}

	void SelectCHRPage(uint16_t slot, uint16_t page, ChrMemoryType memoryType = ChrMemoryType::Default) override
	{
		if(_extRegs[0] & 0x80) {
            // 128KiB inner bank
			BaseMapper::SelectCHRPage(slot, (page & 0x7F) | ((_extRegs[0] & 0x38) << 4), memoryType);
		} else {
            // 256KiB inner bank
			BaseMapper::SelectCHRPage(slot, (page & 0xFF) | ((_extRegs[0] & 0x30) << 4), memoryType);
		}
	}

	void SelectPRGPage(uint16_t slot, uint16_t page, PrgMemoryType memoryType = PrgMemoryType::PrgRom) override
	{
        if(_extRegs[0] & 0x40) {
            // 128KiB inner bank
            BaseMapper::SelectPRGPage(slot, (page & 0x0F) | ((_extRegs[0] & 0x07) << 4) | ((_extRegs[0] & 0x30) << 3), memoryType);
        } else {
            // 256KiB inner bank
            BaseMapper::SelectPRGPage(slot, (page & 0x1F) | ((_extRegs[0] & 0x06) << 4) | ((_extRegs[0] & 0x30) << 3), memoryType);
        }
	}

	void InitMapper() override 
	{
        for(int i = 0; i < 4; ++i) _extRegs[i] = 0;
        AddRegisterRange(0x6000, 0x7fff, MemoryOperation::Write);
		MMC3::InitMapper();
		MMC3::WriteRegister(0xA001, 0x80); // MMC3 WRAM is ALWAYS enabled in this mapper
	}

	void Reset(bool softReset) override 
	{
		if(softReset) {
            // MMC3 regs won't get reset in a soft reset
			for (int i = 0; i < 4; ++i) _extRegs[i] = 0;
			UpdateState();
		}
	}
	
	void StreamState(bool saving) override
	{
		MMC3::StreamState(saving);
		Stream(_extRegs[0], _extRegs[1], _extRegs[2], _extRegs[3]);
	}

    uint8_t ReadRegister(uint16_t addr) override
	{
		if(_extRegs[1] & 0x01) {
			return GetDipSwitches();
		} else {
			return BaseMapper::InternalReadRam(addr);
		}
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		BaseMapper::WritePrgRam(addr, value);
		if((addr & 0xE000) == 0x6000) // external registers (0xE003 mask)
        {
            uint8_t regNum = (addr & 0x0003);
            if ((!(_extRegs[3] & 0x80)) || (regNum == 2)) { // check for lock bit
                _extRegs[regNum] = value;
                UpdateState();
            }
        } else switch(addr & 0xE001) { // MMC3 regs. (0xE001 mask)
			case 0xC000:
            	MMC3::WriteRegister(addr, ~value); // C000 needs inverted value
				break;
			case 0xA001:
				// writes to this register are disabled, WRAM is *always* on
				break;
			default:
            	MMC3::WriteRegister(addr, value);
        }
	}
};
