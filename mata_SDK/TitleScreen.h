#pragma once
#include <SDK_Scene.h>
#include <SDK_Language.h>

#include "Tutorial.h"

enum MenuPageEnum {
	ExitPage,
	MainPage,
	OptionPage,
	ResetPage
};

class TitleScreen : public SDK::Object {
private:
	// Intro played flag
	bool TitleActivateState{};
	// Intro play state
	bool IntroPlayState{};
	// Game start state
	bool GameStartState{};
	// Intro start state
	bool IntroStartState{};
	// Tutorial rendering state
	bool TutorialState{};
	// Tutorial object added state
	bool TutorialAdded{};
	// Credit mode transition state
	bool CreditStartState{};

	// Title image position
	SDK::Vector2 TitleImagePosition{};
	// Title image size
	float TitleImageSize{};

	// Xion vertical size
	float XionVerticalSize{};
	// Xion vertical size loop
	SDK::SinLoop XionVerticalSizeLoop{};

	// ED shake position
	SDK::Vector2 EDShakeOffset{};
	// ED shake interval timer
	SDK::Timer EDShakeTimer{};

	// Camera position and zoom
	SDK::Vector2 TitleCameraPosition{};
	float TitleCameraZoom{};
	// Camera move animation
	SDK::SinMove TitleCameraPositionMove{};
	// Camera zoom animation
	SDK::SinMove TitleCameraZoomMove{};

	// Camera position and zoom excluding intro
	SDK::Vector2 EffectCameraPosition{};
	float EffectCameraZoom{};

	// Title screen text
	SDK::Text TitleText{};
	// Text horizontal offset
	float TitleTextHorizontalOffset{};
	// Text rendering state
	bool TitleTextRenderState{};

	// Title start text size
	float StartTextSize{};
	// Title start text size loop
	SDK::SinLoop StartTextSizeLoop{};

	// Text lists (will be updated dynamically if needed, but here we use keys or direct GET_STR in render)
	std::wstring MenuItem[3];
	std::wstring QuestionItem[2];
	std::wstring QuestionItem2[2];

	// Menu text focusing
	bool ExitPageFocused[2]{ true, false };
	bool MainPageFocused[3]{ true, false, false };
	bool OptionPageFocused[7]{ true, false, false, false, false, false, false };
	bool ResetPageFocused[2]{ true, false };

	// Current menu index
	int ExitPageIndex{};
	int MainPageIndex{};
	int OptionPageIndex{};
	int ResetPageIndex{};

	// Menu page index
	int MenuPage{ 1 };
	 
	// Text background
	SDK::RectBrush TitleRect{};

	// Opacity for tutorial transition
	float RectOpacity{};

	// Game start timer
	SDK::Timer StartTimer{};

public:
	TitleScreen(bool IsIntroPlayed) {
		// Initialize strings
		UpdateLanguageStrings();

		// Text initialization
		TitleText.Init(SDK::FONTNAME.Main);
		TitleText.SetAlign(ALIGN_LEFT);
		TitleText.SetHeightAlign(HEIGHT_ALIGN_MIDDLE);
		TitleText.SetShadow(0.1, -0.1, 0.8);
		TitleText.SetLineGap(0.02);

		// Title screen setup
		SetTitleScreen(IsIntroPlayed);

		// Text background setup
		TitleRect.SetColor(0.0, 0.0, 0.0);
		TitleRect.SetRenderType(RENDER_TYPE_STATIC);

		// BGM play
		SDK::SoundTool.SetVolume(SDK::CHANNEL.BGM, SDK::GLOBAL.BGMVolume);
		SDK::SoundTool.Play(SDK::SOUND.TitleBgm, SDK::CHANNEL.BGM);
	}

	void UpdateLanguageStrings() {
		MenuItem[0] = GET_STR("Start");
		MenuItem[1] = GET_STR("Settings");
		MenuItem[2] = GET_STR("Exit");

		QuestionItem[0] = GET_STR("Yes");
		QuestionItem[1] = GET_STR("No");

		QuestionItem2[0] = GET_STR("No");
		QuestionItem2[1] = GET_STR("Yes");
	}

