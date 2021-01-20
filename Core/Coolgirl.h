#pragma once
#include "stdafx.h"
#include "BaseMapper.h"
#include "CPU.h"
#include "PPU.h"
#include "MemoryManager.h"

class Coolgirl : public BaseMapper
{
    private:
        // general regs
        bool _sramEnabled;
        bool _flashWriteEnabled;
        bool _enablePrgWrite;
        bool _enableChrRamWrite;
        bool _mapRomTo6000;
        bool _fourScreen;
        bool _lock;

        uint8_t _flags;
        uint8_t _mapperSelection;
        uint8_t _sramPage;
        MirroringType _mirroring;

        // prg regs
        uint16_t _prgBase;
        uint16_t _prgMask;
        uint8_t _prgMode;
        uint8_t _prgBank6000;
        uint8_t _prgBanks[4];

        // chr regs
        uint8_t _chrMask;
        uint8_t _chrMode;
        uint8_t _chrBanks[8];
        //uint8_t _chrBanks1[4]; apparently it's not used

        // Mapper regs
        // - MMC1
        uint8_t _mmc1LoadReg;
        // - MMC2/4
        bool _mmc2PpuLatch[2];
        // - MMC3
        uint8_t _mmc3Reg;
        // - M069
        uint8_t _m069Reg;
        // - M090
        uint8_t _m090Xor;
        uint8_t _m090Multiply[2];
        // - M112
        uint8_t _m112Reg;
        // - M163
        uint8_t _m163Reg[6];
        uint8_t _ppuM163Latch;
        
        // IRQ regs
        // - mmc3
        bool _irqMmc3Enabled;
        bool _irqMmc3Reset;
        uint8_t _irqMmc3Counter;
        uint8_t _irqMmc3Latch;
        // - mmc5
        bool _irqMmc5Enabled;
        bool _irqMmc5Pending;
        uint8_t _irqMmc5TargetScanline;
        // - vrc3
        uint8_t _irqVrc3Control;
        uint16_t _irqVrc3Value;
        uint16_t _irqVrc3Latch;
        // - vrc4
        uint8_t _irqVrc4Value;
        uint8_t _irqVrc4Control;
        uint8_t _irqVrc4Latch;
        uint8_t _irqVrc4Prescaler;
        uint8_t _irqVrc4PrescalerCounter;
        // - FME-7
        bool _irqFme7Enabled;
        bool _irqFme7CounterEnabled;
        uint16_t _irqFme7Value;
        // - M018
        uint8_t _irqM018Control;
        uint16_t _irqM018Value;
        uint16_t _irqM018Latch;
        // - M042 (Baby Mario)
        bool _irqM042Enabled;
        uint16_t _irqM042Value;
        // - M065
        bool _irqM065Enabled;
        uint16_t _irqM065Value;
        uint16_t _irqM065Latch;
        

        // FLASH regs.
        uint8_t flashState;
        uint16_t flashBufferAddress[10];
        uint8_t flashBufferValue[10];

        // CFI = Common Flash Memory Interface
        uint8_t cfiMode;
        const uint8_t cfiData[256] = 
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x51, 0x51, 0x52, 0x52, 0x59, 0x59, 0x02, 0x02, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x27, 0x27, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x06, 0x06,
            0x06, 0x06, 0x09, 0x09, 0x13, 0x13, 0x03, 0x03, 0x05, 0x05, 0x03, 0x03, 0x02, 0x02, 0x1B, 0x1B,
            0x02, 0x02, 0x00, 0x00, 0x06, 0x06, 0x00, 0x00, 0x01, 0x01, 0xFF, 0xFF, 0x03, 0x03, 0x00, 0x00,
            0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0x50, 0x50, 0x52, 0x52, 0x49, 0x49, 0x31, 0x31, 0x33, 0x33, 0x14, 0x14, 0x02, 0x02, 0x01, 0x01,
            0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0xB5, 0xB5, 0xC5, 0xC5, 0x05, 0x05,
            0x01, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
        };

        // misc regs.
        uint32_t lreset; // double write block
        uint8_t tksmir[8]; // mirroring


        //TKSMIR is unneeded???

        //flash stuff:
        //lreset 
        //save_flash_szie
        //WRAM
        //UNIFCHRRAMA
        //SAVE_FLASH


        void UpdatePRGState();
        void UpdateCHRState();
        void SelectCHRPage(uint16_t slot, uint16_t page, ChrMemoryType memoryType = ChrMemoryType::ChrRam) override;
        void SelectChrPage2x(uint16_t slot, uint16_t page, ChrMemoryType memoryType = ChrMemoryType::ChrRam) override;
        void SelectChrPage4x(uint16_t slot, uint16_t page, ChrMemoryType memoryType = ChrMemoryType::ChrRam) override;
        void SelectChrPage8x(uint16_t slot, uint16_t page, ChrMemoryType memoryType = ChrMemoryType::ChrRam) override;
        void WriteFlash(uint16_t addr, uint8_t value);
        void DetectScanlineStartMMC5(uint16_t addr);
    protected:
        uint16_t GetPRGPageSize() override { return 0x2000; }
        uint16_t GetChrRamPageSize() override { return 0x0400; }
        uint16_t RegisterStartAddress() override { return 0x4020; }
        uint16_t RegisterEndAddress() override { return 0xFFFF; }
        bool AllowRegisterRead() override { return true; }
        void InitMapper() override;
        void Reset(bool softReset) override;
        void UpdateState();
        void WriteRegister(uint16_t addr, uint8_t value) override;
        uint8_t ReadRegister(uint16_t addr) override;
    public:
        uint8_t MapperReadVRAM(uint16_t addr, MemoryOperationType memoryOperationType) override;
};