#include "stdafx.h"
#include "Mapper539.h"
#include "FdsAudio.h" // FDS exp. audio emulation when desired

void Mapper539::InitMapper()
{
    // fixed banks
    SetCpuMemoryMapping(0x6000, 0x7FFF, 0x0D, PrgMemoryType::PrgRom, MemoryAccessType::Read);
    SelectPRGPage(0, 0x0C);
    SelectPRGPage(2, 0x0E);
    SelectPRGPage(3, 0x0F);
    SelectCHRPage(0, 0);

    SetCpuMemoryMapping(0x6000, 0x60FF,  0 | 0x18, PrgMemoryType::WorkRam, MemoryAccessType::ReadWrite);
    SetCpuMemoryMapping(0x6200, 0x62FF,  2 | 0x18, PrgMemoryType::WorkRam, MemoryAccessType::ReadWrite);
    SetCpuMemoryMapping(0x6400, 0x65FF,  4 | 0x18, PrgMemoryType::WorkRam, MemoryAccessType::ReadWrite);
    SetCpuMemoryMapping(0x8200, 0x82FF,  2 | 0x10, PrgMemoryType::WorkRam, MemoryAccessType::ReadWrite); 
    SetCpuMemoryMapping(0xC000, 0xD1FF,  0 | 0x00, PrgMemoryType::WorkRam, MemoryAccessType::ReadWrite); 
    SetCpuMemoryMapping(0xDF00, 0xDFFF, 31 | 0x00, PrgMemoryType::WorkRam, MemoryAccessType::ReadWrite); 

    // set default bank value (not sure if this is correct)
    WriteRegister(0xA000, 0x00);
    
    // init CHR-RAM to what the original hardware produced
    for(uint16_t i = 0; i < _chrRamSize; ++i) {
        WriteVRAM(i, (i & 0x0002) ? 0xFF : 0x00);
    }

    _audio.reset(new FdsAudio(_console));
    AddRegisterRange(0x4025, 0x4025, MemoryOperation::Write);
    AddRegisterRange(0x4040, 0x4092, MemoryOperation::Any);
    AddRegisterRange(0xFFFF, 0xFFFF, MemoryOperation::Write);
    RemoveRegisterRange(0xA000, 0xAFFF, MemoryOperation::Read);
}

void Mapper539::WriteRegister(uint16_t addr, uint8_t value)
{
    WritePrgRam(addr, value);
    if (0x4040 <= addr && addr <= 0x4092) { // $4040-$4092
        _audio->WriteRegister(addr, value);
    } else if((addr & 0xF000) == 0xA000) { // $A000 (unknown mask)
        SelectPRGPage(1, value);
    } else if(addr == 0xFFFF || addr == 0x4025) {
        SetMirroringType(value & 0x08 ? MirroringType::Horizontal : MirroringType::Vertical);
    }
}

uint8_t Mapper539::ReadRegister(uint16_t addr)
{
    return _audio->ReadRegister(addr);
}

void Mapper539::StreamState(bool saving)
{
    BaseMapper::StreamState(saving);

    SnapshotInfo audio{ _audio.get() };
    Stream(audio);
}

void Mapper539::ProcessCpuClock()
{
    if(GetBootlegExpAudioSetting()) {
        _audio->Clock();
    }
}

ConsoleFeatures Mapper539::GetAvailableFeatures()
{
    return ConsoleFeatures::BootlegExpAudio;
}