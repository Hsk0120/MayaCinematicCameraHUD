/**
 * @file CameraHudMain.cpp
 * @brief プラグインの入口。ロード時に CameraHud ノードと描画オーバーライドを登録し、アンロード時に解除する。
 */
#include "CameraHudDrawOverride.h"
#include "CameraHudNode.h"

#include <maya/MDrawRegistry.h>
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MStatus.h>
#include <maya/MString.h>

namespace {

/// Plug-in Managerに表示するベンダー名。
const char* const kPluginVendor = "MayaCinematicCameraHUD";
/// プラグインのバージョン。
const char* const kPluginVersion = "1.1.0";
/// 動作に必要なMaya APIのバージョン。特定の版に依存しない。
const char* const kRequiredApiVersion = "Any";

/**
 * @brief 登録・解除の失敗を、失敗した処理の名前と共にスクリプトエディターへ出力する。
 * @param action 失敗した処理の説明。Mayaの表示言語に依らず読めるよう英語で渡す。
 * @param status 失敗を表すMStatus。
 * @return 引数のstatus。呼び出し側がそのまま戻り値に使えるようにする。
 */
MStatus reportFailure(const char* action, const MStatus& status)
{
    MGlobal::displayError(
        MString("MayaCinematicCameraHUD: could not ") + action + ": " + status.errorString());
    return status;
}

/**
 * @brief CameraHud の描画分類に描画オーバーライドの生成関数を結び付ける。
 * @return MDrawRegistry の登録結果。
 */
MStatus registerHudDrawOverride()
{
    return MHWRender::MDrawRegistry::registerDrawOverrideCreator(
        CameraHudNode::kDrawClassification,
        CameraHudNode::kDrawRegistrantId,
        CameraHudDrawOverride::create);
}

/**
 * @brief registerHudDrawOverride で行った登録を取り消す。
 * @return MDrawRegistry の解除結果。
 */
MStatus deregisterHudDrawOverride()
{
    return MHWRender::MDrawRegistry::deregisterDrawOverrideCreator(
        CameraHudNode::kDrawClassification,
        CameraHudNode::kDrawRegistrantId);
}

} // namespace

/**
 * @brief プラグインのロード時にMayaから呼ばれ、ノード型と描画オーバーライドを登録する。
 * @param pluginObject Mayaが渡すプラグインのオブジェクト。
 * @return 両方の登録に成功した場合kSuccess。
 * @note 描画オーバーライドの登録に失敗した場合は、描画されないノード型だけが残らないよう
 *       先に登録したノード型も取り消してから失敗を返す。
 */
MStatus initializePlugin(MObject pluginObject)
{
    MFnPlugin plugin(pluginObject, kPluginVendor, kPluginVersion, kRequiredApiVersion);

    MStatus status = plugin.registerNode(
        CameraHudNode::kTypeName,
        CameraHudNode::kTypeId,
        CameraHudNode::create,
        CameraHudNode::defineAttributes,
        MPxNode::kLocatorNode,
        &CameraHudNode::kDrawClassification);
    if (!status) {
        return reportFailure("register the CameraHud node", status);
    }

    status = registerHudDrawOverride();
    if (!status) {
        plugin.deregisterNode(CameraHudNode::kTypeId);
        return reportFailure("register the Viewport 2.0 draw override", status);
    }
    return MS::kSuccess;
}

/**
 * @brief プラグインのアンロード時にMayaから呼ばれ、登録と逆の順で解除する。
 * @param pluginObject Mayaが渡すプラグインのオブジェクト。
 * @return 両方の解除に成功した場合kSuccess。失敗があれば最初の失敗。
 * @note 一方の解除に失敗しても、もう一方の解除は試みる。
 */
MStatus uninitializePlugin(MObject pluginObject)
{
    MFnPlugin plugin(pluginObject);
    MStatus result = MS::kSuccess;

    const MStatus overrideStatus = deregisterHudDrawOverride();
    if (!overrideStatus) {
        result = reportFailure("deregister the Viewport 2.0 draw override", overrideStatus);
    }

    const MStatus nodeStatus = plugin.deregisterNode(CameraHudNode::kTypeId);
    if (!nodeStatus) {
        const MStatus reported = reportFailure("deregister the CameraHud node", nodeStatus);
        if (result) {
            result = reported;
        }
    }
    return result;
}
