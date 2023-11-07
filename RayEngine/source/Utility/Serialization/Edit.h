#pragma once

namespace Utility
{
    template <class T>
    bool Edit(const String& InName, T& InOutData)
    {
        return InOutData.Edit(InName);
    }

    bool Edit(const String& InName, bool& InOutData);
    bool Edit(const String& InName, float& InOutData);
    bool Edit(const String& InName, int32& InOutData);
    bool Edit(const String& InName, Vec3F& InOutData);
    bool Edit(const String& InName, QuatF& InOutData);
    bool Edit(const String& InName, Mat4F& InOutData);
    bool Edit(const String& InName, String& InOutData);
}
