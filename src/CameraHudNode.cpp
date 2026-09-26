/**
 * @file CameraHudNode.cpp
 * @brief CameraHud ノードの型情報とアトリビュート定義。
 */
#include "CameraHudNode.h"
#include "CameraHudUtility.h"

#include <maya/MFnNumericAttribute.h>
#include <maya/MFnNumericData.h>

const char* const CameraHudNode::kTypeName = "CameraHud";
const MTypeId CameraHudNode::kTypeId(0x0013c801);
const MString CameraHudNode::kDrawClassification("drawdb/geometry/CameraHud");
const MString CameraHudNode::kDrawRegistrantId("MayaCinematicCameraHUD");

MObject CameraHudNode::fontSizeAttr;
MObject CameraHudNode::boxTransparencyAttr;

namespace {

/**
 * @brief 範囲付きのfloatアトリビュート1つ分の定義。
 *
 * 長い名前・短い名前は保存済みシーンが参照するため、既存の値から変えない。
 */
struct RangedFloatAttribute
{
    const char* longName;   ///< 長いアトリビュート名。
    const char* shortName;  ///< 短いアトリビュート名(.maファイルのsetAttrで使われる)。
    double defaultValue;    ///< 既定値。
    double hardMin;         ///< 入力できる最小値。
    double hardMax;         ///< 入力できる最大値。
    double sliderMax;       ///< アトリビュートエディターのスライダー上限。hardMax以上なら設定しない。
};

/// 文字サイズ(ピクセル)の上限。HUDとして使う大きさに十分な余裕を持たせた値。
const double kMaxFontSize = 128.0;
/// 文字サイズのスライダー上限。通常使う範囲が操作しやすいよう狭めておく。
const double kFontSizeSliderMax = 48.0;
/// 文字背景の透明度の既定値。背景をうっすら残して文字を読みやすくする。
const double kDefaultBoxTransparency = 0.3;

/**
 * @brief 定義に従ってfloatアトリビュートを作成し、ノード型へ追加する。
 * @param definition 作成するアトリビュートの名前・既定値・範囲。
 * @param[out] attribute 作成したアトリビュート。失敗時の内容は不定。
 * @return 作成と追加に成功した場合kSuccess。失敗時はMayaが返した状態。
 */
MStatus addRangedFloatAttribute(const RangedFloatAttribute& definition, MObject& attribute)
{
    MStatus status;
    MFnNumericAttribute numericFn;
    attribute = numericFn.create(
        definition.longName, definition.shortName,
        MFnNumericData::kFloat, definition.defaultValue, &status);
    if (!status) {
        return status;
    }

    numericFn.setMin(definition.hardMin);
    numericFn.setMax(definition.hardMax);
    if (definition.sliderMax < definition.hardMax) {
        numericFn.setSoftMax(definition.sliderMax);
    }
    return MPxNode::addAttribute(attribute);
}

} // namespace

void* CameraHudNode::create()
{
    return new CameraHudNode();
}

MStatus CameraHudNode::defineAttributes()
{
    // 文字サイズは1ピクセル未満だと描画できないため下限を1にする
    const RangedFloatAttribute fontSize = {
        "textFontSize", "tfs",
        CameraHudUtility::getDefaultFontSize(), 1.0, kMaxFontSize, kFontSizeSliderMax };
    MStatus status = addRangedFloatAttribute(fontSize, fontSizeAttr);
    if (!status) {
        return status;
    }

    // 透明度は不透明度(1 - 値)として描画に使うため0〜1に制限する
    const RangedFloatAttribute boxTransparency = {
        "textBoxTransparency", "tbt",
        kDefaultBoxTransparency, 0.0, 1.0, 1.0 };
    return addRangedFloatAttribute(boxTransparency, boxTransparencyAttr);
}
