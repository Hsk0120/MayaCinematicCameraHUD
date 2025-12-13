#include <maya/MFnPlugin.h>
#include <maya/MDrawRegistry.h>
#include <maya/MStatus.h>
#include <maya/MObject.h>
#include "CameraHudNode.h"
#include "CameraHudDrawOverride.h"
#include "CameraHudManager.h"

// プラグインノードを登録
MStatus initializePlugin(MObject obj) 
{
    MStatus   status;
    MFnPlugin plugin(obj, "Tool", "1.0", "Any");

    status = plugin.registerNode(
        "CameraHud",
        CameraHudNode::id,
        &CameraHudNode::creator,
        &CameraHudNode::initialize,
        MPxNode::kLocatorNode,
        &CameraHudNode::drawDbClassification);
    if (!status) {
        status.perror("registerNode");
        return status;
    }

    status = MHWRender::MDrawRegistry::registerDrawOverrideCreator(
        CameraHudNode::drawDbClassification,
        CameraHudNode::drawRegistrantId,
        CameraHudDrawOverride::createCameraHud);
    if (!status) {
        status.perror("registerDrawOverrideCreator");
        return status;
    }

    return status;
}

//  プラグインノードを解除
MStatus uninitializePlugin(MObject obj)  
{
    MStatus   status;
    MFnPlugin plugin(obj);

    status = MDrawRegistry::deregisterGeometryOverrideCreator(
        CameraHudNode::drawDbClassification,
        CameraHudNode::drawRegistrantId);
    if (!status) {
        status.perror("deregisterGeometryOverrideCreator");
        return status;
    }

    status = plugin.deregisterNode(CameraHudNode::id);
    if (!status) {
        status.perror("deregisterNode");
        return status;
    }

    return status;
};
