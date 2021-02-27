#include "stdafx.h"
#include "NotificationManager.h"
#include "MapperFactory.h"
#include "RomLoader.h"
#include "UnifBoards.h"
#include "Mappers/BaseMapper.h"
#include "RomData.h"
#include "VirtualFile.h"

#include "Mappers/820720C.h"
#include "Mappers/970630C.h"
#include "Mappers/A65AS.h"
#include "Mappers/Ac08.h"
#include "Mappers/Action53.h"
#include "Mappers/ActionEnterprises.h"
#include "Mappers/AsderPC95.h"
#include "Mappers/Ax5705.h"
#include "Mappers/AXROM.h"
#include "Mappers/Bandai74161_7432.h"
#include "Mappers/BandaiFcg.h"
#include "Mappers/BandaiKaraoke.h"
#include "Mappers/Bb.h"
#include "Mappers/BF909x.h"
#include "Mappers/BF9096.h"
#include "Mappers/Bmc11160.h"
#include "Mappers/Bmc12in1.h"
#include "Mappers/Bmc51.h"
#include "Mappers/Bmc63.h"
#include "Mappers/Bmc64in1NoRepeat.h"
#include "Mappers/Bmc70in1.h"
#include "Mappers/Bmc190in1.h"
#include "Mappers/Bmc235.h"
#include "Mappers/Bmc255.h"
#include "Mappers/Bmc60311C.h"
#include "Mappers/Bmc8157.h"
#include "Mappers/Bmc80013B.h"
#include "Mappers/Bmc810544CA1.h"
#include "Mappers/Bmc830118C.h"
#include "Mappers/Bmc830425C4391T.h"
#include "Mappers/Bmc8in1.h"
#include "Mappers/BmcG146.h"
#include "Mappers/BmcGn45.h"
#include "Mappers/BmcHpxx.h"
#include "Mappers/BmcK3046.h"
#include "Mappers/BmcNtd03.h"
#include "Mappers/BnRom.h"
#include "Mappers/Bs5.h"
#include "Mappers/Caltron41.h"
#include "Mappers/Cc21.h"
#include "Mappers/Cheapocabra.h"
#include "Mappers/CityFighter.h"
#include "Mappers/CNROM.h"
#include "Mappers/CpRom.h"
#include "Mappers/ColorDreams.h"
#include "Mappers/ColorDreams46.h"
#include "Mappers/Dance2000.h"
#include "Mappers/DaouInfosys.h"
#include "Mappers/DragonFighter.h"
#include "Mappers/DreamTech01.h"
#include "Mappers/Edu2000.h"
#include "Mappers/Eh8813A.h"
#include "Mappers/FamicomBox.h"
#include "Mappers/FaridSlrom.h"
#include "Mappers/FaridUnrom.h"
#include "Mappers/FDS.h"
#include "Mappers/Fk23C.h"
#include "Mappers/FrontFareast.h"
#include "Mappers/Ghostbusters63in1.h"
#include "Mappers/Gkcx1.h"
#include "Mappers/GoldenFive.h"
#include "Mappers/Gs2004.h"
#include "Mappers/Gs2013.h"
#include "Mappers/GxRom.h"
#include "Mappers/Henggedianzi177.h"
#include "Mappers/Henggedianzi179.h"
#include "Mappers/Hp898f.h"
#include "Mappers/HSK007.h"
#include "Mappers/HuangX.h"
#include "Mappers/IremG101.h"
#include "Mappers/IremH3001.h"
#include "Mappers/IremLrog017.h"
#include "Mappers/IremTamS1.h"
#include "Mappers/JalecoJf11_14.h"
#include "Mappers/JalecoJf13.h"
#include "Mappers/JalecoJf16.h"
#include "Mappers/JalecoJf17_19.h"
#include "Mappers/JalecoJfxx.h"
#include "Mappers/JalecoSs88006.h"
#include "Mappers/JyCompany.h"
#include "Mappers/Jy281.h"
#include "Mappers/Jy282.h"
#include "Mappers/Jy295.h"
#include "Mappers/Jy358.h"
#include "Mappers/Jy386.h"
#include "Mappers/Jy387.h"
#include "Mappers/Jy388.h"
#include "Mappers/Jy397.h"
#include "Mappers/Kaiser202.h"
#include "Mappers/Kaiser7012.h"
#include "Mappers/Kaiser7013B.h"
#include "Mappers/Kaiser7016.h"
#include "Mappers/Kaiser7017.h"
#include "Mappers/Kaiser7022.h"
#include "Mappers/Kaiser7030.h"
#include "Mappers/Kaiser7031.h"
#include "Mappers/Kaiser7037.h"
#include "Mappers/Kaiser7057.h"
#include "Mappers/Kaiser7058.h"
#include "Mappers/Lh10.h"
#include "Mappers/Lh32.h"
#include "Mappers/Lh51.h"
#include "Mappers/Malee.h"
#include "Mappers/MagicKidGooGoo.h"
#include "Mappers/Mapper15.h"
#include "Mappers/Mapper39.h"
#include "Mappers/Mapper40.h"
#include "Mappers/Mapper42.h"
#include "Mappers/Mapper43.h"
#include "Mappers/Mapper50.h"
#include "Mappers/Mapper57.h"
#include "Mappers/Mapper58.h"
#include "Mappers/Mapper60.h"
#include "Mappers/Mapper61.h"
#include "Mappers/Mapper62.h"
#include "Mappers/Mapper83.h"
#include "Mappers/Mapper91.h"
#include "Mappers/Mapper103.h"
#include "Mappers/Mapper106.h"
#include "Mappers/Mapper107.h"
#include "Mappers/Mapper112.h"
#include "Mappers/Mapper116.h"
#include "Mappers/Mapper117.h"
#include "Mappers/Mapper120.h"
#include "Mappers/Mapper170.h"
#include "Mappers/Mapper174.h"
#include "Mappers/Mapper183.h"
#include "Mappers/Mapper200.h"
#include "Mappers/Mapper202.h"
#include "Mappers/Mapper203.h"
#include "Mappers/Mapper204.h"
#include "Mappers/Mapper212.h"
#include "Mappers/Mapper214.h"
#include "Mappers/Mapper216.h"
#include "Mappers/Mapper217.h"
#include "Mappers/Mapper218.h"
#include "Mappers/Mapper221.h"
#include "Mappers/Mapper222.h"
#include "Mappers/Mapper225.h"
#include "Mappers/Mapper226.h"
#include "Mappers/Mapper227.h"
#include "Mappers/Mapper229.h"
#include "Mappers/Mapper230.h"
#include "Mappers/Mapper231.h"
#include "Mappers/Mapper233.h"
#include "Mappers/Mapper234.h"
#include "Mappers/Mapper240.h"
#include "Mappers/Mapper241.h"
#include "Mappers/Mapper242.h"
#include "Mappers/Mapper244.h"
#include "Mappers/Mapper246.h"
#include "Mappers/Mapper253.h"
#include "Mappers/Mapper368.h"
#include "Mappers/Mapper538.h"
#include "Mappers/Mapper539.h"
#include "Mappers/McAcc.h"
#include "Mappers/MMC1.h"
#include "Mappers/MMC1_105.h"
#include "Mappers/MMC1_155.h"
#include "Mappers/MMC1_374.h"
#include "Mappers/MMC2.h"
#include "Mappers/MMC3.h"
#include "Mappers/MMC3_12.h"
#include "Mappers/MMC3_14.h"
#include "Mappers/MMC3_37.h"
#include "Mappers/MMC3_44.h"
#include "Mappers/MMC3_45.h"
#include "Mappers/MMC3_47.h"
#include "Mappers/MMC3_49.h"
#include "Mappers/MMC3_52.h"
#include "Mappers/MMC3_114.h"
#include "Mappers/MMC3_115.h"
#include "Mappers/MMC3_121.h"
#include "Mappers/MMC3_123.h"
#include "Mappers/MMC3_126.h"
#include "Mappers/MMC3_134.h"
#include "Mappers/MMC3_165.h"
#include "Mappers/MMC3_182.h"
#include "Mappers/MMC3_187.h"
#include "Mappers/MMC3_189.h"
#include "Mappers/MMC3_196.h"
#include "Mappers/MMC3_197.h"
#include "Mappers/MMC3_198.h"
#include "Mappers/MMC3_199.h"
#include "Mappers/MMC3_205.h"
#include "Mappers/MMC3_208.h"
#include "Mappers/MMC3_215.h"
//#include "Mappers/MMC3_217.h"
#include "Mappers/MMC3_219.h"
#include "Mappers/MMC3_224.h"
#include "Mappers/MMC3_238.h"
#include "Mappers/MMC3_245.h"
#include "Mappers/MMC3_249.h"
#include "Mappers/MMC3_250.h"
#include "Mappers/MMC3_254.h"
#include "Mappers/MMC3_267_377.h"
#include "Mappers/MMC3_534.h"
#include "Mappers/MMC3_Bmc411120C.h"
#include "Mappers/MMC3_BmcF15.h"
#include "Mappers/MMC3_ChrRam.h"
#include "Mappers/MMC3_Coolboy.h"
#include "Mappers/MMC3_Kof97.h"
#include "Mappers/MMC3_MaliSB.h"
#include "Mappers/MMC3_StreetHeroes.h"
#include "Mappers/MMC4.h"
#include "Mappers/MMC5.h"
#include "Mappers/N715021.h"
#include "Mappers/Namco108.h"
#include "Mappers/Namco108_76.h"
#include "Mappers/Namco108_88.h"
#include "Mappers/Namco108_95.h"
#include "Mappers/Namco108_154.h"
#include "Mappers/Namco163.h"
#include "Mappers/Nanjing.h"
#include "Mappers/Nanjing163.h"
#include "Mappers/Nina01.h"
#include "Mappers/Nina03_06.h"
#include "Mappers/NovelDiamond.h"
#include "Mappers/NROM.h"
#include "Mappers/NsfCart31.h"
#include "Mappers/NsfMapper.h"
#include "Mappers/NtdecTc112.h"
#include "Mappers/OekaKids.h"
#include "Mappers/Racermate.h"
#include "Mappers/Rambo1.h"
#include "Mappers/Rambo1_158.h"
#include "Mappers/Realtec8031.h"
#include "Mappers/Realtec8210.h"
#include "Mappers/ResetTxrom.h"
#include "Mappers/Rt01.h"
#include "Mappers/Sachen_133.h"
#include "Mappers/Sachen_136.h"
#include "Mappers/Sachen_143.h"
#include "Mappers/Sachen_145.h"
#include "Mappers/Sachen_147.h"
#include "Mappers/Sachen_148.h"
#include "Mappers/Sachen_149.h"
#include "Mappers/Sachen74LS374N.h"
#include "Mappers/Sachen8259.h"
#include "Mappers/Sachen9602.h"
#include "Mappers/SealieComputing.h"
#include "Mappers/Smb2j.h"
#include "Mappers/StudyBox.h"
#include "Mappers/Subor166.h"
#include "Mappers/Sunsoft3.h"
#include "Mappers/Sunsoft4.h"
#include "Mappers/Sunsoft89.h"
#include "Mappers/Sunsoft93.h"
#include "Mappers/Sunsoft184.h"
#include "Mappers/SunsoftFme7.h"
#include "Mappers/Supervision.h"
#include "Mappers/Super40in1Ws.h"
#include "Mappers/T230.h"
#include "Mappers/T262.h"
#include "Mappers/TaitoTc0190.h"
#include "Mappers/TaitoTc0690.h"
#include "Mappers/TaitoX1005.h"
#include "Mappers/TaitoX1017.h"
#include "Mappers/Tf1201.h"
#include "Mappers/Txc22000.h"
#include "Mappers/Txc22211A.h"
#include "Mappers/Txc22211B.h"
#include "Mappers/Txc22211C.h"
#include "Mappers/TxSRom.h"
#include "Mappers/Unl158B.h"
#include "Mappers/Unl255in1.h"
#include "Mappers/Unl8237A.h"
#include "Mappers/UnlD1038.h"
#include "Mappers/UnlDripGame.h"
#include "Mappers/UnlPci556.h"
#include "Mappers/UnlPuzzle.h"
#include "Mappers/UNROM.h"
#include "Mappers/UnRom_94.h"
#include "Mappers/UnRom_180.h"
#include "Mappers/UnRom512.h"
#include "Mappers/VRC1.h"
#include "Mappers/VRC2_4.h"
#include "Mappers/VRC3.h"
#include "Mappers/VRC6.h"
#include "Mappers/VRC7.h"
#include "Mappers/VsSystem.h"
#include "Mappers/Waixing162.h"
#include "Mappers/Waixing164.h"
#include "Mappers/Waixing178.h"
#include "Mappers/Waixing252.h"
#include "Mappers/Yoko.h"
#include "Mappers/YY850437C.h"

