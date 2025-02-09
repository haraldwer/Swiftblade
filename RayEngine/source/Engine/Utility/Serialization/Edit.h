#pragma once

namespace Utility
{
    inline String GetEditName(const String& InName, uint32 InOffset)
    {
        return InName + "##PropertyEdit_" + std::to_string(InOffset);
    }

    bool MaybeCollapse(const String& InName, uint32 InOffset);
    
    template <class T>
    bool Edit(const String& InName, T& InOutData, uint32 InOffset = 0)
    {
        if (MaybeCollapse(InName, InOffset))
            return InOutData.Edit(InName, InOffset);
        return false;
    }
    
    bool Edit(const String& InName, bool& InOutData, uint32 InOffset = 0);
    bool Edit(const String& InName, float& InOutData, uint32 InOffset = 0);
    bool Edit(const String& InName, int32& InOutData, uint32 InOffset = 0);
    bool Edit(const String& InName, uint8& InOutData, uint32 InOffset = 0);
    bool Edit(const String& InName, uint32& InOutData, uint32 InOffset = 0);
    bool Edit(const String& InName, uint64& InOutData, uint32 InOffset = 0);
    bool Edit(const String& InName, Vec2F& InOutData, uint32 InOffset = 0);
    bool Edit(const String& InName, Vec3F& InOutData, uint32 InOffset = 0);
    bool Edit(const String& InName, Vec4F& InOutData, uint32 InOffset = 0);
    bool Edit(const String& InName, QuatF& InOutData, uint32 InOffset = 0);
    bool Edit(const String& InName, Mat4F& InOutData, uint32 InOffset = 0);
    bool Edit(const String& InName, String& InOutData, uint32 InOffset = 0);

    bool BeginList(const String& InName, uint32 InOffset);
    void EndList();
    void SameLine();
    bool AddButton(uint32 InOffset);
    bool RemoveButton(uint32 InOffset);
    
    template <class T>
    bool Edit(const String& InName, Vector<T>& InOutData, uint32 InOffset = 0)
    {
        bool edited = false; 
        if (BeginList(InName, InOffset))
        {
            // TODO: Fix naming and format
            for (auto& data : InOutData)
                if (Edit(("##" + InName).c_str(), data, InOffset))
                    edited = true;
            if (AddButton(InOffset))
                InOutData.emplace_back();
            SameLine();
            if (RemoveButton(InOffset))
                InOutData.pop_back();
            EndList();
        }
        return edited; 
    }
    
    template <class T>
    bool Edit(const String& InName, Set<T>& InOutData, uint32 InOffset = 0)
    {
        // Convert to array
        Vector<T> arr = Vector<T>(InOutData.begin(), InOutData.end());

        // Edit array
        const bool edited = Edit(InName, arr, InOffset);

        // Convert back
        if (edited)
            InOutData = Set<T>(arr.begin(), arr.end());
        
        return edited; 
    }
    
    template <class T, int Size>
    bool Edit(const String& InName, Array<T, Size>& InOutData, uint32 InOffset = 0)
    {
        bool edited = false; 
        if (BeginList(InName, InOffset))
        {
            for (auto& data : InOutData)
                if (Edit(("##" + InName).c_str(), data, InOffset))
                    edited = true;
            EndList();
        }
        return edited; 
    }

    template <class Key, class Val>
    bool Edit(const String& InName, Map<Key, Val>& InOutData, uint32 InOffset = 0)
    {
        bool edited = false; 
        if (BeginList(InName, InOffset))
        {
            for (auto& data : InOutData)
            {
                Key k = data.first;
                if (Edit("##Key_" + InName, k, InOffset))
                    edited = true;
                SameLine();
                if (Edit("##Val_" + InName, data.second, InOffset))
                    edited = true;
            }
            if (AddButton(InOffset))
                InOutData[Key()] = Val();
            SameLine();
            if (RemoveButton(InOffset))
                InOutData.clear();
            EndList();
        }
        return edited;
    }
}
