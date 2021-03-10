#pragma once

#include "stdafx.h"

enum EmulationFlags : uint64_t
{
	Paused = 0x01,
	ShowFPS = 0x02,
	VerticalSync = 0x04,
	AllowInvalidInput = 0x08,
	RemoveSpriteLimit = 0x10,
	UseHdPacks = 0x20,
	HasFourScore = 0x40,

	DisableDynamicSampleRate = 0x80,

	PauseOnMovieEnd = 0x0100,

	EnablePpuOamRowCorruption = 0x0200,

	AllowBackgroundInput = 0x0400,
	ReduceSoundInBackground = 0x0800,
	MuteSoundInBackground = 0x1000,

	FdsFastForwardOnLoad = 0x2000,
	FdsAutoLoadDisk = 0x4000,
	Mmc3IrqAltBehavior = 0x8000,
	
	SwapDutyCycles = 0x10000,

	DisableGameSelectionScreen = 0x20000,

	AutoConfigureInput = 0x40000,

	ShowLagCounter = 0x80000,

	SilenceTriangleHighFreq = 0x100000,
	ReduceDmcPopping = 0x200000,

	DisableBackground = 0x400000,
	DisableSprites = 0x800000,
	ForceBackgroundFirstColumn = 0x1000000,
	ForceSpritesFirstColumn = 0x2000000,
	DisablePpu2004Reads = 0x4000000,
	DisableNoiseModeFlag = 0x8000000,
	DisablePaletteRead = 0x10000000,
	DisableOamAddrBug = 0x20000000,
	DisablePpuReset = 0x40000000,
	EnableOamDecay = 0x80000000,

	UseNes101Hvc101Behavior = 0x100000000,
	ShowFrameCounter = 0x200000000,
	ShowGameTimer = 0x400000000,

	FdsAutoInsertDisk = 0x800000000,

	Rewind =  0x1000000000,
	Turbo = 0x2000000000,
	InBackground = 0x4000000000,
	NsfPlayerEnabled = 0x8000000000,
	
	DisplayMovieIcons = 0x10000000000,
	HidePauseOverlay = 0x20000000000,

	UseCustomVsPalette = 0x40000000000,

	AdaptiveSpriteLimit = 0x80000000000,

	EnablePpu2006ScrollGlitch = 0x100000000000,
	EnablePpu2000ScrollGlitch = 0x200000000000,

	ConfirmExitResetPower = 0x400000000000,

	NsfRepeat = 0x800000000000,
	NsfShuffle = 0x1000000000000,

	IntegerFpsMode = 0x2000000000000,

	DebuggerWindowEnabled = 0x4000000000000,
	BreakOnCrash = 0x8000000000000,
	
	AllowMismatchingSaveState = 0x10000000000000,

	RandomizeMapperPowerOnState = 0x20000000000000,
	
	UseHighResolutionTimer = 0x40000000000000,
	DisplayDebugInfo = 0x80000000000000,

	ReduceSoundInFastForward = 0x100000000000000,

	VsDualMuteMaster = 0x200000000000000,
	VsDualMuteSlave = 0x400000000000000,
	
	RandomizeCpuPpuAlignment = 0x800000000000000,
	
	ForceMaxSpeed = 0x4000000000000000,	
	ConsoleMode = 0x8000000000000000,
}; 


enum class AudioChannel
{
	Square1 = 0,
	Square2 = 1,
	Triangle = 2,
	Noise = 3,
	DMC = 4,
	FDS = 5,
	MMC5 = 6,
	VRC6 = 7,
	VRC7 = 8,
	Namco163 = 9,
	Sunsoft5B = 10
};

enum class EqualizerFilterType
{
	None = 0,
	Butterworth = 1,
	Chebyshev1 = 2,
	Chebyshev2 = 3
};

enum class ScaleFilterType
{
	xBRZ = 0,
	HQX = 1,
	Scale2x = 2,
	_2xSai = 3,
	Super2xSai = 4,
	SuperEagle = 5,
	Prescale = 6,
};