/*
Supported mappers:  
... : bad mappers
--- : potentially bad mappers
=== : not supported by both Nestopia & FCEUX
??? : No known roms
-----------------------------------------------------------------
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10| 11| 12| 13| 14| 15|
| 16| 17| 18| 19|...| 21| 22| 23| 24| 25| 26| 27| 28| 29| 30| 31|
| 32| 33| 34| 35| 36| 37| 38| 39| 40| 41| 42| 43| 44| 45| 46| 47|
| 48| 49| 50| 51| 52| 53| 54|???| 56| 57| 58| 59| 60| 61| 62| 63|
| 64| 65| 66| 67| 68| 69| 70| 71| 72| 73| 74| 75| 76| 77| 78| 79|
| 80|===| 82| 83|===| 85| 86| 87| 88| 89| 90| 91| 92| 93| 94| 95|
| 96| 97|===| 99|...|101|===|103|104|105|106|107|108|===|===|111|
|112|113|114|115|116|117|118|119|120|121|===|123|===|125|126|===|
|===|===|===|===|132|133|134|===|136|137|138|139|140|141|142|143|
|144|145|146|147|148|149|150|151|152|153|154|155|156|157|158|159|
|---|===|162|163|164|165|166|167|168|===|170|171|172|173|174|175|
|176|177|178|179|180|---|182|183|184|185|186|187|188|189|190|191|
|192|193|194|195|196|197|198|199|200|201|202|203|204|205|206|207|
|208|209|210|211|212|213|214|215|216|217|218|219|220|221|222|???|
|224|225|226|227|228|229|230|231|232|233|234|235|236|===|238|===|
|240|241|242|243|244|245|246|===|===|249|250|===|252|253|254|255|
-----------------------------------------------------------------
*/

