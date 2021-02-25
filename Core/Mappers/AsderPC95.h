#pragma once
#include "BaseMapper.h"
#include "AsderKeyboard.h"
#include "stdafx.h"

class AsderPC95 : public BaseMapper
{
private:
    shared_ptr<AsderKeyboard> keyboard;
protected:
    uint16_t GetPRGPageSize() override { return 0x2000; }
    uint32_t GetWorkRamPageSize() override { return 0x2000; }
    uint16_t GetCHRPageSize() override { return 0x400; }
    bool AllowRegisterRead() override { return true; }

    void InitMapper() override
    {
        SetCpuMemoryMapping(0x6000, 0x7FFF, 0, PrgMemoryType::WorkRam, MemoryAccessType::ReadWrite);
        for(int i = 0; i < 4; ++i) { WriteRegister(0x8000 | i, i | 0xFC); }
        for(int i = 0; i < 8; ++i) { WriteRegister(0xA000 | i, i); }
        WriteRegister(0xC000, 0);

        RemoveRegisterRange(0x8000, 0xFFFF, MemoryOperation::Read);
        AddRegisterRange(0x4111, 0x491F, MemoryOperation::Write);
        AddRegisterRange(0x4902, 0x4903, MemoryOperation::Read);
        AddRegisterRange(0x4902, 0x4903, MemoryOperation::Read);
        AddRegisterRange(0x4906, 0x4906, MemoryOperation::Read);
        AddRegisterRange(0x4910, 0x491F, MemoryOperation::Read);
        AddRegisterRange(0x5000, 0x5003, MemoryOperation::Read);
        AddRegisterRange(0x5080, 0x5083, MemoryOperation::Read);

        keyboard.reset(new AsderKeyboard(_console, _console->GetSettings()->GetControllerKeys(0)));
        _mapperControlDevice = keyboard;
    }

    void WriteRegister(uint16_t addr, uint8_t value) override
    {
        if(addr == 0x4900) {
            // printer output byte
        } else if(addr == 0x4901) {
            // printer output byte
        } else if(addr == 0x4904) {
            keyboard->WriteRAM(addr, value);
        } else if((0x4910 <= addr && addr <= 0x491F) || addr == 0x4111) {
            // unknown regs
        } else {
            switch(addr & 0xE000) {
                case 0x8000:
                    SelectPRGPage(addr & 0x0003, value | ((addr & 3) == 3 ? 0x01 : 0x00)); break;
                    break;
                case 0xA000:
                    SelectCHRPage(addr & 0x0007, value); break;
                    break;
                case 0xC000: case 0xE000:
                    // mirroring?
                    break;
                default: break;
            }
        }
    }

    uint8_t ReadRegister(uint16_t addr) override
    {
        switch(addr) {
            case 0x4902: return 0x10; // printer unimplemented -> busy signal
            case 0x4906: return keyboard->ReadRAM(addr); // keyboard button state (like subor, for now unimplemented)
            default: return 0x00; // many unknown registers
        }
    }
};