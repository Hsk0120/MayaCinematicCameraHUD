/**
 * @file CameraHudDrawOverride.h
 * @brief CameraHud ノードをViewport 2.0のHUDとして描く描画オーバーライドの宣言。
 */
#pragma once

#include <maya/MDagPath.h>
#include <maya/MFrameContext.h>
#include <maya/MObject.h>
#include <maya/MPxDrawOverride.h>
#include <maya/MUIDrawManager.h>
#include <maya/MUserData.h>

/**
 * @brief CameraHud ノード1つにつき1つ作られ、ビューポートの四隅へ撮影情報を描く。
 *
 * 描画準備(prepareForDraw)でノードの設定値・ビューポートのカメラ・タイムラインを読み、
 * 表示行と画面座標を CameraHudDrawData にまとめる。
 * 描画(addUIDrawables)ではそのデータの行を2Dテキストとして描くだけにする。
 * HUDは時刻やカメラの変化に追従する必要があるため、常に再描画対象(isAlwaysDirty)として作る。
 */
class CameraHudDrawOverride : public MHWRender::MPxDrawOverride
{
public:
    /**
     * @brief MDrawRegistry へ登録する生成関数。
     * @param hudNode 描画対象の CameraHud ノード。
     * @return 新しい描画オーバーライド。所有権はMayaへ渡る。
     */
    static MHWRender::MPxDrawOverride* create(const MObject& hudNode);

    /**
     * @brief 対応する描画APIを返す。
     * @return OpenGL(互換・Core Profile)とDirectX 11のすべて。
     */
    MHWRender::DrawAPI supportedDrawAPIs() const override;

    /**
     * @brief バウンディングボックスを持つかどうかを返す。
     * @param objPath 描画対象ノードのパス(未使用)。
     * @param cameraPath 描画に使うカメラのパス(未使用)。
     * @return 常にfalse。画面固定のHUDなので視錐台カリングの対象にしない。
     */
    bool isBounded(const MDagPath& objPath, const MDagPath& cameraPath) const override;

    /**
     * @brief ノードの設定値とビューポートの状態から、描画する行と座標を求める。
     * @param objPath 描画対象の CameraHud ノードのパス。
     * @param cameraPath 描画に使うカメラのパス(未使用。フレームコンテキストのカメラを使う)。
     * @param frameContext ビューポートの寸法と現在のカメラを取得するコンテキスト。
     * @param oldData 前回返したデータ。CameraHudDrawData であれば再利用する。
     * @return 描画に使う CameraHudDrawData。所有権はMayaが持ち、次回のoldDataとして戻る。
     *         ノードの設定値を読めない、またはビューポートの寸法が0の場合は行が空になる。
     */
    MUserData* prepareForDraw(
        const MDagPath& objPath,
        const MDagPath& cameraPath,
        const MHWRender::MFrameContext& frameContext,
        MUserData* oldData) override;

    /**
     * @brief addUIDrawables を使うかどうかを返す。
     * @return 常にtrue。HUDの文字列はすべてUI描画で出す。
     */
    bool hasUIDrawables() const override;

    /**
     * @brief 描画準備で求めた行を2Dテキストとして描く。
     * @param objPath 描画対象ノードのパス(未使用)。
     * @param drawManager 文字列を登録するUI描画マネージャー。
     * @param frameContext フレームコンテキスト(未使用)。
     * @param data prepareForDraw が返したデータ。CameraHudDrawData 以外なら何も描かない。
     */
    void addUIDrawables(
        const MDagPath& objPath,
        MHWRender::MUIDrawManager& drawManager,
        const MHWRender::MFrameContext& frameContext,
        const MUserData* data) override;

private:
    /**
     * @brief 描画コールバックを使わず、常に再描画対象として初期化する。
     * @param hudNode 描画対象の CameraHud ノード。
     */
    explicit CameraHudDrawOverride(const MObject& hudNode);
};
