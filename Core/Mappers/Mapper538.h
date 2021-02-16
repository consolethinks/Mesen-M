#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class FdsAudio;

class Mapper538 : public BaseMapper
{
private:
    unique_ptr<FdsAudio> _audio;
protected:
    uint16_t GetPRGPageSize() override { return 0x2000; }
    uint16_t GetCHRPageSize() override { return 0x2000; }
    uint32_t GetWorkRamSize() override { return 0; }
    uint16_t RegisterStartAddress() override { return 0xC000; }
    uint16_t RegisterEndAddress() override { return 0xCFFF; }
    bool AllowRegisterRead() override { return true; }

    void InitMapper() override;
    void WriteRegister(uint16_t addr, uint8_t value) override;
    uint8_t ReadRegister(uint16_t addr) override;
    void StreamState(bool saving) override;

    void ProcessCpuClock() override;
public:
    ConsoleFeatures GetAvailableFeatures() override;
};