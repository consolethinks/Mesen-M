#pragma once
#include "stdafx.h"
#include "BaseMapper.h"
#include "CPU.h"
#include "MemoryManager.h"

class JyCompany : virtual public BaseMapper
{
private:
	enum class JyIrqSource
	{
		CpuClock = 0,
		PpuA12Rise = 1,
		PpuRead = 2,
		CpuWrite = 3
	};

	uint8_t _prgRegs[4];
	uint8_t _chrLowRegs[8];
	uint8_t _chrHighRegs[8];
	uint8_t _chrLatch[2];
	
	uint8_t _prgMode;
	bool _enablePrgAt6000;
	
	uint8_t _chrMode;

	uint8_t _mirroringReg;
	bool _romNametable;
	bool _extendedMirroring; // $D001.3 controls the same thing
	bool _disableNtRam;

	uint8_t _ppuAddressSpaceConfig;
	uint8_t _ntLowRegs[4];
	uint8_t _ntHighRegs[4];

	bool _irqEnabled;
	JyIrqSource _irqSource;
	uint8_t _irqCountDirection;
	bool _irqFunkyMode;
	uint8_t _irqFunkyModeReg;
	bool _irqSmallPrescaler;
	uint8_t _irqPrescaler;
	uint8_t _irqCounter;
	uint8_t _irqXorReg;

	uint8_t _multiplyValue1;
	uint8_t _multiplyValue2;
	uint16_t _multiplyResult;
	uint8_t _accumulator;
	uint8_t _regRamValue;

	uint16_t _lastPpuAddr;

	bool _inhibitRomNtExtMir; // disable rom nametable and extended mirroring, like mapper 90
protected:
	bool _autoChrSwitching;

	uint16_t _prgMask;
	uint8_t _prgShift;
	uint8_t _outerPrgReg;

	uint16_t _chrMask;
	uint8_t _chrShift;
	uint8_t _outerChrReg;

	uint16_t GetPRGPageSize() override { return 0x2000; }
	uint16_t GetCHRPageSize() override { return 0x0400; }
	uint16_t GetChrRamPageSize() override { return 0x0400; }
	virtual uint32_t GetWorkRamSize() override {
		if(_romInfo.MapperID == 35) {
			return 0x2000; // 8K WRAM
		} else {
			return 0;
		}
	}
	virtual bool ForceWorkRamSize() override {
		if(_romInfo.MapperID == 35) { return true; }
		else { return false; }
	}
	bool AllowRegisterRead() override { return true; }
	uint32_t GetDipSwitchCount() override { return 2; }

	virtual uint8_t PrgMaskDefault() { return 0x3F; } // 35/90/209/211 (PRG 512K, CHR 256K)
	virtual uint8_t PrgShiftDefault() { return 6; }
	virtual uint16_t ChrMaskDefault() { return 0x00FF; }
	virtual uint8_t ChrShiftDefault() { return 8; }
	virtual bool InhibitRomNtExtMir()
	{ 
		if(_romInfo.MapperID == 90) {
			return true;
		} else {
			return false;
		}
	}

