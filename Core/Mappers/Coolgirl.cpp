#include "stdafx.h"
#include "Coolgirl.h"
#define PRG_MAPPED(prg_register) (((_prgBase & 0x1FFF) << 1) | (_prgBanks[prg_register] & ((~(_prgMask << 1) & 0xFE) | 1)))

void Coolgirl::InitMapper()
{
	_sramEnabled = false;
	_flashWriteEnabled = false;
	_enablePrgWrite = false;
	_enableChrRamWrite = false;
	_mapRomTo6000 = false;
	_fourScreen = false;
	_lock = false;
	
	_flags = 0;
	_mapperSelection = 0;
	_sramPage = 0;
	_mirroring = MirroringType::Vertical;

	_prgBase = 0;
	_prgMask = 0xF8;
	_prgMode = 0;
	_prgBank6000 = 0;
	_prgBanks[0] = 0;
	_prgBanks[1] = 1;
	_prgBanks[2] = 0xFE;
	_prgBanks[3] = 0xFF;

	_chrMask = 0;
	_chrMode = 0;
	for(uint8_t i = 0; i < 8; ++i) { _chrBanks[i] = i; }

	// Mapper init
	_mmc1LoadReg = 0;
	_mmc2PpuLatch[0] = 0;
	_mmc2PpuLatch[1] = 0;
	_mmc3Reg = 0;
	_m069Reg = 0;
	_m090Xor = 0;
	_m090Multiply[0] = 0;
	_m090Multiply[1] = 0;
	_m112Reg = 0;
	for(uint8_t i = 0; i < 8; ++i) { _m163Reg[i] = 0; }
	_ppuM163Latch = 0;

	// IRQ Init
	_irqMmc3Enabled = false;
	_irqMmc3Reset = false;
	_irqMmc3Counter = 0;
	_irqMmc3Latch = 0;
	_irqMmc5Enabled = false;
	_irqMmc5Pending = false;
	_irqMmc5TargetScanline = 0;
	_irqVrc3Value = 0;
	_irqVrc3Control = 0;
	_irqVrc3Latch = 0;
	_irqVrc4Value = 0;
	_irqVrc4Control = 0;
	_irqVrc4Latch = 0;
	_irqVrc4Prescaler = 0;
	_irqVrc4PrescalerCounter = 0;
	_irqFme7Enabled = false;
	_irqFme7CounterEnabled = false;
	_irqFme7Value = 0;
	_irqM018Control = 0;
	_irqM018Value = 0;
	_irqM018Latch = 0;
	_irqM042Enabled = false;
	_irqM042Value = 0;
	_irqM065Enabled = false;
	_irqM065Value = 0;
	_irqM065Latch = 0;
	
	flashState = 0;
	cfiMode = 0;

	UpdateState();
}

void Coolgirl::Reset(bool softReset)
{
	if (softReset) {
		InitMapper();
	}
}

void Coolgirl::UpdateState()
{
	UpdatePRGState();
	UpdateCHRState();
}

void Coolgirl::UpdatePRGState()
{  // new version specifies 32bit integers for the prg banks and base, must be checked
	if(!cfiMode) {
		switch(_prgMode & 0x07) {
			default:
			case 0:
					SelectPrgPage2x(0, PRG_MAPPED(0) & 0xFE);
					SelectPrgPage2x(1, PRG_MAPPED(2) & 0xFE);
					break;
			case 1:
					SelectPrgPage2x(0, PRG_MAPPED(2) & 0xFE);
					SelectPrgPage2x(1, PRG_MAPPED(0) & 0xFE);
					break;
			case 4:
					SelectPRGPage(0, PRG_MAPPED(0));
					SelectPRGPage(1, PRG_MAPPED(1));
					SelectPRGPage(2, PRG_MAPPED(2));
					SelectPRGPage(3, PRG_MAPPED(3));
					break;
			case 5:
					SelectPRGPage(0, PRG_MAPPED(2));
					SelectPRGPage(1, PRG_MAPPED(1));
					SelectPRGPage(2, PRG_MAPPED(0));
					SelectPRGPage(3, PRG_MAPPED(3));
					break;
			case 6:
					SelectPrgPage4x(0, PRG_MAPPED(1) & 0xFC);
					break;
			case 7:
					SelectPrgPage4x(0, PRG_MAPPED(0) & 0xFC);
					break;
		}
	} else {
		for (uint32_t i = 0x8000; i < 0xFFFF; i += sizeof(cfiData)) {
			SetCpuMemoryMapping(i, i + sizeof(cfiData) - 1, (uint8_t*) cfiData, MemoryAccessType::Read);
		}
	}
	

	if(_mapRomTo6000) {
		SetCpuMemoryMapping(0x6000, 0x7fff, (_prgBase << 1) | (_prgBank6000 & ((~(_prgMask << 1) & 0xFE) | 1)), PrgMemoryType::PrgRom, MemoryAccessType::Read);
	} else {
		SetCpuMemoryMapping(0x6000, 0x7fff, _sramPage, PrgMemoryType::SaveRam, MemoryAccessType::ReadWrite);
	} // SRAM stands in for WRAM?
}