	void InputKey(SDK::KeyEvent& Event) {
		if (Event.Type != WM_KEYDOWN)  return;

		if (!TitleActivateState)
			InputFrontPage(Event);

		else if (TitleActivateState && TitleTextRenderState) {
			switch (MenuPage) {
				case ExitPage:
					InputExitPage(Event);  break;
				case MainPage:
					InputMainPage(Event);  break;
				case OptionPage:
					InputOptionPage(Event);  break;
				case ResetPage:
					InputResetPage(Event);  break;
			}
		}
	}

	void UpdateFunc(float FrameTime) {
		if (TutorialState) {
			RectOpacity += 4.0 * FrameTime;
			if (!TutorialAdded && SDK::EXTool.CheckClampValue(RectOpacity, 1.0, CLAMP_GREATER)) {
				SDK::Scene.AddObject(new Tutorial, "tutorial", LAYER7);
				TutorialAdded = true;
			}
		}

		if (CreditStartState) {
			RectOpacity += 1.0 * FrameTime;
			SDK::SoundTool.FadeOut(SDK::CHANNEL.BGM, 1.0, FrameTime);
			if (SDK::EXTool.CheckClampValue(RectOpacity, 1.0, CLAMP_GREATER)) {
				SDK::SoundTool.Stop(SDK::CHANNEL.BGM);
				SDK::Scene.SwitchMode(SDK::MODE.Credit);
			}
		}

		if (!GameStartState) {
			UpdateIntroAnimation(FrameTime);
			UpdateUI(FrameTime);
			UpdateTitleCamera(FrameTime);
			UpdateXionEDAnimation(FrameTime);
		}
		else {
			if (StartTimer.UpdateAndCheckSec(1, CHECK_AND_STOP, FrameTime)) 
				SDK::Scene.SwitchMode(SDK::MODE.Play);
		}
	}

	void RenderFunc() {
		if (!GameStartState) {
			RenderObjects();
			RenderUI();
		}
		else
			TitleRect.Draw(0.0, 0.0, SDK::ASP(2.0), 2.0);

		if (TutorialState || CreditStartState)
			TitleRect.Draw(0.0, 0.0, SDK::ASP(2.0), 2.0, 0.0, RectOpacity);
	}

	void SetTitleScreen(bool IsIntroPlayed) {
		if (IsIntroPlayed) {
			IntroPlayState = false;
			TitleActivateState = true;
			TitleTextRenderState = true;
			TitleTextHorizontalOffset = 0.0;
			TitleImagePosition = SDK::Vector2(SDK::ASP(1.0) - 0.45, -1.0 + 0.2);
			TitleImageSize = 1.0;

			TitleCameraPosition = SDK::Vector2(0.7, 0.4);
			TitleCameraZoom = 1.5;
			SDK::CameraControl.Move(0.7, 0.4);
			SDK::CameraControl.SetZoom(1.5);
		}
		else {
			IntroPlayState = true;
			TitleTextRenderState = false;
			TitleTextHorizontalOffset = 3.0;
			TitleImagePosition = SDK::Vector2(0.0, 0.5);
			TitleImageSize = 1.8;

			TitleCameraPosition = SDK::Vector2(-1.3, 0.4);
			TitleCameraZoom = 2.0;
			SDK::CameraControl.Move(-1.3, 0.4);
			SDK::CameraControl.SetZoom(2.0);

			TitleCameraPositionMove.SetMovePoint(TitleCameraPosition, SDK::Vector2(0.7, 0.4));
			TitleCameraZoomMove.SetMovePoint(TitleCameraZoom, 1.5);
		}
	}

	void InputFrontPage(SDK::KeyEvent& Event) {
		auto Exit = [&]() {
			MenuPage = MainPage;
			ExitPageFocused[ExitPageIndex] = false;
			ExitPageFocused[0] = true;
			ExitPageIndex = 0;
			};

		if (Event.Key == VK_UP || Event.Key == VK_DOWN) {
			if (MenuPage == ExitPage) {
				ExitPageFocused[ExitPageIndex] = false;
				ExitPageIndex += (int)Event.Key - (int)VK_RIGHT;
				SDK::EXTool.ClampValue(ExitPageIndex, 0, 1, CLAMP_RETURN);
				ExitPageFocused[ExitPageIndex] = true;
				SDK::SoundTool.Play(SDK::SOUND.MenuSelect, SDK::CHANNEL.SFX);
			}
		}
		else if (Event.Key == VK_RETURN) {
			switch (MenuPage) {
			case MainPage:
				TitleActivateState = true;
				break;
			case ExitPage:
				if (ExitPageFocused[0]) SDK::System.Exit();
				else if (ExitPageFocused[1]) Exit();
				break;
			}
			SDK::SoundTool.Play(SDK::SOUND.MenuSelect, SDK::CHANNEL.SFX);
		}
		else if (Event.Key == VK_ESCAPE) {
			switch (MenuPage) {
			case MainPage: MenuPage = ExitPage; break;
			case ExitPage: Exit(); break;
			}
			SDK::SoundTool.Play(SDK::SOUND.MenuSelect, SDK::CHANNEL.SFX);
		}
	}
	
