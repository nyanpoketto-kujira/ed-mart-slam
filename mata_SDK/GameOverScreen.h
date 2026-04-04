#pragma once
#include <SDK_Scene.h>
#include <SDK_Language.h>

#include "Cover.h"
#include "TitleMode.h"

struct Reparations {
	int DestroyedType;
	int NumDestroyed;
};

class GameOverScreen : public SDK::Object {
private:
	// Timer for Chloe's slap animation
	SDK::Timer Timer{};

	// Interval for Chloe's slap
	float CheekInterval{};

	// Shiver timer for characters
	SDK::Timer ShiverTimer{};

	// Shiver values for animation
	glm::vec2 ShiverValue{};
	glm::vec2 ShiverValue2{};

	// Size offset for animation
	float VerticalSize{};

	// Xion vertical size for animation
	float XionVerticalSize{};
	SDK::SinLoop XionVerticalSizeLoop{};

	// Xion text size loop
	float XionTextSize{};
	SDK::SinLoop XionTextLoop{};

	// Text size loop
	float TextSize{};
	SDK::SinLoop TextSizeLoop{};

	// Animation frames
	int ChloeFrame{};
	int EDFrame{};

	// ED frame timer
	SDK::Timer EDFrameTimer{};

	// Exit state to Title
	bool ExitState{};

	std::wstring Str{};
	std::wstring Str2{};

	SDK::Text Text{}, ScoreText{};

	// Reparation display flags
	bool ShowRep{};
	bool ShowTotalRep{};
	 
	// List of reparations
	std::vector <Reparations> RepVec{};
	size_t VecSize{};
	int CurrentSize{};

	// Total reparation amount
	int TotalRep{};

	// Timer for showing reparations one by one
	SDK::Timer RepTimer{};

	// UI Rect
	SDK::RectBrush Rect{};

	// Total amount text shake
	glm::vec2 TextShake{};
	float TextShakeValue{0.05};
	SDK::Timer TextShakeTimer{};

	SDK::SoundChannel SndChannel1{};
	SDK::SoundChannel SndChannel2{};
	SDK::SoundChannel SndChannel3{};

	// Delete timer for scene transition
	SDK::Timer DeleteTimer{};

	bool NewHighScore{};
	bool NewHighRep{};

