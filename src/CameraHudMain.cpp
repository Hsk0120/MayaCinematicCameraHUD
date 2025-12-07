#include <maya/MFnDagNode.h>
#include <iostream>
#include <string>
#include <maya/MFrameContext.h>
#include <maya/M3dView.h>
#include "CameraHudManager.h"
#include "CameraHudUtility.h"

//MObject CameraHudManager::aText;
MObject CameraHudManager::aTextBoxTransparency;
MObject CameraHudManager::aTextFontSize;

MTypeId CameraHudManager::id(0x0013c801);
MString CameraHudManager::drawDbClassification("drawdb/geometry/CameraHud");
MString CameraHudManager::drawRegistrantId("CameraHudPlugin");

// お作法
CameraHudManager::CameraHudManager()
{

};

CameraHudManager::~CameraHudManager()
{

};

void* CameraHudManager::creator()
{
    return new CameraHudManager();
};

MStringArray CameraHudManagerData::fFontList;

CameraHudManagerData::CameraHudManagerData()
{

};

CameraHudDrawOverride::CameraHudDrawOverride(
    const MObject& ownerCameraHudNode)
    : MPxDrawOverride(ownerCameraHudNode, nullptr)
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
        //タイムコードをセット
        data->fTimeCode = get_current_timecode();

        //カレントフレームをセット
        data->fFrame = get_frame("Current : ", get_current_frame());

        //スタートフレームをセット
        data->fStartFrame = get_frame("Start : ", get_start_frame());

        //エンドフレームをセット
        data->fEndFrame = get_frame("End : ", get_end_frame());

        //アトリビュートテキストをセット
        //data->fText = MPlug(CameraHudNode, CameraHudManager::aText).asString();

        // カメラネームを取得してセット
        data->fCamera = get_camera_name(MFnDagNode(frameContext.getCurrentCameraPath(), &status).name().asChar());

        // カメラの焦点距離を取得してセット
        data->fFocalLength = get_camera_focalLength(MFnCamera(MFnDagNode(frameContext.getCurrentCameraPath(), &status).object()).focalLength());


        // カメラのキャッシュ状況を取得してセット
        bool camera_cache_bool = MFnDagNode(frameContext.getCurrentCameraPath(), &status).findPlug(MString("camera_cache")).asBool();
        //bool camera_cache_bool = true;
        data->fCameraCache = get_camera_cache(camera_cache_bool);

        // カットナンバーを取得してセット
        data->fCut = get_cut_num(frameContext.getCurrentCameraPath());

        // テキストサイズ取得
        MPlug plugTextFontSize(CameraHudNode, CameraHudManager::aTextFontSize);
        data->fTextFontSize = plugTextFontSize.asFloat();

        // テキストボックスの透明度取得
        MPlug plugTextBoxTransparency(CameraHudNode, CameraHudManager::aTextBoxTransparency);
        data->fTextBoxColor.a = 1.0f - plugTextBoxTransparency.asFloat();


        // ビューポートサイズ取得
        int a, b, width, height;
        frameContext.getViewportDimensions(a, b, width, height);



        // カメラのオーバースキャンを取得して、ゲートマスクオフセットの計算
        double overscan = MFnCamera(MFnDagNode(frameContext.getCurrentCameraPath(), &status).object()).overscan();
        if(overscan != 1){
            MString filmFit = getFilmFitAsString(MFnCamera(MFnDagNode(frameContext.getCurrentCameraPath(), &status).object()).filmFit());
            if (filmFit == "fill") {
                data->fMaskOffsetPosition = get_camera_resolution_fill(overscan, width, height);
            }
            else {
                data->fMaskOffsetPosition = get_camera_resolution_horizontal(overscan, width, height);
            }

        }
        else{
            data->fMaskOffsetPosition = MPoint(0,0,0);
        }

        // ビューポートサイズからフォントサイズを変更
        data->fTextFontSize = data->fTextFontSize * (height - data->fMaskOffsetPosition.y * 2) * 0.05 * 0.7;
        data->fMarginPosition = MPoint(data->fTextFontSize*0.1, (height - data->fMaskOffsetPosition.y * 2) * 0.08 * 0.7, 0);
        data->fOffsetPosition = MPoint(width * 0.01, (height - data->fMaskOffsetPosition.y * 2) * 0.005 * 0.7, 0);

        // 各テキストのポジションを設定
        //data->fPosition             = get_viewport_centerTop(data->fNum, width, height, data->fTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);
        
        data->fStartFramePosition   = get_viewport_leftBottom(data->fStartFrameNum, width, height, data->fTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);
        data->fEndFramePosition     = get_viewport_leftBottom(data->fEndFrameNum, width, height, data->fTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);
        data->fFramePosition        = get_viewport_leftBottom(data->fFrameNum, width, height, data->fTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);
        
        data->fCameraPosition       = get_viewport_leftTop(data->fCameraNum, width, height, data->fTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);
        data->fFocalLengthPosition  = get_viewport_leftTop(data->fFocalLengthNum, width, height, data->fTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);
        data->fCutLengthPosition    = get_viewport_leftTop(data->fCutLengthNum, width, height, data->fTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);
        data->fCameraCachePosition  = get_viewport_leftTop(data->fCameraCacheNum, width, height, data->fTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);


        data->fTimeCodePosition     = get_viewport_rightBottom(data->fTimeCodeNum, width, height, data->fTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);
        
        data->fCurrentDatePosition  = get_viewport_rightTop(data->fCurrentDateNum, width, height, data->fTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);
        data->fUserNamePosition     = get_viewport_rightTop(data->fUserNameNum, width, height, data->fTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);

        data->fSceneNamePosition    = get_viewport_centerBottom(data->fSceneNameNum, width, height, data->fTextFontSize, data->fOffsetPosition, data->fMarginPosition, data->fMaskOffsetPosition);
    
    };
    return data;
};

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

    // UI表示処理開始
    drawManager.beginDrawable();

    // テスト　デプス確認
    drawManager.setDepthPriority(100);


    // テキストのスタイルをセット
    drawManager.setColor(thisdata->fColor);
    drawManager.setFontSize(thisdata->fTextFontSize);
    drawManager.setFontIncline(thisdata->fTextIncline);
    drawManager.setFontWeight(thisdata->fTextWeight);
    drawManager.setFontName(thisdata->fFontFaceName);
    int boxSize[] = { thisdata->fTextBoxWidth, thisdata->fTextBoxHeight };

    // アトリビュートテキスト表示
    //drawManager.text2d(thisdata->fPosition, thisdata->fText, thisdata->fTextAlignment,
    //    boxSize[0] + boxSize[1] == 0 ? NULL : boxSize, &thisdata->fTextBoxColor, false);
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