	virtual void InitMapper() override
	{
		RemoveRegisterRange(0x8000, 0xFFFF, MemoryOperation::Read);
		AddRegisterRange(0x5000, 0x5FFF, MemoryOperation::Any);

		_chrLatch[0] = 0;
		_chrLatch[1] = 4;

		memset(_prgRegs, 0, sizeof(_prgRegs));
		memset(_chrLowRegs, 0, sizeof(_chrLowRegs));
		memset(_chrHighRegs, 0, sizeof(_chrHighRegs));

		_prgMode = 0;
		_prgMask = PrgMaskDefault();
		_prgShift = PrgShiftDefault();
		_outerPrgReg = 0;
		_enablePrgAt6000 = false;

		_chrMode = 0;
		_chrMask = ChrMaskDefault();
		_chrShift = ChrShiftDefault();
		_outerChrReg = 0;
		_autoChrSwitching = false;

		_mirroringReg = 0;
		_romNametable = false;
		_extendedMirroring = false;
		_disableNtRam = false;
		_inhibitRomNtExtMir = InhibitRomNtExtMir();

		_ppuAddressSpaceConfig = 0x00;
		memset(_ntLowRegs, 0, sizeof(_ntLowRegs));
		memset(_ntHighRegs, 0, sizeof(_ntHighRegs));

		_irqEnabled = false;
		_irqSource = JyIrqSource::CpuClock;
		_lastPpuAddr = 0;
		_irqCountDirection = 0;
		_irqFunkyMode = false;
		_irqFunkyModeReg = 0;
		_irqSmallPrescaler = false;
		_irqPrescaler = 0;
		_irqCounter = 0;
		_irqXorReg = 0;

		_multiplyValue1 = 0;
		_multiplyValue2 = 0;
		_multiplyResult = 0;
		_accumulator = 0;
		_regRamValue = 0;

		UpdateState();
	}

	void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);

		ArrayInfo<uint8_t> prgRegs{ _prgRegs, 4 };
		ArrayInfo<uint8_t> chrLowRegs{ _chrLowRegs, 8 };
		ArrayInfo<uint8_t> chrHighRegs{ _chrHighRegs, 8 };
		ArrayInfo<uint8_t> ntLowRegs{ _ntLowRegs, 4 };
		ArrayInfo<uint8_t> ntHighRegs{ _ntHighRegs, 4 };

		Stream(_chrLatch[0], _chrLatch[1], _prgMode, _prgMask, _prgShift, _outerPrgReg, _enablePrgAt6000, _chrMode, _chrMask, _chrShift, _outerChrReg, _autoChrSwitching, _mirroringReg, _romNametable, _extendedMirroring,
			_disableNtRam, _inhibitRomNtExtMir, _ppuAddressSpaceConfig, _irqEnabled, _irqSource, _lastPpuAddr, _irqCountDirection, _irqFunkyMode, _irqFunkyModeReg, _irqSmallPrescaler,
			_irqPrescaler, _irqCounter, _irqXorReg, _multiplyValue1, _multiplyValue2, _multiplyResult, _accumulator, _regRamValue, prgRegs, chrLowRegs, chrHighRegs, ntLowRegs, ntHighRegs);

		if(!saving) {
			UpdateState();
		}
	}

	void UpdateState()
	{
		UpdatePrgState();
		UpdateChrState();
		UpdateMirroringState();
	}

	uint8_t InvertPrgBits(uint8_t prgReg)
	{
		return (prgReg & 0x01) << 6 | (prgReg & 0x02) << 4 | (prgReg & 0x04) << 2 | (prgReg & 0x10) >> 2 | (prgReg & 0x20) >> 4 | (prgReg & 0x40) >> 6;
	}