void Coolgirl::UpdateCHRState()
{ // good for latest revision of COOLGIRL (add TKSMIR?)
	switch(_chrMode & 0x07) {
			default:
			case 0:
				// CNROM-like
				SelectChrPage8x(0, _chrBanks[0] >> 3);
				break;
			case 1:
				// Mapper 163
				SelectChrPage4x(0, _ppuM163Latch);
				SelectChrPage4x(4, _ppuM163Latch);
				break;
			case 2:
				// MMC3 - Config 1
				SelectChrPage2x(0, _chrBanks[0] >> 1);
				tksmir[0] = tksmir[1] = _chrBanks[0];
				SelectChrPage2x(2, _chrBanks[2] >> 1);
				tksmir[2] = tksmir[3] = _chrBanks[2];
				for (uint8_t i = 3; i < 8; ++i) {
					SelectCHRPage(i, _chrBanks[i]);
					tksmir[i] = _chrBanks[i];
				}
				break;
			case 3:
				// MMC3 - Config 2
				for (uint8_t i = 0; i < 4; ++i) {
					SelectCHRPage(i, _chrBanks[i+4]);
					tksmir[i] = _chrBanks[i+4];
				}
				SelectChrPage2x(4, _chrBanks[0] >> 1);
				tksmir[4] = tksmir[5] = _chrBanks[0];
				SelectChrPage2x(6, _chrBanks[2] >> 1);
				tksmir[4] = tksmir[5] = _chrBanks[2];
				break;
			case 4:
				// generic 2 chr bank mapper
				SelectChrPage4x(0, _chrBanks[0] >> 2);
				SelectChrPage4x(4, _chrBanks[4] >> 2);
				break;
			case 5:
				SelectChrPage4x(0, _chrBanks[_mmc2PpuLatch[0] ? 1 : 0] >> 2);
				SelectChrPage4x(4, _chrBanks[_mmc2PpuLatch[0] ? 5 : 4] >> 2);
				break;
			case 6:
				// generic 4 chr bank mapper
				for (uint8_t i = 0; i < 4; ++i) {
					SelectChrPage2x(i*2, _chrBanks[i*2] >> 1);
				}
				break;
			case 7:
				// Extended-MMC3
				for (uint8_t i = 0; i < 8; ++i) {
					SelectCHRPage(i, _chrBanks[i]);
				}
				break;
	}

	if(_fourScreen) { // not sure if the four screen mirroring setting is affected by the check below
			SetMirroringType(MirroringType::FourScreens);
	} else if (!((_mapperSelection == 20) && (_flags & 0x01))) { // for mapper 189
			SetMirroringType(_mirroring);
	}
}

// NOTE: the chr multi-page selectors work differently from the BaseMapper's
void Coolgirl::SelectCHRPage(uint16_t slot, uint16_t page, ChrMemoryType memoryType)
{
	// memory type is ignored, always CHR-RAM
	uint16_t pageSize = std::min((uint32_t)GetChrRamPageSize(), _chrRamSize);

	uint16_t startAddr = slot * pageSize;
	uint16_t endAddr = startAddr + pageSize - 1;
	SetPpuMemoryMapping(startAddr, endAddr, page & ((((~_chrMask & 0x1F) + 1) << 3) - 1), ChrMemoryType::ChrRam, _enableChrRamWrite ? MemoryAccessType::ReadWrite : MemoryAccessType::Read);
}

void Coolgirl::SelectChrPage2x(uint16_t slot, uint16_t page, ChrMemoryType memoryType)
{
	SelectCHRPage(slot*2, (page << 1), memoryType);
	SelectCHRPage(slot*2+1, (page << 1)+1, memoryType);
}

void Coolgirl::SelectChrPage4x(uint16_t slot, uint16_t page, ChrMemoryType memoryType)
{
	SelectChrPage2x(slot*2, (page << 1), memoryType);
	SelectChrPage2x(slot*2+1, (page << 1)+1, memoryType);
}

void Coolgirl::SelectChrPage8x(uint16_t slot, uint16_t page, ChrMemoryType memoryType)
{
	SelectChrPage4x(slot, (page << 1), memoryType);
	SelectChrPage4x(slot*2+1, (page << 1)+1, memoryType);
}

