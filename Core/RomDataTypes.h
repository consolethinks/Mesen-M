#pragma once

struct NsfHeader
{
	char Header[5];
	uint8_t Version;
	uint8_t TotalSongs;
	uint8_t StartingSong;
	uint16_t LoadAddress;
	uint16_t InitAddress;
	uint16_t PlayAddress;
	char SongName[256];
	char ArtistName[256];
	char CopyrightHolder[256];
	uint16_t PlaySpeedNtsc;
	uint8_t BankSetup[8];
	uint16_t PlaySpeedPal;
	uint8_t Flags;
	uint8_t SoundChips;
	uint8_t Padding[4];

	//NSFe extensions
	char RipperName[256];
	char TrackName[20000];
	int32_t TrackLength[256];
	int32_t TrackFade[256];
}; 
