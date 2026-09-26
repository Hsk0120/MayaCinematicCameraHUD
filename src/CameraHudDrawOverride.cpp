/**
 * @file CameraHudDrawOverride.cpp
 * @brief CameraHudDrawOverride の実装。表示行の収集、画面上の配置、2Dテキストの描画を行う。
 */
#include "CameraHudDrawOverride.h"
#include "CameraHudDrawData.h"
#include "CameraHudNode.h"
#include "CameraHudUtility.h"

#include <maya/MColor.h>
#include <maya/MFnCamera.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>
#include <maya/MPoint.h>
#include <maya/MStatus.h>
#include <maya/MString.h>

#include <algorithm>
#include <string>
#include <vector>

using MHWRender::MUIDrawManager;

namespace {

/// 文字色(不透明の白)。
const MColor kTextColor(1.0f, 1.0f, 1.0f, 1.0f);
/// 書体。シーン名やユーザー名に含まれる日本語も表示できるものを使う。
const char* const kFontName = "Meiryo";
/// 深度の優先度。シーン内の他の描画物より手前に出すため大きめの値にする。
const unsigned int kDepthPriority = 100;

// 以下の比率はHUDの見た目(余白と行間)を決める値。
// 上下方向は「高さ × 比率 × kVerticalScale」の順に掛け、従来の表示位置とビット単位で一致させている。
/// 左右端からの余白。ビューポート幅に対する比率。
const double kSideOffsetPerWidth = 0.01;
/// 左右端からの追加の余白。文字サイズに対する比率。
const double kSidePaddingPerFontSize = 0.1;
/// 上下端からの余白。ゲートマスクを除いた高さに対する比率(縮尺を掛ける前)。
const double kEdgeOffsetPerHeight = 0.005;
/// 行と行の間隔。ゲートマスクを除いた高さに対する比率(縮尺を掛ける前)。
const double kLineGapPerHeight = 0.08;
/// 上下方向の余白と行間にまとめて掛ける縮尺。
const double kVerticalScale = 0.7;

/**
 * @brief 行を積み上げる基準の位置。上側は上端から下へ、下側は下端から上へ並べる。
 */
enum class HudCorner
{
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    BottomCenter,
};

/**
 * @brief 画面座標が決まる前の表示行。
 */
struct HudEntry
{
    HudCorner corner;  ///< 基準の位置。
    int row;           ///< 基準の位置から数えた行番号。0が画面の端に最も近い。
    MString text;      ///< 表示する文字列。
};

/**
 * @brief CameraHud ノードから読んだ表示設定。
 */
struct HudSettings
{
    double fontSize;         ///< 文字サイズ(ピクセル)。
    double boxTransparency;  ///< 文字背景の透明度(0〜1)。
};

/**
 * @brief 基準の位置に応じた文字列の揃え方を返す。
 * @param corner 行を積み上げる基準の位置。
 * @return 左側はkLeft、右側はkRight、中央はkCenter。
 */
MUIDrawManager::TextAlignment alignmentOf(HudCorner corner)
{
    switch (corner) {
    case HudCorner::TopLeft:
    case HudCorner::BottomLeft:
        return MUIDrawManager::kLeft;
    case HudCorner::TopRight:
    case HudCorner::BottomRight:
        return MUIDrawManager::kRight;
    case HudCorner::BottomCenter:
        break;
    }
    return MUIDrawManager::kCenter;
}

/**
 * @brief 基準の位置がビューポートの上側かどうかを返す。
 * @param corner 行を積み上げる基準の位置。
 * @return 上側(TopLeft / TopRight)ならtrue。
 */
bool isTopCorner(HudCorner corner)
{
    return corner == HudCorner::TopLeft || corner == HudCorner::TopRight;
}

/**
 * @brief ビューポートの寸法・文字サイズ・ゲートマスクから、各行の画面座標を求める。
 */
class HudLayout
{
public:
    /**
     * @brief 余白と1行の送り量を計算する。
     * @param width ビューポートの幅(ピクセル)。
     * @param height ビューポートの高さ(ピクセル)。
     * @param fontSize 文字サイズ(ピクセル)。
     * @param gateMask ゲートの外側に残るマスクの片側分の幅(x)と高さ(y)。ピクセル単位。
     */
    HudLayout(int width, int height, double fontSize, const MPoint& gateMask)
        : width_(width)
        , height_(height)
        , gateMask_(gateMask)
    {
        const double gateHeight = height - gateMask.y * 2.0;
        sideOffset_ = width * kSideOffsetPerWidth;
        sidePadding_ = fontSize * kSidePaddingPerFontSize;
        edgeOffset_ = gateHeight * kEdgeOffsetPerHeight * kVerticalScale;
        lineStep_ = gateHeight * kLineGapPerHeight * kVerticalScale + fontSize;
    }

