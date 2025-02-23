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
        return InOutData.Edit(InName, InOffset);
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
            Map<Key, Key> remappings;
            Vector<Key> removed;
            static Map<String, Map<Key, uint32>> persistance; // Name -> id -> key
            auto& idMap = persistance[InName];
            static uint32 idC = 0;
            
            for (auto& data : InOutData)
            {
                // Add id
                if (!idMap.contains(data.first))
                {
                    idMap[data.first] = idC;
                    idC++;
                }
                
                const uint32 id = idMap[data.first];
                const String idStr = std::to_string(id);

                if (Button("-##" + idStr, InOffset))
                    removed.push_back(data.first);
                SameLine();
                Key k = data.first;
                if (Edit("##Key_" + InName + idStr, k, InOffset))
                    remappings[data.first] = k;
                SameLine();
                if (Edit("##Val_" + InName + idStr, data.second, InOffset))
                    edited = true;
            }

            for (auto& rem : removed)
            {
                CHECK_CONTINUE(!InOutData.contains(rem));
                InOutData.erase(rem);
                idMap.erase(rem);
            }
                
            // Remap edited entries
            for (auto& remap : remappings)
            {
                CHECK_CONTINUE(remap.second == Key());
                CHECK_CONTINUE(InOutData.contains(remap.second));
                CHECK_CONTINUE(!InOutData.contains(remap.first));
                InOutData[remap.second] = InOutData[remap.first];
                InOutData.erase(remap.first);
                idMap[remap.second] = idMap[remap.first];
                idMap.erase(remap.first);
                edited = true;
            }
            
            if (AddButton(InOffset))
            {
                if (!InOutData.contains(Key()))
                {
                    InOutData[Key()] = Val();
                    edited = true;
                }
                
            }
            SameLine();
            if (Button("Clear", InOffset))
            {
                InOutData.clear();
                idMap.clear();
                edited = true;
            }
            EndList();
        }
        return edited;
    }
}
