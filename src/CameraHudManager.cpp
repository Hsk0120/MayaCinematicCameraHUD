#include "CameraHudManager.h"
#include "CameraHudUtility.h"

CameraHudManagerData::CameraHudManagerData()
    : MUserData(false)
{
    fCurrentDate = CameraHudUtility::getCurrentDate().c_str();
    fUserName = CameraHudUtility::getUserName().c_str();
    fSceneName = CameraHudUtility::getSceneName();
}