	void InputExitPage(SDK::KeyEvent& Event) {
		auto Exit = [&]() {
			MenuPage = MainPage;
			ExitPageFocused[ExitPageIndex] = false;
			ExitPageFocused[0] = true;
			ExitPageIndex = 0;
			};

		if (Event.Key == VK_UP || Event.Key == VK_DOWN) {
			ExitPageFocused[ExitPageIndex] = false;
			ExitPageIndex += (int)Event.Key - (int)VK_RIGHT;
			SDK::EXTool.ClampValue(ExitPageIndex, 0, 1, CLAMP_RETURN);
			ExitPageFocused[ExitPageIndex] = true;
			SDK::SoundTool.Play(SDK::SOUND.MenuSelect, SDK::CHANNEL.SFX);
		}
		else if (Event.Key == VK_RETURN) {
			if (ExitPageFocused[0]) SDK::System.Exit();
			else if (ExitPageFocused[1]) Exit();
			SDK::SoundTool.Play(SDK::SOUND.MenuSelect, SDK::CHANNEL.SFX);
		}
		else if (Event.Key == VK_ESCAPE) {
			Exit();
			SDK::SoundTool.Play(SDK::SOUND.MenuSelect, SDK::CHANNEL.SFX);
		}
	}

	void InputMainPage(SDK::KeyEvent& Event) {
		if (!TutorialState && (Event.Key == VK_UP || Event.Key == VK_DOWN)) {
			MainPageFocused[MainPageIndex] = false;
			MainPageIndex += (int)Event.Key - (int)VK_RIGHT;
			SDK::EXTool.ClampValue(MainPageIndex, 0, 2, CLAMP_RETURN);
			MainPageFocused[MainPageIndex] = true;
			SDK::SoundTool.Play(SDK::SOUND.MenuSelect, SDK::CHANNEL.SFX);
		}
		else if (Event.Key == VK_RETURN) {
			if (!TutorialState) {
				switch (MainPageIndex) {
				case 0:
					if (!SDK::GLOBAL.NeedTutorial) {
						GameStartState = true;
						SDK::Scene.DeleteInputObject(this);
					}
					else TutorialState = true;
					break;
				case 1: MenuPage = OptionPage;  break;
				case 2: MenuPage = ExitPage;  break;
				}
				if (MainPageIndex != 0)
					SDK::SoundTool.Play(SDK::SOUND.MenuSelect, SDK::CHANNEL.SFX);
				else {
					if (!TutorialState) {
						SDK::SoundTool.Play(SDK::SOUND.CartCrash, SDK::CHANNEL.SFX);
						SDK::SoundTool.Stop(SDK::CHANNEL.BGM);
					}
					else SDK::SoundTool.Play(SDK::SOUND.MenuSelect, SDK::CHANNEL.SFX);
				}
			}
			else {
				GameStartState = true;
				if (auto Tutorial = SDK::Scene.Find("tutorial"); Tutorial)
					Tutorial->DisableRenderState();
				SDK::SoundTool.Play(SDK::SOUND.CartCrash, SDK::CHANNEL.SFX);
				SDK::SoundTool.Stop(SDK::CHANNEL.BGM);
			}
		}
		else if (!TutorialState && Event.Key == VK_ESCAPE) {
			MenuPage = ExitPage;
			SDK::SoundTool.Play(SDK::SOUND.MenuSelect, SDK::CHANNEL.SFX);
		}
	}

