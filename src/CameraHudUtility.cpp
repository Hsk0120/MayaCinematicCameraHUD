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
#include "CameraHudUtility.h"

// Qt Core includes for verification
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QCoreApplication>

namespace CameraHudUtility {

std::string getCurrentDate()
{
    // Qt�ł̎����������i���ؗp�j
    // QString qtDate = QDate::currentDate().toString("yyyy/MM/dd");
    // MGlobal::displayInfo(MString("Qt Date: ") + qtDate.toStdString().c_str());
    
    // ���ݎ������擾
    auto now = std::chrono::system_clock::now();
    std::time_t time_now = std::chrono::system_clock::to_time_t(now);

    // tm �\�̂ɕϊ�
    std::tm* time_info = std::localtime(&time_now);

    // �t�H�[�}�b�g
    std::stringstream ss;
    ss << std::put_time(time_info, "%Y/%m/%d");
    return ss.str();
}

void getFPSFromTimeUnit(
    const MTime::Unit& unit,
    double& framesPerSecond)
{
    switch (unit) {
    case MTime::kGames:
        framesPerSecond = 15.0;
        break;
    case MTime::kFilm:
        framesPerSecond = 24.0;
        break;
    case MTime::kPALFrame:
        framesPerSecond = 25.0;
        break;
    case MTime::kNTSCFrame:
        framesPerSecond = 30.0;
        break;
    case MTime::kShowScan:
        framesPerSecond = 48.0;
        break;
    case MTime::kPALField:
        framesPerSecond = 50.0;
        break;
    case MTime::kNTSCField:
        framesPerSecond = 60.0;
        break;
    case MTime::k120FPS:
        framesPerSecond = 120.0;
        break;
    default:
        framesPerSecond = 60.0;
        break;
    }
}

void secondsToHMSF(
    double seconds,
    int& hours,
    int& minutes,
    int& secondsOut,
    int& frames,
    double framesPerSecond,
    MTime::Unit unit)
{
    getFPSFromTimeUnit(unit, framesPerSecond);

    // ���ԁA���A�b���v�Z
    hours = static_cast<int>(std::floor(seconds / 3600.0));
    seconds -= hours * 3600.0;
    minutes = static_cast<int>(std::floor(seconds / 60.0));
    seconds -= minutes * 60.0;
    secondsOut = static_cast<int>(std::floor(seconds));
    seconds -= secondsOut;
    frames = static_cast<int>(std::round(seconds * framesPerSecond));

    // �t���[�����̃I�[�o�[�t���[�𒲐�
    if (frames >= static_cast<int>(framesPerSecond)) {
        frames = 0;
        ++secondsOut;
    }

    if (secondsOut >= 60) {
        secondsOut = 0;
        ++minutes;
    }

    if (minutes >= 60) {
        minutes = 0;
        ++hours;
    }
}

MString getCurrentTimecode()
{
    MTime currentTime = MAnimControl::currentTime();
    double currentSeconds = currentTime.as(MTime::kSeconds);

    int hours, minutes, seconds, frames;
    double framesPerSecond = 60.0;

    MTime::Unit unit = currentTime.unit();
    secondsToHMSF(currentSeconds, hours, minutes, seconds, frames, framesPerSecond, unit);

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hours << ":"
        << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << seconds << ":"
        << std::setfill('0') << std::setw(2) << frames;

    return MString(ss.str().c_str());
}

double getCurrentFrame()
{
    MTime currentTime = MAnimControl::currentTime();
    double currentFrame = currentTime.as(currentTime.unit());
    return currentFrame;
}

double getStartFrame()
{
    MTime startTime = MAnimControl::minTime();
    double startFrame = startTime.as(startTime.unit());
    return startFrame;
}

double getEndFrame()
{
    MTime endTime = MAnimControl::maxTime();
    double endFrame = endTime.as(endTime.unit());
    return endFrame;
}

void formatDoubleToBuffer(
    double value, char* str,
    int precision)
{
    sprintf(str, "%.*f", precision, value);
}

MString formatFrameString(const char* prefix, double frame)
{
    std::stringstream ss;
    ss << prefix << static_cast<int>(std::round(frame)) << "f";
    return MString(ss.str().c_str());
}

std::string getUserName()
{
    char username[UNLEN + 1];
    DWORD size = sizeof(username);

    if (GetUserNameA(username, &size)) {
        return std::string(username);
    }
    else {
        return std::string();
    }
}

MString getSceneName()
{
    MString fileObj = MFileIO::currentFile();
    MStringArray nameArray;
    fileObj.split('/', nameArray);
    return nameArray[nameArray.length() - 1];
}

MString getCameraNameFromString(const std::string& camName)
{
    std::string local = camName;
    std::size_t pos = local.find("Shape");
    if (pos != std::string::npos) {
        local.erase(pos);
    }
    return MString(local.c_str());
}

MString getNamespaceFromDagPath(
    const MDagPath& dagPath)
{
    MStatus status;
    MString namespaceObj = MNamespace::getNamespaceFromName(dagPath.partialPathName(), &status);

    // �����擾
    int length = namespaceObj.length();

    // ����3������؂����ĕ\���p�ɂ���
    MString cut_number = namespaceObj.substring(length - 3, length);
    cut_number = MString("Cut:") + cut_number;
    return cut_number;
}

MString getCutNumber(
    const MDagPath& camDagPath)
{
    MString namespaceName = getNamespaceFromDagPath(camDagPath);
    return namespaceName;
}

MString formatFocalLength(
    double focalLength)
{
    char str[64];
    formatDoubleToBuffer(focalLength, str, 2);
    strcat(str, "mm");
    return MString(str);
}

MPoint computeViewportLeftBottom(
    int num,
    int width,
    int height,
    double fontSize,
    MPoint offset,
    MPoint margin,
    MPoint maskOffset)
{
    return MPoint(
        offset.x + margin.x + maskOffset.x,
        offset.y + maskOffset.y + (margin.y + fontSize) * num,
        0);
}

MPoint computeViewportLeftTop(
    int num,
    int width,
    int height,
    double fontSize,
    MPoint offset,
    MPoint margin,
    MPoint maskOffset)
{
    return MPoint(
        offset.x + margin.x + maskOffset.x,
        height - maskOffset.y - offset.y - (margin.y + fontSize) * (num + 1),
        0);
}

MPoint computeViewportRightBottom(
    int num,
    int width,
    int height,
    double fontSize,
    MPoint offset,
    MPoint margin,
    MPoint maskOffset)
{
    return MPoint(
        width - offset.x - margin.x - maskOffset.x,
        offset.y + maskOffset.y + (margin.y + fontSize) * num,
        0);
}

MPoint computeViewportRightTop(
    int num,
    int width,
    int height,
    double fontSize,
    MPoint offset,
    MPoint margin,
    MPoint maskOffset)
{
    return MPoint(
        width - offset.x - margin.x - maskOffset.x,
        height - maskOffset.y - offset.y - (margin.y + fontSize) * (num + 1),
        0);
}

MPoint computeViewportCenterBottom(
    int num,
    int width,
    int height,
    double fontSize,
    MPoint offset,
    MPoint margin,
    MPoint maskOffset)
{
    return MPoint(
        width / 2,
        offset.y + maskOffset.y + (margin.y + fontSize) * num,
        0);
}

MPoint computeViewportCenterTop(
    int num,
    int width,
    int height,
    double fontSize,
    MPoint offset,
    MPoint margin,
    MPoint maskOffset)
{
    return MPoint(
        width / 2,
        height - maskOffset.y - offset.y - (margin.y + fontSize) * (num + 1),
        0);
}

MPoint computeCameraResolutionFill(
    double overscan,
    int viewport_width,
    int viewport_height)
{
    double device_aspect_ratio = 1.778;
    double viewport_aspect_ratio = static_cast<double>(viewport_width) / static_cast<double>(viewport_height);
    double mask_width = 0.0;
    double mask_height = 0.0;

    // ���G�ȏ����͊������W�b�N���ێ�
    if (viewport_aspect_ratio > device_aspect_ratio * overscan) {
        MGlobal::displayInfo(MString("if                                    :"));
        mask_height = 0;
        mask_width = (viewport_width - viewport_width / overscan) / 2;
    }
    else if (device_aspect_ratio * overscan > viewport_aspect_ratio && viewport_aspect_ratio > device_aspect_ratio / viewport_aspect_ratio / overscan * 2) {
        MGlobal::displayInfo(MString("else if 01                                     :"));
        mask_height = (viewport_height - viewport_height / overscan) / 2 * (device_aspect_ratio * overscan - viewport_aspect_ratio) * 2;
        mask_width = (viewport_width - viewport_width / overscan) / 2;
    }
    else if ((device_aspect_ratio - (viewport_aspect_ratio / overscan * 2)) < 0) {
        MGlobal::displayInfo(MString("else if 02                                     :"));
        mask_height = (viewport_height - viewport_height / overscan) / 2 * device_aspect_ratio * device_aspect_ratio / (viewport_aspect_ratio / overscan * 2);
        mask_width = (viewport_width - viewport_width / overscan) / 2 * std::pow(((viewport_aspect_ratio / overscan * 2) - device_aspect_ratio) * overscan, 0.5);
    }
    else {
        MGlobal::displayInfo(MString("else                                     :"));
        mask_height = (viewport_height - viewport_height / overscan) / 2 * device_aspect_ratio;
        mask_width = 0;
    }

    MGlobal::displayInfo(MString("viewport_aspect_ratio                                         :") + viewport_aspect_ratio);
    MGlobal::displayInfo(MString("device_aspect_ratio                                           :") + device_aspect_ratio);
    MGlobal::displayInfo(MString("viewport_aspect_ratio / overscan                              :") + (viewport_aspect_ratio / overscan));
    MGlobal::displayInfo(MString("viewport_aspect_ratio / overscan * 2                          :") + (viewport_aspect_ratio / overscan * 2));
    MGlobal::displayInfo(MString("device_aspect_ratio / viewport_aspect_ratio / overscan * 2    :") + (device_aspect_ratio / viewport_aspect_ratio / overscan * 2));
    MGlobal::displayInfo(MString("device_aspect_ratio / overscan                                :") + (device_aspect_ratio / overscan));
    MGlobal::displayInfo(MString("viewport_aspect_ratio * overscan                              :") + (viewport_aspect_ratio * overscan));
    MGlobal::displayInfo(MString("device_aspect_ratio * overscan                                :") + (device_aspect_ratio * overscan));
    MGlobal::displayInfo(MString("(device_aspect_ratio * overscan - viewport_aspect_ratio) * 2  :") + ((device_aspect_ratio * overscan - viewport_aspect_ratio) * 2));
    MGlobal::displayInfo(MString(" ( (viewport_aspect_ratio / overscan * 2) - device_aspect_ratio) * overscan:") + (((viewport_aspect_ratio / overscan * 2) - device_aspect_ratio) * overscan));
    MGlobal::displayInfo(MString("mask_height                                                   :") + mask_height);
    MGlobal::displayInfo(MString("mask_width                                                    :") + mask_width);

    return MPoint(mask_width, mask_height, 0.0);
}

MPoint computeCameraResolutionHorizontal(
    double overscan,
    int viewport_width,
    int viewport_height)
{
    double device_aspect_ratio = 1.778;
    double viewport_aspect_ratio = static_cast<double>(viewport_width) / static_cast<double>(viewport_height);
    double mask_width = 0.0;
    double mask_height = 0.0;

    if (viewport_aspect_ratio > device_aspect_ratio * overscan) {
        // viewport is wider than camera aspect with overscan
        mask_height = 0;
        mask_width = (viewport_width - viewport_width / overscan) / 2;
    }
    else if (viewport_aspect_ratio < device_aspect_ratio * overscan) {
        // viewport is taller (or narrower) than camera aspect with overscan
        mask_height = (viewport_height - viewport_height / overscan) / 2 * (device_aspect_ratio * overscan - viewport_aspect_ratio) * 2;
        mask_width = (viewport_width - viewport_width / overscan) / 2;
    }

    return MPoint(mask_width, mask_height, 0.0);
}

MString filmFitToString(MFnCamera::FilmFit filmFit)
{
    MString filmFitString;
    switch (filmFit)
    {
    case MFnCamera::kFillFilmFit:
        filmFitString = "fill";
        break;
    case MFnCamera::kHorizontalFilmFit:
        filmFitString = "horizontal";
        break;
    case MFnCamera::kVerticalFilmFit:
        filmFitString = "vertical";
        break;
    case MFnCamera::kOverscanFilmFit:
        filmFitString = "overscan";
        break;
    default:
        MGlobal::displayError("Unknown film fit.");
        return MString();
    }
    return filmFitString;
}

MString cameraCacheToString(
    bool camera_cache)
{
    if (camera_cache) {
        return MString("Cache : ON");
    }
    else {
        return MString("Cache : OFF");
    }
}

// Mayaのデフォルト文字サイズを取得
// Mayaの標準DPIは96 DPIです
// この関数はMayaのUI表示における標準的なフォントサイズの基準値を返します
double getDefaultFontSize()
{
    // Maya標準のデフォルトフォントサイズ
    // MayaのUI DPIは96 DPI（Windows標準）を基準としています
    // このプラグインでは相対的なスケール値として1.0を使用
    return 1.0;
}

std::string testQtCore()
{
    // Qt Core �̋@�\���g���ăe�X�g
    QString testString = QString("Qt Core is working! Current time: %1")
        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    
    // Maya �̃��O�ɏo��
    MGlobal::displayInfo(MString("Qt Test: ") + testString.toStdString().c_str());
    
    // Qt �̃o�[�W���������擾
    QString versionInfo = QString("Qt Version: %1").arg(qVersion());
    MGlobal::displayInfo(MString("Qt Version: ") + versionInfo.toStdString().c_str());
    
    return testString.toStdString();
}

} // namespace CameraHudUtility