#define PrgMapped(reg, shift) (((_outerPrgReg << _prgShift) & ~_prgMask) | ((reg << shift) & _prgMask))
	void UpdatePrgState()
	{
		uint8_t prgLastBank = (_prgMode & 0x04) ? _prgRegs[3] : 0x3F;
		switch(_prgMode & 0x03) {
			case 0:
				SelectPrgPage4x(0, PrgMapped(prgLastBank, 2));
				if(_enablePrgAt6000) {
					SetCpuMemoryMapping(0x6000, 0x7FFF, PrgMapped((_prgRegs[3] << 2) | 3, 0), PrgMemoryType::PrgRom, MemoryAccessType::Read);
				}
				break;

			case 1:
				SelectPrgPage2x(0, PrgMapped(_prgRegs[1], 1));
				SelectPrgPage2x(1, PrgMapped(prgLastBank, 1));
				if(_enablePrgAt6000) {
					SetCpuMemoryMapping(0x6000, 0x7FFF, PrgMapped((_prgRegs[3] << 1) | 1, 0), PrgMemoryType::PrgRom, MemoryAccessType::Read);
				}
				break;

			case 2:
				SelectPRGPage(0, PrgMapped(_prgRegs[0], 0));
				SelectPRGPage(1, PrgMapped(_prgRegs[1], 0));
				SelectPRGPage(2, PrgMapped(_prgRegs[2], 0));
				SelectPRGPage(3, PrgMapped(prgLastBank, 0));
				if(_enablePrgAt6000) {
					SetCpuMemoryMapping(0x6000, 0x7FFF, PrgMapped(_prgRegs[3], 0), PrgMemoryType::PrgRom, MemoryAccessType::Read);
				}
				break;
			case 3:
				SelectPRGPage(0, PrgMapped(InvertPrgBits(_prgRegs[0]), 0));
				SelectPRGPage(1, PrgMapped(InvertPrgBits(_prgRegs[1]), 0));
				SelectPRGPage(2, PrgMapped(InvertPrgBits(_prgRegs[2]), 0));
				SelectPRGPage(3, PrgMapped(InvertPrgBits(prgLastBank), 0));
				if(_enablePrgAt6000) {
					SetCpuMemoryMapping(0x6000, 0x7FFF, PrgMapped(InvertPrgBits(_prgRegs[3]), 0), PrgMemoryType::PrgRom, MemoryAccessType::Read);
				}
				break;
		}

		if(!_enablePrgAt6000) {
			if(_workRamSize > 0) {
				SetCpuMemoryMapping(0x6000, 0x7FFF, 0, PrgMemoryType::WorkRam, MemoryAccessType::ReadWrite);
			} else {
				RemoveCpuMemoryMapping(0x6000, 0x7FFF);
			}
		}
	}
#undef PrgMapped

#define ChrMapped(chrNum, shift) (((_outerChrReg << _chrShift) & ~_chrMask) | ((((_chrHighRegs[chrNum] << 8) | _chrLowRegs[chrNum]) << shift) & _chrMask))
	void UpdateChrState()
	{
		MemoryAccessType chrAccess = ((_ppuAddressSpaceConfig & 0x40) && !BaseMapper::HasChrRom()) ? MemoryAccessType::ReadWrite : MemoryAccessType::Read;
		switch(_chrMode) {
			case 0: 
				SelectChrPage8x(0, ChrMapped(0, 3), ChrMemoryType::Default, chrAccess);
				break;

			case 1: 
				if(_autoChrSwitching) {
					SelectChrPage4x(0, ChrMapped(_chrLatch[0], 2), ChrMemoryType::Default, chrAccess);
					SelectChrPage4x(1, ChrMapped(_chrLatch[1], 2), ChrMemoryType::Default, chrAccess);
				} else {
					SelectChrPage4x(0, ChrMapped(0, 2), ChrMemoryType::Default, chrAccess);
					SelectChrPage4x(1, ChrMapped(4, 2), ChrMemoryType::Default, chrAccess);
				}
				
				break;

			case 2:
				for(int i = 0; i < 4; ++i) {
					SelectChrPage2x(i, ChrMapped(i*2, 1), ChrMemoryType::Default, chrAccess);
				}
				break;

			case 3:
				for(int i = 0; i < 8; i++) {
					SelectCHRPage(i, ChrMapped(i, 0), ChrMemoryType::Default, chrAccess);
				}
				break;
		}
	}
