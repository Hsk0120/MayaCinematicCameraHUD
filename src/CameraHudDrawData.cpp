/**
 * @file CameraHudDrawData.cpp
 * @brief CameraHudDrawData の初期化。
 */
#include "CameraHudDrawData.h"
#include "CameraHudUtility.h"

CameraHudDrawData::CameraHudDrawData()
    : MUserData()
    , userName(CameraHudUtility::getUserName().c_str())
    , fontSize(CameraHudUtility::getDefaultFontSize())
    , boxColor(0.0f, 0.0f, 0.0f, 1.0f)
{
}
