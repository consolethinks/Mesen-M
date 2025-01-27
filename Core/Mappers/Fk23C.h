#pragma once
#include "stdafx.h"
#include "BaseMapper.h"
#include "A12Watcher.h"

class Fk23C : public BaseMapper
{
private:
	uint8_t _prgBankingMode;
	uint8_t _outerChrBankSize;
	bool _selectChrRam;
	bool _nromChrMode;
	bool _cnromChrMode;
	uint16_t _prgBase;
	uint8_t _chrBase;
	bool _extendedMmc3Mode;
	uint8_t _wramBankSelect;
	bool _ramInFirstChrBank;
	bool _fk23RegistersEnabled;
	bool _wramConfigEnabled;
	
	bool _wramEnabled;
	
	bool _invertPrgA14;
	bool _invertChrA12;

	uint8_t _currentMmc3Register;

	uint8_t _irqReloadValue;
	uint8_t _irqCounter;
	bool _irqReload;
	bool _irqEnabled;

	uint8_t _mirroringReg;

	uint8_t _cnromChrReg;
	uint8_t _mmc3Registers[12];
	
	uint8_t _irqDelay;
	A12Watcher _a12Watcher;

protected:
	uint16_t GetPRGPageSize() override { return 0x2000; }
	uint16_t GetCHRPageSize() override { return 0x0400; }
	
	//uint32_t GetChrRamSize() override { return 0x40000; } //NO CHR-RAM in iNES without DB entry
	uint16_t GetChrRamPageSize() override { return 0x400; }

	uint32_t GetWorkRamSize() override { return 0x8000; } //Somes games have up to 32kb of Work RAM (only used for iNES 1.0 files w/ no DB entry)
	uint32_t GetWorkRamPageSize() override { return 0x2000; }

	uint32_t GetDipSwitchCount() override { return 3; }

	void InitMapper() override
	{
		// detect submapper type if header is not NES2.0
		if(!_romInfo.IsNes20Header) {
			if(_prgSize == 1024*1024 && _prgSize == _chrRomSize) {
				_romInfo.SubMapperID = 1;
			} else if(_prgSize == 16384*1024 && _chrRomSize == 0) {
				_romInfo.SubMapperID = 2;
			}
		}

		//$5000
		_prgBankingMode = 0;
		_outerChrBankSize = 0;
		_selectChrRam = false;
		_nromChrMode = false;

		//$5000 + $5001 + $5002
		_prgBase = (_romInfo.SubMapperID == 1) ? 0x20 : 0;

		//$5002
		_chrBase = 0;

		//$5003
		_extendedMmc3Mode = (_romInfo.SubMapperID == 1) ? true : false;
		_cnromChrMode = (_romInfo.SubMapperID == 3) ? true : false;

		//$A000
		_mirroringReg = 0;

		//$A001
		_wramBankSelect = 0;
		_ramInFirstChrBank = false;
		_wramConfigEnabled = false;
		_fk23RegistersEnabled = false;
		_wramEnabled = false;

		//$8000
		_currentMmc3Register = 0;
		_invertPrgA14 = false;
		_invertChrA12 = false;
		
		//$8000-9FFF, $C000-$FFFF
		_cnromChrReg = 0;

		constexpr uint8_t initValues[12] = { 0,2,4,5,6,7,0,1,0xFE, 0xFF, 0xFF, 0xFF };
		for(int i = 0; i < 12; i++) {
			_mmc3Registers[i] = initValues[i];
		}

		_irqCounter = 0;
		_irqEnabled = false;
		_irqReload = false;
		_irqReloadValue = 0;
		_irqDelay = 0;

		AddRegisterRange(0x5000, 0x5FFF, MemoryOperation::Write);
		UpdateState();
	}

	void Reset(bool softReset) override
	{
		if(softReset) {
			InitMapper();
		}
	}