	// Skip flag for reparations
	bool RepSkip{};

public:
	GameOverScreen() {
		if(SDK::GLOBAL.Ending == GameOver_TimeOut || SDK::GLOBAL.Ending == GameOver_HitXion)
			SDK::System.SetBackColorRGB(74, 78, 101);
		else
			SDK::System.SetBackColorRGB(255, 178, 80);

		Text.Init(SDK::FONTNAME.Main, FW_DONTCARE);
		Text.SetAlign(ALIGN_MIDDLE);
		Text.SetHeightAlign(HEIGHT_ALIGN_MIDDLE);
		Text.EnableShadow();
		Text.SetShadow(0.1, -0.1, 0.7);
		Text.SetColor(1.0, 1.0, 1.0);
		Text.SetLineGap(0.01);
		Text.Rotate(-10.0);

		ScoreText.Init(SDK::FONTNAME.Main, FW_DONTCARE);
		ScoreText.SetAlign(ALIGN_MIDDLE);
		ScoreText.SetHeightAlign(HEIGHT_ALIGN_MIDDLE);
		ScoreText.EnableShadow();
		ScoreText.SetShadow(0.1, -0.1, 0.7);
		ScoreText.SetColor(1.0, 1.0, 1.0);
		ScoreText.SetLineGap(0.02);

		if (SDK::GLOBAL.Ending == GameOver_TimeOut) {
			CheekInterval = 0.7;
			Str = GET_STR("EndingTimeOut1");
			Str2 = GET_STR("EndingTimeOut2");
		}
		else if (SDK::GLOBAL.Ending == GameOver_HitXion) {
			CheekInterval = 0.25;
			Str = GET_STR("EndingHitXion1");
			Str2 = GET_STR("EndingHitXion2");
		}
		else if (SDK::GLOBAL.Ending == GameOver_Suppressed) {
			Str = GET_STR("EndingSuppressed1");
			Str2 = GET_STR("EndingSuppressed2");
		}

		SDK::SoundTool.Play(SDK::SOUND.GameEnd[SDK::GLOBAL.Ending], SDK::CHANNEL.BGM);

		// Calculate reparations
		for (int i = 0; i < 6; i++) {
			Reparations Rep{};
			if (SDK::GLOBAL.DestroyedItems[i] > 0) {
				Rep.DestroyedType = i;
				Rep.NumDestroyed = SDK::GLOBAL.DestroyedItems[i];
				RepVec.emplace_back(Rep);
			}
		}
		VecSize = RepVec.size();

		TotalRep += SDK::GLOBAL.DestroyedItems[Item_BoxCoffee] * 25000;
		TotalRep += SDK::GLOBAL.DestroyedItems[Item_BottleCoffee] * 2500;
		TotalRep += SDK::GLOBAL.DestroyedItems[Item_CanCoffee] * 1200;
		TotalRep += SDK::GLOBAL.DestroyedItems[Item_People] * 2000000;
		TotalRep += SDK::GLOBAL.DestroyedItems[Item_Xion] * 1000000;
		TotalRep += SDK::GLOBAL.DestroyedItems[Item_HouseDoc] * 100000;

		if (SDK::GLOBAL.HighScore < SDK::GLOBAL.Score) {
			NewHighScore = true;
			SDK::GLOBAL.IsHighScore = true;
			SDK::FILE.HighscoreData.UpdateDigitData("HighScore", "Score", (float)SDK::GLOBAL.Score);
			SDK::GLOBAL.HighScore = SDK::GLOBAL.Score;
		}

		if (SDK::GLOBAL.MaxRep < TotalRep) {
			NewHighRep = true;
			SDK::GLOBAL.IsHighRep = true;
			SDK::FILE.HighscoreData.UpdateDigitData("HighScore", "Rep", (float)TotalRep);
			SDK::GLOBAL.MaxRep = TotalRep;
		}

		SDK::SoundTool.SetVolume(SndChannel2, SDK::GLOBAL.SFXVolume);
		SDK::SoundTool.SetVolume(SndChannel3, SDK::GLOBAL.SFXVolume);

		Rect.SetColor(0.0, 0.0, 0.0);
		Rect.SetRenderType(RENDER_TYPE_STATIC);
	}

	void InputKey(SDK::KeyEvent& Event) {
		if (Event.Type == WM_KEYDOWN && Event.Key == VK_RETURN) {
			if (SDK::GLOBAL.Ending == GameOver_TimeOut || SDK::GLOBAL .Ending == GameOver_HitXion) {
				if (!ShowRep) {
					ShowRep = true;
					ScoreText.DisableShadow();
				}
				else if (ShowRep && !ShowTotalRep) {
					ShowTotalRep = true;
					RepSkip = true;
					CurrentSize = (int)VecSize;
					SDK::SoundTool.Play(SDK::SOUND.RepTotal, SndChannel3);
				}
				else if (ShowTotalRep) {
					ExitState = true;
					SDK::Scene.AddObject(new Cover(0.5), "cover", LAYER7);
					SDK::Scene.DeleteInputObject(this);
				}
			}
			else {
				ExitState = true;
				SDK::Scene.AddObject(new Cover(0.5), "cover", LAYER7);
				SDK::Scene.DeleteInputObject(this);
			}
		}
	}

	void UpdateFunc(float FrameTime) {
		UpdateChloeEDXionAnimation(FrameTime);
		UpdateRepResult(FrameTime);
		UpdateTextAnimation(FrameTime);
		UpdateExit(FrameTime);
	}

	void RenderFunc() {
		RenderObjects();
		if (!ShowRep)  RenderGameoverText();
		else		   RenderRepResult();
	}

