#include "CameraHudDrawOverride.h"
#include "CameraHudManager.h"
#include "CameraHudUtility.h"
#include "CameraHudNode.h"

#include <maya/MFnDagNode.h>
#include <maya/MFnCamera.h>
#include <maya/MPlug.h>
#include <maya/MGlobal.h>

CameraHudDrawOverride::CameraHudDrawOverride(
    const MObject& ownerCameraHudNode)
    : MPxDrawOverride(ownerCameraHudNode, nullptr),
      mTextFontSize(static_cast<float>(CameraHudUtility::getDefaultFontSize()))
{
}

MHWRender::MPxDrawOverride* CameraHudDrawOverride::createCameraHud(
    const MObject& ownerCameraHudNode)
{
    return new CameraHudDrawOverride(ownerCameraHudNode);
}

MHWRender::DrawAPI CameraHudDrawOverride::supportedDrawAPIs() const
{
    return (MHWRender::kOpenGL | MHWRender::kDirectX11 | MHWRender::kOpenGLCoreProfile);
}

bool CameraHudDrawOverride::isBounded(
    const MDagPath&, 
    const MDagPath&) const
{
    return false;
}

MUserData* CameraHudDrawOverride::prepareForDraw(
    const MDagPath& objPath, 
    const MDagPath& cameraPath, 
    const MHWRender::MFrameContext& frameContext, 
    MUserData* oldData)
{
    CameraHudManagerData* data = dynamic_cast<CameraHudManagerData*>(oldData);
    if (!data) {
        data = new CameraHudManagerData();
    };

    MStatus status;
    MObject CameraHudNode = objPath.node(&status);
    if (status) {
        // タイムコードをセット
        data->fTimeCode = CameraHudUtility::getCurrentTimecode();

        // カレントフレームをセット
        data->fFrame = CameraHudUtility::formatFrameString("Current : ", CameraHudUtility::getCurrentFrame());

        // スタートフレームをセット
        data->fStartFrame = CameraHudUtility::formatFrameString("Start : ", CameraHudUtility::getStartFrame());

        // エンドフレームをセット
        data->fEndFrame = CameraHudUtility::formatFrameString("End : ", CameraHudUtility::getEndFrame());

        // カメラネームを取得してセット
        MFnDagNode camDag(frameContext.getCurrentCameraPath(), &status);
        data->fCamera = CameraHudUtility::getCameraNameFromString(std::string(camDag.name().asChar()));

        // カメラの焦点距離を取得してセット
        double focalLen = MFnCamera(camDag.object()).focalLength();
        data->fFocalLength = CameraHudUtility::formatFocalLength(focalLen);

        // カメラのキャッシュ状況を取得してセット
        bool camera_cache_bool = camDag.findPlug(MString("camera_cache")).asBool();
        data->fCameraCache = CameraHudUtility::cameraCacheToString(camera_cache_bool);

        // カットナンバーを取得してセット
        data->fCut = CameraHudUtility::getCutNumber(frameContext.getCurrentCameraPath());

        // テキストサイズ取得
        MPlug plugTextFontSize(CameraHudNode, CameraHudNode::aTextFontSize);
        mTextFontSize = plugTextFontSize.asFloat();

        // テキストボックスの透明度取得
        MPlug plugTextBoxTransparency(CameraHudNode, CameraHudNode::aTextBoxTransparency);
        data->fTextBoxColor.a = 1.0f - plugTextBoxTransparency.asFloat();

        // ビューポートサイズ取得
        int a, b, width, height;
        frameContext.getViewportDimensions(a, b, width, height);

        // カメラのオーバースキャンを取得し、ゲートマスクオフsetの計算
        double overscan = MFnCamera(camDag.object()).overscan();
        if (overscan != 1) {
            MString filmFit = CameraHudUtility::filmFitToString(MFnCamera(camDag.object()).filmFit());
            if (filmFit == "fill") {
                data->fMaskOffsetPosition = CameraHudUtility::computeCameraResolutionFill(overscan, width, height);
            }
            else {
                data->fMaskOffsetPosition = CameraHudUtility::computeCameraResolutionHorizontal(overscan, width, height);
            }

        }
        else {
            data->fMaskOffsetPosition = MPoint(0,0,0);
        }

        // ビューポートサイズからフォントサイズでレイアウト調整
        data->fMarginPosition = MPoint(mTextFontSize*0.1, (height - data->fMaskOffsetPosition.y * 2) * 0.08 * 0.7, 0);
        data->fOffsetPosition = MPoint(width * 0.01, (height - data->fMaskOffsetPosition.y * 2) * 0.005 * 0.7, 0);

        data->fStartFramePosition   = CameraHudUtility::computeViewportLeftBottom(data->fStartFrameNum, width, height, mTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);
        data->fEndFramePosition     = CameraHudUtility::computeViewportLeftBottom(data->fEndFrameNum, width, height, mTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);
        data->fFramePosition        = CameraHudUtility::computeViewportLeftBottom(data->fFrameNum, width, height, mTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);
        
        data->fCameraPosition       = CameraHudUtility::computeViewportLeftTop(data->fCameraNum, width, height, mTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);
        data->fFocalLengthPosition  = CameraHudUtility::computeViewportLeftTop(data->fFocalLengthNum, width, height, mTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);
        data->fCutLengthPosition    = CameraHudUtility::computeViewportLeftTop(data->fCutLengthNum, width, height, mTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);
        data->fCameraCachePosition  = CameraHudUtility::computeViewportLeftTop(data->fCameraCacheNum, width, height, mTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);

        data->fTimeCodePosition     = CameraHudUtility::computeViewportRightBottom(data->fTimeCodeNum, width, height, mTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);
        
        data->fCurrentDatePosition  = CameraHudUtility::computeViewportRightTop(data->fCurrentDateNum, width, height, mTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);
        data->fUserNamePosition     = CameraHudUtility::computeViewportRightTop(data->fUserNameNum, width, height, mTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);

        data->fSceneNamePosition    = CameraHudUtility::computeViewportCenterBottom(data->fSceneNameNum, width, height, mTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);
    };
    return data;
}