	void StreamState(bool saving) override 
	{
		BaseMapper::StreamState(saving);
		SnapshotInfo a12Watcher { &_a12Watcher };
		ArrayInfo<uint8_t> regs { _mmc3Registers, 12 };

		Stream(
			_prgBankingMode, _outerChrBankSize, _selectChrRam, _nromChrMode, _cnromChrMode, _prgBase, _chrBase, _extendedMmc3Mode,
			_wramBankSelect, _ramInFirstChrBank, _wramConfigEnabled, _wramEnabled, _fk23RegistersEnabled,
			_invertPrgA14, _invertChrA12, _currentMmc3Register, _irqReloadValue,_irqCounter, _irqReload, _irqEnabled, _mirroringReg, _cnromChrReg,
			_irqDelay, regs, a12Watcher
		);

		if(!saving) {
			UpdateState();
		}
	}

	void SelectCHRPage(uint16_t slot, uint16_t page, ChrMemoryType memoryType = ChrMemoryType::Default) override
	{
		bool useChrRam = !HasChrRom() || (_selectChrRam && _chrRamSize > 0) || (_wramConfigEnabled && _ramInFirstChrBank && page <= 7);
		BaseMapper::SelectCHRPage(slot, page, useChrRam ? ChrMemoryType::ChrRam : ChrMemoryType::ChrRom);
	}

	void UpdatePrg()
	{
		switch(_prgBankingMode) {
			case 0:
			case 1:
			case 2:
			case 5:
			case 6:
			case 7:
				if(_extendedMmc3Mode) {
					// Extended MMC3
					uint8_t swap = _invertPrgA14 ? 2 : 0;
					uint16_t outer = (_prgBase << 1);
					//MessageManager::DisplayMessage("PRG-EXT", std::to_string(_mmc3Registers[6] | outer));
					SelectPRGPage(0 ^ swap, _mmc3Registers[6] | outer, PrgMemoryType::PrgRom, MemoryAccessType::Read);
					SelectPRGPage(1, _mmc3Registers[7] | outer, PrgMemoryType::PrgRom, MemoryAccessType::Read);
					SelectPRGPage(2 ^ swap, _mmc3Registers[8] | outer, PrgMemoryType::PrgRom, MemoryAccessType::Read);
					SelectPRGPage(3, _mmc3Registers[9] | outer, PrgMemoryType::PrgRom, MemoryAccessType::Read);
				} else {
					// MMC3
					uint8_t swap = _invertPrgA14 ? 2 : 0;
					uint8_t innerMask = 0x3F >> _prgBankingMode;
					uint16_t outer = (_prgBase << 1) & ~innerMask;
					//MessageManager::DisplayMessage("PRG", std::to_string(_mmc3Registers[6] | outer));
					SelectPRGPage(0 ^ swap, (_mmc3Registers[6] & innerMask) | outer, PrgMemoryType::PrgRom, MemoryAccessType::Read);
					SelectPRGPage(1, (_mmc3Registers[7] & innerMask) | outer, PrgMemoryType::PrgRom, MemoryAccessType::Read);
					SelectPRGPage(2 ^ swap, (0xFE & innerMask) | outer, PrgMemoryType::PrgRom, MemoryAccessType::Read);
					SelectPRGPage(3, (0xFF & innerMask) | outer, PrgMemoryType::PrgRom, MemoryAccessType::Read);
				}
				break;

			case 3:
				// NROM-128
				_cnromChrReg = 0; // no chr switching
				SelectPrgPage2x(0, _prgBase << 1);
				SelectPrgPage2x(1, _prgBase << 1);
				break;

			case 4:
				// NROM-256
				SelectPrgPage4x(0, (_prgBase & 0xFFE) << 1);
				break;

			default:
				break;
		}
	}

