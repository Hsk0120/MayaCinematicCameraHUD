/**
 * @file CameraHudUtility.h
 * @brief HUDに表示する文字列の取得・整形と、ゲートマスクの計算を行う補助関数群。
 */
#pragma once
#include <maya/MGlobal.h>
#include <maya/MString.h>
#include <maya/MTime.h>
#include <maya/MAnimControl.h>
#include <maya/MPoint.h>
#include <maya/MFileObject.h>
#include <maya/MFileIO.h>
#include <maya/MStringArray.h>
#include <maya/MFnDagNode.h>
#include <maya/MFrameContext.h>
#include <maya/MFnCamera.h>
#include <maya/MDagPath.h>
#include <maya/MNamespace.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <cmath>

/**
 * @brief HUDの表示内容を作るための補助関数をまとめた名前空間。
 */
namespace CameraHudUtility {

/**
 * @brief 今日の日付を取得する。
 * @return ローカル時刻の日付。"YYYY/MM/DD" 形式。
 */
std::string getCurrentDate();

/**
 * @brief MTimeの単位から1秒あたりのフレーム数を求める。
 * @param unit 時間単位。
 * @param[out] outFramesPerSecond フレームレート。対応表に無い単位では60。
 */
void getFPSFromTimeUnit(const MTime::Unit& unit, double& outFramesPerSecond);

/**
 * @brief 秒数を 時・分・秒・フレーム に分解する。
 * @param seconds 分解する秒数。
 * @param[out] hours 時。
 * @param[out] minutes 分(0〜59)。
 * @param[out] secondsOut 秒(0〜59)。
 * @param[out] frames 1秒未満の端数をフレーム数にしたもの。
 * @param framesPerSecond 使われない。フレームレートはunitから求め直す。
 * @param unit フレームレートを決める時間単位。
 */
void secondsToHMSF(
    double seconds,
    int& hours,
    int& minutes,
    int& secondsOut,
    int& frames,
    double framesPerSecond,
    MTime::Unit unit);

/**
 * @brief 現在時刻をタイムコードとして取得する。
 * @return "HH:MM:SS:FF" 形式の文字列。
 */
MString getCurrentTimecode();

/**
 * @brief 現在のフレームを取得する。
 * @return 現在時刻を、MAnimControlが返す時刻自身の単位で表したフレーム値。
 */
double getCurrentFrame();

/**
 * @brief 再生範囲の開始フレームを取得する。
 * @return 開始時刻を、MAnimControlが返す時刻自身の単位で表したフレーム値。
 */
double getStartFrame();

/**
 * @brief 再生範囲の終了フレームを取得する。
 * @return 終了時刻を、MAnimControlが返す時刻自身の単位で表したフレーム値。
 */
double getEndFrame();

/**
 * @brief 実数を指定した小数点以下の桁数で文字列へ書き出す。
 * @param value 書き出す値。
 * @param[out] str 書き出し先。十分な大きさのバッファを呼び出し側で用意する。
 * @param precision 小数点以下の桁数。
 */
void formatDoubleToBuffer(double value, char* str, int precision);

/**
 * @brief フレーム値を整数へ丸め、接頭辞と "f" を付けた表示用文字列にする。
 * @param prefix 先頭に付ける文字列。
 * @param frame フレーム値。
 * @return 例: prefixが "Start : " でframeが1.0なら "Start : 1f"。
 */
MString formatFrameString(const char* prefix, double frame);

/**
 * @brief OSのログインユーザー名を取得する。
 * @return ユーザー名。取得できなかった場合は空文字列。
 */
std::string getUserName();

/**
 * @brief 現在のシーンのファイル名を取得する。
 * @return ディレクトリを除いたファイル名。
 */
MString getSceneName();

/**
 * @brief カメラのシェイプ名から "Shape" 以降を取り除いた表示名を返す。
 * @param camName カメラのノード名。
 * @return 最初に現れる "Shape" より前の部分。含まれなければcamNameそのもの。
 */
MString getCameraNameFromString(const std::string& camName);

/**
 * @brief DAGパスのネームスペースの末尾3文字を、カット番号として整形する。
 * @param dagPath ネームスペースを調べるノードのパス。
 * @return "Cut:" に末尾3文字を続けた文字列。
 */
MString getNamespaceFromDagPath(const MDagPath& dagPath);

/**
 * @brief カメラのネームスペースからカット番号の表示文字列を求める。
 * @param camDagPath カメラのパス。
 * @return getNamespaceFromDagPath と同じ形式の文字列。
 */
MString getCutNumber(const MDagPath& camDagPath);

/**
 * @brief 焦点距離を小数点以下2桁の "mm" 付き文字列にする。
 * @param focalLength 焦点距離(mm)。
 * @return 例: "35.00mm"。
 */
MString formatFocalLength(double focalLength);

/**
 * @brief フィルムフィットがFillのときに、ゲートの外側へ残るマスクの大きさを求める。
 * @param overscan カメラのオーバースキャン値。
 * @param viewportWidth ビューポートの幅(ピクセル)。
 * @param viewportHeight ビューポートの高さ(ピクセル)。
 * @return マスクの片側分の幅(x)と高さ(y)。
 */
MPoint computeCameraResolutionFill(
    double overscan,
    int viewportWidth,
    int viewportHeight);

/**
 * @brief フィルムフィットがFill以外のときに、ゲートの外側へ残るマスクの大きさを求める。
 * @param overscan カメラのオーバースキャン値。
 * @param viewportWidth ビューポートの幅(ピクセル)。
 * @param viewportHeight ビューポートの高さ(ピクセル)。
 * @return マスクの片側分の幅(x)と高さ(y)。
 */
MPoint computeCameraResolutionHorizontal(
    double overscan,
    int viewportWidth,
    int viewportHeight);

/**
 * @brief カメラのcamera_cacheアトリビュートの状態を表示用文字列にする。
 * @param cameraCache アトリビュートの値。
 * @return "Cache : ON" または "Cache : OFF"。
 */
MString cameraCacheToString(bool cameraCache);

/**
 * @brief HUDの既定の文字サイズを返す。
 * @return 文字サイズ(ピクセル)。現在は固定で12。
 */
double getDefaultFontSize();

/**
 * @brief Qt Coreを利用できるかを確かめる検証用の関数。
 * @return 現在時刻を含む確認用の文字列。あわせてQtのバージョンをスクリプトエディターへ出力する。
 */
std::string testQtCore();

} // namespace CameraHudUtility
