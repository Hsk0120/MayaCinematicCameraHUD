#include "CameraHudNode.h"

MObject CameraHudNode::aText;
MObject CameraHudNode::aTextBoxTransparency;
MObject CameraHudNode::aTextFontSize;

MTypeId CameraHudNode::id(0x0013c801);
MString CameraHudNode::drawDbClassification("drawdb/geometry/CameraHud");
MString CameraHudNode::drawRegistrantId("CameraHudPlugin");

void* CameraHudNode::creator()
{
    return new CameraHudNode();
}

MStatus CameraHudNode::initialize()
{
    MStatus status;
    MFnNumericAttribute nAttr;

    aTextFontSize = nAttr.create("textFontSize", "tfs", MFnNumericData::kFloat,     1.0);
    nAttr.setMin(-1);
    nAttr.setMax(99);
    MPxNode::addAttribute(aTextFontSize);

    aTextBoxTransparency = nAttr.create("textBoxTransparency", "tbt", MFnNumericData::kFloat, 0.3);
    nAttr.setSoftMin(0.0);
    nAttr.setSoftMax(1.0);
    MPxNode::addAttribute(aTextBoxTransparency);

    return MS::kSuccess;
}