void Coolgirl::WriteRegister(uint16_t addr, uint8_t value)
{
	if(_sramEnabled && addr >= 0x6000 && addr < 0x8000) {
		BaseMapper::WritePrgRam(addr, value);
	} else if(_flashWriteEnabled && addr >= 0x8000) {
		WriteFlash(addr, value);
	}

	// Coolgirl blocks two writes in a row

	// COOLGIRL specific registers
	if(!(_lock) && ((addr & 0xF000) == 0x5000)) {
		switch(addr & 0x0007) {
			case 0:
					_prgBase = (value << 8) | (_prgBase & 0xFF); // upper byte
					break;
			case 1:
					_prgBase = (_prgBase & 0xFF00) | value; // lower byte
					break;
			case 2:
					_prgMask = value;
					break;
			case 3:
					_prgMode = (value >> 5);
					_chrBanks[0] = (value << 3) | (_chrBanks[0] & 0x07);
					break;
			case 4:
					_chrMode = value >> 5;
					_chrMask = value & 0x1F;
					break;
			case 5:
					_prgBanks[0] = (_prgBanks[0] & 0xC1) | ((value & 0x7C) >> 1);
					_sramPage = value & 0x03;
					break;
			case 6:
					_flags = (value >> 5);
					_mapperSelection = value & 0x1F;
					break;
			case 7:
					_lock = (bool) (value >> 7);
					_fourScreen = (bool) ((value >> 5) & 0x01);
					switch((value >> 3) & 0x03) {
						case 0:
							_mirroring = MirroringType::Vertical;
							break;
						case 1:
							_mirroring = MirroringType::Horizontal;
							break;
						case 2:
							_mirroring = MirroringType::ScreenAOnly;
							break;
						case 3:
							_mirroring = MirroringType::ScreenBOnly;
							break;
					}
					_enablePrgWrite = (bool) ((value >> 2) & 0x01);
					_enableChrRamWrite = (bool) ((value >> 1) & 0x01);
					_sramEnabled = (bool) (value & 0x01);
					break;
		}

		if(_mapperSelection == 17) _prgBanks[1] = 0xFD;
		if(_mapperSelection == 14) _prgBanks[1] = 0x01;
	}

	// mapper specific registers
	switch(_mapperSelection) {
		case 0: break; // no mapper
		case 1:
			// UxROM + Mapper #71 (Flag 1 = Fire Hawk)
			if(addr >= 0x8000) {
					if(((addr & 0xF000) == 0x9000) && (_flags & 0x01)) {
						_mirroring = ((value >> 4) & 0x01) ? MirroringType::ScreenBOnly : MirroringType::ScreenAOnly;
					} else {
						_prgBanks[0] = (_prgBanks[0] & 0xC1) | ((value & 0x1F) << 1);
					}
			}
			break;
		case 2:
			// CNROM
			if(addr >= 0x8000) {
					// NOTE: more than 4 banks??
					_chrBanks[0] = (value << 3) | (_chrBanks[0] & 0x07); // first 3 bits are unused?
			}
			break;
		case 3:
			// Mapper 78
			if(addr >= 0x8000) {
					_prgBanks[0] = (_prgBanks[0] & 0xF1) | ((value & 0x07) << 1); // first bit is unused?
					_chrBanks[0] = (_chrBanks[0] & 0x87) | ((value & 0xF0) >> 1); // first 3 bits are unused?
					_mirroring = ((value >> 3) & 0x01) ? MirroringType::Vertical : MirroringType::Horizontal; // inverted
			}
			break;
		case 4:
			// TAM-S1
			if(addr >= 0x8000) { // original mapper doesn't take writes above 0xBFFF...
					_prgBanks[0] = (_prgBanks[0] & 0xE1) | ((value & 0x0F) << 1); // first bit is 0, prg bit5 is not kept
					// NOTE: this part is using the original TAM-S1 def:
					_mirroring = (value >> 7) ? MirroringType::Vertical : MirroringType::Horizontal; // inverted
			}
			break;
		case 5:
			// Sunsoft-2
			if(addr >= 0x8000) {
					_prgBanks[0] = (_prgBanks[0] & 0xF1) | ((value & 0x70) >> 3); // first bit unused?
					_enableChrRamWrite = (bool) (value & 0x01);
			}
			break;
		case 6:
			// Mapper 163
			if((addr & 0xF000) == 0x5000) {
					switch((addr >> 8) & 0x000F) {
						case 0: // 0x5000
							_prgMode |= 1;
							_prgBanks[0] = (_prgBanks[0] & 0xC3) | ((value & 0x0F) << 2); // first 2 bits are unused?
							_chrMode = (_chrMode & 0xFE) | (value >> 7);
							_m163Reg[1] = value;
							break;
						case 1: // 0x5100
							if((addr & 0x0001) == 1) {
									if(_m163Reg[4] && value == 0) {
										_m163Reg[5] ^= 1;
									}
									_m163Reg[4] = value;
							} else if(value == 6) {
									_prgMode &= 0xFE;
									_prgBanks[1] = 12;
							} else {
									_m163Reg[3] = value;    
							}
							break;
						case 2: // 0x5200
							_prgMode |= 1;
							_prgBanks[0] = (_prgBanks[0] & 0x3F) | ((value & 0x03) << 6); // first 2 bits are unused?
							_m163Reg[0] = value;
							break;
						case 3: // 0x5300
							_m163Reg[2] = value;
							break;
						default:
							break;
					}
			}
			break;
		case 7:
			// Mapper 18 - Jaleco SS 88006
			if(addr >= 0x8000) {
					uint8_t regSelect = ((addr >> 10) & 0x1C) | (addr & 0x03);
					if (regSelect < 6) {
						_prgBanks[regSelect >> 1] = (regSelect & 0x01) ? 
																	(((value & 0x0F) << 4) | (_prgBanks[regSelect >> 1] & 0x0F)) :
																	((_prgBanks[regSelect >> 1] & 0xF0) | (value & 0x0F));
					} else if((8 <= regSelect) && (regSelect < 24)) {
						_chrBanks[(regSelect - 8) >> 1] = (regSelect & 0x01) ? 
																		(((value & 0x0F) << 4) | (_chrBanks[(regSelect - 8) >> 1] & 0x0F)) :
																		((_chrBanks[(regSelect - 8) >> 1] & 0xF0) | (value & 0x0F));
					} else if(regSelect < 28) {
						_irqVrc3Latch = (_irqVrc3Latch & ~(0x000F << (regSelect - 24))) | ((value & 0x0F) << (regSelect -24));
					} else switch(regSelect) {
						case 28:
							_console->GetCpu()->ClearIrqSource(IRQSource::External);
							_irqVrc3Value = _irqVrc3Latch;
							break;
						case 29:
							_console->GetCpu()->ClearIrqSource(IRQSource::External);
							_irqVrc3Control = (value & 0x0F);
							break;
						case 30:
							switch(value & 0x03) {
									case 0: _mirroring = MirroringType::Horizontal; break;
									case 1: _mirroring = MirroringType::Vertical; break;
									case 2: _mirroring = MirroringType::ScreenAOnly; break;
									case 3: _mirroring = MirroringType::ScreenBOnly; break;
							}
							break;
						case 31: /* uPD775x is not supported */ break;
					}
			}
			break;
		case 8:
			// AxROM / BNROM
			if(addr >= 0x8000) {
					_prgBanks[0] = (_prgBanks[0] & 0xC3) | ((value & 0x0F) << 2);
					// AxROM only mirroring setting:
					if (!(_flags & 0x01)) { _mirroring = ((value >> 4) & 0x01) ? MirroringType::ScreenBOnly : MirroringType::ScreenAOnly; }
			}
			break;
		case 9:
			// Cheetahmen II
			if(addr >= 0x8000) {
					_prgBanks[0] = (_prgBanks[0] & 0xC3) | ((addr & 0x780) >> 5);
					_chrBanks[0] = ((addr & 0x07) << 5) | ((value & 0x03) << 3) | (_chrBanks[0] & 0x07);
					_mirroring = ((addr >> 13) & 0x01) ? MirroringType::Horizontal : MirroringType::Vertical;
			}
			break;
		case 10:
			// ColorDreams
			if(addr >= 0x8000) {
					_prgBanks[0] = (_prgBanks[0] & 0xF3) | ((value & 0x03) << 2);
					_chrBanks[0] = (_chrBanks[0] & 0x87) | ((value & 0xF0) >> 1);
			}
			break;
		case 11:
			// GxROM
			if(addr >= 0x8000) {
					_prgBanks[0] = (_prgBanks[0] & 0xF3) | ((value & 0x30) >> 2);
					_chrBanks[0] = (_chrBanks[0] & 0xE7) | ((value & 0x03) << 3);
			}
			break;
		case 12:
			// Mapper 87 - CNROM-like
			if ((addr & 0xE000) == 0x6000) { // range is 6000-7fff
					_chrBanks[0] = (_chrBanks[0] & 0xE7) | ((value & 0x01) << 4) | ((value & 0x02) << 2);
			}
			break;
		case 13:
			// Mapper 90 - J.Y. ASIC
			switch(addr & 0xF800) {
					case 0x8000: _prgBanks[addr & 0x0003] = (_prgBanks[addr & 0x0003] & 0xC0) | (value & 0x3F); break;
					case 0x9000:
					case 0xA000: _chrBanks[addr & 0x0007] = value; break;
					// no CIRAM settings because Mapper 90
					case 0xC000:
						switch(addr & 0x0007) {
							case 0: 
									if (value & 0x01) {
										_irqMmc3Enabled = true;
									} else {
										_irqMmc3Enabled = false;
										_console->GetCpu()->ClearIrqSource(IRQSource::External);
									}
							case 1: break; // no IRQ mode flags
							case 2: 
									_irqMmc3Enabled = false;
									_console->GetCpu()->ClearIrqSource(IRQSource::External);
									break;
							case 3: _irqMmc3Enabled = true; break;
							case 4: break; // no prescaler
							case 5: 
									_irqMmc3Latch = value ^ _m163Reg[0];
									_irqMmc3Reset = true;
									break;
							case 6: _m163Reg[0] = value; break;
							case 7: break; // unknown
						}
						break;
					case 0xD000: // no mapping modes
						if((addr & 0x0003) == 0x0001) { // $D001
							switch(value & 0x03) {
									case 0: _mirroring = MirroringType::Vertical;
									case 1: _mirroring = MirroringType::Horizontal;
									case 2: _mirroring = MirroringType::ScreenAOnly;
									case 3: _mirroring = MirroringType::ScreenBOnly;
							}
						}
						break;
			}
			break;
		case 14:
			// IREM H3001 (M065)
			switch(addr & 0xE007) {
					case 0x8000: _prgBanks[0] = (_prgBanks[0] & 0xC0) | (value & 0x3F); break;
					case 0xA000: _prgBanks[1] = (_prgBanks[1] & 0xC0) | (value & 0x3F); break;
					case 0x9000: break;
					case 0x9001: _mirroring = ((value >> 7) & 0x01) ? MirroringType::Horizontal : MirroringType::Vertical; break;
					case 0x9003: 
						_irqVrc3Control = (_irqVrc3Control & 0xFE) | ((value >> 7) & 0x01);
						_console->GetCpu()->ClearIrqSource(IRQSource::External);
						break;
					case 0x9004: 
						_irqVrc3Value = (_m163Reg[0] << 8) | _m163Reg[1];
						_console->GetCpu()->ClearIrqSource(IRQSource::External);
						break;
					case 0x9005: _m163Reg[0] = value; break;
					case 0x9006: _m163Reg[1] = value; break;
					case 0xC000: _prgBanks[2] = (_prgBanks[2] & 0xC0) | (value & 0x3F); break;
					default:
						if((addr & 0xF000) == 0xB000) {
							_chrBanks[addr & 0x0007] = value;
						}
						break;
			}
			break;
		case 15:
			// MMC5
			switch(addr & 0xFFFF) {
					// $5100-$5104 regs are not available
					case 0x5105:
						if(value == 0xFF) {
							_fourScreen = true;
						} else {
							switch(((value >> 3) & 0x02) | ((value >> 2) & 0x01)) {
									case 0: _mirroring = MirroringType::ScreenAOnly; break;
									case 1: _mirroring = MirroringType::Vertical; break;
									case 2: _mirroring = MirroringType::Horizontal; break;
									case 3: _mirroring = MirroringType::ScreenBOnly; break;
							}
						}
						break;
					case 0x5115:
						_prgBanks[0] = value & 0xFE;
						_prgBanks[1] = value | 0x01;
						break;
					case 0x5116: _prgBanks[2] = value; break;
					case 0x5117: _prgBanks[3] = value; break;
					case 0x5203: _irqMmc5TargetScanline = value; break;
					case 0x5204:
						_irqMmc5Enabled = (bool) (value & 0x80);
						if(!_irqMmc5Enabled) {
							_console->GetCpu()->ClearIrqSource(IRQSource::External);
						} else if(_irqMmc5Pending && _irqMmc5Enabled) {
							_console->GetCpu()->SetIrqSource(IRQSource::External);
						}
						break;
					default:
						if(((addr & 0xFFFF) >= 0x5120) && ((addr & 0xFFFF) < 0x512C)) {
							_chrBanks[addr & 0x000F] = value;
						}
			}
			break;
		case 16:
			// MMC1
			if(addr >= 0x8000) {
					if(value & 0x80) {
						_m163Reg[0] = (_m163Reg[0] & 0xC0) | 0x20;
						_prgMode = 0;
						_prgBanks[2] = (_prgBanks[2] & 0xE0) | 0x1E;
					} else {
						_m163Reg[0] = (_m163Reg[0] & 0xC0) | ((value & 0x01) << 5) | ((_m163Reg[0] & 0x3E) >> 1);
						if(_m163Reg[0] & 0x01) {
							switch((addr >> 13) & 0x03) {
									case 0: 
										// $8000-$9FFF
										if((_m163Reg[0] & 0x18) == 0x18) {
											_prgMode = 0;
											_prgBanks[2] = (_prgBanks[2] & 0xE0) | 0x1E;
										} else if((_m163Reg[0] & 0x18) == 0x10) {
											_prgMode = 1;
											_prgBanks[2] = (_prgBanks[2] & 0xE0);
										} else {
											_prgMode = 7;
										}
										if((_m163Reg[0] >> 5) & 0x01) {
											_chrMode = 4;
										} else {
											_chrMode = 0;
										}
									case 1: 
										// $A000-$BFFF
										_chrBanks[0] = (_chrBanks[0] & 0x83) | ((_m163Reg[0] & 0x3E) << 1);
										_prgBanks[0] = (_prgBanks[0] & 0xDF) | (_m163Reg[0] & 0x20);
										_prgBanks[2] = (_prgBanks[2] & 0xDF) | (_m163Reg[0] & 0x20);
										break;
									case 2:
										// $C000-$DFFF
										_chrBanks[4] = (_chrBanks[4] & 0x83) | ((_m163Reg[0] & 0x3E) << 1);
										break;
									case 3:
										// $E000-$FFFF
										_prgBanks[0] = (_prgBanks[0] & 0xE1) | (_m163Reg[0] & 0x1E);
										_sramEnabled = !((_m163Reg[0] >> 5) & 0x01);
										break;
							}
							_m163Reg[0] = (_m163Reg[0] & 0xC0) | 0x20;
							if(_flags & 0x01) {
									if(_chrMode & 0x04) {
										_sramPage = 0x02 | ((_chrBanks[0] >> 6) ^ 0x01);
									} else {
										_sramPage = 0x02 | ((_chrBanks[0] >> 5) ^ 0x01);
									}
							}
						}
					}
			}
			break;
		case 17:
			// MMC2/MMC4
			switch((addr >> 12) & 0x0007) {
					case 2:
						//$A000-$AFFF
						if(_flags & 0x01) {
							// MMC4
							_prgBanks[0] = (_prgBanks[0] & 0xE1) | ((value & 0x0F) << 1);
						} else {
							// MMC2
							_prgBanks[0] = (_prgBanks[0] & 0xF0) | (value & 0x0F);
						}
						break;
					case 3: _chrBanks[0] = (_chrBanks[0] & 0x83) | ((value & 0x1F) << 2); break;
					case 4: _chrBanks[1] = (_chrBanks[1] & 0x83) | ((value & 0x1F) << 2); break;
					case 5: _chrBanks[4] = (_chrBanks[4] & 0x83) | ((value & 0x1F) << 2); break;
					case 6: _chrBanks[5] = (_chrBanks[5] & 0x83) | ((value & 0x1F) << 2); break;
					case 7: _mirroring = value & 0x01 ? MirroringType::Horizontal : MirroringType::Vertical; break;
					default: break;
			}
			break;
		case 18:
			// Mapper 152
			if(addr > 0x8000) {
					_prgBanks[0] = (_prgBanks[0] & 0xF1) | ((value & 0x70) >> 3);
					_chrBanks[0] = (_chrBanks[0] & 0x87) | ((value & 0x0F) << 3);
					_mirroring = (value & 0x80) ? MirroringType::ScreenBOnly : MirroringType::ScreenAOnly;
			}
			break;
		case 19:
			// VRC3
			if(addr >= 0x8000) {
					switch((addr >> 12) & 0x07) {
						case 0: _irqVrc3Latch = (_irqVrc3Latch & 0xFFF0) | (value & 0x0F); break;
						case 1: _irqVrc3Latch = (_irqVrc3Latch & 0xFF0F) | ((value & 0x0F) << 4); break;
						case 2: _irqVrc3Latch = (_irqVrc3Latch & 0xF0FF) | ((value & 0x0F) << 8); break;
						case 3: _irqVrc3Latch = (_irqVrc3Latch & 0x0FFF) | ((value & 0x0F) << 12); break;
						case 4:
							_console->GetCpu()->ClearIrqSource(IRQSource::External);
							_irqVrc3Control = (_irqVrc3Control & 0xF8) | (value & 0x07);
							if(_irqVrc3Control & 0x02) {
									_irqVrc3Value = _irqVrc3Latch;
							}
							break;
						case 5:
							_console->GetCpu()->ClearIrqSource(IRQSource::External);
							_irqVrc3Control = (_irqVrc3Control & 0xF8) | (value & 0x07);
							break;
						case 7:
							_prgBanks[0] = (_prgBanks[0] & 0xF1) | ((value & 0x07) << 1);
							break;
						default: 
							break;
					}
			}
			break;
		case 20:
			// MMC3-like (and mapper 189 in 6000-7FFF range?)
			if((_flags & 0x02) && addr < 0x8000 && addr >= 0x4120) {
					_prgBanks[0] = (_prgBanks[0] & 0xC3) | ((value & 0xF0) >> 2) | ((value & 0x0F) << 2);
			} else {
					switch(addr & 0xE001) {
						case 0x8000:
							_m163Reg[0] = (_m163Reg[0] & 0xF8) | (value & 0x07);
							if(!(_flags & 0x02)) {
									_prgMode = (value & 0x40) ? 5 : 4;
							}
							_chrMode = (value & 0x80) ? 3 : 2;
							break;
						case 0x8001:
							switch(_m163Reg[0] & 0x07) {
									case 0: _chrBanks[0] = value;
									case 1: _chrBanks[2] = value;
									case 2: _chrBanks[4] = value;
									case 3: _chrBanks[5] = value;
									case 4: _chrBanks[6] = value;
									case 5: _chrBanks[7] = value;
									case 6:
										if(!(_flags & 0x02)) { 
											_prgBanks[0] = (_prgBanks[0] & 0xC0) | (value & 0x3F); 
										}
										break;
									case 7:
										if(!(_flags & 0x02)) { 
											_prgBanks[1] = (_prgBanks[1] & 0xC0) | (value & 0x3F); 
										}
										break;
							}
							break;
						case 0xA000: _mirroring = (value & 0x01) ? MirroringType::Horizontal : MirroringType::Vertical; break;
						// $A001 is not implemented
						case 0xC000: _irqMmc3Latch = value; break;
						case 0xC001: _irqMmc3Reset = true; break;
						case 0xE000: 
							_irqMmc3Enabled = false;
							_console->GetCpu()->ClearIrqSource(IRQSource::External);
							break;
						case 0xE001: _irqMmc3Enabled = true; break;
						default:
							break;
					}
			}
			break;
		case 21:
			// Mapper 112
			switch(addr & 0xE001) {
					case 0x8000: _m163Reg[0] = (_m163Reg[0] & 0xF8) | (value & 0x07); break;
					case 0xA000:
						switch(_m163Reg[0] & 0x07) {
							case 0: _prgBanks[0] = (_prgBanks[0] & 0xC0) | (value & 0x3F); break;
							case 1: _prgBanks[1] = (_prgBanks[1] & 0xC0) | (value & 0x3F); break;
							case 2: _chrBanks[0] = value; break;
							case 3: _chrBanks[2] = value; break;
							case 4: _chrBanks[4] = value; break;
							case 5: _chrBanks[5] = value; break;
							case 6: _chrBanks[6] = value; break;
							case 7: _chrBanks[7] = value; break;
						}
						break;
					case 0xE000: _mirroring = (value & 0x01) ? MirroringType::Horizontal : MirroringType::Vertical; break;
			}
			break;
		case 22:
			// Mapper 33/48, flag.0 = 0 -> 33 and flag.0 = 1 -> 48
			if (addr > 0x8000) {
					switch(((addr >> 11) & 0x0C) | (addr & 0x03)) {
						case 0: 
							_prgBanks[0] = (_prgBanks[0] & 0xC0) | (value & 0x3F);
							if(!(_flags & 0x01)) { // M33 mirroring
									_mirroring = (value & 0x40) ? MirroringType::Horizontal : MirroringType::Vertical;
							}
							break;
						case 1: _prgBanks[1] = (_prgBanks[1] & 0xC0) | (value & 0x3F); break;
						case 2: _chrBanks[0] = value << 1; break;
						case 3: _chrBanks[2] = value << 1; break;
						case 4: _chrBanks[3] = value; break;
						case 5: _chrBanks[4] = value; break;
						case 6: _chrBanks[5] = value; break;
						case 7: _chrBanks[6] = value; break;
						case 8: _irqMmc3Latch = value; break;
						case 9: _irqMmc3Reset = true; break;
						case 10:_irqMmc3Enabled = true; break;
						case 11:
							_irqMmc3Enabled = false;
							_console->GetCpu()->ClearIrqSource(IRQSource::External);
							break;
						case 12:
							if(_flags & 0x01) { // M48 mirroring
									_mirroring = (value & 0x40) ? MirroringType::Horizontal : MirroringType::Vertical;
							}
					}
			}
			break;
		case 23:
			// UNUSED
			break;
		case 24:
			// VRC2/4
			if (addr >= 0x8000) {
					uint8_t vrc2bHi = ((addr >> 7) & 0x01) | ((addr >> 5) & 0x01) | ((addr >> 3) & 0x01) | ((addr >> 1) & 0x01); 
					uint8_t vrc2bLo = ((addr >> 6) & 0x01) | ((addr >> 4) & 0x01) | ((addr >> 2) & 0x01) | (addr & 0x01);
					uint8_t vrc2b = ((addr >> 10) & 0x1C) | (((_flags & 0x01) ? vrc2bLo : vrc2bHi) << 1) | ((_flags & 0x01) ? vrc2bHi : vrc2bLo);

					if          (vrc2b < 4) { 
						_prgBanks[0] = (_prgBanks[0] & 0xE0) | (value & 0x1F); 
					} else if   (vrc2b < 6) {
						if(value != 0xFF) {
							switch(value & 0x03) {
									case 0: _mirroring = MirroringType::Vertical; break;
									case 1: _mirroring = MirroringType::Horizontal; break;
									case 2: _mirroring = MirroringType::ScreenAOnly; break;
									case 3: _mirroring = MirroringType::ScreenBOnly; break;
							}
						}
					} else if   (vrc2b < 8) { 
						_prgMode = (_prgMode & 0xFE) | ((value >> 1) & 0x01);
					} else if   (vrc2b < 12) { 
						_prgBanks[1] = (_prgBanks[1] & 0xE0) | (value & 0x1F); 
					} else if(vrc2b < 28) {
						// VRC2a
						if (vrc2b & 0x01) {
							_chrBanks[(vrc2b - 12) >> 1] = ((_chrBanks[(vrc2b - 12) >> 1] & 0x0F) | ((value & 0x0F) << 4)) >> ((_flags & 0x02) ? 1 : 0);
						} else {
							_chrBanks[(vrc2b - 12) >> 1] = ((_chrBanks[(vrc2b - 12) >> 1] & 0xF0) | (value & 0x0F))        >> ((_flags & 0x02) ? 1 : 0);
						}
					}
					if (((addr >> 12) & 0x07) == 0x07) {
						switch((((_flags & 0x01) ? vrc2bLo : vrc2bHi) << 1) | ((_flags & 0x01) ? vrc2bHi : vrc2bLo)) {
							case 0: _irqVrc3Latch = (_irqVrc3Latch & 0xF0) | (value & 0x0F); break;
							case 1: _irqVrc3Latch = (value & 0xF0) | (_irqVrc3Latch & 0x0F); break;
							case 2:
									_console->GetCpu()->ClearIrqSource(IRQSource::External);
									if (_irqVrc3Control & 0x02) {
										_irqVrc4Prescaler = 0;
										_irqVrc4PrescalerCounter = 0;
										_irqVrc3Value = _irqVrc3Latch;
									}
									break;
							case 3:
									_console->GetCpu()->ClearIrqSource(IRQSource::External);
									_irqVrc3Control = (_irqVrc3Control & 0xFD) | ((_irqVrc3Control & 0x01) << 1);
									break;
						}
					}
			}
			
			break;
		case 25:
			// Sunsoft FME-7
			if((addr & 0xE000) == 0x8000) { // command reg
					_m163Reg[0] = (_m163Reg[0] & 0xF0) | (value & 0x0F);
			} else if ((addr & 0xE000) == 0xA000) { // parameter register
				if(_m163Reg[0] < 8) {
					_chrBanks[_m163Reg[0]] = value;
				} else if(_m163Reg[0] == 8) {
					_sramEnabled = (bool) (value & 0x80);
					_mapRomTo6000 = (bool) !(value & 0x40);
					_prgBank6000 = (value & 0x3F);
				} else if(_m163Reg[0] < 12) {
					_prgBanks[_m163Reg[0] - 9] = ((_prgBanks[_m163Reg[0] - 9]) & 0xC0) | (value & 0x3F);
				} else switch(_m163Reg[0]) {
					case 13:
						_irqVrc3Control = ((value >> 6) & 0x01) | (value & 0x01);
						_console->GetCpu()->ClearIrqSource(IRQSource::External);
						break;
					case 14: _irqVrc3Value = (_irqVrc3Value & 0xFF00) | value; break;
					case 15: _irqVrc3Value = (_irqVrc3Value & 0x00FF) | (value << 8); break;
					default: break;
				}
			}
			break;
		case 26:
			// IREM G-101
			switch(addr & 0xF000) {
					case 0x8000: _prgBanks[0] = (_prgBanks[0] & 0xC0) | (value & 0x3F); break;
					case 0x9000: _prgMode = (_prgMode & 0x06) | ((value >> 1) & 0x01); break;
					case 0xA000: _prgBanks[1] = (_prgBanks[1] & 0xC0) | (value & 0x3F); break;
					case 0xB000: _chrBanks[addr & 0x07] = value; break;
			}
			break;
		case 31: // weird testing mode
			if(addr >= 0x8000) {
					_prgBank6000 = ((value >> 1) & 0x0F) + 0x04;
					_mapRomTo6000 = true;
			}
			break;
		default:
			break;
	}

	UpdateState();
}

