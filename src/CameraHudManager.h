#pragma once
#include <maya/MPxLocatorNode.h>
#include <maya/MFnPlugin.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnStringData.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MPointArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MUintArray.h>
#include <maya/MDrawRegistry.h>
#include <maya/MPxDrawOverride.h>
#include <maya/MUserData.h>
#include <maya/MUIDrawManager.h>
#include <algorithm>
#include "CameraHudUtility.h"


// ノードを作るお作法
class CameraHudManager : public MPxLocatorNode
 {
public:
    CameraHudManager();
    ~CameraHudManager() override;

    static void* creator(); //登録
    static MStatus initialize(); //設定

    virtual bool excludeAsLocator() const
    {
        return false;
    }

    static MObject aText;
    static MObject aTextBoxTransparency;
    static MObject aTextFontSize;

public:
    static MTypeId id;
    static MString drawDbClassification;
    static MString drawRegistrantId;
 };

// uiの情報をまとめているデータ
class CameraHudManagerData : public MUserData
 {
public:
    CameraHudManagerData();

    // テキスト情報の初期値
    MColor                      fColor{ 1.0f, 1.0f, 1.0f, 1.0f };
    float                       fLineWidth{ 2.f };
    MString                     fFontFaceName = "Meiryo";
    MUIDrawManager::LineStyle   fLineStyle{ MUIDrawManager::kSolid };
    float                       fTextFontSize{ 1.0 };
    unsigned int                fFontFaceIndex{ 0 };
    static MStringArray         fFontList;
    int                         fTextIncline{ MUIDrawManager::kInclineNormal };
    int                         fTextWeight{ MUIDrawManager::kWeightNormal };
    int                         fTextBoxWidth{ 0 };
    int                         fTextBoxHeight{ 0 };
    MColor                      fTextBoxColor{ 0.0f, 0.0f, 0.0f, 0.3f };

    // オフセットマージン
    MPoint  fOffsetPosition{ 20,20,0 };
    MPoint  fMarginPosition{ 0,50,0 };
    MPoint  fMaskOffsetPosition{ 0,0,0 };


    // アトリビュートテキスト
    //MString fText{ "uiDrawManager-Text" };
    //int     fNum = 0;
    //MPoint  fPosition{ 0, 0, 0 };
    //MUIDrawManager::TextAlignment fTextAlignment{ MUIDrawManager::kCenter };


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


    // カメラネーム
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


    //日付
    MString fCurrentDate{ get_current_date().data() };
    int     fCurrentDateNum = 0;
    MPoint  fCurrentDatePosition{ 0, 0, 0 };
    MUIDrawManager::TextAlignment fCurrentDateTextAlignment{ MUIDrawManager::kRight };

    //ユーザーネーム
    MString fUserName{ get_username().data() };
    int     fUserNameNum = 1;
    MPoint  fUserNamePosition{ 0, 0, 0 };
    MUIDrawManager::TextAlignment fUserNameTextAlignment{ MUIDrawManager::kRight };



    //シーンネーム
    MString fSceneName = get_scene_name();
    int     fSceneNameNum = 0;
    MPoint  fSceneNamePosition{ 0, 0, 0 };
    MUIDrawManager::TextAlignment fSceneNameTextAlignment{ MUIDrawManager::kCenter };

};

// カメラ用 HUD を描画するためのクラス。
class CameraHudDrawOverride : public MHWRender::MPxDrawOverride
{
public:
	// CameraHudDrawOverride クラスのインスタンスを生成
    static MHWRender::MPxDrawOverride* createCameraHud(const MObject& ownerCameraHudNode);

    // 対応する描画 API（OpenGL / DirectX 等）を返す
    MHWRender::DrawAPI supportedDrawAPIs() const override;

    // バウンディングボックスを持たない（HUD は常に描画する）
    bool isBounded(
        const MDagPath& objPath,
        const MDagPath& cameraPath) const override;

    // 計算結果や状態を MUserData にキャッシュ
    MUserData* prepareForDraw(
        const MDagPath& objPath,
        const MDagPath& cameraPath,
        const MFrameContext& frameContext,
        MUserData* oldData) override;

    // このクラスが UI 描画（2D テキストなど）を行う True。
    bool hasUIDrawables() const override;

    // 実際の UI 要素（HUD テキストやラインなど）を追加するメソッド。MUIDrawManagerを使用して描画指示を行う
    void addUIDrawables(
        const MDagPath& objPath,
        MHWRender::MUIDrawManager& drawManager,
        const MHWRender::MFrameContext& frameContext,
        const MUserData* data) override;

private:
    // 外部から newさせないためにコンストラクタは private化
    CameraHudDrawOverride(const MObject& ownerCameraHudNode);
};