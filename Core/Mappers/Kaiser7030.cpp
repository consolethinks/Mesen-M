#include "Kaiser7030.h"
#include "FdsAudio.h" // FDS exp. audio emulation when desired

void Kaiser7030::InitMapper()
{
    // fixed banks
    SetCpuMemoryMapping(0x6000, 0x6BFF,   0, PrgMemoryType::WorkRam, MemoryAccessType::ReadWrite);
    SetCpuMemoryMapping(0x8000, 0xB7FF,  96, PrgMemoryType::PrgRom,  MemoryAccessType::Read); // 64+32
    SetCpuMemoryMapping(0xB800, 0xBFFF,   3, PrgMemoryType::WorkRam, MemoryAccessType::ReadWrite);
    SetCpuMemoryMapping(0xCC00, 0xD7FF,   5, PrgMemoryType::WorkRam, MemoryAccessType::ReadWrite);
    SetCpuMemoryMapping(0xD800, 0xFFFF, 118, PrgMemoryType::PrgRom,  MemoryAccessType::Read); // 64+32+22

    // set default reg. values (not sure if these are correct)
    WriteRegister(0x8000, 0xFF);
    WriteRegister(0x9000, 0xFF);

    _audio.reset(new FdsAudio(_console));
    AddRegisterRange(0x4040, 0x4092, MemoryOperation::Any);
    RemoveRegisterRange(0x8000, 0x9FFF, MemoryOperation::Read);
}

void Kaiser7030::WriteRegister(uint16_t addr, uint8_t value)
{
    if (0x4040 <= addr && addr <= 0x4092) { // $4040-$4092
        _audio->WriteRegister(addr, value);
    } else if(0x8000 <= addr && addr <= 0x8FFF)  { // $8000-$8FFF
        SetCpuMemoryMapping(0x7000, 0x7FFF, 64 + ((addr & 0x0007) << 2), PrgMemoryType::PrgRom, MemoryAccessType::Read);
        SetMirroringType((addr & 0x0008) ? MirroringType::Horizontal : MirroringType::Vertical);
    } else if (0x9000 <= addr && addr <= 0x9FFF) { // $9000-$9FFF
        SetCpuMemoryMapping(0xC000, 0xCBFF, ((addr & 0x000F) << 2),   PrgMemoryType::PrgRom, MemoryAccessType::Read);
        SetCpuMemoryMapping(0x6C00, 0x6FFF, ((addr & 0x000F) << 2) + 3, PrgMemoryType::PrgRom, MemoryAccessType::Read);
    }
}

uint8_t Kaiser7030::ReadRegister(uint16_t addr)
{
    return _audio->ReadRegister(addr);
    return 0;
}

void Kaiser7030::StreamState(bool saving)
{
    BaseMapper::StreamState(saving);

    SnapshotInfo audio{ _audio.get() };
    Stream(audio);
}

void Kaiser7030::ProcessCpuClock()
{
    _audio->Clock();
}