#include "stdafx.h"
#include "MMC3.h"
#include "Mapper40.h"

class N49C300 : public MMC3, public Mapper40
{
private:
    uint8_t _exReg;
    uint8_t _lastBankOnM40;

protected:
    uint16_t GetPRGPageSize() override
    { 
        if(_exReg < 0x13) {
            return 0x8000; // NROM (8K)
        } else if(_exReg < 0x37) {
            return Mapper40::GetPRGPageSize();
        } else {
            return MMC3::GetPRGPageSize();
        }
    }

    uint16_t GetCHRPageSize() override
    {
        if(_exReg < 0x13) {
            return 0x2000; // NROM (8K)
        } else if(_exReg < 0x37) {
            return Mapper40::GetCHRPageSize();
        } else {
            return MMC3::GetCHRPageSize();
        }
    }

    uint16_t RegisterStartAddress() override { return 0x4000; }

    void SetCpuMemoryMapping(uint16_t startAddr, uint16_t endAddr, int16_t pageNumber, PrgMemoryType type, int8_t accessType = -1) override
    {
        if(_exReg == 0x00) {
            BaseMapper::SetCpuMemoryMapping(startAddr, endAddr, 0x00, type, accessType); // pagenumber is in 32K blocks
        } else if(_exReg < 0x13) {
            BaseMapper::SetCpuMemoryMapping(startAddr, endAddr, 0x01, type, accessType);
        } else if(_exReg < 0x37) {
            BaseMapper::SetCpuMemoryMapping(startAddr, endAddr, 0x08 | (pageNumber & 0x07), type, accessType); // pagenumber is in 8k blocks
        } else if(_exReg < 0xFF) {
            BaseMapper::SetCpuMemoryMapping(startAddr, endAddr, 0x10 | (pageNumber & 0x0F), type, accessType);
        } else {
            BaseMapper::SetCpuMemoryMapping(startAddr, endAddr, 0x20 | (pageNumber & 0x1F), type, accessType);
        }
    }

    void SetPpuMemoryMapping(uint16_t startAddr, uint16_t endAddr, uint16_t pageNumber, ChrMemoryType type = ChrMemoryType::Default, int8_t accessType = -1) override
    {
        if(_exReg == 0x00) {
            BaseMapper::SetPpuMemoryMapping(startAddr, endAddr, 0x00, type, accessType);  // pagenumber is in 8K blocks
        } else if(_exReg < 0x13) {
            BaseMapper::SetPpuMemoryMapping(startAddr, endAddr, 0x01, type, accessType);
        } else if(_exReg < 0x37) {
            BaseMapper::SetPpuMemoryMapping(startAddr, endAddr, 0x03, type, accessType);
        } else if(_exReg < 0xFF) {
            BaseMapper::SetPpuMemoryMapping(startAddr, endAddr, 0x80 | (pageNumber & 0x7F), type, accessType); // pagenumber is in 1K blocks
        } else {
            BaseMapper::SetPpuMemoryMapping(startAddr, endAddr, 0x100 | (pageNumber & 0x7F), type, accessType);
        }
    }

    void InitMapper() override
    {
        RemoveRegisterRange(0x5000, 0x7FFF, MemoryOperation::Any);
        _exReg = 0;

        MMC3::InitMapper();
        Mapper40::InitMapper();

        RemoveCpuMemoryMapping(0x4000, 0x7FFF);
        UpdateState();
    }

    void Reset(bool softReset) override
    {
        if(softReset) {
            InitMapper();
        }
    }

    void UpdateState() override
    {
        if(_exReg < 0x13) {
            BaseMapper::SelectPRGPage(0, 0);
            BaseMapper::SelectCHRPage(0, 0);
            MMC3::UpdateMirroring();
        } else if(_exReg < 0x37) {
            SelectPRGPage(0, 4);
		    SelectPRGPage(1, 5);
		    SelectPRGPage(3, 7);
            Mapper40::WriteRegister(0xE000, _lastBankOnM40);
		    SelectCHRPage(0, 0);
            MMC3::UpdateMirroring();
        } else {
            MMC3::UpdateState();
        }
    }

    void StreamState(bool saving) override
    {
        MMC3::StreamState(saving);
        Mapper40::StreamState(saving);
        Stream(_exReg, _lastBankOnM40);
        if(!saving) {
            UpdateState();
        }
    }

    void WriteRegister(uint16_t addr, uint8_t value) override
    {
        if((addr & 0xE100) == 0x4100) {
            _exReg = value;
            RemoveCpuMemoryMapping(0x6000, 0x7FFF);

            // clear IRQ's
            Mapper40::WriteRegister(0x8000, 0x00);
            MMC3::WriteRegister(0xE000, 0x00);

            UpdateState();
        } else if((addr & 0xE001) == 0xA000) {
            MMC3::WriteRegister(addr, value);
        } else if(_exReg >= 0x13 && _exReg < 0x37) {
            if((addr & 0xE000) == 0xE000) {
                _lastBankOnM40 = value; // this is needed because the M40 implem. does not save last bank written in a register
            }
            UpdateState();
        } else {
            MMC3::WriteRegister(addr, value);
        }
    }

    void ProcessCpuClock() override
    {
        if(_exReg >= 0x13 && _exReg < 0x37) {
            Mapper40::ProcessCpuClock();
        }
    }

public:
    void NotifyVRAMAddressChange(uint16_t addr) override
    {
        if(_exReg >= 0x37) {
            MMC3::NotifyVRAMAddressChange(addr);
        }
    }
};