enum class VideoFilterType
{
	None = 0,
	NTSC = 1,
	BisqwitNtscQuarterRes = 2,
	BisqwitNtscHalfRes = 3,
	BisqwitNtsc = 4,
	xBRZ2x = 5,
	xBRZ3x = 6,
	xBRZ4x = 7,
	xBRZ5x = 8,
	xBRZ6x = 9,
	HQ2x = 10,
	HQ3x = 11,
	HQ4x = 12,
	Scale2x = 13,
	Scale3x = 14,
	Scale4x = 15,
	_2xSai = 16,
	Super2xSai = 17,
	SuperEagle = 18,
	Prescale2x = 19,
	Prescale3x = 20,
	Prescale4x = 21,
	Prescale6x = 22,
	Prescale8x = 23,
	Prescale10x = 24,
	Raw = 25,
	HdPack = 999
};

enum class VideoResizeFilter
{
	NearestNeighbor = 0,
	Bilinear = 1
};

enum class VideoAspectRatio
{
	NoStretching = 0,
	Auto = 1,
	NTSC = 2,
	PAL = 3,
	Standard = 4,
	Widescreen = 5,
	Custom = 6
};

struct OverscanDimensions
{
	uint32_t Left = 0;
	uint32_t Right = 0;
	uint32_t Top = 0;
	uint32_t Bottom = 0;

	uint32_t GetPixelCount()
	{
		return GetScreenWidth() * GetScreenHeight();
	}

	uint32_t GetScreenWidth()
	{
		return 256 - Left - Right;
	}

	uint32_t GetScreenHeight()
	{
		return 240 - Top - Bottom;
	}
};

struct PictureSettings
{
	double Brightness = 0;
	double Contrast = 0;
	double Saturation = 0;
	double Hue = 0;
	double ScanlineIntensity = 0;
};

struct NtscFilterSettings
{
	double Sharpness = 0;
	double Gamma = 0;
	double Resolution = 0;
	double Artifacts = 0;
	double Fringing = 0;
	double Bleed = 0;
	bool MergeFields = false;
	bool VerticalBlend = false;
	bool KeepVerticalResolution = false;

	double YFilterLength = 0;
	double IFilterLength = 0;
	double QFilterLength = 0;
};

enum class RamPowerOnState
{
	AllZeros = 0,
	AllOnes = 1,
	Random = 2
};

extern const vector<string> NesModelNames;
enum class NesModel
{
	Auto = 0,
	NTSC = 1,
	PAL = 2,
	Dendy = 3,
};

extern const vector<string> ConsoleTypeNames;
enum class ConsoleType
{
	Nes = 0,
	Famicom = 1
};

extern const vector<string> ControllerTypeNames;
enum class ControllerType
{
	None = 0,
	StandardController = 1,
	Zapper = 2,
	ArkanoidController = 3,
	SnesController = 4,
	PowerPad = 5,
	SnesMouse = 6,
	SuborMouse = 7,
	VsZapper = 8,
	VbController = 9,
};

extern const vector<string> ExpansionPortDeviceNames;
enum class ExpansionPortDevice
{
	None = 0,
	Zapper = 1,
	FourPlayerAdapter = 2,
	ArkanoidController = 3,
	OekaKidsTablet = 4,
	FamilyTrainerMat = 5,
	KonamiHyperShot = 6,
	FamilyBasicKeyboard = 7,
	PartyTap = 8,
	Pachinko = 9,
	ExcitingBoxing = 10,
	JissenMahjong = 11,
	SuborKeyboard = 12,
	BarcodeBattler = 13,
	HoriTrack = 14,
	BandaiHyperShot = 15,
	AsciiTurboFile = 16,
	BattleBox = 17,
};

enum class VsInputType
{
	Default = 0,
	SwapControllers = 1,
	SwapAB = 2
};

struct KeyMapping
{
	uint32_t A = 0;
	uint32_t B = 0;
	uint32_t Up = 0;
	uint32_t Down = 0;
	uint32_t Left = 0;
	uint32_t Right = 0;
	uint32_t Start = 0;
	uint32_t Select = 0;
	uint32_t TurboA = 0;
	uint32_t TurboB = 0;
	uint32_t TurboStart = 0;
	uint32_t TurboSelect = 0;
	uint32_t Microphone = 0;
	uint32_t LButton = 0;
	uint32_t RButton = 0;

