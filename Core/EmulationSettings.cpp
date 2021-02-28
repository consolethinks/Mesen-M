#include "stdafx.h"
#include "EmulationSettings.h"
#include "Console.h"
#include "RewindManager.h"

//Version 0.10.0
uint16_t EmulationSettings::_versionMajor = 0;
uint8_t EmulationSettings::_versionMinor = 10;
uint8_t EmulationSettings::_versionRevision = 0;

SimpleLock EmulationSettings::_lock;
SimpleLock EmulationSettings::_shortcutLock;
SimpleLock EmulationSettings::_equalizerLock;

const vector<uint32_t> EmulationSettings::_speedValues = { { 1, 3, 6, 12, 25, 50, 75, 100, 150, 200, 250, 300, 350, 400, 450, 500, 750, 1000, 2000, 4000 } };

Language EmulationSettings::_displayLanguage = Language::English;

uint32_t EmulationSettings::GetEmulationSpeed(bool ignoreTurbo)
{
	if(ignoreTurbo) {
		return _emulationSpeed;
	} else if(CheckFlag(EmulationFlags::ForceMaxSpeed)) {
		return 0;
	} else if(CheckFlag(EmulationFlags::Turbo)) {
		return _turboSpeed;
	} else if(CheckFlag(EmulationFlags::Rewind)) {
		return _rewindSpeed;
	} else {
		return _emulationSpeed;
	}
}

double EmulationSettings::GetAspectRatio(shared_ptr<Console> console)
{
	switch(_aspectRatio) {
		case VideoAspectRatio::NoStretching: return 0.0;

		case VideoAspectRatio::Auto:
		{
			NesModel model = GetNesModel();
			if(model == NesModel::Auto) {
				model = console->GetModel();
			}
			return (model == NesModel::PAL || model == NesModel::Dendy) ? (9440000.0 / 6384411.0) : (128.0 / 105.0);
		}

		case VideoAspectRatio::NTSC: return 128.0 / 105.0;
		case VideoAspectRatio::PAL: return 9440000.0 / 6384411.0;
		case VideoAspectRatio::Standard: return 4.0 / 3.0;
		case VideoAspectRatio::Widescreen: return 16.0 / 9.0;
		case VideoAspectRatio::Custom: return _customAspectRatio;
	}
	return 0.0;
}

