#pragma once

#include <maya/MPxDrawOverride.h>
#include <maya/MUserData.h>
#include <maya/MUIDrawManager.h>
#include <maya/MFrameContext.h>
#include <maya/MDagPath.h>

// CameraHud の描画オーバーライドクラス
class CameraHudDrawOverride : public MHWRender::MPxDrawOverride
{
public:
    static MHWRender::MPxDrawOverride* createCameraHud(const MObject& ownerCameraHudNode);

    MHWRender::DrawAPI supportedDrawAPIs() const override;
    bool isBounded(const MDagPath& objPath, const MDagPath& cameraPath) const override;
    MUserData* prepareForDraw(const MDagPath& objPath, const MDagPath& cameraPath, const MFrameContext& frameContext, MUserData* oldData) override;
    bool hasUIDrawables() const override;
    void addUIDrawables(const MDagPath& objPath, MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext, const MUserData* data) override;

private:
    CameraHudDrawOverride(const MObject& ownerCameraHudNode);
    float mTextFontSize{ 0.f };
};