	void UpdateChloeEDXionAnimation(float FrameTime) {
		if (SDK::GLOBAL.Ending == GameOver_TimeOut || SDK::GLOBAL.Ending == GameOver_HitXion) {
			Timer.Update(FrameTime);
			if (Timer.CheckMiliSec(CheekInterval, 2, CHECK_AND_INTERPOLATE)) {
				ChloeFrame++;
				if (ChloeFrame == 1) {
					EDFrame = 1;
					EDFrameTimer.Reset();
					VerticalSize = 0.3;
					SDK::SoundTool.Play(SDK::SOUND.HitCheek, SndChannel2);
				}
				SDK::EXTool.ClampValue(ChloeFrame, 0, 1, CLAMP_RETURN);
			}

			ShiverTimer.Update(FrameTime);
			if (ShiverTimer.CheckMiliSec(0.02, 2, CHECK_AND_INTERPOLATE)) {
				ShiverValue.x = SDK::Random.Gen(RANDOM_TYPE_REAL, -0.01, 0.01);
				ShiverValue.y = SDK::Random.Gen(RANDOM_TYPE_REAL, -0.01, 0.01);
				ShiverValue2.x = SDK::Random.Gen(RANDOM_TYPE_REAL, -0.01, 0.01);
				ShiverValue2.y = SDK::Random.Gen(RANDOM_TYPE_REAL, -0.01, 0.01);
			}

			SDK::Math.Lerp(VerticalSize, 0.0, 10.0, FrameTime);

			if (EDFrame == 1) {
				EDFrameTimer.Update(FrameTime);
				if (EDFrameTimer.CheckMiliSec(CheekInterval * 0.5, 2, CHECK_AND_RESET))
					EDFrame = 0;
			}
		}
		else
			XionVerticalSizeLoop.Update(XionVerticalSize, 0.05, 20.0, FrameTime);
	}

	void UpdateTextAnimation(float FrameTime) {
		TextSizeLoop.Update(TextSize, 0.01, 10.0, FrameTime);
		XionTextLoop.Update(XionTextSize, 0.01, 20.0, FrameTime);
	}

	void UpdateRepResult(float FrameTime) {
		if (ShowRep) {
			if (!RepSkip)
				RepTimer.Update(FrameTime);

			if (CurrentSize != VecSize && RepTimer.CheckMiliSec(0.5, 1, CHECK_AND_INTERPOLATE)) {
				CurrentSize++;
				SDK::SoundTool.Play(SDK::SOUND.Rep, SndChannel3);
			}
			else if (CurrentSize == VecSize && RepTimer.CheckMiliSec(1, 1, CHECK_AND_INTERPOLATE)) {
				ShowTotalRep = true;
				if (!RepSkip)
					SDK::SoundTool.Play(SDK::SOUND.RepTotal, SndChannel3);
				RepTimer.Stop();
				RepTimer.Reset();
			}

			if (ShowTotalRep) {
				TextShakeTimer.Update(FrameTime);
				if (TextShakeTimer.CheckMiliSec(0.02, 2, CHECK_AND_INTERPOLATE)) {
					TextShake.x = SDK::Random.Gen(RANDOM_TYPE_REAL, -TextShakeValue, TextShakeValue);
					TextShake.y = SDK::Random.Gen(RANDOM_TYPE_REAL, -TextShakeValue, TextShakeValue);
				}
				SDK::Math.Lerp(TextShakeValue, 0.0, 5.0, FrameTime);
			}
		}
	}

	void UpdateExit(float FrameTime) {
		if (ExitState) {
			SDK::SoundTool.FadeOut(SDK::CHANNEL.BGM, 0.5, FrameTime);
			SDK::SoundTool.FadeOut(SndChannel2, 0.5, FrameTime);
			SDK::SoundTool.FadeOut(SndChannel3, 0.5, FrameTime);

			if (auto Cover = SDK::Scene.Find("cover"); Cover)
				if (Cover->GetState()) {
					SDK::System.SetBackColorRGB(122, 138, 154);
					DeleteTimer.Update(FrameTime);
					if (DeleteTimer.Sec() >= 1) {
						SDK::SoundTool.Stop(SDK::CHANNEL.BGM);
						SDK::SoundTool.SetVolume(SDK::CHANNEL.BGM, SDK::GLOBAL.BGMVolume);
						SDK::Scene.SwitchMode(SDK::MODE.Title);
						SDK::Scene.AddObject(new DisappearCover, "dis_cover", LAYER7);
					}
				}
		}
	}

