#define _CRT_SECURE_NO_WARNINGS
#include "SDK_Resource.h"
#include "SDK_Language.h"
#include <filesystem>

// Use LoadImageT(), LoadClipT(), and LoadSpriteSheetT() to load image files and sprite sheets in this thread.
DWORD WINAPI ImageResourceLoader(LPVOID lpParam) {
...
...
DWORD WINAPI FileResourceLoader(LPVOID lpParam) {
	std::filesystem::path FolderPath = std::filesystem::path(std::getenv("USERPROFILE")) / "Eld Mart Slam";

	SDK::FILE.HighscoreData.Load(FolderPath.string() + "//HighScore", SDK::FORMAT.HighScore);
	SDK::FILE.UserSettingData.Load(FolderPath.string() + "//UserSettings", SDK::FORMAT.UserSettings);

	SDK::GLOBAL.FullscreenAcvivated = SDK::FILE.UserSettingData.LoadDigitData("Setting", "Fullscreen");
	SDK::GLOBAL.BGMVolume = SDK::FILE.UserSettingData.LoadDigitData("Setting", "BGMVolume");
	SDK::GLOBAL.SFXVolume = SDK::FILE.UserSettingData.LoadDigitData("Setting", "SFXVolume");
	SDK::GLOBAL.Language = (int)SDK::FILE.UserSettingData.LoadDigitData("Setting", "Language");

	SDK::GLOBAL.HighScore = SDK::FILE.HighscoreData.LoadDigitData("HighScore", "Score");
	SDK::GLOBAL.MaxRep = SDK::FILE.HighscoreData.LoadDigitData("HighScore", "Rep");
	SDK::GLOBAL.NeedTutorial = SDK::FILE.HighscoreData.LoadDigitData("TutorialNeed", "Bool");
	
	SDK::LANG.Load(SDK::GLOBAL.Language);

	return 0;
}

DWORD WINAPI FontResourceLoader(LPVOID lpParam) {
	SDK::FontLoader.LoadT("Assets//Font//pixelroborobo.otf");

	return 0;
} 
