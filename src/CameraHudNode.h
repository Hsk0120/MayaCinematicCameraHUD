#pragma once

#include <maya/MPxLocatorNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MStatus.h>
#include <maya/MObject.h>
#include <maya/MString.h>

// CameraHud ノード (ヘッダ)
class CameraHudNode : public MPxLocatorNode
{
public:
    static void* creator();           // ノード生成関数
    static MStatus initialize();      // アトリビュート登録

    static MObject aText;
    static MObject aTextBoxTransparency;
    static MObject aTextFontSize;

public:
    static MTypeId id;
    static MString drawDbClassification;
    static MString drawRegistrantId;
};