bool CameraHudDrawOverride::hasUIDrawables() const 
{ 
    return true; 
}

void CameraHudDrawOverride::addUIDrawables(
    const MDagPath&, 
    MHWRender::MUIDrawManager& drawManager, 
    const MHWRender::MFrameContext&, 
    const MUserData* data)
{
    const CameraHudManagerData* thisdata = dynamic_cast<const CameraHudManagerData*>(data);
    if (!thisdata) {
        return;
    }

    // UI描画開始
    drawManager.beginDrawable();

    // テキストデプス優先度
    drawManager.setDepthPriority(100);

    // テキストのスタイルをセット
    drawManager.setColor(thisdata->fColor);
    drawManager.setFontSize(mTextFontSize);
    drawManager.setFontIncline(thisdata->fTextIncline);
    drawManager.setFontWeight(thisdata->fTextWeight);
    drawManager.setFontName(thisdata->fFontFaceName);
    int boxSize[] = { thisdata->fTextBoxWidth, thisdata->fTextBoxHeight };

    // カメラ名表示
    drawManager.text2d(thisdata->fCameraPosition, thisdata->fCamera, thisdata->fCameraTextAlignment,
        boxSize[0] + boxSize[1] == 0 ? NULL : boxSize, &thisdata->fTextBoxColor, false);
    // 焦点距離表示
    drawManager.text2d(thisdata->fFocalLengthPosition, thisdata->fFocalLength, thisdata->fFocalLengthTextAlignment,
        boxSize[0] + boxSize[1] == 0 ? NULL : boxSize, &thisdata->fTextBoxColor, false);
    // カットナンバーを表示
    drawManager.text2d(thisdata->fCutLengthPosition, thisdata->fCut, thisdata->fCutLengthTextAlignment,
        boxSize[0] + boxSize[1] == 0 ? NULL : boxSize, &thisdata->fTextBoxColor, false);
    // カメラキャッシュ表示
    drawManager.text2d(thisdata->fCameraCachePosition, thisdata->fCameraCache, thisdata->fCameraCacheTextAlignment,
        boxSize[0] + boxSize[1] == 0 ? NULL : boxSize, &thisdata->fTextBoxColor, false);
    // タイムコード表示
    drawManager.text2d(thisdata->fTimeCodePosition, thisdata->fTimeCode, thisdata->fTimeCodeTextAlignment,
        boxSize[0] + boxSize[1] == 0 ? NULL : boxSize, &thisdata->fTextBoxColor, false);
    // カレントフレーム表示
    drawManager.text2d(thisdata->fFramePosition, thisdata->fFrame, thisdata->fFrameTextAlignment,
        boxSize[0] + boxSize[1] == 0 ? NULL : boxSize, &thisdata->fTextBoxColor, false);
    //  スタートフレーム表示
    drawManager.text2d(thisdata->fStartFramePosition, thisdata->fStartFrame, thisdata->fStartFrameTextAlignment,
        boxSize[0] + boxSize[1] == 0 ? NULL : boxSize, &thisdata->fTextBoxColor, false);
    // エンドフレーム表示
    drawManager.text2d(thisdata->fEndFramePosition, thisdata->fEndFrame, thisdata->fEndFrameTextAlignment,
        boxSize[0] + boxSize[1] == 0 ? NULL : boxSize, &thisdata->fTextBoxColor, false);
    // ユーザー名表示
    drawManager.text2d(thisdata->fUserNamePosition, thisdata->fUserName, thisdata->fUserNameTextAlignment,
        boxSize[0] + boxSize[1] == 0 ? NULL : boxSize, &thisdata->fTextBoxColor, false);
    // シーン名表示
    drawManager.text2d(thisdata->fSceneNamePosition, thisdata->fSceneName, thisdata->fSceneNameTextAlignment,
        boxSize[0] + boxSize[1] == 0 ? NULL : boxSize, &thisdata->fTextBoxColor, false);
    // 今日の日付を表示
    drawManager.text2d(thisdata->fCurrentDatePosition, thisdata->fCurrentDate, thisdata->fCurrentDateTextAlignment,
        boxSize[0] + boxSize[1] == 0 ? NULL : boxSize, &thisdata->fTextBoxColor, false);

    drawManager.endDrawable();
}
