#pragma once
#include <maya/MGlobal.h>
#include <maya/MString.h>
#include <maya/MTime.h>
#include <maya/MAnimControl.h>
#include <windows.h>
#include <string>
#include <lmcons.h>
#include <maya/MFileObject.h>
#include <maya/MFileIO.h>
#include <maya/MStringArray.h>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <maya/MFnDagNode.h>
#include <maya/MFrameContext.h>
#include <maya/MFnCamera.h>
#include <maya/MDagPath.h>
#include <cmath>
#include <maya/MNamespace.h>
#include <iostream>

// 今日の日付を年/月/日 で取得する
std::string get_current_date();

// MTimeのUnitからfpsを取得
void convert_unitToTime(const MTime::Unit&, double&);

// 秒数とfpsから時間:分:秒:フレーム を取得
void convert_secondsToTime(
    double seconds, 
    int& hours, 
    int& minutes, 
    int& secondsOut, 
    int& frames, 
    double framesPerSecond, 
    MTime::Unit unit);

// タイムコード取得
MString get_current_timecode();

// カレントフレームを取得する関数
double get_current_frame();

// スタートフレーム取得
double get_start_frame();

// エンドフレームを取得
double get_end_frame();

// doubleからcharに変換
void double_to_char(
    double value, 
    char* str, 
    int precision);

// フレーム数の取得と+fの文字列足し
char* get_frame(
    static char result[64], 
    double frame);

// ユーザー名を取得
std::string get_username();

// シーン名取得
MString get_scene_name();

// カメラネームからShape削除
MString get_camera_name(std::string camName);

// オブジェクトからネームスペースを取得
MString GetNamespaceFromObject(const MDagPath& dagPath);

// カットナンバー取得
MString get_cut_num(const MDagPath &camDagPath);

// カメラの焦点距離を取得
MString get_camera_focalLength(double focalLength);

// テキスト位置 左下
MPoint get_viewport_leftBottom(
    int num, 
    int width, 
    int height, 
    double fontSize, 
    MPoint offset, 
    MPoint margin, 
    MPoint maskOffset);

// テキスト位置 左上
MPoint get_viewport_leftTop(
    int num, 
    int width, 
    int height, 
    double fontSize, 
    MPoint offset, 
    MPoint margin, 
    MPoint maskOffset);

// テキスト位置 右下
MPoint get_viewport_rightBottom(
    int num, 
    int width, 
    int height, 
    double fontSize,
    MPoint offset, 
    MPoint margin, 
    MPoint maskOffset);

// テキスト位置 右上
MPoint get_viewport_rightTop(
    int num, 
    int width, 
    int height, 
    double fontSize, 
    MPoint offset, 
    MPoint margin, 
    MPoint maskOffset);

// テキスト位置 真ん中下
MPoint get_viewport_centerBottom(
    int num, 
    int width, 
    int height, 
    double fontSize, 
    MPoint offset, 
    MPoint margin, 
    MPoint maskOffset);

// テキスト位置 真ん中上
MPoint get_viewport_centerTop(
    int num, 
    int width, 
    int height, 
    double fontSize, 
    MPoint offset, 
    MPoint margin, 
    MPoint maskOffset);

// カメラのレゾリューションゲートマスク取得と計算 ※Fillの場合
MPoint get_camera_resolution_fill(
    double overscan,
    int viewport_width,
    int viewport_height
    );

// カメラのレゾリューションゲートマスク取得と計算 ※Fillの場合
MPoint get_camera_resolution_horizontal(
    double overscan,
    int viewport_width,
    int viewport_height);

// filmFitを文字列に変換
MString getFilmFitAsString(const MFnCamera::FilmFit& filmFit);

// カメラキャッシュのアトリビュート状態を取得
MString get_camera_cache(bool camera_cache);