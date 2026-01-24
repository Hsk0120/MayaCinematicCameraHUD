#pragma once

#include <maya/MUserData.h>
#include <maya/MPoint.h>
#include <maya/MUIDrawManager.h>
#include <maya/MString.h>
#include <maya/MColor.h>

// CameraHud の描画データを保持するクラス
class CameraHudManagerData : public MUserData
{
public:
    CameraHudManagerData();

    // テキスト描画の設定
    MColor                      fColor{ 1.0f, 1.0f, 1.0f, 1.0f };
    float                       fLineWidth{ 2.f };
    MString                     fFontFaceName = "Meiryo";
    MUIDrawManager::LineStyle   fLineStyle{ MUIDrawManager::kSolid };
    unsigned int                fFontFaceIndex{ 0 };
    int                         fTextIncline{ MUIDrawManager::kInclineNormal };
    int                         fTextWeight{ MUIDrawManager::kWeightNormal };
    int                         fTextBoxWidth{ 0 };
    int                         fTextBoxHeight{ 0 };
    MColor                      fTextBoxColor{ 0.0f, 0.0f, 0.0f, 0.3f };

    // レイアウトオフセット
    MPoint  fOffsetPosition{ 20,20,0 };
    MPoint  fMarginPosition{ 0,50,0 };
    MPoint  fMaskOffsetPosition{ 0,0,0 };

    // スタートフレーム
    MString fStartFrame{ "0f" };
    int     fStartFrameNum = 2;
    MPoint  fStartFramePosition{ 0, 0, 0 };
    MUIDrawManager::TextAlignment fStartFrameTextAlignment{ MUIDrawManager::kLeft };

    // エンドフレーム
    MString fEndFrame{ "120f" };
    int     fEndFrameNum = 1;
    MPoint  fEndFramePosition{ 0, 0, 0 };
    MUIDrawManager::TextAlignment fEndFrameTextAlignment{ MUIDrawManager::kLeft };

    // カレントフレーム
    MString fFrame{ "0f" };
    int     fFrameNum = 0;
    MPoint  fFramePosition{ 0, 0, 0 };
    MUIDrawManager::TextAlignment fFrameTextAlignment{ MUIDrawManager::kLeft };

    // カメラ名
    MString fCamera{ "uiDrawManager-Text" };
    int     fCameraNum = 0;
    MPoint  fCameraPosition{ 0, 0, 0 };
    MUIDrawManager::TextAlignment fCameraTextAlignment{ MUIDrawManager::kLeft };

    // 焦点距離
    MString fFocalLength{ "uiDrawManager-Text" };
    int     fFocalLengthNum = 1;
    MPoint  fFocalLengthPosition{ 0, 0, 0 };
    MUIDrawManager::TextAlignment fFocalLengthTextAlignment{ MUIDrawManager::kLeft };

    // カット番号
    MString fCut{ "uiDrawManager-Text" };
    int     fCutLengthNum = 2;
    MPoint  fCutLengthPosition{ 0, 0, 0 };
    MUIDrawManager::TextAlignment fCutLengthTextAlignment{ MUIDrawManager::kLeft };

    // カメラキャッシュ
    MString fCameraCache{ "camera_cache:" };
    int     fCameraCacheNum = 3;
    MPoint  fCameraCachePosition{ 0, 0, 0 };
    MUIDrawManager::TextAlignment fCameraCacheTextAlignment{ MUIDrawManager::kLeft };

    // タイムコード
    MString fTimeCode{ "0:0:0:0" };
    int     fTimeCodeNum = 0;
    MPoint  fTimeCodePosition{ 0, 0, 0 };
    MUIDrawManager::TextAlignment fTimeCodeTextAlignment{ MUIDrawManager::kRight };

    // 日付 (runtime 初期化)
    MString fCurrentDate{ "" };
    int     fCurrentDateNum = 0;
    MPoint  fCurrentDatePosition{ 0, 0, 0 };
    MUIDrawManager::TextAlignment fCurrentDateTextAlignment{ MUIDrawManager::kRight };

    // ユーザーネーム (runtime 初期化)
    MString fUserName{ "" };
    int     fUserNameNum = 1;
    MPoint  fUserNamePosition{ 0, 0, 0 };
    MUIDrawManager::TextAlignment fUserNameTextAlignment{ MUIDrawManager::kRight };

    // シーンネーム (runtime 初期化)
    MString fSceneName{ "" };
    int     fSceneNameNum = 0;
    MPoint  fSceneNamePosition{ 0, 0, 0 };
    MUIDrawManager::TextAlignment fSceneNameTextAlignment{ MUIDrawManager::kCenter };
};