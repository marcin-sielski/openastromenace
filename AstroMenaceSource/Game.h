/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Game.h

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.2

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


#ifndef GAME_H
#define GAME_H


// ядро
#include "Core/Core.h"
// определения
#include "Defines.h"
// структуры
#include "Struct.h"



// скриптовый движек
#include "ScriptEngine/Script.h"
// эффекты
#include "GraphicFX/SpaceStars/SpaceStars.h"
#include "GraphicFX/Text/Text.h"
// модели и все обьъекты
#include "Object3D/Object3D.h"
#include "Object3D/SpaceShip/EarthSpaceFighter/EarthSpaceFighter.h"
#include "Object3D/SpaceShip/AlienSpaceFighter/AlienSpaceFighter.h"
#include "Object3D/SpaceShip/AlienSpaceMotherShip/AlienSpaceMotherShip.h"
#include "Object3D/SpaceShip/PirateShip/PirateShip.h"
#include "Object3D/GroundObject/Building/Building.h"
#include "Object3D/GroundObject/MilitaryBuilding/MilitaryBuilding.h"
#include "Object3D/GroundObject/Wheeled/Wheeled.h"
#include "Object3D/GroundObject/Tracked/Tracked.h"
#include "Object3D/SpaceObject/Asteroid/Asteroid.h"
#include "Object3D/SpaceObject/BigAsteroid/BigAsteroid.h"
#include "Object3D/SpaceObject/BasePart/BasePart.h"
#include "Object3D/SpaceObject/ShipPart/ShipPart.h"
#include "Object3D/SpaceObject/Planet/Planet.h"








//------------------------------------------------------------------------------------
// Main.cpp
//------------------------------------------------------------------------------------
extern GameSetup Setup;
extern eDevCaps *CAPS;
extern eGameStatus GameStatus;
extern SDL_Joystick *Joystick;
extern eParticleSystem2D *CursorParticleSystem2D;
extern CSpaceStars *psSpaceStatic;
extern VECTOR3D GamePoint;
extern VECTOR3D GameCameraMovement;

extern  sVideoModes *VideoModes;
extern  int VideoModesNum;
extern  sVideoModes CurrentVideoMode;

extern bool LoadedTypes[1000];
extern char DatFileName[MAX_PATH];
extern char ProgrammDir[MAX_PATH];
extern char ScreenshotDir[MAX_PATH];
extern char VFSFileNamePath[MAX_PATH];
extern char VFSLangFileNamePath[MAX_PATH];
extern char UserPath[MAX_PATH];
extern bool Quit;
extern bool CanQuit;
extern bool NeedReCreate;
extern bool SDL_MouseCurrentStatus[8];
extern int JoystickAxisX;
extern int JoystickAxisY;
extern bool JoysticButtons[100];

void SaveGameData();
void CodeXOR(char *Text, char *Key, int Count);





//------------------------------------------------------------------------------------
// Setup.cpp
//------------------------------------------------------------------------------------
bool LoadXMLSetupFile(bool NeedSafeMode);
void SaveXMLSetupFile();






//------------------------------------------------------------------------------------
// loop_proc.cpp
//------------------------------------------------------------------------------------
extern ScriptEngine *Script;

extern int ComBuffer;
extern int CurrentCursorStatus;
extern bool DrawGameCursor;

void Loop_Proc();
void CreateCursor();








//------------------------------------------------------------------------------------
// loop_audio.cpp
//------------------------------------------------------------------------------------
extern bool GameMainMusicSet;
extern char GameMainMusic[MAX_PATH];
extern bool GameBossMusicSet;
extern char GameBossMusic[MAX_PATH];
extern bool GameDeathMusicSet;
extern char GameDeathMusic[MAX_PATH];

int Audio_PlayMenuSound(int SoundID, float fVol, bool Loop = false);
int Audio_PlaySound(int SoundID, float fVol, float Pitch, VECTOR3D Location, bool Loop, int AtType);
void Audio_LoopProc();
bool InitAudio();
void StartMusicWithFade(int StartMusic, float FadeInTime, float FadeOutTime);






//------------------------------------------------------------------------------------
// loading.cpp
//------------------------------------------------------------------------------------
void LoadGameData(int LoadType);