	void RenderObjects() {
		Begin();
		SDK::Transform.Scale(SDK::MoveMatrix, 3.0, 3.0);
		if (SDK::GLOBAL.Ending == GameOver_TimeOut || SDK::GLOBAL.Ending == GameOver_HitXion)
			SDK::ImageTool.RenderImage(SDK::IMAGE.GameOverBackGround, 0.7);
		else
			SDK::ImageTool.RenderImage(SDK::IMAGE.GameOverBackGround2, 0.7);

		Begin();
		if (SDK::GLOBAL.Ending == GameOver_Suppressed || SDK::GLOBAL.Ending == GameOver_HitXion)
			SDK::Transform.Move(SDK::MoveMatrix, SDK::ASP(-1.0) + 0.5, -0.9);
		else
			SDK::Transform.Move(SDK::MoveMatrix, SDK::ASP(-1.0) + 0.5 - 0.075, -0.9);
		SDK::Transform.Scale(SDK::MoveMatrix, 0.75, 0.75);
		SDK::ImageTool.RenderImage(SDK::IMAGE.Shadow);

		Begin();
		SDK::Transform.Move(SDK::MoveMatrix, SDK::ASP(-1.0) + 0.5 + ShiverValue.x, -0.3 + ShiverValue.y + XionVerticalSize * 0.5);
		SDK::Transform.Scale(SDK::MoveMatrix, 1.5, 1.5 + XionVerticalSize);
		if (SDK::GLOBAL.Ending == GameOver_HitXion)
			SDK::ImageTool.RenderStaticSpriteSheet(SDK::IMAGE.Xion, Xion_Hurt);
		else if (SDK::GLOBAL.Ending == GameOver_TimeOut)
			SDK::ImageTool.RenderStaticSpriteSheet(SDK::IMAGE.Xion, Xion_Cry1);
		else if (SDK::GLOBAL.Ending == GameOver_Suppressed) {
			SDK::ImageTool.RenderStaticSpriteSheet(SDK::IMAGE.Xion, Xion_Happy);
		}

		Begin();
		SDK::Transform.Move(SDK::MoveMatrix, SDK::ASP(1.0) - 1.03 + ShiverValue2.x - VerticalSize * 0.5, -0.7 - VerticalSize * 0.5 + ShiverValue2.y - XionVerticalSize * 0.5);
		SDK::Transform.Scale(SDK::MoveMatrix, 2.0 + VerticalSize, 2.0 - VerticalSize - XionVerticalSize);
		SDK::Transform.Tilt(SDK::MoveMatrix, -VerticalSize * 0.5, 0.0);
		if (SDK::GLOBAL.Ending == GameOver_TimeOut || SDK::GLOBAL.Ending == GameOver_HitXion)
			SDK::ImageTool.RenderStaticSpriteSheet(SDK::IMAGE.ED_GameOver, EDFrame);
		else {
			SDK::Transform.Flip(FLIP_TYPE_H);
			SDK::ImageTool.RenderStaticSpriteSheet(SDK::IMAGE.ED, ED_Happy);
		}

		if (SDK::GLOBAL.Ending == GameOver_TimeOut || SDK::GLOBAL.Ending == GameOver_HitXion) {
			Begin();
			SDK::Transform.Move(SDK::MoveMatrix, SDK::ASP(1.0) - 0.5 + ShiverValue2.x - VerticalSize * 0.5, -0.6 - VerticalSize * 0.5 + ShiverValue2.y);
			SDK::Transform.Scale(SDK::MoveMatrix, 2.2 + VerticalSize, 2.2 - VerticalSize);
			SDK::Transform.Tilt(SDK::MoveMatrix, -VerticalSize * 0.5, 0.0);
			SDK::ImageTool.RenderStaticSpriteSheet(SDK::IMAGE.Chloe, ChloeFrame);
		}
	}