#undef ChrMapped

	void UpdateMirroringState()
	{
		//"Mapper 211 behaves as though N were always set (1), and mapper 090 behaves as though N were always clear(0)."
		if((_romNametable || _extendedMirroring || _romInfo.MapperID == 211) && !_inhibitRomNtExtMir) {
			for(int i = 0; i < 4; i++) {
				SetNametable(i, _ntLowRegs[i] & 0x01);
			}
		} else {
			switch(_mirroringReg) {
				case 0: SetMirroringType(MirroringType::Vertical); break;
				case 1: SetMirroringType(MirroringType::Horizontal); break;
				case 2: SetMirroringType(MirroringType::ScreenAOnly); break;
				case 3: SetMirroringType(MirroringType::ScreenBOnly); break;
			}
		}
	}

	uint8_t ReadRegister(uint16_t addr) override
	{
		if (addr == 0x5000 || addr == 0x5400 || addr == 0x5C00) {
			return (GetDipSwitches() << 6); //Dip switches
		}
		switch(addr & 0xF803) {
			case 0x5800: return _multiplyResult & 0xFF;
			case 0x5801: return (_multiplyResult >> 8) & 0xFF;
			case 0x5802: return _accumulator;
			case 0x5803: return _regRamValue;
		}

		return _console->GetMemoryManager()->GetOpenBus();
	}

	virtual void D003Write(uint8_t value) // Mapper 35/90/209/211
	{
		_autoChrSwitching = (value & 0x80) == 0x80;
		_chrMask = (value & 0x20) ? 0x01FF : 0x00FF;
		_outerChrReg = ((value & 0x18) >> 2) | (value & 0x01);
		_outerPrgReg = ((value & 0x06) >> 1);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if(addr < 0x8000) {
			switch(addr & 0xF803) {
				case 0x5800: _multiplyValue1 = value; break;
				case 0x5801: 
					_multiplyValue2 = value; 
					_multiplyResult = _multiplyValue1 * _multiplyValue2; // this is supposed to be available after 8 cycles but whatever
					break;
				case 0x5802: _accumulator += value; break;
				case 0x5803: 
					_accumulator = 0;
					_regRamValue = value; 
					break;
			}
		} else {
			switch(addr & 0xF800) {
				case 0x8000:
					_prgRegs[addr & 0x03] = value & 0x7F;
					break;

				case 0x9000:
					_chrLowRegs[addr & 0x07] = value;
					break;

				case 0xA000:
					_chrHighRegs[addr & 0x07] = value;
					break;

				case 0xB000:
					if(addr & 0x04) {
						_ntHighRegs[addr & 0x03] = value;
					} else {
						_ntLowRegs[addr & 0x03] = value;
					}
					break;

				case 0xC000: case 0xC800: // mask is $F007
					switch(addr & 0x07) {
						case 0x00: 												// $C000
							if(value & 0x01) {
								_irqEnabled = true;
							} else {
								_irqEnabled = false;
								_irqPrescaler = 0;
								_console->GetCpu()->ClearIrqSource(IRQSource::External);
							}
							break;
						case 0x01: 												// $C001
							_irqCountDirection = (value >> 6) & 0x03;
							_irqFunkyMode = (value & 0x08) == 0x08;
							_irqSmallPrescaler = ((value >> 2) & 0x01) == 0x01;
							_irqSource = (JyIrqSource)(value & 0x03);
							break;
						case 0x02: 												// $C002
							_irqEnabled = false;
							_irqPrescaler = 0;
							_console->GetCpu()->ClearIrqSource(IRQSource::External);
							break;
						case 0x03: _irqEnabled = true; break; 					// $C003
						case 0x04: _irqPrescaler = value ^ _irqXorReg; break; 	// $C004
						case 0x05: _irqCounter = value ^ _irqXorReg; break; 	// $C005
						case 0x06: _irqXorReg = value; break; 					// $C006
						case 0x07: _irqFunkyModeReg = value; break; 			// $C007
					}
					break;

				case 0xD000:
					switch(addr & 0x03) {
						case 0x00: 												// $D000
							_prgMode = value & 0x07;
							_chrMode = (value >> 3) & 0x03;
							_romNametable = (value & 0x20) == 0x20;
							_disableNtRam = (value & 0x40) == 0x40;
							_enablePrgAt6000 = (value & 0x80) == 0x80;
							break;
						case 0x01: 												// $D001
							_mirroringReg = value & 0x03; 
							_extendedMirroring = (value & 0x08) == 0x08;
							break;
						case 0x02: _ppuAddressSpaceConfig = value & 0xC0; break;// $D002
						case 0x03: D003Write(value); break; 					// $D003
					}
					break;

			}
		}

		UpdateState();
	}

	void ProcessCpuClock() override
	{
		if(_irqEnabled && (_irqSource == JyIrqSource::CpuClock || (_irqSource == JyIrqSource::CpuWrite && _console->GetCpu()->IsCpuWrite()))) {
			TickIrqCounter();
		}
	}