	void UpdateChr()
	{
		if(_nromChrMode) {
			uint16_t innerMask = _cnromChrMode ? (_outerChrBankSize ? 1 : 3) : 0;
			for(int i = 0; i < 8; i++) {
				SelectCHRPage(i, (((_cnromChrReg & innerMask) | _chrBase) << 3) + i);
			}
		} else {
			uint8_t swap = _invertChrA12 ? 0x04 : 0;
			if(_extendedMmc3Mode) {
				uint16_t outer = (_chrBase << 3);
				SelectCHRPage(0 ^ swap, _mmc3Registers[0] | outer);
				SelectCHRPage(1 ^ swap, _mmc3Registers[10] | outer);
				SelectCHRPage(2 ^ swap, _mmc3Registers[1] | outer);
				SelectCHRPage(3 ^ swap, _mmc3Registers[11] | outer);
				SelectCHRPage(4 ^ swap, _mmc3Registers[2] | outer);
				SelectCHRPage(5 ^ swap, _mmc3Registers[3] | outer);
				SelectCHRPage(6 ^ swap, _mmc3Registers[4] | outer);
				SelectCHRPage(7 ^ swap, _mmc3Registers[5] | outer);
			} else {
				uint8_t innerMask = (_outerChrBankSize ? 0x7F : 0xFF);
				uint16_t outer = (_chrBase << 3) & ~innerMask;

				SelectCHRPage(0 ^ swap, ((_mmc3Registers[0] & 0xFE) & innerMask) | outer);
				SelectCHRPage(1 ^ swap, ((_mmc3Registers[0] | 0x01) & innerMask) | outer);
				SelectCHRPage(2 ^ swap, ((_mmc3Registers[1] & 0xFE) & innerMask) | outer);
				SelectCHRPage(3 ^ swap, ((_mmc3Registers[1] | 0x01) & innerMask) | outer);
				SelectCHRPage(4 ^ swap, (_mmc3Registers[2] & innerMask) | outer);
				SelectCHRPage(5 ^ swap, (_mmc3Registers[3] & innerMask) | outer);
				SelectCHRPage(6 ^ swap, (_mmc3Registers[4] & innerMask) | outer);
				SelectCHRPage(7 ^ swap, (_mmc3Registers[5] & innerMask) | outer);
			}
		}
	}

