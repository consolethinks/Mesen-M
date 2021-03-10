#pragma once
#include "stdafx.h"
#include <cmath>
#include "Types.h"
#include "NESHeader.h"
#include "RomDataTypes.h"

enum class RomHeaderVersion
{
	iNes = 0,
	Nes2_0 = 1,
	OldiNes = 2
};

struct GameInfo
{
	uint32_t Crc;
	string System;
	string Board;
	string Pcb;
	string Chip;
	uint16_t MapperID;
	uint32_t PrgRomSize;
	uint32_t ChrRomSize;
	uint32_t ChrRamSize;
	uint32_t WorkRamSize;
	uint32_t SaveRamSize;
	bool HasBattery;
	string Mirroring;
	GameInputType InputType;
	string BusConflicts;
	string SubmapperID;
	VsSystemType VsType;
	PpuModel VsPpuModel;
};

struct RomInfo
{
	string RomName;
	string Filename;
	RomFormat Format;

	bool IsNes20Header = false;
	bool IsInDatabase = false;
	bool IsHeaderlessRom = false;

	uint32_t FilePrgOffset = 0;

	uint16_t MapperID = 0;
	uint8_t SubMapperID = 0;
	
	GameSystem System = GameSystem::Unknown;
	VsSystemType VsType = VsSystemType::Default;
	GameInputType InputType = GameInputType::Unspecified;
	PpuModel VsPpuModel = PpuModel::Ppu2C02;

	bool HasChrRam = false;
	bool HasBattery = false;
	bool HasTrainer = false;
	MirroringType Mirroring = MirroringType::Horizontal;
	BusConflictType BusConflicts = BusConflictType::Default;

	HashInfo Hash;

	NESHeader NesHeader;
	NsfHeader NsfInfo;
	GameInfo DatabaseInfo;
};

struct PageInfo
{
	uint32_t LeadInOffset;
	uint32_t AudioOffset;
	vector<uint8_t> Data;
};

struct StudyBoxData
{
	string FileName;
	vector<uint8_t> AudioFile;
	vector<PageInfo> Pages;
};

struct RomData
{
	RomInfo Info;

	int32_t ChrRamSize = -1;
	int32_t SaveChrRamSize = -1;
	int32_t SaveRamSize = -1;
	int32_t WorkRamSize = -1;
	
	vector<uint8_t> PrgRom;
	vector<uint8_t> ChrRom;
	vector<uint8_t> TrainerData;
	vector<vector<uint8_t>> FdsDiskData;
	vector<vector<uint8_t>> FdsDiskHeaders;
	StudyBoxData StudyBox;

	vector<uint8_t> RawData;

	bool Error = false;
	bool BiosMissing = false;
};
