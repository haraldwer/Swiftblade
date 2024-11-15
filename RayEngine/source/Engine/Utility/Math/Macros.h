#pragma once
#define SAFE_DELETE(pointer) delete(pointer); (pointer) = nullptr;
#define SAFE_DELETE_ARRAY(pointer) delete[](pointer); (pointer) = nullptr;
#define PI 3.14159265358979323846
#define PI_FLOAT 3.14159265358979323846f