	uint32_t PowerPadButtons[12] = {};
	uint32_t FamilyBasicKeyboardButtons[72] = {};
	uint32_t PartyTapButtons[6] = {};
	uint32_t PachinkoButtons[2] = {};
	uint32_t ExcitingBoxingButtons[8] = {};
	uint32_t JissenMahjongButtons[21] = {};
	uint32_t SuborKeyboardButtons[99] = {};
	uint32_t BandaiMicrophoneButtons[3] = {};
	uint32_t VirtualBoyButtons[14] = {};

	bool HasKeySet()
	{
		if(A || B || Up || Down || Left || Right || Start || Select || TurboA || TurboB || TurboStart || TurboSelect || Microphone || LButton || RButton) {
			return true;
		}

		bool hasKeyBinding = false;
		hasKeyBinding |= HasKeyBinding(PowerPadButtons, sizeof(PowerPadButtons) / sizeof(PowerPadButtons[0]));
		hasKeyBinding |= HasKeyBinding(FamilyBasicKeyboardButtons, sizeof(FamilyBasicKeyboardButtons) / sizeof(FamilyBasicKeyboardButtons[0]));
		hasKeyBinding |= HasKeyBinding(PartyTapButtons, sizeof(PartyTapButtons) / sizeof(PartyTapButtons[0]));
		hasKeyBinding |= HasKeyBinding(PachinkoButtons, sizeof(PachinkoButtons) / sizeof(PachinkoButtons[0]));
		hasKeyBinding |= HasKeyBinding(ExcitingBoxingButtons, sizeof(ExcitingBoxingButtons) / sizeof(ExcitingBoxingButtons[0]));
		hasKeyBinding |= HasKeyBinding(JissenMahjongButtons, sizeof(JissenMahjongButtons) / sizeof(JissenMahjongButtons[0]));
		hasKeyBinding |= HasKeyBinding(SuborKeyboardButtons, sizeof(SuborKeyboardButtons) / sizeof(SuborKeyboardButtons[0]));
		hasKeyBinding |= HasKeyBinding(BandaiMicrophoneButtons, sizeof(BandaiMicrophoneButtons) / sizeof(BandaiMicrophoneButtons[0]));
		hasKeyBinding |= HasKeyBinding(VirtualBoyButtons, sizeof(VirtualBoyButtons) / sizeof(VirtualBoyButtons[0]));
		return hasKeyBinding;
	}

private:
	bool HasKeyBinding(uint32_t* buttons, uint32_t count)
	{
		for(uint32_t i = 0; i < count; i++) {
			if(buttons[i] != 0) {
				return true;
			}
		}
		return false;
	}
};

struct KeyMappingSet
{
	KeyMapping Mapping1;
	KeyMapping Mapping2;
	KeyMapping Mapping3;
	KeyMapping Mapping4;
	uint32_t TurboSpeed = 0;
	bool PowerpadUseSideA = false;

	vector<KeyMapping> GetKeyMappingArray()
	{
		vector<KeyMapping> keyMappings;
		if(Mapping1.HasKeySet()) {
			keyMappings.push_back(Mapping1);
		}
		if(Mapping2.HasKeySet()) {
			keyMappings.push_back(Mapping2);
		}
		if(Mapping3.HasKeySet()) {
			keyMappings.push_back(Mapping3);
		}
		if(Mapping4.HasKeySet()) {
			keyMappings.push_back(Mapping4);
		}
		return keyMappings;
	}
};

enum class EmulatorShortcut
{
	FastForward,
	Rewind,
	RewindTenSecs,
	RewindOneMin,

	SelectSaveSlot1,
	SelectSaveSlot2,
	SelectSaveSlot3,
	SelectSaveSlot4,
	SelectSaveSlot5,
	SelectSaveSlot6,
	SelectSaveSlot7,
	SelectSaveSlot8,
	SelectSaveSlot9,
	SelectSaveSlot10,
	MoveToNextStateSlot,
	MoveToPreviousStateSlot,
	SaveState,
	LoadState,

	InsertNextDisk,
	VsServiceButton,
	VsServiceButton2,

	ToggleCheats,
	ToggleFastForward,
	ToggleRewind,
	ToggleKeyboardMode,

