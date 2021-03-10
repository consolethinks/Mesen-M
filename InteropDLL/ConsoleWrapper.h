#include <stdint.h>
#include "stdafx.h"
#include "../Core/EmulationSettingsTypes.h"

#include "../Core/CheatManager.h"
#include "../Utilities/AviWriterCodec.h"
#include "../Core/Types.h"
#include "../Core/VideoDecoderTypes.h"
#include "../Core/RomDataTypes.h"
#include "../Core/INotificationListener.h"

typedef void (__stdcall *NotificationListenerCallback)(int, void*);

namespace InteropDll {
	struct InteropRomInfo
	{
		const char* RomName;
		uint32_t Crc32;
		uint32_t PrgCrc32;
		RomFormat Format;
		bool IsChrRam;
		bool HasBusConflicts;
		uint16_t MapperId;
		uint32_t FilePrgOffset;
		char Sha1[40];
	};
	
   extern "C" {
      bool __stdcall TestDll();
      uint32_t __stdcall GetMesenVersion();
      void __stdcall InitDll();
      void __stdcall InitializeEmu(const char* homeFolder, void *windowHandle, void *viewerHandle, bool noAudio, bool noVideo, bool noInput);
      void __stdcall InitializeDualSystem(void *windowHandle, void *viewerHandle);
      void __stdcall ReleaseDualSystemAudioVideo();
      bool __stdcall HistoryViewerEnabled();
      void __stdcall HistoryViewerInitialize(void *windowHandle, void *viewerHandle);
      void __stdcall HistoryViewerRelease();
      void __stdcall HistoryViewerRun();
      uint32_t __stdcall HistoryViewerGetHistoryLength();
      void __stdcall HistoryViewerGetSegments(uint32_t* segmentBuffer, uint32_t &bufferSize);
      bool __stdcall HistoryViewerCreateSaveState(const char* outputFile, uint32_t position);
      bool __stdcall HistoryViewerSaveMovie(const char* movieFile, uint32_t startPosition, uint32_t endPosition);
      void __stdcall HistoryViewerResumeGameplay(uint32_t resumeAtSecond);
      void __stdcall HistoryViewerSetPosition(uint32_t seekPosition);
      uint32_t __stdcall HistoryViewerGetPosition();
      void __stdcall SetFullscreenMode(bool fullscreen, void *windowHandle, uint32_t monitorWidth, uint32_t monitorHeight);
      bool __stdcall IsRunning();
      int32_t __stdcall GetStopCode();
      void __stdcall LoadROM(char* filename, char* patchFile);
      void __stdcall AddKnownGameFolder(char* folder);
      void __stdcall SetFolderOverrides(char* saveFolder, char* saveStateFolder, char* screenshotFolder);
      void __stdcall LoadRecentGame(char* filepath, bool resetGame);
      const char* __stdcall GetArchiveRomList(char* filename);
      void __stdcall SetControllerType(uint32_t port, ControllerType type);
      void __stdcall SetControllerKeys(uint32_t port, KeyMappingSet mappings);
      void __stdcall SetZapperDetectionRadius(uint32_t detectionRadius);
      void __stdcall SetControllerDeadzoneSize(uint32_t deadzoneSize);
      void __stdcall SetExpansionDevice(ExpansionPortDevice device);
      void __stdcall SetConsoleType(ConsoleType type);
      void __stdcall SetMouseSensitivity(MouseDevice device, double sensitivity);
      void __stdcall ClearShortcutKeys();
      void __stdcall SetShortcutKey(EmulatorShortcut shortcut, KeyCombination keyCombination, int keySetIndex);
      ControllerType __stdcall GetControllerType(uint32_t port);
      ExpansionPortDevice GetExpansionDevice();
      ConsoleType __stdcall GetConsoleType();
      bool __stdcall HasZapper();
      bool __stdcall HasFourScore();
      bool __stdcall HasArkanoidPaddle();
      void __stdcall SetMousePosition(double x, double y);
      void __stdcall SetMouseMovement(int16_t x, int16_t y);
      void __stdcall UpdateInputDevices();
      void __stdcall GetPressedKeys(uint32_t *keyBuffer);
      void __stdcall DisableAllKeys(bool disabled);
      void __stdcall SetKeyState(int32_t scanCode, bool state);
      void __stdcall ResetKeyState();
      const char* __stdcall GetKeyName(uint32_t keyCode);
      uint32_t __stdcall GetKeyCode(char* keyName);
      void __stdcall Run();
      void __stdcall Resume(ConsoleId consoleId);
      bool __stdcall IsPaused(ConsoleId consoleId);
      void __stdcall Pause(ConsoleId consoleId);
      void __stdcall Stop();
      void __stdcall GetRomInfo(InteropRomInfo &interopRomInfo, char* filename);
      void __stdcall Reset();
      void __stdcall PowerCycle();
      void __stdcall ReloadRom();
      void __stdcall ResetLagCounter();
      void __stdcall StartServer(uint16_t port, char* password, char* hostPlayerName);
      void __stdcall StopServer();
      bool __stdcall IsServerRunning();
      void __stdcall Connect(char* host, uint16_t port, char* password, char* playerName, bool spectator);
      void __stdcall Disconnect();
      bool __stdcall IsConnected();
      ControllerType __stdcall NetPlayGetControllerType(int32_t port);
      int32_t __stdcall NetPlayGetAvailableControllers();
      void __stdcall NetPlaySelectController(int32_t port);
      int32_t __stdcall NetPlayGetControllerPort();
      void __stdcall Release();
      void __stdcall TakeScreenshot();
      INotificationListener* __stdcall RegisterNotificationCallback(ConsoleId consoleId, NotificationListenerCallback callback);
      void __stdcall UnregisterNotificationCallback(INotificationListener *listener);
      void __stdcall DisplayMessage(char* title, char* message, char* param1);
      const char* __stdcall GetLog();
      void __stdcall WriteLogEntry(char* message);
      void __stdcall SaveState(uint32_t stateIndex);
      void __stdcall LoadState(uint32_t stateIndex);
      void __stdcall SaveStateFile(char* filepath);
      void __stdcall LoadStateFile(char* filepath);
      int32_t __stdcall GetSaveStatePreview(char* saveStatePath, uint8_t* pngData);
      void __stdcall MoviePlay(char* filename);
      void __stdcall MovieRecord(RecordMovieOptions *options);
      void __stdcall MovieStop();
      bool __stdcall MoviePlaying();
      bool __stdcall MovieRecording();
      void __stdcall AviRecord(char* filename, VideoCodec codec, uint32_t compressionLevel);
      void __stdcall AviStop();
      bool __stdcall AviIsRecording();
      void __stdcall WaveRecord(char* filename);
      void __stdcall WaveStop();
      bool __stdcall WaveIsRecording();
      int32_t __stdcall RunRecordedTest(char* filename);
      int32_t __stdcall RunAutomaticTest(char* filename);
      void __stdcall RomTestRecord(char* filename, bool reset);
      void __stdcall RomTestRecordFromMovie(char* testFilename, char* movieFilename);
      void __stdcall RomTestRecordFromTest(char* newTestFilename, char* existingTestFilename);
      void __stdcall RomTestStop();
      bool __stdcall RomTestRecording();
      void __stdcall SetCheats(CheatInfo cheats[], uint32_t length);
      void __stdcall SetOsdState(bool enabled);
      void __stdcall SetGameDatabaseState(bool enabled);
      bool __stdcall CheckFlag(EmulationFlags flags);
      void __stdcall SetFlags(EmulationFlags flags);
      void __stdcall ClearFlags(EmulationFlags flags);
      void __stdcall SetRamPowerOnState(RamPowerOnState state);
      void __stdcall SetDisplayLanguage(Language lang);
      void __stdcall SetChannelVolume(uint32_t channel, double volume);
      void __stdcall SetChannelPanning(uint32_t channel, double panning);
      void __stdcall SetEqualizerFilterType(EqualizerFilterType filter);
      void __stdcall SetBandGain(uint32_t band, double gain);
      void __stdcall SetEqualizerBands(double *bands, uint32_t bandCount);
      void __stdcall SetMasterVolume(double volume, double volumeReduction, ConsoleId consoleId);
      void __stdcall SetSampleRate(uint32_t sampleRate);
      void __stdcall SetAudioLatency(uint32_t msLatency);
      void __stdcall SetAudioFilterSettings(AudioFilterSettings settings);
      void __stdcall SetRunAheadFrames(uint32_t frameCount);
      NesModel __stdcall GetNesModel();
      void __stdcall SetNesModel(uint32_t model);
      void __stdcall SetOverscanDimensions(uint32_t left, uint32_t right, uint32_t top, uint32_t bottom);
      void __stdcall SetEmulationSpeed(uint32_t emulationSpeed);
      void __stdcall IncreaseEmulationSpeed();
      void __stdcall DecreaseEmulationSpeed();
      uint32_t __stdcall GetEmulationSpeed();
      void __stdcall SetTurboRewindSpeed(uint32_t turboSpeed, uint32_t rewindSpeed);
      void __stdcall SetRewindBufferSize(uint32_t seconds);
      bool __stdcall IsRewinding();
      void __stdcall SetPpuNmiConfig(uint32_t extraScanlinesBeforeNmi, uint32_t extraScanlinesAfterNmi);
      void __stdcall SetVideoScale(double scale, ConsoleId consoleId);
      void __stdcall SetScreenRotation(uint32_t angle);
      void __stdcall SetExclusiveRefreshRate(uint32_t angle);
      void __stdcall SetVideoAspectRatio(VideoAspectRatio aspectRatio, double customRatio);
      void __stdcall SetVideoFilter(VideoFilterType filter);
      void __stdcall SetVideoResizeFilter(VideoResizeFilter filter);
      void __stdcall GetRgbPalette(uint32_t *paletteBuffer);
      void __stdcall SetRgbPalette(uint32_t *paletteBuffer, uint32_t paletteSize);
      void __stdcall SetPictureSettings(double brightness, double contrast, double saturation, double hue, double scanlineIntensity);
      void __stdcall SetNtscFilterSettings(double artifacts, double bleed, double fringing, double gamma, double resolution, double sharpness, bool mergeFields, double yFilterLength, double iFilterLength, double qFilterLength, bool verticalBlend);
      void __stdcall SetPauseScreenMessage(char* message);
      void __stdcall SetInputDisplaySettings(uint8_t visiblePorts, InputDisplayPosition displayPosition, bool displayHorizontally);
      void __stdcall SetAutoSaveOptions(uint32_t delayInMinutes, bool showMessage);
      const char* __stdcall GetAudioDevices();
      void __stdcall SetAudioDevice(char* audioDevice);
      void __stdcall GetScreenSize(ConsoleId consoleId, ScreenSize &size, bool ignoreScale);
      void __stdcall InputBarcode(uint64_t barCode, int32_t digitCount);
      void __stdcall LoadTapeFile(char *filepath);
      void __stdcall StartRecordingTapeFile(char *filepath);
      void __stdcall StopRecordingTapeFile();
      bool __stdcall IsRecordingTapeFile();
      bool __stdcall IsKeyboardMode();
      ConsoleFeatures __stdcall GetAvailableFeatures();
      //NSF functions
      bool __stdcall IsNsf();
      uint32_t __stdcall NsfGetFrameCount();
      void __stdcall NsfSelectTrack(uint8_t trackNumber);
      int32_t __stdcall NsfGetCurrentTrack();
      void __stdcall NsfGetHeader(NsfHeader* header);
      void __stdcall NsfSetNsfConfig(int32_t autoDetectSilenceDelay, int32_t moveToNextTrackTime, bool disableApuIrqs);
      //FDS functions
      uint32_t __stdcall FdsGetSideCount();
      void __stdcall FdsEjectDisk();
      void __stdcall FdsInsertDisk(uint32_t diskNumber);
      void __stdcall FdsSwitchDiskSide();
      bool __stdcall FdsIsAutoInsertDiskEnabled();
      //VS System functions
      bool __stdcall IsVsSystem();
      bool __stdcall IsVsDualSystem();
      void __stdcall VsInsertCoin(uint32_t port);
      uint32_t __stdcall GetDipSwitchCount();
      void __stdcall SetDipSwitches(uint32_t dipSwitches);
      void SetBootlegExpAudioSetting(bool value);
      // note: the capacity of respecting this setting is in ConsoleFeatures
      bool __stdcall IsHdPpu();
      void __stdcall HdBuilderStartRecording(char* saveFolder, ScaleFilterType filterType, uint32_t scale, uint32_t flags, uint32_t chrRamBankSize);
      void __stdcall HdBuilderStopRecording();
      void __stdcall HdBuilderGetChrBankList(uint32_t* bankBuffer);
      void __stdcall HdBuilderGetBankPreview(uint32_t bankNumber, uint32_t pageNumber, uint32_t *rgbBuffer);
   }
}
