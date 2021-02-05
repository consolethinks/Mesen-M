#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Waixing162 : public BaseMapper
{
protected:
    uint8_t regs[4];
    bool ppuA9Latch;
    bool ppuA13Latch;

    uint16_t GetPRGPageSize() override { return 0x8000; }
	uint16_t GetCHRPageSize() override { return 0x2000; }
    uint16_t RegisterStartAddress() override { return 0x5000; }
    uint16_t RegisterEndAddress() override { return 0x53FF; }
    bool AllowRegisterRead() override { return true; }

    uint8_t SwapBits(uint8_t x)
    {
        return ((x & 0xFC) | ((x & 0x01) << 1) | ((x & 0x02) >> 1));
    }

    virtual void UpdateState()
    {
        SelectPRGPage(0, ((regs[2] & 0x03) << 4) // upper prg reg
                        | (regs[0] & 0x0F) // lower prg reg
                        | ((regs[3] & 0x01) ? (regs[1] & 0x01) : ((regs[1] & 0x02) >> 1)) //pin4
                        | ((regs[3] & 0x04) ? 0x00 : 0x03)); // $5300.2 flag
    }

    void InitMapper() override
    {
        RemoveRegisterRange(0x5000, 0x50FF, MemoryOperation::Read);
        RemoveRegisterRange(0x5200, 0x53FF, MemoryOperation::Read);
        AddRegisterRange(0x5500, 0x55FF, MemoryOperation::Read);

        SelectCHRPage(0, 0);
        memset(regs, 0, 4);
        ppuA9Latch = false;
        ppuA13Latch = false;
        UpdateState();
    }

    void StreamState(bool saving) override
    {
        BaseMapper::StreamState(saving);
        Stream(regs[0], regs[1], regs[2], regs[3], ppuA9Latch, ppuA13Latch);
    }

    uint8_t ReadRegister(uint16_t addr) override
    {
        return (regs[1] & 0x04);
    }

    virtual void WriteRegister(uint16_t addr, uint8_t value) override
    {
        if(regs[3] & 0x01 && ((addr & 0xFF00) != 0x5300)) {
            regs[(addr & 0x0300) >> 8] = SwapBits(value);
        } else {
            regs[(addr & 0x0300) >> 8] = value;
        }

        // if PRG size is 1MiB, then unify A19/A20
        if(_prgSize == 1024*1024) {
            regs[2] = regs[2] ? 0x01 : 0x00;
        }
        
        UpdateState();
    }

public:
    void Reset(bool softReset) override
    {
        if(softReset) {
            memset(regs, 0, 4);
            UpdateState();
        }
    }

    void NotifyVRAMAddressChange(uint16_t addr) override
    {
        bool newA13 = (addr & 0x2000);
        if(!ppuA13Latch && newA13) {
            ppuA9Latch = (addr & 0x0200);
        }
        ppuA13Latch = newA13;
    }

    uint8_t MapperReadVRAM(uint16_t addr, MemoryOperationType operationType) override
    {
        if(!(addr & 0x2000) && (regs[0] & 0x80)) {
            return InternalReadVRAM((ppuA9Latch ? 0x1000 : 0x0000) | (addr & 0x0FFF));
        } else {
            return InternalReadVRAM(addr);
        }
    }
};