	RunSingleFrame,

	// Everything below this is handled UI-side
	SwitchDiskSide,
	EjectDisk,

	InsertCoin1,
	InsertCoin2,
	InsertCoin3,
	InsertCoin4,
	
	InputBarcode,

	TakeScreenshot,

	ToggleRecordVideo,
	ToggleRecordAudio,
	ToggleRecordMovie,

	IncreaseSpeed,
	DecreaseSpeed,
	MaxSpeed,

	Pause,
	Reset,
	PowerCycle,
	ReloadRom,
	PowerOff,
	Exit,

	SetScale1x,
	SetScale2x,
	SetScale3x,
	SetScale4x,
	SetScale5x,
	SetScale6x,
	ToggleFullscreen,
	ToggleFps,
	ToggleGameTimer,
	ToggleFrameCounter,
	ToggleLagCounter,
	ToggleOsd,
	ToggleAlwaysOnTop,
	ToggleSprites,
	ToggleBackground,
	ToggleDebugInfo,
	ToggleAudio,
	IncreaseVolume,
	DecreaseVolume,

	LoadRandomGame,
	SaveStateSlot1,
	SaveStateSlot2,
	SaveStateSlot3,
	SaveStateSlot4,
	SaveStateSlot5,
	SaveStateSlot6,
	SaveStateSlot7,
	SaveStateSlot8,
	SaveStateSlot9,
	SaveStateSlot10,
	SaveStateToFile,
	SaveStateDialog,

	LoadStateSlot1,
	LoadStateSlot2,
	LoadStateSlot3,
	LoadStateSlot4,
	LoadStateSlot5,
	LoadStateSlot6,
	LoadStateSlot7,
	LoadStateSlot8,
	LoadStateSlot9,
	LoadStateSlot10,
	LoadStateSlotAuto,
	LoadStateFromFile,
	LoadStateDialog,

	LoadLastSession,

	OpenFile,
	ShortcutCount
};

struct KeyCombination
{
	uint32_t Key1 = 0;
	uint32_t Key2 = 0;
	uint32_t Key3 = 0;

	vector<uint32_t> GetKeys()
	{
		vector<uint32_t> result;
		if(Key1) {
			result.push_back(Key1);
		}
		if(Key2) {
			result.push_back(Key2);
		}
		if(Key3) {
			result.push_back(Key3);
		}
		return result;
	}

	bool IsSubsetOf(KeyCombination keyCombination)
	{
		vector<uint32_t> myKeys = GetKeys();
		vector<uint32_t> otherKeys = keyCombination.GetKeys();

		if(otherKeys.size() > myKeys.size()) {
			for(size_t i = 0; i < myKeys.size(); i++) {
				if(std::find(otherKeys.begin(), otherKeys.end(), myKeys[i]) == otherKeys.end()) {
					//Current key combination contains a key not found in the other combination, so it's not a subset
					return false;
				}
			}
			return true;
		}
		return false;
	}
};

enum class Language
{
	//SystemDefault = 0,  //This value is never used by the C++ core
	English = 1,
	French = 2,
	Japanese = 3,
	Russian = 4,
	Spanish = 5,
	Ukrainian = 6,
	Portuguese = 7,
	Catalan = 8,
	Chinese = 9,
	Italian = 10,
};

enum class MouseDevice
{
	Unknown = 0,
	SnesMouse,
	SuborMouse,
	ArkanoidController,
	HoriTrack
};

enum class StereoFilter
{
	None = 0,
	Delay = 1,
	Panning = 2,
	CombFilter = 3,
};

struct AudioFilterSettings
{
	StereoFilter Filter = StereoFilter::None;
	double Angle = 0;
	int32_t Delay = 0;
	int32_t Strength = 0;

	double ReverbDelay = 0;
	double ReverbStrength = 0;

	int32_t CrossFadeRatio = 0;
};

enum class InputDisplayPosition
{
	TopLeft = 0,
	TopRight = 1,
	BottomLeft = 2,
	BottomRight = 3
};

struct InputDisplaySettings
{
	uint8_t VisiblePorts;
	InputDisplayPosition DisplayPosition;
	bool DisplayHorizontally;
};
