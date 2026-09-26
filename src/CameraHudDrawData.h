/**
 * @file CameraHudDrawData.h
 * @brief 描画準備(prepareForDraw)で集めたHUDの表示内容を描画段階へ渡すデータ。
 */
#pragma once

#include <maya/MColor.h>
#include <maya/MPoint.h>
#include <maya/MString.h>
#include <maya/MUIDrawManager.h>
#include <maya/MUserData.h>

#include <vector>

/**
 * @brief ビューポートへ描く1行分の文字列と、その配置。
 */
struct CameraHudLabel
{
    /// 表示する文字列。
    MString text;
    /// ビューポート左下を原点とするピクセル座標。
    MPoint position;
    /// positionに対する文字列の揃え方。
    MHWRender::MUIDrawManager::TextAlignment alignment;
};

/**
 * @brief CameraHudDrawOverride が描画準備で作り、描画時に読み取るHUDの内容。
 *
 * ノードやカメラへの問い合わせは描画準備の段階で済ませ、描画時はこのデータだけを使う
 * (描画中にDGへ問い合わせることはAPIで禁止されているため)。
 * インスタンスはMayaが保持し、次の描画準備へ前回のデータとして戻ってくるので、
 * 変わらない値(ユーザー名)は生成時に1回だけ取得して使い回す。
 */
class CameraHudDrawData : public MUserData
{
public:
    /**
     * @brief OSのログインユーザー名を取得して初期化する。表示行は空の状態で始まる。
     */
    CameraHudDrawData();

    /// OSのログインユーザー名。取得できなかった場合は空文字列。
    const MString userName;

    /// 描画する行の一覧。描画準備のたびに作り直す。
    std::vector<CameraHudLabel> labels;
    /// 文字サイズ(ピクセル)。配置計算にはこの値を、描画には整数へ切り捨てた値を使う。
    double fontSize;
    /// 文字背景の色(アルファは1 - textBoxTransparency)。
    MColor boxColor;
};
