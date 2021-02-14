#pragma once
#include "VRC2_4.h"
#include "MMC3.h"
#include "MMC1.h"

// SubMapper#0: Huang-1 (Somari-W)
// SubMapper#1: Huang-1 (Somari-P)
// SubMapper#2: Huang-2 (Somari-P)
// SubMapper#3: SL-FC5-1

class HuangX : public VRC2_4, public MMC3, public MMC1
{
private:
    uint8_t _supervisorReg;
    uint8_t _gameSelect; // for reset based multicarts built upon this mapper
protected:
    uint32_t GetSaveRamSize() override { return 0; } // no game uses save ram, since supervisor reg. in $4000-$7FFF range
    bool AllowRegisterRead() override { return false; } // no register reading

    uint16_t GetPRGPageSize() override
    {
        switch(_supervisorReg & 0x03) {
            case 0:  return VRC2_4::GetPRGPageSize();
            case 1:  return MMC3::GetPRGPageSize();
            default: return MMC1::GetPRGPageSize();
        }
    }

    uint16_t GetCHRPageSize() override
    {
        switch(_supervisorReg & 0x03) {
            case 0:  return VRC2_4::GetCHRPageSize();
            case 1:  return MMC3::GetCHRPageSize();
            default: return MMC1::GetCHRPageSize();
        }
    }

    void SetCpuMemoryMapping(uint16_t startAddr, uint16_t endAddr, int16_t pageNumber, PrgMemoryType type, int8_t accessType = -1) override
    {
        uint8_t prgOr;
        uint16_t prgAnd;

        if((_supervisorReg & 0x03) < 2 || _romInfo.SubMapperID != 2) {
            prgOr = _gameSelect ? ((_gameSelect + 1) << 4) : 0x00;
            prgAnd = _gameSelect ? 0x0F : 0x1F;
        }
        if((_supervisorReg & 0x03) >= 2) {
            if(_romInfo.SubMapperID == 2) { // For AV Kyuukyoku Mahjong 2 (Asia) (Unl) (haven't been able to test it)
                // masking is disabled in SubMapper 2 in MMC1 mode
                prgOr = 0x00;
                prgAnd = 0xFFFF;
                // values written are shifted once to the right, correcting it is necessary
                pageNumber >>= 1;
            } else {
                prgOr >>= 1;
                prgAnd >>= 1;
            }
        }

        BaseMapper::SetCpuMemoryMapping(startAddr, endAddr, prgOr | (pageNumber & prgAnd), type, accessType);
    }

    void SetPpuMemoryMapping(uint16_t startAddr, uint16_t endAddr, uint16_t pageNumber, ChrMemoryType type = ChrMemoryType::Default, int8_t accessType = -1) override
    {
        uint16_t chrOr;
        uint16_t chrAnd;

        if((_supervisorReg & 0x03) < 2 || _romInfo.SubMapperID != 2) {
            chrOr = (_gameSelect ? ((_gameSelect + 1) << 7) : 0x00) | ((_supervisorReg & 0x04) << 6);
            chrAnd = _gameSelect ? 0x7F : 0xFF;
        }
        if((_supervisorReg & 0x03) >= 2) {
            if(_romInfo.SubMapperID == 2) {
                // masking is disabled in SubMapper 2 while in MMC1 mode
                chrOr = 0x0000;
                chrAnd = 0xFFFF;
                // values written are shifted once to the right, correcting it is necessary
                pageNumber >>= 1;
            } else {
                chrOr >>= 1;
                chrAnd >>= 1;
            }
        }

        BaseMapper::SetPpuMemoryMapping(startAddr, endAddr, chrOr | (pageNumber & chrAnd), type, accessType);
    }

    void UpdateStateHuang()
    {
        switch(_supervisorReg & 0x03) {
            case 0: VRC2_4::UpdateState(); break;
            case 1: MMC3::UpdateState(); 
                    MMC3::UpdateMirroring(); break;
            case 2:
            case 3: MMC1::UpdateState(); break;
        }
    }

    void InitMapper() override
    {
        AddRegisterRange(0x4100, 0x5FFF, MemoryOperation::Write);

        VRC2_4::InitMapperHuang();
        MMC3::InitMapper();
        MMC1::InitMapper();

        _supervisorReg = 1; // always start in MMC3 mode

        UpdateStateHuang();
    }

    void WriteRegister(uint16_t addr, uint8_t value) override
    {
        if((addr & 0xC100) == 0x4100) {
            _supervisorReg = value;
            
            if((value & 0x03) >= 2 && _romInfo.SubMapperID != 1) {
                MMC1::WriteRegister(0x8000, 0x80); // reset shift register for Somari
            }
            if((value & 0x03) != 1) {
                _console->GetCpu()->ClearIrqSource(IRQSource::External); // IRQ is disabled when switched away from MMC3
                _irqEnabled = false; // Kart Fighter *definitely* requires the IRQ to be disabled
            }

            UpdateStateHuang();
        } else {
            switch(_supervisorReg & 0x03) {
                case 0: if(!(addr >= 0xF000)) { VRC2_4::WriteRegister(addr, value); } break; // no irq writes allowed
                case 1: MMC3::WriteRegister(addr, value); break;
                case 2:
                case 3: MMC1::WriteRegister(addr, value); break;
            }
        }
    }

    void StreamState(bool saving) override
    {
        VRC2_4::StreamState(saving);
        MMC3::StreamState(saving);
        MMC1::StreamState(saving);
        Stream(_supervisorReg, _gameSelect);
        if(!saving) {
            if((_supervisorReg & 0x03) != 1) {
                _console->GetCpu()->ClearIrqSource(IRQSource::External);
                _irqEnabled = false;
            }
            UpdateStateHuang();
        }
    }

    void Reset(bool softReset) override
    {
        if(softReset) {
            if(_romInfo.SubMapperID == 3) {
                _gameSelect = (_gameSelect + 1) % 5;
                InitMapper();
            }
        }
    }

public:
    void NotifyVRAMAddressChange(uint16_t addr) override
    {
        if((_supervisorReg & 0x03) == 1) {
            MMC3::NotifyVRAMAddressChange(addr);
        }
    }
};