uint8_t Coolgirl::ReadRegister(uint16_t addr)
{
	switch(_mapperSelection) {
		case 0:
			if(addr > 0x5000 && addr < 0x6000) {
					return 0;
			}
			break;
		case 6:
			
			break;
		case 15:
			if(addr == 0x5204) {
					//_console->GetPpu()-> || (_console->GetPpu()->GetCurrentScanline() >= 240);

					_console->GetCpu()->ClearIrqSource(IRQSource::External);
					return 0;//(_irqMmc5Pending << 7) | ()
			}
			break;
		default:
			break;
	}
	return _console->GetMemoryManager()->GetOpenBus();
}

void Coolgirl::DetectScanlineStartMMC5(uint16_t addr)
{
	/*if(addr >= 0x2000 && addr <= 0x2FFF) {
		if(_lastPpuReadAddr == addr) {
			//Count consecutive identical reads
			_ntReadCounter++;
		} else {
			_ntReadCounter = 0;
		}

		if(_ntReadCounter >= 2) {
			if(!_ppuInFrame && !_needInFrame) {
				_needInFrame = true;
				_scanlineCounter = 0;
			} else {
				_scanlineCounter++;
				if(_irqCounterTarget == _scanlineCounter) {
					_irqPending = true;
					if(_irqEnabled) {
						_console->GetCpu()->SetIrqSource(IRQSource::External);
					}
				}
			}
			_splitTileNumber = 0;
		}
	} else {
	_ntReadCounter = 0;*/
}


uint8_t Coolgirl::MapperReadVRAM(uint16_t addr, MemoryOperationType memoryOperationType)
{
	/*if(_mapperSelection == 20) {
		bool isNtFetch = addr >= 0x2000 && addr <= 0x2FFF && (addr & 0x3FF) < 0x3C0;
	if(isNtFetch) {
		//Nametable data, not an attribute fetch
		if(_ppuInFrame) {
			UpdateChrBanks(false);
		} else if(_needInFrame) {
			_needInFrame = false;
			_ppuInFrame = true;
			UpdateChrBanks(false);
		}
	}
	DetectScanlineStartMMC5(addr);

	_ppuIdleCounter = 3;
	_lastPpuReadAddr = addr;

	// no vertical split, no extended ram modes, no midframe ch...
	}*/
	return InternalReadVRAM(addr);
}

void Coolgirl::WriteFlash(uint16_t addr, uint8_t value)
{

}