//------------------------------------------------------------------------------------
// Camera.cpp, GameCamera.cpp & CameraMath.cpp
//------------------------------------------------------------------------------------
void GameCameraUpdate(float Time);
void InitGameCamera();
float GameCameraGetDeviation();
float GameCameraGetSpeed();
void GameCameraSetExplosion(VECTOR3D Location, float Power);






//------------------------------------------------------------------------------------
// SkyBox.cpp
//------------------------------------------------------------------------------------
void SkyBoxSetTexture(eTexture *nTexture, int Side);
void SkyBoxCreate(float nx, float ny, float nz, float nwidth, float nheight, float nlength);
void SkyBoxDraw(void);






//------------------------------------------------------------------------------------
// StarSystem.cpp
//------------------------------------------------------------------------------------
void StarSystemInit(int Num, VECTOR3D SetBaseRotation);
void StarSystemDraw();
void StarSystemUpdate();
void StarSystemRelease();







//------------------------------------------------------------------------------------
// Font.cpp
//------------------------------------------------------------------------------------
void DrawFont(int X, int Y, float FlattenWidth, float MaxWidth, int CharSet, float Transp, const char *Text, ...);
int FontSize(const char *Text, ...);
void DrawFont2(int X, int Y, float FlattenWidth, float MaxWidth, int CharSet, float Transp, const char *Text);
int FontSize2(const char *Text);
void DrawFontX2(int X, int Y, float FlattenWidth, float MaxWidth, int CharSet, float Transp, const char *Text, ...);
int FontSizeX2(const char *Text, ...);
void DrawFontX15(int X, int Y, float FlattenWidth, float MaxWidth, int CharSet, float Transp, const char *Text, ...);
int FontSizeX15(const char *Text, ...);
void GetFontData(int Char, RECT *SrcRest, int *X, int *Y);






//------------------------------------------------------------------------------------
// Button.cpp
//------------------------------------------------------------------------------------
bool DrawButton384(int X, int Y, const char *Text, float Transp, float *ButTransp, float *Update);
bool DrawButton256(int X, int Y, const char *Text, float Transp, float *ButTransp, float *Update, bool Off=false);
bool DrawButton128_2(int X, int Y, const char *Text, float Transp, bool Off);
bool DrawButton200_2(int X, int Y, const char *Text, float Transp, bool Off);
void DrawCheckBox(int X, int Y, bool *CheckBoxStatus, const char *Text, float Transp);








//------------------------------------------------------------------------------------
// Menu.cpp
//------------------------------------------------------------------------------------
extern float Button1Transp;
extern float LastButton1UpdateTime;
extern float Button2Transp;
extern float LastButton2UpdateTime;
extern float Button3Transp;
extern float LastButton3UpdateTime;
extern float Button4Transp;
extern float LastButton4UpdateTime;
extern float Button5Transp;
extern float LastButton5UpdateTime;
extern float Button6Transp;
extern float LastButton6UpdateTime;
extern float Button7Transp;
extern float LastButton7UpdateTime;
extern float Button8Transp;
extern float LastButton8UpdateTime;
extern float Button9Transp;
extern float LastButton9UpdateTime;
extern float Button10Transp;
extern float LastButton10UpdateTime;
extern float Button11Transp;
extern float LastButton11UpdateTime;
extern float Button12Transp;
extern float LastButton12UpdateTime;
extern float Button13Transp;
extern float LastButton13UpdateTime;
extern float Button14Transp;
extern float LastButton14UpdateTime;
extern float MenuContentTransp;
extern float LastMenuOnOffUpdateTime;

void InitMenu();
void SetMenu(eGameStatus Menu);
void DrawMenu();
void DrawTransparent(RECT *DstRest, RECT *SrcRest, eTexture *Tex, eTexture *Tex2, bool Alpha, float Transp, float RotateAngle, int DrawCorner, float R, float G, float B);
void MainMenu();










//------------------------------------------------------------------------------------
// Menu_Credits.cpp
//------------------------------------------------------------------------------------
extern float CreditsCurrentPos;
extern float LastCreditsCurrentPosUpdateTime;

void CreditsMenu();









//------------------------------------------------------------------------------------
// Menu_Options.cpp
//------------------------------------------------------------------------------------
extern int Options_Width;
extern int Options_Height;
extern int Options_BPP;
extern int Options_Frequency;
extern int Options_TexturesQuality;
extern int Options_iAspectRatioWidth;

void OptionsMenu();