	void InputOptionPage(SDK::KeyEvent& Event) {
		auto Exit = [&]() {
			MenuPage = MainPage;
			OptionPageFocused[OptionPageIndex] = false;
			OptionPageFocused[0] = true;
			OptionPageIndex = 0;
			};

		if (Event.Key == VK_UP || Event.Key == VK_DOWN) {
			OptionPageFocused[OptionPageIndex] = false;
			OptionPageIndex += (int)Event.Key - (int)VK_RIGHT;
			SDK::EXTool.ClampValue(OptionPageIndex, 0, 6, CLAMP_RETURN);
			OptionPageFocused[OptionPageIndex] = true;
			SDK::SoundTool.Play(SDK::SOUND.MenuSelect, SDK::CHANNEL.SFX);
		}

		if (((0 < OptionPageIndex && OptionPageIndex < 4) || OptionPageIndex == 6) && (Event.Key == VK_RIGHT || Event.Key == VK_LEFT)) {
			switch (OptionPageIndex) {
			case 1:
				SDK::EXTool.SwitchBool(SDK::GLOBAL.FullscreenAcvivated);
				SDK::FILE.UserSettingData.UpdateDigitData("Setting", "Fullscreen", (int)SDK::GLOBAL.FullscreenAcvivated);
				SDK::System.SwitchScreenState();
				break;
			case 2:
				SDK::GLOBAL.BGMVolume += (float)((int)Event.Key - (int)VK_UP) * 0.1;
				SDK::EXTool.ClampValue(SDK::GLOBAL.BGMVolume, 0.0, 1.0, CLAMP_FIXED);
				SDK::SoundTool.SetVolume(SDK::CHANNEL.BGM, SDK::GLOBAL.BGMVolume);
				SDK::FILE.UserSettingData.UpdateDigitData("Setting", "BGMVolume", SDK::GLOBAL.BGMVolume);
				break;
			case 3:
				SDK::GLOBAL.SFXVolume += (float)((int)Event.Key - (int)VK_UP) * 0.1;
				SDK::EXTool.ClampValue(SDK::GLOBAL.SFXVolume, 0.0, 1.0, CLAMP_FIXED);
				SDK::SoundTool.SetVolume(SDK::CHANNEL.SFX, SDK::GLOBAL.SFXVolume);
				SDK::FILE.UserSettingData.UpdateDigitData("Setting", "SFXVolume", SDK::GLOBAL.SFXVolume);
				break;
			case 6: // Language switch
				if (Event.Key == VK_RIGHT) SDK::GLOBAL.Language++;
				else SDK::GLOBAL.Language--;
				SDK::EXTool.ClampValue(SDK::GLOBAL.Language, 0, 2, CLAMP_RETURN);
				SDK::LANG.Load(SDK::GLOBAL.Language);
				SDK::FILE.UserSettingData.UpdateDigitData("Setting", "Language", (float)SDK::GLOBAL.Language);
				UpdateLanguageStrings();
				break;
			}
			SDK::SoundTool.Play(SDK::SOUND.MenuSelect, SDK::CHANNEL.SFX);
		}
		else if ((OptionPageIndex == 0 || OptionPageIndex == 4 || OptionPageIndex == 5) && Event.Key == VK_RETURN) {
			switch (OptionPageIndex) {
			case 0: Exit();  break;
			case 4: MenuPage = ResetPage;  break;
			case 5:
				CreditStartState = true;
				SDK::Scene.DeleteInputObject(this);
				break;
			}
			SDK::SoundTool.Play(SDK::SOUND.MenuSelect, SDK::CHANNEL.SFX);
		}
		else if (Event.Key == VK_ESCAPE) {
			Exit();
			SDK::SoundTool.Play(SDK::SOUND.MenuSelect, SDK::CHANNEL.SFX);
		}
	}