BaseMapper* MapperFactory::GetMapperFromID(RomData &romData)
{
#ifdef _DEBUG
	MessageManager::DisplayMessage("GameInfo", "Mapper", std::to_string(romData.Info.MapperID), std::to_string(romData.Info.SubMapperID));
#endif

	switch(romData.Info.MapperID) {
		case 0: return new NROM();
		case 1: return new MMC1();
		case 2: return new UNROM();
		case 3: return new CNROM(false);
		case 4: 
			if(romData.Info.SubMapperID == 3) {
				return new McAcc();
			} else {
				return new MMC3();
			}

		case 5: return new MMC5();
		case 6: return new FrontFareast();
		case 7: return new AXROM();
		case 8: return new FrontFareast();
		case 9: return new MMC2();
		case 10: return new MMC4();
		case 11: return new ColorDreams();
		case 12: return new MMC3_12();
		case 13: return new CpRom();
		case 14: return new MMC3_14();
		case 15: return new Mapper15();
		case 16: return new BandaiFcg();
		case 17: return new FrontFareast();
		case 18: return new JalecoSs88006();
		case 19: return new Namco163();
		case 21: return new VRC2_4();
		case 22: return new VRC2_4();
		case 23: return new VRC2_4();
		case 24: return new VRC6(VRCVariant::VRC6a);
		case 25: return new VRC2_4();
		case 26: return new VRC6(VRCVariant::VRC6b);
		case 27: return new VRC2_4();
		case 28: return new Action53();
		case 29: return new SealieComputing();
		case 30: return new UnRom512();
		case 31: return new NsfCart31();
		case 32: return new IremG101();
		case 33: return new TaitoTc0190();
		case 34: 
			switch(romData.Info.SubMapperID) {
				case 0: return (romData.ChrRom.size() > 0) ? (BaseMapper*)new Nina01() : (BaseMapper*)new BnRom(); //BnROM uses CHR RAM (so no CHR rom in the .NES file)
				case 1: return new Nina01();
				case 2: return new BnRom();
			}
		case 35: return new JyCompany();
		case 36: return new Txc22000();
		case 37: return new MMC3_37();
		case 38: return new UnlPci556();
		case 39: return new Mapper39();
		case 40: return new Mapper40();
		case 41: return new Caltron41();
		case 42: return new Mapper42();
		case 43: return new Mapper43();
		case 44: return new MMC3_44();
		case 45: return new MMC3_45();
		case 46: return new ColorDreams46();
		case 47: return new MMC3_47();
		case 48: return new TaitoTc0690();
		case 49: return new MMC3_49();
		case 50: return new Mapper50();
		case 51: return new Bmc51();
		case 52: return new MMC3_52();
		case 53: return new Supervision();
		case 54: return new NovelDiamond();
		case 56: return new Kaiser202();
		case 57: return new Mapper57();
		case 58: return new Mapper58();
		case 59: return new UnlD1038();
		case 60: return new Mapper60();
		case 61: return new Mapper61();
		case 62: return new Mapper62();
		case 63: return new Bmc63();
		case 64: return new Rambo1();
		case 65: return new IremH3001();
		case 66: return new GxRom();
		case 67: return new Sunsoft3();
		case 68: return new Sunsoft4();
		case 69: return new SunsoftFme7();
		case 70: return new Bandai74161_7432(false);
		case 71: return new BF909x();
		case 72: return new JalecoJf17_19(false);
		case 73: return new VRC3();
		case 74: return new MMC3_ChrRam(0x08, 0x09, 2);
		case 75: return new VRC1();
		case 76: return new Namco108_76();
		case 77: return new IremLrog017();
		case 78: return new JalecoJf16();
		case 79: return new Nina03_06(false);
		case 80: return new TaitoX1005(false);
		case 81: return new N715021();
		case 82: return new TaitoX1017();
		case 83: return new Mapper83();
		case 85: return new VRC7();
		case 86: return new JalecoJf13();
		case 87: return new JalecoJfxx(false);
		case 88: return new Namco108_88();
		case 89: return new Sunsoft89();
		case 90: return new JyCompany();
		case 91: return new Mapper91();
		case 92: return new JalecoJf17_19(true);
		case 93: return new Sunsoft93();
		case 94: return new UnRom_94();
		case 95: return new Namco108_95();
		case 96: return new OekaKids();
		case 97: return new IremTamS1();
		case 99: return new VsSystem();
		case 101: return new JalecoJfxx(true);
		case 103: return new Mapper103();
		case 104: return new GoldenFive();
		case 105: return new MMC1_105();
		case 106: return new Mapper106();
		case 107: return new Mapper107();
		case 108: return new Bb();
		case 111: return new Cheapocabra();
		case 112: return new Mapper112();
		case 113: return new Nina03_06(true);
		case 114: return new MMC3_114();
		case 115: return new MMC3_115();
		case 116: return new HuangX();
		case 117: return new Mapper117();
		case 118: return new TxSRom();
		case 119: return new MMC3_ChrRam(0x40, 0x7F, 8);
		case 120: return new Mapper120();
		case 121: return new MMC3_121();
		case 123: return new MMC3_123();
		case 125: return new Lh32();
		case 126: return new MMC3_126();
		case 132: return new Txc22211A();
		case 133: return new Sachen_133();
		case 134: return new MMC3_134();
		case 136: return new Sachen_136();
		case 137: return new Sachen8259(Sachen8259Variant::Sachen8259D);
		case 138: return new Sachen8259(Sachen8259Variant::Sachen8259B);
		case 139: return new Sachen8259(Sachen8259Variant::Sachen8259C);
		case 140: return new JalecoJf11_14();
		case 141: return new Sachen8259(Sachen8259Variant::Sachen8259A);
		case 142: return new Kaiser202();
		case 143: return new Sachen_143();
		case 144: return new ColorDreams();
		case 145: return new Sachen_145();
		case 146: return new Nina03_06(false);
		case 147: return new Sachen_147();
		case 148: return new Sachen_148();
		case 149: return new Sachen_149();
		case 150: return new Sachen74LS374N();
		case 151: return new VRC1();
		case 152: return new Bandai74161_7432(true);
		case 153: return new BandaiFcg();
		case 154: return new Namco108_154();
		case 155: return new MMC1_155();
		case 156: return new DaouInfosys();
		case 157: return new BandaiFcg();
		case 158: return new Rambo1_158();
		case 159: return new BandaiFcg();
		//case 162: return new Waixing162();
		//case 163: return new Nanjing();
		case 162: return new Waixing162();
		case 163: return new Nanjing163();
		case 164: return new Waixing164();
		case 165: return new MMC3_165();
		case 166: return new Subor166();
		case 167: return new Subor166();
		case 168: return new Racermate();
		//   169 - Yuxing, undocumented mapper
		case 170: return new Mapper170();
		case 171: return new Kaiser7058();
		case 172: return new Txc22211B();
		case 173: return new Txc22211C();
		case 174: return new Mapper174();
		case 175: return new Kaiser7022();
		case 176: return new Fk23C();
		case 177: return new Henggedianzi177();
		case 178: return new Waixing178();
		case 179: return new Henggedianzi179();
		case 180: return new UnRom_180();
		case 182: return new MMC3_182();
		case 183: return new Mapper183();
		case 184: return new Sunsoft184();
		case 185: return new CNROM(true);
		case 186: break; //The study box is handled as a bios file, not a iNES rom
		case 187: return new MMC3_187();
		case 188: return new BandaiKaraoke();
		case 189: return new MMC3_189();
		case 190: return new MagicKidGooGoo();
		case 191: return new MMC3_ChrRam(0x80, 0xFF, 2);
		case 192: return new MMC3_ChrRam(0x08, 0x0B, 4);
		case 193: return new NtdecTc112();
		case 194: return new MMC3_ChrRam(0x00, 0x01, 2);
		case 195: return new MMC3_ChrRam(0x00, 0x03, 4); //FS303(); unsure about the new implementation
		case 196: return new MMC3_196();
		case 197: return new MMC3_197();
		case 198: return new MMC3_198();
		case 199: return new MMC3_199();
		case 200: return new Mapper200();
		case 201: return new NovelDiamond();
		case 202: return new Mapper202();
		case 203: return new Mapper203();
		case 204: return new Mapper204();
		case 205: return new MMC3_205();
		case 206: return new Namco108();
		case 207: return new TaitoX1005(true);
		case 208: return new MMC3_208();
		case 209: return new JyCompany();
		case 210: return new Namco163();
		case 211: return new JyCompany();
		case 212: return new Mapper212();
		case 213: return new Mapper58(); // Mapper 213 is a duplicate of Mapper 58
		case 214: return new Mapper214();
		case 215: return new MMC3_215();
		case 216: return new Mapper216();
		//case 217: return new MMC3_217();
		case 217: return new Mapper217(); // Mapper 217 was redefined
		case 218: return new Mapper218();
		case 219: return new MMC3_219();
		case 220: return new Kaiser7057();
		case 221: return new Mapper221();
		case 222: return new Mapper222();
		case 224: return new MMC3_224();
		case 225: return new Mapper225();
		case 226: return new Mapper226();
		case 227: return new Mapper227();
		case 228: return new ActionEnterprises();
		case 229: return new Mapper229();
		case 230: return new Mapper230();
		case 231: return new Mapper231();
		case 232: return new BF9096();
		case 233: return new Mapper233();
		case 234: return new Mapper234();
		case 235: return new Bmc235();
		case 236: return new Bmc70in1();
		case 238: return new MMC3_238();
		case 240: return new Mapper240();
		case 241: return new Mapper241();
		case 242: return new Mapper242();
		case 243: return new Sachen74LS374N();
		case 244: return new Mapper244();
		case 245: return new MMC3_245();
		case 246: return new Mapper246();
		case 249: return new MMC3_249();
		case 250: return new MMC3_250();
		case 252: return new Waixing252();
		case 253: return new Mapper253();
		case 254: return new MMC3_254();
		case 255: return new Bmc255();

		case 256: break; //ONEBUS
		case 257: break; //PEC-586
		case 258: return new Unl158B();
		case 259: return new MMC3_BmcF15();
		case 260: return new BmcHpxx();
		case 261: return new Bmc810544CA1();
		case 262: return new MMC3_StreetHeroes();
		case 263: return new MMC3_Kof97();
		case 264: return new Yoko();
		case 265: return new T262();
		case 266: return new CityFighter();
		case 267: return new MMC3_267_377();
		case 268: return new MMC3_Coolboy();
		case 269: break; // MMC3 with outer bank
		case 270: break; // OneBus multicart
		case 271: break; // 22026
		case 272: break; // bootleg of Akumajō Special: Boku Dracula-kun
		case 273: break; // bootleg of Gremlins 2
		case 274: return new Bmc80013B();
		//   275-280 - Reserved mappers
		case 281: return new Jy281();
		case 282: return new Jy282();
		case 283: return new Gs2004();
		case 284: return new UnlDripGame();
		case 285: return new A65AS();
		case 286: return new Bs5();
		case 287: return new MMC3_Bmc411120C(); //+ K-3088
		case 288: return new Gkcx1();
		case 289: return new Bmc60311C();
		case 290: return new BmcNtd03();
		case 291: break; // Kasheng 2in1
		case 292: return new DragonFighter();
		case 293: break; // Newstar12in1, 76in1
		case 294: break; // T4A54A / WX-KB4K / BS-5652
		case 295: return new Jy295(); //13IN1JY110
		case 296: break; // VT3x hardware
		case 297: break; // TXC 01-22110-000
		case 298: return new Tf1201();
		case 299: return new Bmc11160();
		case 300: return new Bmc190in1();
		case 301: return new Bmc8157();
		case 302: return new Kaiser7057();
		case 303: return new Kaiser7017();
		case 304: return new Smb2j();
		case 305: return new Kaiser7031();
		case 306: return new Kaiser7016();
		case 307: return new Kaiser7037();
		case 308: break; //TH2131-1
		case 309: return new Lh51();
		case 310: return new Lh32(); // duplicate of 125
		//   311 - Bad mapper
		case 312: return new Kaiser7013B();
		case 313: return new ResetTxrom();
		case 314: return new Bmc64in1NoRepeat();
		case 315: break; //830134C
		//   316-318 - Reserved mappers
		case 319: return new Hp898f();
		case 320: return new Bmc830425C4391T();
		case 321: return new MMC3_Bmc411120C(); // duplicate of 287
		case 322: break; //K-3033
		case 323: return new FaridSlrom();
		case 324: return new FaridUnrom();
		case 325: return new MMC3_MaliSB();
		case 327: break; //10-24-C-A1
		case 328: return new Rt01();
		case 329: return new Edu2000();
		case 330: break; // bootleg Sangokushi II: Hao no Tairiku
		case 331: return new Bmc12in1();
		case 332: return new Super40in1Ws();
		case 333: return new Bmc8in1(); // + NEWSTAR-GRM070-8IN1
		case 334: break; // "5/20in1 1993 Copyright" cart
		case 335: break; //CTC-09
		case 336: return new BmcK3046();
		case 337: break; //CTC-12IN1
		case 338: break; //SA005-A
		case 339: break; //K-3006
		case 340: break; //K-3036
		case 341: break; //TJ-03
		case 342: break; //COOLGIRL
		//   343 - "Certain" reset base 4in1
		case 344: break; //GN26
		case 345: break; //L6IN1
		case 346: return new Kaiser7012();
		case 347: return new Kaiser7030();
		case 348: return new Bmc830118C();
		case 349: return new BmcG146();
		case 350: break; //891227
		case 351: break; //
		case 352: break; //
		case 353: break; //
		case 354: break; //
		case 355: break; //
		case 356: break; //
		case 357: break; //
		case 358: return new Jy358();
		case 359: break; //
		case 361: break; //
		case 362: break; //
		case 363: break; //
		case 364: break; //
		case 365: return new AsderPC95(); // Asder PC-95 (Polish, Arabic)
		case 366: return new BmcGn45();
		case 367: break; //
		case 368: return new Mapper368();
		case 369: break; //
		case 370: break; //
		case 371: break; //
		case 372: break; //
		case 373: break; //
		case 374: return new MMC1_374(); // MMC1 reset-based multicart
		case 375: break; //
		case 376: break; //
		case 377: return new MMC3_267_377();
		case 378: break; //
		case 379: break; //
		case 380: return new _970630C();
		case 381: break; // 
		case 382: break; // 
		case 383: break; // 
		case 384: break; // 
		case 385: break; //
		case 386: return new Jy386();
		case 387: return new Jy387();
		case 388: return new Jy388();
		case 389: break; // CALTRON 9
		case 390: return new Realtec8031();
		case 391: break; // NC7000M
		case 392: break; // 00202650
		case 393: return new _820720C();
		case 394: return new HSK007();
		case 395: return new Realtec8210();
		case 396: return new YY850437C();
		case 397: return new Jy397();
		case 398: break; //
		case 399: break; //
		case 400: break; //
		case 401: break; //
		case 402: break; //
		case 403: break; //
		case 404: break; //
		case 405: break; //
		case 406: break; //
		case 407: break; //
		case 408: break; //
		case 409: break; //
		case 410: break; //
		case 411: break; //
		case 412: break; //
		case 413: break; //
		case 414: break; //
		case 415: break; //
		case 416: break; //
		case 417: break; //
		case 418: break; //
		case 419: break; //
		case 420: break; //
		case 421: break; //
		case 422: break; //
		case 423: break; //
		case 424: break; //
		case 425: break; //
		case 426: break; //
		case 427: break; //
		case 428: break; //
		// 429-430 aren't assigned
		case 431: break; //
		case 432: break; //
		case 433: break; //
		case 434: break; //
		// 434-511 aren't assigned

		case 512: break; // Zhongguo Daheng
		case 513: return new Sachen9602();
		case 514: break; //
		case 515: break; //
		case 516: break; //
		case 517: break; //
		case 518: return new Dance2000();
		case 519: return new Eh8813A();
		case 520: break; //
		case 521: return new DreamTech01();
		case 522: return new Lh10();
		case 523: break; //
		case 524: break; //900218
		case 525: break; //KS7021A
		case 526: break; //BJ56
		case 527: break; //AX40G
		case 528: break; //831128C
		case 529: return new T230();
		case 530: return new Ax5705();
		case 531: break; // LittleCom PC-95 (not enough info)
		case 532: break; // VirtuaNES Ex overlay??? (not sure if this should be implemented)
		case 533: break; // Sachen 3014
		case 534: return new MMC3_534();
		case 535: break; //
		case 536: break; //
		case 537: break; //
		case 538: return new Mapper538(); // bootleg cart conversion of Super Soccer Champion
		case 539: return new Mapper539(); // bootleg cart conversion of Palutena no Kagami
		case 540: break; //
		case 541: break; //
		case 542: break; //
		case 543: break; //
		case 544: break; //
		case 545: break; //
		case 546: break; //
		case 547: break; // Konami QTa
		case 548: break; //
		case 549: break; //
		case 550: break; //
		case 551: break; //
		case 552: break; //
		case 553: break; //
		case 554: break; //
		case 555: break; //
		case 556: break; //
		case 557: break; //
		case 558: break; //

		case UnifBoards::Ac08: return new Ac08(); //mapper 42?
		case UnifBoards::Cc21: return new Cc21();
		case UnifBoards::Ghostbusters63in1: return new Ghostbusters63in1(); //mapper 226?
		case UnifBoards::Gs2013: return new Gs2013();
		case UnifBoards::Malee: return new Malee(); //mapper 42?
		case UnifBoards::SssNrom256: return new FamicomBox();
		case UnifBoards::Unl255in1: return new Unl255in1();
		case UnifBoards::Unl8237A: return new Unl8237A(); //mapper 215.1
		case UnifBoards::UnlPuzzle: return new UnlPuzzle();

		case MapperFactory::StudyBoxMapperID: return new StudyBox();
		case MapperFactory::NsfMapperID: return new NsfMapper();
		case MapperFactory::FdsMapperID: return new FDS();
	}

	if(romData.Info.MapperID != UnifBoards::UnknownBoard) {
		MessageManager::DisplayMessage("Error", "UnsupportedMapper", "iNES #" + std::to_string(romData.Info.MapperID));
	} else {
		MessageManager::DisplayMessage("Error", "UnsupportedMapper", "Unknown UNIF board");
	}
	return nullptr;
}

shared_ptr<BaseMapper> MapperFactory::InitializeFromFile(shared_ptr<Console> console, VirtualFile &romFile, RomData &romData)
{
	RomLoader loader;

	if(loader.LoadFile(romFile)) {
		romData = loader.GetRomData();

		if((romData.Info.IsInDatabase || romData.Info.IsNes20Header) && romData.Info.InputType != GameInputType::Unspecified) {
			//If in DB or a NES 2.0 file, auto-configure the inputs
			if(console->GetSettings()->CheckFlag(EmulationFlags::AutoConfigureInput)) {
				console->GetSettings()->InitializeInputDevices(romData.Info.InputType, romData.Info.System, false);
			}
		}

		shared_ptr<BaseMapper> mapper(GetMapperFromID(romData));
		if(mapper) {
			return mapper;
		}
	} else if(loader.GetRomData().BiosMissing) {
		console->GetNotificationManager()->SendNotification(ConsoleNotificationType::BiosNotFound, (void*)loader.GetRomData().Info.Format);
	}
	return nullptr;
}

