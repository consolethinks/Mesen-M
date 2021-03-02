#include "stdafx.h"
#include "BaseMapper.h"
#include "Console.h"
#include "MemoryManager.h"

class Mapper428 : public BaseMapper
{
private:
    uint8_t _regs[2];
    uint8_t _dataLatch;
protected:
    uint16_t GetPRGPageSize() override { return 0x4000; }
    uint16_t GetCHRPageSize() override { return 0x2000; }
    uint16_t RegisterStartAddress() override { return 0x6000; }
    uint32_t GetDipSwitchCount() override { return 2; }
    bool AllowRegisterRead() override { return true; }

    void InitMapper() override
    {
        memset(_regs, 0, 2);
        _dataLatch = 0;
        RemoveRegisterRange(0x8000, 0xFFFF, MemoryOperation::Read);

        UpdateState();
    }

    void Reset(bool softReset) override
    {
        if(softReset) {
            memset(_regs, 0, 2);
            _dataLatch = 0;
            UpdateState();
        }
    }

    void StreamState(bool saving) override
	{
        BaseMapper::StreamState(saving);
		Stream(_regs[0], _regs[1], _dataLatch);
		if(!saving) { 
            UpdateState();
        }
	}

    void UpdateState()
    {
        if(_regs[0] & 0x10) {
            SelectPrgPage2x(0, (_regs[0] >> 5) & 0xFE);
        } else {
            SelectPRGPage(0, _regs[0] >> 5);
            SelectPRGPage(1, _regs[0] >> 5);
        }

        if(_regs[1] & 0x80) {
            SelectCHRPage(0, (_regs[0] & 0x04) |  (_dataLatch & 0x03));
        } else {
            SelectCHRPage(0, _regs[0] & 0x07);
        }

        SetMirroringType((_regs[0] & 0x08) ? MirroringType::Horizontal : MirroringType::Vertical);
    }

    uint8_t ReadRegister(uint16_t addr) override
    {
        return (_console->GetMemoryManager()->GetOpenBus() & 0xFC) | (GetDipSwitches() & 0x03);
    }

    void WriteRegister(uint16_t addr, uint8_t value) override
    {
        if(addr < 0x8000) {
            if((addr & 0x0003) == 0x01) {
                _regs[0] = value;
            } else if((addr & 0x0003) == 0x02) {
                _regs[1] = value;
            }
        } else {
            _dataLatch = value;
        }

        UpdateState();
    }
};