#define NtMapped(ntNum) (((_outerChrReg << _chrShift) & ~_chrMask) | ((((_ntHighRegs[ntNum] << 8) | _ntLowRegs[ntNum])) & _chrMask))
	uint8_t MapperReadVRAM(uint16_t addr, MemoryOperationType type) override
	{
		if(_irqEnabled && _irqSource == JyIrqSource::PpuRead && type == MemoryOperationType::PpuRenderingRead) {
			TickIrqCounter();
		}

		if(addr >= 0x2000) {
			//This behavior only affects reads, not writes.
			//Additional info: https://forums.nesdev.com/viewtopic.php?f=3&t=17198
			if((_romNametable || _romInfo.MapperID == 211) && !_inhibitRomNtExtMir) {
				uint8_t ntIndex = ((addr & 0x2FFF) - 0x2000) / 0x400;
				if(_disableNtRam || (_ntLowRegs[ntIndex] & 0x80) != (_ppuAddressSpaceConfig & 0x80)) {
					uint16_t chrPage = NtMapped(ntIndex);
					uint32_t chrOffset = (chrPage << 10) | (addr & 0x3FF);
					if(BaseMapper::HasChrRom()) {
						return _chrRom[chrOffset % _chrRomSize];
					} else {
						return _chrRam[chrOffset % _chrRamSize];
					}
				}
			}
		}

		return BaseMapper::MapperReadVRAM(addr, type);
	}
#undef NtMapped
	void TickIrqCounter()
	{
		bool clockIrqCounter = false;
		uint8_t mask = _irqSmallPrescaler ? 0x07 : 0xFF;
		uint8_t prescaler = _irqPrescaler & mask;
		if(_irqCountDirection == 0x01) {
			prescaler++;
			if((prescaler & mask) == 0) {
				clockIrqCounter = true;
			}
		} else if(_irqCountDirection == 0x02) {
			prescaler--;
			if((prescaler & mask) == mask) {
				clockIrqCounter = true;
			}
		}
		_irqPrescaler = (_irqPrescaler & ~mask) | (prescaler & mask);

		if(clockIrqCounter) {
			if(_irqCountDirection == 0x01) {
				_irqCounter++;
				if(_irqCounter == 0) { // _irqEnabled is true when this function is called now
					_console->GetCpu()->SetIrqSource(IRQSource::External);
				}
			} else if(_irqCountDirection == 0x02) {
				_irqCounter--;
				if(_irqCounter == 0xFF) { // _irqEnabled is true when this function is called now
					_console->GetCpu()->SetIrqSource(IRQSource::External);
				}
			}
		}
	}
	
public:
	void NotifyVRAMAddressChange(uint16_t addr) override
	{
		if(_irqEnabled && (_irqSource == JyIrqSource::PpuA12Rise) && (addr & 0x1000) && !(_lastPpuAddr & 0x1000)) {
			TickIrqCounter();
		}
		_lastPpuAddr = addr;

		if(_autoChrSwitching) {
			switch(addr & 0x2FF8) {
				case 0x0FD8:
				case 0x0FE8:
					_chrLatch[addr >> 12] = addr >> 4 & ((addr >> 10 & 0x04) | 0x02);
					UpdateChrState();
					break;
			}
		}
	}
};