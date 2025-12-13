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

namespace CameraHudUtility {

// 今日の日付を "YYYY/MM/DD" 形式で取得
std::string getCurrentDate();

// MTime の Unit から FPS を取得
void getFPSFromTimeUnit(const MTime::Unit& unit, double& outFramesPerSecond);

// 秒数とfpsから時間:分:秒:フレーム を取得
void secondsToHMSF(
    double seconds,
    int& hours,
    int& minutes,
    int& secondsOut,
    int& frames,
    double framesPerSecond,
    MTime::Unit unit);

// タイムコード取得
MString getCurrentTimecode();

// カレントフレームを取得する
double getCurrentFrame();

// スタートフレーム取得
double getStartFrame();

// エンドフレームを取得
double getEndFrame();

// doubleからcharに変換 (バッファは呼び出し側で確保)
void formatDoubleToBuffer(double value, char* str, int precision);

// フレーム数の取得とプレフィックス付き文字列を返す
MString formatFrameString(const char* prefix, double frame);

// ユーザー名を取得
std::string getUserName();

// シーン名取得
MString getSceneName();

// カメラネームからシェイプ部分を取り除いた名前を返す
MString getCameraNameFromString(const std::string& camName);

// オブジェクトからネームスペースを取得
MString getNamespaceFromDagPath(const MDagPath& dagPath);

// カットナンバー取得
MString getCutNumber(const MDagPath& camDagPath);

// カメラの焦点距離をフォーマットして返す
MString formatFocalLength(double focalLength);

// ビューポート上のテキスト位置を計算 (左下)
MPoint computeViewportLeftBottom(
    int index,
    int viewportWidth,
    int viewportHeight,
    double fontSize,
    MPoint offset,
    MPoint margin,
    MPoint maskOffset);

// 左上
MPoint computeViewportLeftTop(
    int index,
    int viewportWidth,
    int viewportHeight,
    double fontSize,
    MPoint offset,
    MPoint margin,
    MPoint maskOffset);

// 右下
MPoint computeViewportRightBottom(
    int index,
    int viewportWidth,
    int viewportHeight,
    double fontSize,
    MPoint offset,
    MPoint margin,
    MPoint maskOffset);

// 右上
MPoint computeViewportRightTop(
    int index,
    int viewportWidth,
    int viewportHeight,
    double fontSize,
    MPoint offset,
    MPoint margin,
    MPoint maskOffset);

// 中央下
MPoint computeViewportCenterBottom(
    int index,
    int viewportWidth,
    int viewportHeight,
    double fontSize,
    MPoint offset,
    MPoint margin,
    MPoint maskOffset);

// 中央上
MPoint computeViewportCenterTop(
    int index,
    int viewportWidth,
    int viewportHeight,
    double fontSize,
    MPoint offset,
    MPoint margin,
    MPoint maskOffset);

// カメラのレゾリューションゲートマスク取得と計算 ※Fillの場合
MPoint computeCameraResolutionFill(
    double overscan,
    int viewportWidth,
    int viewportHeight);

// カメラのレゾリューションゲートマスク取得と計算 ※Horizontalの場合
MPoint computeCameraResolutionHorizontal(
    double overscan,
    int viewportWidth,
    int viewportHeight);

// filmFit を文字列に変換
MString filmFitToString(MFnCamera::FilmFit filmFit);

// カメラキャッシュのアトリビュート状態を取得
MString cameraCacheToString(bool cameraCache);

} // namespace CameraHudUtility