    /**
     * @brief 基準の位置と行番号から、行を描く画面座標を返す。
     * @param corner 行を積み上げる基準の位置。
     * @param row 基準の位置から数えた行番号(0始まり)。
     * @return ビューポート左下を原点とするピクセル座標。
     * @note 加減算の順序は従来の計算と同じにしてあり、並べ替えると丸め誤差で位置が変わることがある。
     */
    MPoint positionOf(HudCorner corner, int row) const
    {
        double x = 0.0;
        switch (alignmentOf(corner)) {
        case MUIDrawManager::kLeft:
            x = sideOffset_ + sidePadding_ + gateMask_.x;
            break;
        case MUIDrawManager::kRight:
            x = width_ - sideOffset_ - sidePadding_ - gateMask_.x;
            break;
        default:
            // 中央は整数の半分(奇数幅では切り捨て)
            x = static_cast<double>(width_ / 2);
            break;
        }

        // 上側は1行目も上端から1行分下げた位置に置く
        const double y = isTopCorner(corner)
            ? height_ - gateMask_.y - edgeOffset_ - lineStep_ * (row + 1)
            : edgeOffset_ + gateMask_.y + lineStep_ * row;
        return MPoint(x, y, 0.0);
    }

private:
    int width_;            ///< ビューポートの幅。
    int height_;           ///< ビューポートの高さ。
    MPoint gateMask_;      ///< ゲートの外側に残るマスクの片側分の幅(x)と高さ(y)。
    double sideOffset_;    ///< 左右端からの余白のうち、ビューポート幅に比例する分。
    double sidePadding_;   ///< 左右端からの余白のうち、文字サイズに比例する分。
    double edgeOffset_;    ///< 上下端(マスクの内側)から最も端の行までの余白。
    double lineStep_;      ///< 1行ごとの送り量。
};

/**
 * @brief CameraHud ノードから表示設定を読む。
 * @param hudPath CameraHud ノードのパス。
 * @param[out] settings 読み取った設定。文字サイズは1以上、透明度は0〜1に収める。
 * @return 読み取れた場合true。ノードまたはプラグを取得できなかった場合false。
 */
bool readHudSettings(const MDagPath& hudPath, HudSettings& settings)
{
    MStatus status;
    const MObject hudNode = hudPath.node(&status);
    if (!status) {
        return false;
    }
    const MFnDependencyNode nodeFn(hudNode, &status);
    if (!status) {
        return false;
    }
    const MPlug fontSizePlug = nodeFn.findPlug(CameraHudNode::fontSizeAttr, false, &status);
    if (!status) {
        return false;
    }
    const MPlug transparencyPlug = nodeFn.findPlug(CameraHudNode::boxTransparencyAttr, false, &status);
    if (!status) {
        return false;
    }

    settings.fontSize = std::max(1.0, static_cast<double>(fontSizePlug.asFloat()));
    settings.boxTransparency = std::min(1.0, std::max(0.0, static_cast<double>(transparencyPlug.asFloat())));
    return true;
}

/**
 * @brief オーバースキャン時にゲートの外側へ残るマスクの大きさを求める。
 * @param cameraFn ビューポートのカメラ。
 * @param width ビューポートの幅(ピクセル)。
 * @param height ビューポートの高さ(ピクセル)。
 * @return マスクの片側分の幅(x)と高さ(y)。オーバースキャンが1ならすべて0。
 */
MPoint computeGateMask(const MFnCamera& cameraFn, int width, int height)
{
    const double overscan = cameraFn.overscan();
    if (overscan == 1.0) {
        return MPoint(0.0, 0.0, 0.0);
    }
    // Fill以外(Horizontal / Vertical / Overscan)はHorizontalと同じ計算を使う
    if (cameraFn.filmFit() == MFnCamera::kFillFilmFit) {
        return CameraHudUtility::computeCameraResolutionFill(overscan, width, height);
    }
    return CameraHudUtility::computeCameraResolutionHorizontal(overscan, width, height);
}

/**
 * @brief カメラのカスタムアトリビュート camera_cache の値を読む。
 * @param cameraFn ビューポートのカメラ。
 * @return アトリビュートの値。アトリビュートが無い場合はfalse。
 */
bool readCameraCacheFlag(const MFnCamera& cameraFn)
{
    MStatus status;
    const MPlug cachePlug = cameraFn.findPlug("camera_cache", false, &status);
    if (!status) {
        return false;
    }
    return cachePlug.asBool();
}

/**
 * @brief 左上に積むカメラ関連の行(カメラ名・焦点距離・カット番号・キャッシュ状態)を追加する。
 * @param cameraFn ビューポートのカメラ。
 * @param cameraPath ビューポートのカメラのパス。カット番号をネームスペースから求めるのに使う。
 * @param[in,out] entries 行の追加先。
 */
void appendCameraEntries(const MFnCamera& cameraFn, const MDagPath& cameraPath, std::vector<HudEntry>& entries)
{
    const std::string cameraName(cameraFn.name().asChar());
    entries.push_back({ HudCorner::TopLeft, 0, CameraHudUtility::getCameraNameFromString(cameraName) });
    entries.push_back({ HudCorner::TopLeft, 1, CameraHudUtility::formatFocalLength(cameraFn.focalLength()) });
    entries.push_back({ HudCorner::TopLeft, 2, CameraHudUtility::getCutNumber(cameraPath) });
    entries.push_back({ HudCorner::TopLeft, 3, CameraHudUtility::cameraCacheToString(readCameraCacheFlag(cameraFn)) });
}

/**
 * @brief カメラに依存しない行(タイムコード・フレーム範囲・ユーザー名・シーン名・日付)を追加する。
 * @param userName 右上に出すユーザー名。
 * @param[in,out] entries 行の追加先。
 * @note 追加順がそのまま描画順になる。Maya 2022では描画順によって文字の縁の
 *       ピクセルがわずかに変わることを確認したため、従来の描画順を保っている。
 */
void appendSceneEntries(const MString& userName, std::vector<HudEntry>& entries)
{
    using namespace CameraHudUtility;

    entries.push_back({ HudCorner::BottomRight, 0, getCurrentTimecode() });

    // 左下は下から現在・終了・開始の行番号を振るので、画面上では開始・終了・現在の順に並ぶ
    entries.push_back({ HudCorner::BottomLeft, 0, formatFrameString("Current : ", getCurrentFrame()) });
    entries.push_back({ HudCorner::BottomLeft, 2, formatFrameString("Start : ", getStartFrame()) });
    entries.push_back({ HudCorner::BottomLeft, 1, formatFrameString("End : ", getEndFrame()) });

    entries.push_back({ HudCorner::TopRight, 1, userName });
    entries.push_back({ HudCorner::BottomCenter, 0, getSceneName() });
    entries.push_back({ HudCorner::TopRight, 0, MString(getCurrentDate().c_str()) });
}

} // namespace

