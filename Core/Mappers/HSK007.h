#pragma once
#include "BaseMapper.h"
#include "MMC3.h"
#include "JyCompany.h"

class HSK007 : public MMC3, public JyCompany
{
private:
    uint8_t _mode;
    uint8_t _prgOuterBank;
    uint8_t _chrOuterBank;

    bool _prgA17FromMMC3;
    bool _chrA17FromMMC3;

    uint8_t _bnRomPrgBank;
protected:
    uint16_t GetPRGPageSize() override { return 0x2000; }
	uint16_t GetCHRPageSize() override { return 0x0400; }

    // unsure if a reset method that resets the multicart to the initial state would be accurate

    void StreamState(bool saving) override
    {
        JyCompany::StreamState(saving);
        // MMC3 streaming (rewritten here to avoid calling BaseMapper's StreamState twice)
        ArrayInfo<uint8_t> registers = { MMC3::_registers, 8 };
		SnapshotInfo a12Watcher{ &_a12Watcher };
		Stream(_state.Reg8000, _state.RegA000, _state.RegA001, _currentRegister, MMC3::_chrMode, MMC3::_prgMode,
				_irqReloadValue, MMC3::_irqCounter, _irqReload, MMC3::_irqEnabled, a12Watcher,
				_wramEnabled, _wramWriteProtected, registers);
        
        Stream(_mode, _prgOuterBank, _chrOuterBank, _prgA17FromMMC3, _chrA17FromMMC3, _bnRomPrgBank);
        
        if(!saving) {
            UpdateStateHSK();
        }
    }
    
    void SetCpuMemoryMapping(uint16_t startAddr, uint16_t endAddr, int16_t pageNumber, PrgMemoryType memoryType, int8_t accessType = -1) override
    {
        if(memoryType == PrgMemoryType::PrgRom) {
            if((_mode == 1 && !_prgA17FromMMC3) || _mode == 0) {
                BaseMapper::SetCpuMemoryMapping(startAddr, endAddr, (_prgOuterBank << 4) | (pageNumber & 0x0F), memoryType, accessType);
            } else {
                BaseMapper::SetCpuMemoryMapping(startAddr, endAddr, ((_prgOuterBank & 0x02) << 4) | (pageNumber & 0x1F), memoryType, accessType);
            }
        } else {
            BaseMapper::SetCpuMemoryMapping(startAddr, endAddr, pageNumber, memoryType, accessType);
        }
    }
    
    void SetPpuMemoryMapping(uint16_t startAddr, uint16_t endAddr, uint16_t pageNumber, ChrMemoryType memoryType = ChrMemoryType::Default, int8_t accessType = -1) override
    {
        if(memoryType == ChrMemoryType::Default || memoryType == ChrMemoryType::ChrRom) {
            if(_mode <= 1 && !_chrA17FromMMC3) {
                BaseMapper::SetPpuMemoryMapping(startAddr, endAddr, (_chrOuterBank << 7) | (pageNumber & 0x7F), memoryType, accessType);
            } else {
                BaseMapper::SetPpuMemoryMapping(startAddr, endAddr, ((_chrOuterBank & 0x02) << 7) | (pageNumber & 0xFF), memoryType, accessType);
            }
        } else {
            BaseMapper::SetPpuMemoryMapping(startAddr, endAddr, pageNumber, memoryType, accessType);
        }
    }

    void InitMapper() override
    {
        AddRegisterRange(0x5000, 0x5FFF, MemoryOperation::Write);
        //_modeReg = 0x0F;
        //_outerBankReg = 0x10;
        _prgA17FromMMC3 = true;
        _chrA17FromMMC3 = false;
        _mode = 1; // startup in MMC3 mode, 256K PRG / 128K CHR
        _prgOuterBank = 0x02;
        _chrOuterBank = 0x02;
        _bnRomPrgBank = 0;

        JyCompany::InitMapper();
        MMC3::InitMapper();
    }
    
    void UpdatePrgMapping() override
    {
        if(_mode == 0) {
            SelectPrgPage4x(0, (_bnRomPrgBank << 2), PrgMemoryType::PrgRom, MemoryAccessType::Read);
        } else {
            MMC3::UpdatePrgMapping();
        }
    }

    void UpdateStateHSK()
    {
        switch(_mode) {
            case 0: // BNROM mode is MMC3 with different PRG banking
            case 1:
                MMC3::UpdateState();
                break;
            case 2:
            case 3:
                JyCompany::UpdateState();
                break;
        }
    }

    void WriteRegister(uint16_t addr, uint8_t value) override
    {
        switch(addr) {
            case 0x5009: //unknown mask
                _mode = ((value & 0x18) >> 3);
                _prgOuterBank = ((value & 0x01) << 1) | (_prgOuterBank & 0x01);
                _chrOuterBank = ((value & 0x01) << 1) | (_chrOuterBank & 0x01);
                UpdateStateHSK();
                break;
            case 0x500B: //unknown mask
                _prgOuterBank = (_prgOuterBank & 0x02) | ((value & 0x08) >> 3);
                _chrOuterBank = (_chrOuterBank & 0x02) | ((value & 0x40) >> 6);
                _prgA17FromMMC3 = (value & 0x10);
                _chrA17FromMMC3 = (value & 0x80);
                _bnRomPrgBank = ((value & 0x06) >> 1);
                UpdateStateHSK();
                break;
            default:
                if(_mode <= 1) {
                    MMC3::WriteRegister(addr, value);
                } else if(_mode == 2 || _mode == 3) {
                    JyCompany::WriteRegister(addr, value);
                }
                break;
        }
    }
public:
    void NotifyVRAMAddressChange(uint16_t addr) override
    {
        if(_mode == 1) {
            MMC3::NotifyVRAMAddressChange(addr);
        } else if(_mode == 2 || _mode == 3) {
            JyCompany::NotifyVRAMAddressChange(addr);
        }
    }
};