void EmulationSettings::InitializeInputDevices(GameInputType inputType, GameSystem system, bool silent)
{
	ControllerType controllers[4] = { ControllerType::StandardController, ControllerType::StandardController, ControllerType::None, ControllerType::None };
	ExpansionPortDevice expDevice = ExpansionPortDevice::None;
	ClearFlags(EmulationFlags::HasFourScore);

	auto log = [silent](string text) {
		if(!silent) {
			MessageManager::Log(text);
		}
	};

	bool isFamicom = (system == GameSystem::Famicom || system == GameSystem::FDS || system == GameSystem::Dendy);

	switch(inputType) {
		case GameInputType::VsZapper:
			//VS Duck Hunt, etc. need the zapper in the first port
			log("[Input] VS Zapper connected");
			controllers[0] = ControllerType::Zapper;
			break;
		case GameInputType::Zapper:
			log("[Input] Zapper connected");
			if(isFamicom) {
				expDevice = ExpansionPortDevice::Zapper;
			} else {
				controllers[1] = ControllerType::Zapper;
			}
			break;
		case GameInputType::FourScore:
			log("[Input] Four score connected");
			SetFlags(EmulationFlags::HasFourScore);
			controllers[2] = controllers[3] = ControllerType::StandardController;
			break;
		case GameInputType::FourPlayerAdapter:
			log("[Input] Four player adapter connected");
			SetFlags(EmulationFlags::HasFourScore);
			expDevice = ExpansionPortDevice::FourPlayerAdapter;
			controllers[2] = controllers[3] = ControllerType::StandardController;
			break;
		case GameInputType::ArkanoidControllerFamicom:
			log("[Input] Arkanoid controller (Famicom) connected");
			expDevice = ExpansionPortDevice::ArkanoidController;
			break;
		case GameInputType::ArkanoidControllerNes:
			log("[Input] Arkanoid controller (NES) connected");
			controllers[1] = ControllerType::ArkanoidController;
			break;
		case GameInputType::DoubleArkanoidController:
			log("[Input] 2x arkanoid controllers (NES) connected");
			controllers[0] = ControllerType::ArkanoidController;
			controllers[1] = ControllerType::ArkanoidController;
			break;
		case GameInputType::OekaKidsTablet:
			log("[Input] Oeka Kids Tablet connected");
			system = GameSystem::Famicom;
			expDevice = ExpansionPortDevice::OekaKidsTablet;
			break;
		case GameInputType::KonamiHyperShot:
			log("[Input] Konami Hyper Shot connected");
			system = GameSystem::Famicom;
			expDevice = ExpansionPortDevice::KonamiHyperShot;
			break;
		case GameInputType::FamilyBasicKeyboard:
			log("[Input] Family Basic Keyboard connected");
			system = GameSystem::Famicom;
			expDevice = ExpansionPortDevice::FamilyBasicKeyboard;
			break;
		case GameInputType::PartyTap:
			log("[Input] Party Tap connected");
			system = GameSystem::Famicom;
			expDevice = ExpansionPortDevice::PartyTap;
			break;
		case GameInputType::PachinkoController:
			log("[Input] Pachinko controller connected");
			system = GameSystem::Famicom;
			expDevice = ExpansionPortDevice::Pachinko;
			break;
		case GameInputType::ExcitingBoxing:
			log("[Input] Exciting Boxing controller connected");
			system = GameSystem::Famicom;
			expDevice = ExpansionPortDevice::ExcitingBoxing;
			break;
		case GameInputType::SuborKeyboard:
			log("[Input] Subor keyboard connected");
			system = GameSystem::Famicom;
			expDevice = ExpansionPortDevice::SuborKeyboard;
			break;
		case GameInputType::SuborKeyboardMouse1:
			log("[Input] Subor mouse connected");
			log("[Input] Subor keyboard connected");
			system = GameSystem::Famicom;
			expDevice = ExpansionPortDevice::SuborKeyboard;
			controllers[1] = ControllerType::SuborMouse;
			break;
		case GameInputType::JissenMahjong:
			log("[Input] Jissen Mahjong controller connected");
			system = GameSystem::Famicom;
			expDevice = ExpansionPortDevice::JissenMahjong;
			break;
		case GameInputType::BarcodeBattler:
			log("[Input] Barcode Battler barcode reader connected");
			system = GameSystem::Famicom;
			expDevice = ExpansionPortDevice::BarcodeBattler;
			break;
		case GameInputType::BandaiHypershot:
			log("[Input] Bandai Hyper Shot gun connected");
			system = GameSystem::Famicom;
			expDevice = ExpansionPortDevice::BandaiHyperShot;
			break;
		case GameInputType::BattleBox:
			log("[Input] Battle Box connected");
			system = GameSystem::Famicom;
			expDevice = ExpansionPortDevice::BattleBox;
			break;
		case GameInputType::TurboFile:
			log("[Input] Ascii Turbo File connected");
			system = GameSystem::Famicom;
			expDevice = ExpansionPortDevice::AsciiTurboFile;
			break;
		case GameInputType::FamilyTrainerSideA:
		case GameInputType::FamilyTrainerSideB:
			log("[Input] Family Trainer mat connected");
			system = GameSystem::Famicom;
			expDevice = ExpansionPortDevice::FamilyTrainerMat;
			break;
		case GameInputType::PowerPadSideA:
		case GameInputType::PowerPadSideB:
			log("[Input] Power Pad connected");
			system = GameSystem::NesNtsc;
			controllers[1] = ControllerType::PowerPad;
			break;
		case GameInputType::SnesControllers:
			log("[Input] 2 SNES controllers connected");
			controllers[0] = ControllerType::SnesController;
			controllers[1] = ControllerType::SnesController;
			break;
		default:
			log("[Input] 2 standard controllers connected");
	}

	isFamicom = (system == GameSystem::Famicom || system == GameSystem::FDS || system == GameSystem::Dendy);
	SetConsoleType(isFamicom ? ConsoleType::Famicom : ConsoleType::Nes);
	for(int i = 0; i < 4; i++) {
		SetControllerType(i, controllers[i]);
	}
	SetExpansionDevice(expDevice);
}

const vector<string> NesModelNames = {
	"Auto",
	"NTSC",
	"PAL",
	"Dendy"
};

const vector<string> ConsoleTypeNames = {
	"Nes",
	"Famicom",
};

const vector<string> ControllerTypeNames = {
	"None",
	"StandardController",
	"Zapper",
	"ArkanoidController",
	"SnesController",
	"PowerPad",
	"SnesMouse",
	"SuborMouse",
	"VsZapper",
	"VbController",
};

const vector<string> ExpansionPortDeviceNames = {
	"None",
	"Zapper",
	"FourPlayerAdapter",
	"ArkanoidController",
	"OekaKidsTablet",
	"FamilyTrainerMat",
	"KonamiHyperShot",
	"FamilyBasicKeyboard",
	"PartyTap",
	"Pachinko",
	"ExcitingBoxing",
	"JissenMahjong",
	"SuborKeyboard",
	"BarcodeBattler",
	"HoriTrack",
	"BandaiHyperShot",
	"AsciiTurboFile",
	"BattleBox",
};
