#pragma once
#include "stdafx.h"
#include "BaseMapper.h"
#include "MemoryManager.h"
#include "CPU.h"

class Mapper368 : public BaseMapper // YUNG-08's SMB2 port
{
private:
    uint8_t _latch;
    uint16_t _irqCounter;
protected:
    uint16_t GetPRGPageSize() override { return 0x2000; }
    uint16_t GetCHRPageSize() override { return 0x2000; }
    uint16_t RegisterStartAddress() override { return 0x4022; }
    uint16_t RegisterEndAddress() override { return 0x4F22; }
    bool AllowRegisterRead() override { return true; }

    void InitMapper() override
    {
        SetCpuMemoryMapping(0x6000, 0x7FFF, 2, PrgMemoryType::PrgRom);
        SetCpuMemoryMapping(0x8000, 0x9FFF, 1, PrgMemoryType::PrgRom);
        SetCpuMemoryMapping(0xA000, 0xBFFF, 0, PrgMemoryType::PrgRom);
        SetCpuMemoryMapping(0xC000, 0xDFFF, 4, PrgMemoryType::PrgRom); // value 0 = bank 4 (but not sure about the init. value)
        SetCpuMemoryMapping(0xE000, 0xFFFF, 8, PrgMemoryType::PrgRom);
        SelectCHRPage(0, 0);

        _latch = 0;
        _irqCounter = 0;
    }

    void WriteRegister(uint16_t addr, uint8_t value) override
    {
        switch(addr & 0xF1FF) {
            case 0x4022:
                switch(value & 0x07) {
                    case 0: SetCpuMemoryMapping(0xC000, 0xDFFF, 4, PrgMemoryType::PrgRom); break;
                    case 1: SetCpuMemoryMapping(0xC000, 0xDFFF, 3, PrgMemoryType::PrgRom); break;
                    case 2: SetCpuMemoryMapping(0xC000, 0xDFFF, 5, PrgMemoryType::PrgRom); break;
                    case 3: SetCpuMemoryMapping(0xC000, 0xDFFF, 3, PrgMemoryType::PrgRom); break;
                    case 4: SetCpuMemoryMapping(0xC000, 0xDFFF, 6, PrgMemoryType::PrgRom); break;
                    case 5: SetCpuMemoryMapping(0xC000, 0xDFFF, 3, PrgMemoryType::PrgRom); break;
                    case 6: SetCpuMemoryMapping(0xC000, 0xDFFF, 7, PrgMemoryType::PrgRom); break;
                    case 7: SetCpuMemoryMapping(0xC000, 0xDFFF, 3, PrgMemoryType::PrgRom); break;
                }
                break;
            case 0x4122:
                _latch = value;

                if(!(value & 0x01)) {
                    _irqCounter = 0;
                    _console->GetCpu()->ClearIrqSource(IRQSource::External);
                }
                break;
            default:
                break;
        }
    }

    uint8_t ReadRegister(uint16_t addr) override
    {
        if((addr & 0xF1FF) == 0x4122) {
            return 0x8A | (_latch & 0x35);
        } else {
            return _console->GetMemoryManager()->GetOpenBus();
        }
    }

    void ProcessCpuClock() override
    {
        if(_latch & 0x01) {
            if(!((++_irqCounter) & 0x0FFF)) {
                _irqCounter = 0;
                _console->GetCpu()->SetIrqSource(IRQSource::External);
            }
        }
    }
};