	void InputResetPage(SDK::KeyEvent& Event) {
		auto Exit = [&]() {
			MenuPage = OptionPage;
			ResetPageFocused[ResetPageIndex] = false;
			ResetPageFocused[0] = true;
			ResetPageIndex = 0;
			};

		if (Event.Key == VK_UP || Event.Key == VK_DOWN) {
			ResetPageFocused[ResetPageIndex] = false;
			ResetPageIndex += (int)Event.Key - (int)VK_RIGHT;
			SDK::EXTool.ClampValue(ResetPageIndex, 0, 1, CLAMP_RETURN);
			ResetPageFocused[ResetPageIndex] = true;
			SDK::SoundTool.Play(SDK::SOUND.MenuSelect, SDK::CHANNEL.SFX);
		}
		else if (Event.Key == VK_RETURN) {
			if (ResetPageIndex == 1) {
				SDK::FILE.HighscoreData.ResetData();
				SDK::GLOBAL.NeedTutorial = true;
				SDK::GLOBAL.IsHighScore = false;
				SDK::GLOBAL.IsHighRep = false;
				SDK::GLOBAL.MaxRep = 0;
				SDK::GLOBAL.HighScore = 0;
			}
			Exit();
			SDK::SoundTool.Play(SDK::SOUND.MenuSelect, SDK::CHANNEL.SFX);
		}
		else if (Event.Key == VK_ESCAPE) {
			Exit();
			SDK::SoundTool.Play(SDK::SOUND.MenuSelect, SDK::CHANNEL.SFX);
		}
	}

	void UpdateIntroAnimation(float FrameTime) {
		if (IntroPlayState) {
			if (!GameStartState) {
				if (TitleActivateState) {
					TitleCameraZoomMove.Update(TitleCameraZoom, 0.5, FrameTime);
					TitleCameraPositionMove.Update(TitleCameraPosition, 0.5, FrameTime);
					SDK::Math.Lerp(TitleImagePosition.x, SDK::ASP(1.0) - 0.45, 5.0, FrameTime);
					SDK::Math.Lerp(TitleImagePosition.y, -1.0 + 0.2, 5.0, FrameTime);
					SDK::Math.Lerp(TitleImageSize, 1.0, 5.0, FrameTime);
				}
				if (TitleCameraPosition.x > 0.6) TitleTextRenderState = true;
			}
		}
		else {
			TitleCameraPosition.x = 0.7;
			TitleCameraZoom = 1.5;
			TitleImagePosition = SDK::Vector2(SDK::ASP(1.0) - 0.45, -1.0 + 0.2);
		}
	}

	void UpdateUI(float FrameTime) {
		if (TitleTextRenderState) SDK::Math.Lerp(TitleTextHorizontalOffset, 0.0, 5.0, FrameTime);
		else StartTextSizeLoop.Update(StartTextSize, 0.02, 5.0, FrameTime);
	}

	void UpdateTitleCamera(float FrameTime) {
		if (TitleActivateState) {
			switch (MenuPage) {
			case MainPage:
				SDK::Math.Lerp(EffectCameraPosition, SDK::Vector2(0.0, 0.0), 2.0, FrameTime);
				SDK::Math.Lerp(EffectCameraZoom, 0.0, 2.0, FrameTime);
				break;
			case ExitPage:
				SDK::Math.Lerp(EffectCameraPosition, SDK::Vector2(0.3, -0.25), 2.0, FrameTime);
				SDK::Math.Lerp(EffectCameraZoom, 3.0, 2.0, FrameTime);
				break;
			case OptionPage: case ResetPage:
				SDK::Math.Lerp(EffectCameraPosition, SDK::Vector2(-2.0, 0.0), 2.0, FrameTime);
				SDK::Math.Lerp(EffectCameraZoom, 0.0, 2.0, FrameTime);
				break;
			}
		}
		SDK::CameraControl.Move(TitleCameraPosition + EffectCameraPosition);
		SDK::CameraControl.SetZoom(TitleCameraZoom + EffectCameraZoom);
	}

	void UpdateXionEDAnimation(float FrameTime) {
		XionVerticalSizeLoop.Update(XionVerticalSize, 0.03, 15.0, FrameTime);
		if (EDShakeTimer.UpdateAndCheckMiliSec(0.02, 2, CHECK_AND_INTERPOLATE, FrameTime)) {
			EDShakeOffset.x = SDK::Random.Gen(RANDOM_TYPE_REAL, -0.01, 0.01);
			EDShakeOffset.y = SDK::Random.Gen(RANDOM_TYPE_REAL, -0.01, 0.01);
		}
	}