	void RenderGameoverText() {
		ScoreText.SetColor(1.0, 1.0, 1.0);
		Text.RenderWString(SDK::ASP(-1.0) + 0.8, 0.85, 0.2 + TextSize, Str);
		Text.RenderWString(SDK::ASP(-1.0) + 0.6, 0.4, 0.07 + XionTextSize, Str2);
		ScoreText.Render(0.0, 0.4, 0.2, L"SCORE\n%d", SDK::GLOBAL.Score);
		if (NewHighScore) {
			ScoreText.SetColorRGB(255, 213, 80);
			ScoreText.RenderWString(0.0, 0.55, 0.1, GET_STR("HighScoreNew"));
		}
		ScoreText.SetColor(1.0, 1.0, 1.0);
		ScoreText.SetAlign(ALIGN_LEFT);
		ScoreText.SetHeightAlign(HEIGHT_ALIGN_UNDER);
		ScoreText.RenderWString(SDK::ASP(1.0) - 0.05, 0.95, 0.1, GET_STR("EnterToMain"));
		ScoreText.SetAlign(ALIGN_MIDDLE);
		ScoreText.SetHeightAlign(HEIGHT_ALIGN_MIDDLE);
	}

	void RenderRepResult() {
		Rect.Draw(0.0, 0.0, SDK::ASP(2.0), 2.0, 0.0, 0.6);
		float RenderHeight{ 0.8 };
		for (int i = 0; i < CurrentSize; i++) {
			ScoreText.SetColor(1.0, 1.0, 1.0);
			switch (RepVec[i].DestroyedType) {
			case Item_BoxCoffee:
				ScoreText.Render(0.0, RenderHeight, 0.1, GET_STR("ItemBox"), SDK::GLOBAL.DestroyedItems[RepVec[i].DestroyedType]);
				break;
			case Item_BottleCoffee:
				ScoreText.Render(0.0, RenderHeight, 0.1, GET_STR("ItemBottle"), SDK::GLOBAL.DestroyedItems[RepVec[i].DestroyedType]);
				break;
			case Item_CanCoffee:
				ScoreText.Render(0.0, RenderHeight, 0.1, GET_STR("ItemCan"), SDK::GLOBAL.DestroyedItems[RepVec[i].DestroyedType]);
				break;
			case Item_People:
				ScoreText.Render(0.0, RenderHeight, 0.1, GET_STR("ItemPeople"), SDK::GLOBAL.DestroyedItems[RepVec[i].DestroyedType]);
				break;
			case Item_Xion:
				ScoreText.RenderWString(0.0, RenderHeight, 0.1, GET_STR("ItemXion"));
				break;
			case Item_HouseDoc:
				ScoreText.Render(0.0, RenderHeight, 0.1, GET_STR("ItemHouse"), SDK::GLOBAL.DestroyedItems[RepVec[i].DestroyedType]);
				break;
			}
			RenderHeight -= 0.2;
		}

		if (ShowTotalRep) {
			ScoreText.SetColorRGB(255, 213, 80);
			ScoreText.Render(TextShake.x, -0.7 + TextShake.y, 0.15, L"%s%d %s", GET_STR("Total"), TotalRep, GET_STR("Currency"));
			if (NewHighRep)
				ScoreText.RenderWString(TextShake.x, -0.5 + TextShake.y, 0.1, GET_STR("HighRepNew"));
		}

		ScoreText.SetColor(1.0, 1.0, 1.0);
		ScoreText.SetAlign(ALIGN_LEFT);
		ScoreText.SetHeightAlign(HEIGHT_ALIGN_DEFAULT);
		ScoreText.RenderWString(SDK::ASP(1.0) - 0.05, -0.95, 0.1, GET_STR("EnterToMain"));
		ScoreText.SetAlign(ALIGN_MIDDLE);
		ScoreText.SetHeightAlign(HEIGHT_ALIGN_MIDDLE);
	}
};