	void UpdateState()
	{
		switch(_mirroringReg & (_wramConfigEnabled ? 0x03 : 0x01)) { // single screen only when wramConfigReg is enabled
			case 0: SetMirroringType(MirroringType::Vertical); break;
			case 1: SetMirroringType(MirroringType::Horizontal); break;
			case 2: SetMirroringType(MirroringType::ScreenAOnly); break;
			case 3: SetMirroringType(MirroringType::ScreenBOnly); break;
		}

		UpdatePrg();
		UpdateChr();

		if(_wramConfigEnabled) {
			uint8_t nextBank = (_wramBankSelect + 1) & 0x03;
			SetCpuMemoryMapping(0x4000, 0x5FFF, nextBank, HasBattery() ? PrgMemoryType::SaveRam : PrgMemoryType::WorkRam, MemoryAccessType::ReadWrite);
			SetCpuMemoryMapping(0x6000, 0x7FFF, _wramBankSelect, HasBattery() ? PrgMemoryType::SaveRam : PrgMemoryType::WorkRam, MemoryAccessType::ReadWrite);
		} else {
			if(_wramEnabled) {
				SetCpuMemoryMapping(0x6000, 0x7FFF, 0, PrgMemoryType::WorkRam, _fk23RegistersEnabled ? MemoryAccessType::Read : MemoryAccessType::ReadWrite);
			} else {
				RemoveCpuMemoryMapping(0x6000, 0x7FFF);
			}
			RemoveCpuMemoryMapping(0x4000, 0x5FFF);
		};
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if(addr < 0x8000) {
			if((_fk23RegistersEnabled || !_wramConfigEnabled) && !(addr & 0x0008)) { // BS-8029 random write has to be ignored
				uint16_t mask = (0x0010 << GetDipSwitches());
				if(!(addr & mask)) {
					//not a register
					return;
				}

				switch(addr & 0x03) {
					case 0:
						_prgBankingMode = value & 0x07;
						_outerChrBankSize = (value & 0x10) >> 4;
						_selectChrRam = (value & 0x20) != 0;
						_nromChrMode = (value & 0x40) != 0;
						_prgBase = (_prgBase & ~0x180) | ((value & 0x80) << 1) | ((value & 0x08) << 4);
						
						break;

					case 1:
						_prgBase = (_prgBase & ~0x7F) | (value & 0x7F);
						break;

					case 2:
						_prgBase = (_prgBase & ~0xE00) | ((value & 0xC0) << 3) | ((value & 0x20) << 6);
						_chrBase = value;
						_cnromChrReg = 0;
						break;

					case 3:
						_extendedMmc3Mode = (value & 0x02) != 0;
						break;
				}
				UpdateState();
			} else {
				//FK23C Registers disabled, $5000-$5FFF maps to the second 4 KiB of the 8 KiB WRAM bank 2
				WritePrgRam(addr, value);
			}
		} else {
			if(_cnromChrMode && ((addr <= 0x9FFF || addr >= 0xC000) || ((addr & 0xFFF) != 0))) {
				_cnromChrReg = value & 0x03;
				UpdateState();
			}

			switch(addr & 0xE001) {
				case 0x8000:
					if((_romInfo.SubMapperID == 2) && (value == 0x46 || value == 0x47)) {
						//Subtype 2, 16384 KiB PRG-ROM, no CHR-ROM: Like Subtype 0, but MMC3 registers $46 and $47 swapped.
						value ^= 1;
					}

					_invertPrgA14 = (value & 0x40) != 0;
					_invertChrA12 = (value & 0x80) != 0;
					_currentMmc3Register = value & 0x0F;
					UpdateState();
					break;

				case 0x8001: {
					if (addr == 0x9FFF) { return; } // weird ignored write on BS-6028 180 in 1 - Dr. Mario
					uint8_t reg = _currentMmc3Register & (_extendedMmc3Mode ? 0x0F : 0x07);
					if(reg < 12) {
						_mmc3Registers[reg] = value;
						UpdateState();
					}
					break;
				}

				case 0xA000:
					_mirroringReg = value & 0x03;
					UpdateState();
					break;

				case 0xA001:
					if((value & 0x20) == 0) {
						//Ignore extra bits if bit 5 is not set
						value &= 0xA0;
					}

					_wramBankSelect = (value & 0x03);
					_ramInFirstChrBank = (value & 0x04) != 0;
					_wramConfigEnabled = (value & 0x20) != 0;
					_fk23RegistersEnabled = (value & 0x40) != 0;
					_wramEnabled = (value & 0x80) != 0;

					UpdateState();
					break;

				case 0xC000:
					_irqReloadValue = value;
					break;

				case 0xC001:
					_irqCounter = 0;
					_irqReload = true;
					break;

				case 0xE000:
					_irqEnabled = false;
					_console->GetCpu()->ClearIrqSource(IRQSource::External);
					break;

				case 0xE001:
					_irqEnabled = true;
					break;
			}
		}
	}

public:
	void ProcessCpuClock() override
	{
		if(_irqDelay > 0) {
			_irqDelay--;
			if(_irqDelay == 0) {
				_console->GetCpu()->SetIrqSource(IRQSource::External);
			}
		}
	}

	void NotifyVRAMAddressChange(uint16_t addr) override
	{
		switch(_a12Watcher.UpdateVramAddress(addr, _console->GetPpu()->GetFrameCycle())) {
			case A12StateChange::None:
			case A12StateChange::Fall:
				break;

			case A12StateChange::Rise:
				if(_irqCounter == 0 || _irqReload) {
					_irqCounter = _irqReloadValue;
				} else {
					_irqCounter--;
				}

				if(_irqCounter == 0 && _irqEnabled) {
					_irqDelay = 2;
				}
				_irqReload = false;
				break;
		}
	}
};