	void RenderObjects() {
		Begin();
		SDK::Transform.Move(SDK::MoveMatrix, -1.3, XionVerticalSize * 0.5);
		SDK::Transform.Scale(SDK::MoveMatrix, 2.0, 2.0 + XionVerticalSize);
		SDK::ImageTool.RenderStaticSpriteSheet(SDK::IMAGE.Xion, Xion_Nervous);
		Begin();
		SDK::Transform.Move(SDK::MoveMatrix, EDShakeOffset);
		SDK::Transform.Scale(SDK::MoveMatrix, 2.0, 2.0);
		SDK::ImageTool.RenderStaticSpriteSheet(SDK::IMAGE.ED, ED_Title);
		Begin();
		SDK::Transform.Move(SDK::MoveMatrix, 0.7, -0.3);
		SDK::Transform.Scale(SDK::MoveMatrix, 1.5, 1.5);
		SDK::ImageTool.RenderStaticSpriteSheet(SDK::IMAGE.Cart, 0);
	}

	void RenderUI() {
		RenderText();
		if (MenuPage == MainPage) {
			Begin(RENDER_TYPE_STATIC);
			SDK::Transform.Move(SDK::MoveMatrix, TitleImagePosition);
			SDK::Transform.Scale(SDK::MoveMatrix, TitleImageSize, TitleImageSize);
			SDK::ImageTool.RenderImage(SDK::IMAGE.Title);
		}
	}

	void RenderText() {
		TitleText.DisableShadow();
		switch (MenuPage) {
		case ExitPage: RenderExitPage();  break;
		case MainPage: RenderMainPage();  break;
		case OptionPage: RenderOptionPage();  break;
		case ResetPage: RenderResetPage();  break;
		}
	}

	void RenderMainPage() {
		if (TitleActivateState) {
			TitleRect.Draw(SDK::ASP(1.0) + TitleTextHorizontalOffset, 0.0, 1.8, 2.0, 0.0, 0.5);
			TitleText.SetAlign(ALIGN_LEFT);
			TitleText.SetHeightAlign(HEIGHT_ALIGN_MIDDLE);
			float TextHorizontalPositon = SDK::ASP(1.0) - 0.1 + TitleTextHorizontalOffset;
			for (int i = 0; i < 3; i++) {
				if (MainPageFocused[i])  TitleText.SetColorRGB(255, 213, 80);
				else				     TitleText.SetColor(1.0, 1.0, 1.0);
				TitleText.RenderWString(TextHorizontalPositon, 0.25 - 0.25 * i, 0.1, MenuItem[i]);
			}
			if (SDK::GLOBAL.IsHighScore)  TitleText.SetColor(0.0, 1.0, 0.0);
			else						  TitleText.SetColor(1.0, 1.0, 1.0);
			if (SDK::GLOBAL.HighScore > 0)
				TitleText.Render(TextHorizontalPositon, 0.85, 0.08, L"%s\n%d", GET_STR("HighScore"), SDK::GLOBAL.HighScore);
			else
				TitleText.Render(TextHorizontalPositon, 0.85, 0.08, L"%s\n-", GET_STR("HighScore"));
			if (SDK::GLOBAL.IsHighRep)  TitleText.SetColor(0.0, 1.0, 0.0);
			else						TitleText.SetColor(1.0, 1.0, 1.0);
			if (SDK::GLOBAL.MaxRep > 0)
				TitleText.Render(TextHorizontalPositon, 0.6, 0.08, L"%s\n%d%s", GET_STR("MaxRep"), SDK::GLOBAL.MaxRep, GET_STR("Currency"));
			else
				TitleText.Render(TextHorizontalPositon, 0.6, 0.08, L"%s\n-%s", GET_STR("MaxRep"), GET_STR("Currency"));
		}
		else {
			TitleText.SetAlign(ALIGN_MIDDLE);
			TitleText.SetColorRGB(255, 213, 80);
			TitleText.EnableShadow();
			TitleText.RenderWString(0.0, -0.7, 0.15 + StartTextSize, GET_STR("EnterToStart"));
			TitleText.SetAlign(ALIGN_DEFAULT);
			TitleText.SetHeightAlign(HEIGHT_ALIGN_DEFAULT);
			TitleText.SetColor(1.0, 1.0, 1.0);
			TitleText.Render(SDK::ASP(-1.0) + 0.01, -1.0 + 0.05, 0.1, GET_STR("Version"), APPLICATION_VERSION);
		}
	}

