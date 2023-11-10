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
    bool Edit(const String& InName, uint8& InOutData);
    bool Edit(const String& InName, uint32& InOutData);
    bool Edit(const String& InName, Vec3F& InOutData);
    bool Edit(const String& InName, QuatF& InOutData);
    bool Edit(const String& InName, Mat4F& InOutData);
    bool Edit(const String& InName, String& InOutData);

    bool BeginList(const String& InName);
    void EndList();
    void SameLine();
    bool AddButton();
    bool RemoveButton();
    
    template <class T>
    bool Edit(const String& InName, Vector<T>& InOutData)
    {
        bool edited = false; 
        if (BeginList(InName))
        {
            // TODO: Fix naming and format
            for (auto& data : InOutData)
                if (Edit(("##" + InName).c_str(), data))
                    edited = true;
            if (AddButton())
                InOutData.emplace_back();
            SameLine();
            if (RemoveButton())
                InOutData.pop_back();
            EndList();
        }
        return edited; 
    }

    template <class Key, class Val>
    bool Edit(const String& InName, Map<Key, Val>& InOutData)
    {
        bool edited = false; 
        if (BeginList(InName))
        {
            for (auto& data : InOutData)
            {
                Key k = data.first;
                if (Edit(("##Key_" + InName).c_str(), k))
                    edited = true;
                SameLine();
                if (Edit(("##Val_" + InName).c_str(), data.second))
                    edited = true;
            }
            if (AddButton())
                InOutData[Key()] = Val();
            SameLine();
            if (RemoveButton())
                InOutData.clear();
            EndList();
        }
        return edited;
    }
}
