#include "MMC3.h"

class Realtec8210 : public MMC3
{
private:
    uint8_t _outerPrgBank;
    uint8_t _outerChrBank;
    uint8_t _prgMask;
    uint16_t _chrMask;
    bool _lock;

public:
    void InitMapper() override
    {
        _outerPrgBank = 0;
        _outerChrBank = 0;
        _prgMask = 0x1F;
        _chrMask = 0xFF;
        _lock = false;

        AddRegisterRange(0x6000,0x7FFF, MemoryOperation::Write);

        MMC3::InitMapper();
    }

    void Reset(bool softReset) override
    {
        if(softReset) {
            _outerPrgBank = 0;
            _outerChrBank = 0;
            _prgMask = 0x1F;
            _chrMask = 0xFF;
            _lock = false;

            MMC3::UpdateState();
        }
    }

    void StreamState(bool saving) override
    {
        Stream(_outerPrgBank, _outerChrBank, _prgMask, _chrMask, _lock);
        MMC3::StreamState(saving);
    }

    void SelectPRGPage(uint16_t slot, uint16_t page, PrgMemoryType memoryType = PrgMemoryType::PrgRom) override
    {
        if(memoryType == PrgMemoryType::PrgRom) {
            BaseMapper::SelectPRGPage(slot, ((_outerPrgBank << 4) & ~_prgMask) | (page & _prgMask), memoryType);
        } else {
            BaseMapper::SelectPRGPage(slot, page, memoryType);
        }
    }

    void SelectCHRPage(uint16_t slot, uint16_t page, ChrMemoryType memoryType = ChrMemoryType::Default) override
    {
        if((memoryType == ChrMemoryType::ChrRom) || (memoryType == ChrMemoryType::Default)) {
            BaseMapper::SelectCHRPage(slot, ((_outerChrBank << 7) & ~_chrMask) | (page & _chrMask), memoryType);
        } else {
            BaseMapper::SelectCHRPage(slot, page, memoryType);
        }
    }

    void WriteRegister(uint16_t addr, uint8_t value) override
    {
        if(addr < 0x8000) {
            WritePrgRam(addr, value);
            if(!_lock) {
                if((addr & 0xE010) == 0x6000) {
                    _outerPrgBank = (value & 0x08) | ((value & 0x30) >> 3) | (_outerPrgBank & 0x01);
                    _outerChrBank = (_outerChrBank & 0x80) | ((value & 0x30) >> 3) | (_outerChrBank & 0x01);

                    MMC3::UpdateState();
                } else if((addr & 0xE010) == 0x6010) {
                    _outerPrgBank = (_outerPrgBank & 0x0E) | (value & 0x01);
                    _outerChrBank = ((value & 0x20) >> 2) | (_outerChrBank & 0x06) | ((value & 0x10) >> 4);
                    _prgMask = (value & 0x08) ? 0x0F : 0x1F;
                    _chrMask = (value & 0x40) ? 0x7F : 0xFF;
                    _lock = value & 0x80;

                    MMC3::UpdateState();
                }
            }
        } else {
            MMC3::WriteRegister(addr, value);
        }
    }
};