	void RenderOptionPage() {
		TitleRect.Draw(0.0, 0.0, SDK::ASP(2.0), 2.0, 0.0, 0.7);
		TitleText.SetAlign(ALIGN_MIDDLE);
		TitleText.SetHeightAlign(HEIGHT_ALIGN_MIDDLE);
		TitleText.SetColor(1.0, 1.0, 1.0);
		TitleText.RenderWString(0.0, 0.8, 0.15, GET_STR("Title"));
		TitleText.SetAlign(ALIGN_LEFT);
		for (int i = 0; i < 7; i++) {
			if (OptionPageFocused[i]) TitleText.SetColorRGB(255, 213, 80);
			else					  TitleText.SetColor(1.0, 1.0, 1.0);
			switch (i) {
			case 0: TitleText.RenderWString(SDK::ASP(1.0) - 0.1, 0.625 - i * 0.2, 0.08, GET_STR("BackToTitle")); break;
			case 1:
				if (SDK::GLOBAL.FullscreenAcvivated)
					TitleText.Render(SDK::ASP(1.0) - 0.1, 0.625 - i * 0.2, 0.08, L"%s%s", GET_STR("ScreenMode"), GET_STR("Fullscreen"));
				else
					TitleText.Render(SDK::ASP(1.0) - 0.1, 0.625 - i * 0.2, 0.08, L"%s%s", GET_STR("ScreenMode"), GET_STR("Windowed"));
				break;
			case 2: TitleText.Render(SDK::ASP(1.0) - 0.1, 0.625 - i * 0.2, 0.08, GET_STR("BGMVolume"), SDK::GLOBAL.BGMVolume); break;
			case 3: TitleText.Render(SDK::ASP(1.0) - 0.1, 0.625 - i * 0.2, 0.08, GET_STR("SFXVolume"), SDK::GLOBAL.SFXVolume); break;
			case 4: TitleText.RenderWString(SDK::ASP(1.0) - 0.1, 0.625 - i * 0.2, 0.08, GET_STR("ResetData")); break;
			case 5: TitleText.RenderWString(SDK::ASP(1.0) - 0.1, 0.625 - i * 0.2, 0.08, GET_STR("Credits")); break;
			case 6: TitleText.RenderWString(SDK::ASP(1.0) - 0.1, 0.625 - i * 0.2, 0.08, GET_STR("Language")); break;
			}
		}
		if ((0 < OptionPageIndex && OptionPageIndex < 4) || OptionPageIndex == 6) {
			TitleText.SetHeightAlign(HEIGHT_ALIGN_DEFAULT);
			TitleText.SetAlign(ALIGN_DEFAULT);
			TitleText.RenderWString(SDK::ASP(-1.0) + 0.1, -0.9, 0.08, GET_STR("ControlHint"));
		}
	}

	void RenderResetPage() {
		TitleRect.Draw(0.0, 0.0, SDK::ASP(2.0), 2.0, 0.0, 0.7);
		TitleText.SetColor(1.0, 1.0, 1.0);
		TitleText.SetAlign(ALIGN_MIDDLE);
		TitleText.RenderWString(0.0, 0.8, 0.15, GET_STR("ResetConfirm"));
		TitleText.SetAlign(ALIGN_LEFT);
		for (int i = 0; i < 2; i++) {
			if (ResetPageFocused[i])  TitleText.SetColorRGB(255, 213, 80);
			else				      TitleText.SetColor(1.0, 1.0, 1.0);
			TitleText.RenderWString(SDK::ASP(1.0) - 0.1, 0.125 - i * 0.25, 0.1, QuestionItem2[i]);
		}
	}

	void RenderExitPage() {
		TitleRect.Draw(0.0, 0.0, SDK::ASP(2.0), 2.0, 0.0, 0.7);
		TitleText.SetColor(1.0, 1.0, 1.0);
		TitleText.SetAlign(ALIGN_MIDDLE);
		TitleText.RenderWString(0.0, 0.8, 0.15, GET_STR("ExitConfirm"));
		TitleText.SetAlign(ALIGN_LEFT);
		for (int i = 0; i < 2; i++) {
			if (ExitPageFocused[i]) TitleText.SetColorRGB(255, 213, 80);
			else				    TitleText.SetColor(1.0, 1.0, 1.0);
			TitleText.RenderWString(SDK::ASP(1.0) - 0.1, 0.125 - i * 0.25, 0.1, QuestionItem[i]);
		}
	}
};
