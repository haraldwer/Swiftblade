#pragma once

namespace Utility
{
    inline String GetEditName(const String& InName, uint32 InOffset)
    {
        return InName + "##PropertyEdit_" + std::to_string(InOffset);
    }

    bool MaybeCollapse(const String& InName, uint32 InOffset, bool& OutHeader);
    bool Button(const String& InName, uint32 InOffset);
    bool BeginList(const String& InName, uint32 InOffset);
    void EndList();
    void SameLine();
    bool AddButton(uint32 InOffset);
    bool RemoveButton(uint32 InOffset);
    void Separator();
    
    template <class T>
    bool Edit(const String& InName, T& InOutData, uint32 InOffset = 0)
    {
        bool header = false;
        if (MaybeCollapse(InName, InOffset, header))
        {
            bool result = InOutData.Edit(InName, InOffset);
            if (header)
                Separator();
            return result;
        }
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
    
    template <class T>
    bool Edit(const String& InName, Vector<T>& InOutData, uint32 InOffset = 0)
    {
        bool edited = false; 
        if (BeginList(InName, InOffset))
        {
            // TODO: Fix naming and format
            int off = 0;
            int moveUp = -1;
            for (auto& data : InOutData)
            {
                if (Button("^", off))
                    moveUp = off;
                SameLine();
                String offStr = std::to_string(off + 1);
                String name = offStr + "##" + InName + offStr; 
                if (Edit(name.c_str(), data, InOffset + 1234 * off))
                    edited = true;
                off++;
            }
            if (AddButton(InOffset))
            {
                InOutData.emplace_back();
                edited = true;
            }
            if (!InOutData.empty())
            {
                SameLine();
                if (RemoveButton(InOffset))
                {
                    InOutData.pop_back();
                    edited = true;
                }
            }
            EndList();

            if (moveUp > 0)
            {
                InOutData.insert(InOutData.begin() + moveUp - 1, InOutData.at(moveUp));
                InOutData.erase(InOutData.begin() + moveUp + 1);
                edited = true;
            }
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
