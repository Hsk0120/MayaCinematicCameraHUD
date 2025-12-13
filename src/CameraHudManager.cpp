#include "CameraHudManager.h"
#include "CameraHudUtility.h"

CameraHudManagerData::CameraHudManagerData()
    : MUserData(false)
{
    fCurrentDate = getCurrentDate().c_str();
    fUserName = getUserName().c_str();
    fSceneName = getSceneName();
}
