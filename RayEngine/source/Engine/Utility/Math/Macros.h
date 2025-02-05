#pragma once
#define SAFE_DELETE(pointer) delete(pointer); (pointer) = nullptr;
#define SAFE_DELETE_ARRAY(pointer) delete[](pointer); (pointer) = nullptr;
#define PI 3.14159265358979323846
#define PI_FLOAT 3.14159265358979323846f

#define KINDA_SMALL_NUMBER 1.e-4f
#define SMALL_NUMBER 1.e-8f