// ノードのアトリビュート登録処理
MStatus CameraHudManager::initialize()
{
    MStatus status;
    MFnNumericAttribute nAttr;
    MFnEnumAttribute eAttr;
    MFnTypedAttribute typedAttr;

    // Add text attributes.
    //aText = typedAttr.create("text", "t", MFnData::kString, MFnStringData().create("Text"));
    //MPxNode::addAttribute(aText);

    aTextFontSize = nAttr.create("textFontSize", "tfs", MFnNumericData::kFloat,     1.0);
    nAttr.setMin(-1);
    nAttr.setMax(99);
    MPxNode::addAttribute(aTextFontSize);

    aTextBoxTransparency = nAttr.create("textBoxTransparency", "tbt", MFnNumericData::kFloat, 0.3);
    nAttr.setSoftMin(0.0);
    nAttr.setSoftMax(1.0);
    MPxNode::addAttribute(aTextBoxTransparency);

    return MS::kSuccess;
};

// プラグインノードを登録
MStatus initializePlugin(MObject obj) 
{
    MStatus   status;
    MFnPlugin plugin(obj, "Tool", "1.0", "Any");

    status = plugin.registerNode(
        "CameraHud",
        CameraHudManager::id,
        &CameraHudManager::creator,
        &CameraHudManager::initialize,
        MPxNode::kLocatorNode,
        &CameraHudManager::drawDbClassification);
    if (!status) {
        status.perror("registerNode");
        return status;
    }

    status = MHWRender::MDrawRegistry::registerDrawOverrideCreator(
        CameraHudManager::drawDbClassification,
        CameraHudManager::drawRegistrantId,
        CameraHudDrawOverride::createCameraHud);
    if (!status) {
        status.perror("registerDrawOverrideCreator");
        return status;
    }

    return status;
};

//  プラグインノードを解除
MStatus uninitializePlugin(MObject obj)  
{
    MStatus   status;
    MFnPlugin plugin(obj);

    status = MDrawRegistry::deregisterGeometryOverrideCreator(
        CameraHudManager::drawDbClassification,
        CameraHudManager::drawRegistrantId);
    if (!status) {
        status.perror("deregisterGeometryOverrideCreator");
        return status;
    }

    status = plugin.deregisterNode(CameraHudManager::id);
    if (!status) {
        status.perror("deregisterNode");
        return status;
    }

    return status;
};
