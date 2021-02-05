#pragma once
#include "Waixing162.h"

class Nanjing163 : public Waixing162
{
protected:
    void UpdateState() override
    {
        SelectPRGPage(0, ((regs[2] & 0x03) << 4) | (regs[0] & 0x0F) | (regs[3] & 0x04 ? 0x03 : 0x00));
    }

    void WriteRegister(uint16_t addr, uint8_t value) override
    {
        if((addr & 0xFF00) == 0x5100) {
            bool pin1Old = ((regs[3] & 0x01) ? SwapBits(regs[1]) : regs[1]) & 0x01;
            bool pin1New = ((regs[3] & 0x01) ? SwapBits(value) : value) & 0x01;
            bool pin4 = ((regs[3] & 0x01) ? SwapBits(value) : value) & 0x02;
            bool pin31 = (regs[1] & 0x04);
            
            // pin31 update
            if(addr & 0x0001) {
                if(pin1Old && !pin1New) {
                    pin31 = !pin31;
                }
            } else {
                pin31 = !(value & 0x04);
            }
            
            regs[1] = (int(pin31) << 2) | (int(pin4) << 1) | int(pin1New);
        } else {
            Waixing162::WriteRegister(addr, value);
        }
    }
};

 