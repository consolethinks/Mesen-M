#include "stdafx.h"
#include "Mapper538.h"
#include "FdsAudio.h" // FDS exp. audio emulation when desired

uint8_t bankLookupTable[16] = {0, 1, 2, 1, 3, 1, 4, 1, 5, 5, 1, 1, 6, 6, 7, 7};

void Mapper538::InitMapper()
{
    // fixed banks
    SelectPRGPage(1, 14);
    SelectPRGPage(2, 7);
    SelectPRGPage(3, 15);
    SelectCHRPage(0, 0);

    // set default reg. values (not sure if these are correct)
    WriteRegister(0x6000, 0xFF);
    WriteRegister(0x8000, 0xFF);

    _audio.reset(new FdsAudio(_console));
    AddRegisterRange(0x4040, 0x4092, MemoryOperation::Any);
    RemoveRegisterRange(0xC000, 0xCFFF, MemoryOperation::Read);
}

void Mapper538::WriteRegister(uint16_t addr, uint8_t value)
{
    if (0x4040 <= addr && addr <= 0x4092) { // $4040-$4092
        _audio->WriteRegister(addr, value);
    } else { // $C000-$CFFF (uncertain)
        SetCpuMemoryMapping(0x6000, 0x7FFF, 0x08 + ((value & 0x0F) >> 1), PrgMemoryType::PrgRom, MemoryAccessType::Read);
        SelectPRGPage(0, bankLookupTable[value & 0x0F], PrgMemoryType::PrgRom, MemoryAccessType::Read);
    }
}

uint8_t Mapper538::ReadRegister(uint16_t addr)
{
    return _audio->ReadRegister(addr);
}

void Mapper538::StreamState(bool saving)
{
    BaseMapper::StreamState(saving);

    SnapshotInfo audio{ _audio.get() };
    Stream(audio);
}

void Mapper538::ProcessCpuClock()
{
    if(GetBootlegExpAudioSetting()) {
        _audio->Clock();
    }
}

ConsoleFeatures Mapper538::GetAvailableFeatures()
{
    return ConsoleFeatures::BootlegExpAudio;
}