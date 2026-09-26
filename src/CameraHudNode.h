/**
 * @file CameraHudNode.h
 * @brief HUDの設定値を持つロケーターノード CameraHud の宣言。
 */
#pragma once

#include <maya/MObject.h>
#include <maya/MPxLocatorNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

/**
 * @brief HUDの文字サイズと文字背景の透明度を保持するロケーターノード。
 *
 * ノード自体は計算を行わず、表示は CameraHudDrawOverride が
 * このノードのアトリビュートを読んで行う。
 * ノード型名・型ID・アトリビュート名(長い名前と短い名前)は保存済みシーンから
 * 参照されるため、互換性のため変更しない。
 */
class CameraHudNode : public MPxLocatorNode
{
public:
    /// Mayaへ登録するノード型名。
    static const char* const kTypeName;
    /// ノード型ID。保存済みシーンとの互換性のため固定値。
    static const MTypeId kTypeId;
    /// ノード型と描画オーバーライドを対応付ける描画分類文字列。
    static const MString kDrawClassification;
    /// 描画オーバーライドを登録・解除するときの登録者ID。
    static const MString kDrawRegistrantId;

    /// 文字サイズ(ピクセル)。長い名前 textFontSize / 短い名前 tfs。
    static MObject fontSizeAttr;
    /// 文字背景の透明度(0で不透明、1で透明)。長い名前 textBoxTransparency / 短い名前 tbt。
    static MObject boxTransparencyAttr;

    /**
     * @brief Mayaがノードを作るときに呼ぶ生成関数。
     * @return 新しいノードのインスタンス。所有権はMayaへ渡る。
     */
    static void* create();

    /**
     * @brief ノード型の登録時に1回だけ呼ばれ、アトリビュートを定義する。
     * @return すべてのアトリビュートを追加できた場合kSuccess。失敗時はその原因。
     */
    static MStatus defineAttributes();
};
