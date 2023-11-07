#pragma once
#define MIN(aLeftValue, aRightValue) (((aLeftValue)<(aRightValue))?(aLeftValue):(aRightValue))
#define MAX(aLeftValue, aRightValue) (((aLeftValue)>(aRightValue))?(aLeftValue):(aRightValue))
#define CLAMP(aValue, aMinValue, aMaxValue) ((aValue)< MIN(aMinValue, aMaxValue))?(MIN((aMinValue), (aMaxValue))):((aValue)> MAX((aMinValue), (aMaxValue)))?(MAX((aMinValue), (aMaxValue))):((aValue))
#define SAFE_DELETE(pointer) delete(pointer); (pointer) = nullptr;
#define SAFE_DELETE_ARRAY(pointer) delete[](pointer); (pointer) = nullptr;
#define PI 3.14159265359
#define PI_FLOAT 3.14159265359f
#define LERP(a, b, t) a + (b - a) * (MIN(MAX(t, 0), 1.f))