//------------------------------------------------------------------------------------
// Menu_ConfControl.cpp
//------------------------------------------------------------------------------------
extern int NeedCheck;
extern int ButQuant;
extern float But[10];

void ConfControlMenu();
char * MouseCodeName(char Num);
char * JoystickCodeName(char Num);
void CheckMouseKeybJState();







//------------------------------------------------------------------------------------
// Game_Options.cpp
//------------------------------------------------------------------------------------
void GameOptions();








//------------------------------------------------------------------------------------
// OptionsAdvMenu.cpp
//------------------------------------------------------------------------------------
extern int Options_TexturesCompression;
extern int Options_UseGLSL;
extern int Options_MultiSampleType;

void OptionsAdvMenu();








//------------------------------------------------------------------------------------
// Menu_Profile.cpp
//------------------------------------------------------------------------------------
extern int CurrentProfile;

void ProfileMenu();
void DeleteRecord();








//------------------------------------------------------------------------------------
// Menu_Information.cpp
//------------------------------------------------------------------------------------
extern int CreateNum;

void InformationMenu();
void InformationDrawObject();
void CreateInfoObject();
void DestroyInfoObject();









//------------------------------------------------------------------------------------
// Menu_TopScores.cpp
//------------------------------------------------------------------------------------
extern char GameName[10][128];
extern int GameScore[10];

void TopScoresMenu();
void AddTopScores(int Score, char Name[128], bool Type);








//------------------------------------------------------------------------------------
// Menu_Difficulty.cpp
//------------------------------------------------------------------------------------
void DifficultyMenu();







//------------------------------------------------------------------------------------
// Menu_Demo.cpp
//------------------------------------------------------------------------------------
void DemoMenu();








//------------------------------------------------------------------------------------
// Menu_Mission.cpp
//------------------------------------------------------------------------------------
extern int CurrentMission;
extern int MissionLimitation;
extern int AllMission;
extern int StartMission;
extern int EndMission;

void MissionMenu();
char *GetMissionFileName();
void MissionsListRelease();
void MissionsListInit();








//------------------------------------------------------------------------------------
// Menu_Workshop.cpp
//------------------------------------------------------------------------------------
extern int CurrentWorkshop;
extern int	NewWeaponControlType;
extern int	NewWeaponControlTypeData;
extern int VoiceNeedMoreEnergy;
extern int VoiceAmmoOut;

void WorkshopMenu();
void WorkshopCreate();
void WorkshopDestroyData();
char *GetWeaponIconName(int Num);










//------------------------------------------------------------------------------------
// game.cpp
//------------------------------------------------------------------------------------
extern CEarthSpaceFighter *PlayerFighter;

extern int GameNPCWeaponPenalty;
extern int GameNPCArmorPenalty;
extern int GameNPCTargetingSpeedPenalty;
extern int GameLimitedAmmo;
extern int GameDestroyableWeapon;
extern int GameWeaponTargetingMode;
extern int GameSpaceShipControlMode;
extern int GameEngineSystem;
extern int GameTargetingSystem;
extern int GameAdvancedProtectionSystem;
extern int GamePowerSystem;
extern int GameTargetingMechanicSystem;
extern int GameMenuStatus;
extern float GameContentTransp;
extern float GameButton1Transp;
extern float LastGameButton1UpdateTime;
extern bool NeedShowGameMenu;
extern bool NeedHideGameMenu;
extern bool GameMissionCompleteStatus;
extern float StarsTileStartTransparentLayer1;
extern float StarsTileEndTransparentLayer1;
extern float StarsTileStartTransparentLayer2;
extern float StarsTileEndTransparentLayer2;

void InitGame();
void DrawGame();
void ExitGame();
void RealExitGame();
void ExitGameWithSave();
void SetGameMissionComplete();









//------------------------------------------------------------------------------------
// Dialog.cpp
//------------------------------------------------------------------------------------
void SetCurrentDialogBox(int DialogBox);
bool isDialogBoxDrawing();
void DrawDialogBox();









//------------------------------------------------------------------------------------
// Game_Mission.cpp
//------------------------------------------------------------------------------------
void GameSetMissionTitleData(float ShowTime, int Num);
void GameDrawMissionTitle();
void GameSetMissionFailedData(float ShowTime);
void GameDrawMissionFailed();








//------------------------------------------------------------------------------------
// Game_WeaponSlot.cpp
//------------------------------------------------------------------------------------
void DrawGameWeaponSlots();












#endif // GAME_H