CameraHudDrawOverride::CameraHudDrawOverride(const MObject& hudNode)
    : MHWRender::MPxDrawOverride(hudNode, nullptr, true)
{
}

MHWRender::MPxDrawOverride* CameraHudDrawOverride::create(const MObject& hudNode)
{
    return new CameraHudDrawOverride(hudNode);
}

MHWRender::DrawAPI CameraHudDrawOverride::supportedDrawAPIs() const
{
    return MHWRender::kAllDevices;
}

bool CameraHudDrawOverride::isBounded(const MDagPath& /*objPath*/, const MDagPath& /*cameraPath*/) const
{
    return false;
}

MUserData* CameraHudDrawOverride::prepareForDraw(
    const MDagPath& objPath,
    const MDagPath& /*cameraPath*/,
    const MHWRender::MFrameContext& frameContext,
    MUserData* oldData)
{
    CameraHudDrawData* drawData = dynamic_cast<CameraHudDrawData*>(oldData);
    if (drawData == nullptr) {
        drawData = new CameraHudDrawData();
    }
    drawData->labels.clear();

    HudSettings settings = { 0.0, 0.0 };
    if (!readHudSettings(objPath, settings)) {
        return drawData;
    }

    int originX = 0;
    int originY = 0;
    int width = 0;
    int height = 0;
    frameContext.getViewportDimensions(originX, originY, width, height);
    if (width <= 0 || height <= 0) {
        return drawData;
    }

    std::vector<HudEntry> entries;
    MPoint gateMask(0.0, 0.0, 0.0);

    MStatus cameraStatus;
    const MDagPath cameraPath = frameContext.getCurrentCameraPath(&cameraStatus);
    if (cameraStatus) {
        const MFnCamera cameraFn(cameraPath, &cameraStatus);
        if (cameraStatus) {
            gateMask = computeGateMask(cameraFn, width, height);
            appendCameraEntries(cameraFn, cameraPath, entries);
        }
    }
    appendSceneEntries(drawData->userName, entries);

    const HudLayout layout(width, height, settings.fontSize, gateMask);
    drawData->fontSize = settings.fontSize;
    drawData->boxColor = MColor(0.0f, 0.0f, 0.0f, static_cast<float>(1.0 - settings.boxTransparency));
    drawData->labels.reserve(entries.size());
    for (const HudEntry& entry : entries) {
        drawData->labels.push_back({ entry.text, layout.positionOf(entry.corner, entry.row), alignmentOf(entry.corner) });
    }
    return drawData;
}

bool CameraHudDrawOverride::hasUIDrawables() const
{
    return true;
}

void CameraHudDrawOverride::addUIDrawables(
    const MDagPath& /*objPath*/,
    MHWRender::MUIDrawManager& drawManager,
    const MHWRender::MFrameContext& /*frameContext*/,
    const MUserData* data)
{
    const CameraHudDrawData* drawData = dynamic_cast<const CameraHudDrawData*>(data);
    if (drawData == nullptr || drawData->labels.empty()) {
        return;
    }

    drawManager.beginDrawable();
    drawManager.setDepthPriority(kDepthPriority);
    drawManager.setColor(kTextColor);
    drawManager.setFontName(kFontName);
    drawManager.setFontSize(static_cast<unsigned int>(drawData->fontSize));
    drawManager.setFontWeight(MUIDrawManager::kWeightNormal);
    drawManager.setFontIncline(MUIDrawManager::kInclineNormal);

    // 背景の大きさは指定せず(nullptr)Mayaに任せ、背景の色と不透明度だけを渡す
    for (const CameraHudLabel& label : drawData->labels) {
        drawManager.text2d(label.position, label.text, label.alignment, nullptr, &drawData->boxColor, false);
    }

    drawManager.endDrawable();
}
