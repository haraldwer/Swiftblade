#ifndef EDIT_H
#define EDIT_H

#ifdef IMGUI_ENABLE

namespace Utility
{
    String GetEditName(const String& InName, uint32 InOffset);
    bool BeginTable(const String& InName, uint32 InOffset);
    void EndTable(const String& InName, uint32 InOffset);
    
    bool MaybeCollapse(const String& InName, uint32 InOffset, bool& OutHeader);
    bool Button(const String& InName, uint32 InOffset);
    bool BeginSection(const String& InName);
    void EndSection();
    void SameLine();
    bool AddButton(uint32 InOffset);
    bool RemoveButton(uint32 InOffset);
    void Separator();
    void Indent();
    void Unindent();
    void Label(const String& InStr);
    bool Checkbox(const String& InName, bool& InValue, uint32 InOffset);
    bool Int(const String& InName, int& InValue, uint32 InOffset);
    bool Int2(const String& InName, Vec2I& InValue, uint32 InOffset);
    bool Int3(const String& InName, Vec3I& InValue, uint32 InOffset);
    bool Int4(const String& InName, Vec4I& InValue, uint32 InOffset);
    bool Float(const String& InName, float& InValue, uint32 InOffset);
    bool Float2(const String& InName, Vec2F& InValue, uint32 InOffset);
    bool Float3(const String& InName, Vec3F& InValue, uint32 InOffset);
    bool Float4(const String& InName, Vec4F& InValue, uint32 InOffset);
    bool Text(const String& InName, String& InValue, uint32 InOffset);
    
    Vec2F GetCursorPos();
    void SetCursorPos(const Vec2F& InPos);
    float GetWindowWidth();
    void SetNextItemWidth(float InW);
    
    inline bool Edit(const String& InName, bool& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            result = Checkbox(InName, InOutData, InOffset);
            EndTable(InName, InOffset);
        }
        return result;
    }

    inline bool Edit(const String& InName, float& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            result = Float(InName, InOutData, InOffset);
            EndTable(InName, InOffset);
        }
        return result;
    }

    inline bool Edit(const String& InName, int32& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            result = Int(InName, InOutData, InOffset);
            EndTable(InName, InOffset);
        }
        return result;
    }

    inline bool Edit(const String& InName, uint8& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            int i = InOutData;
            result = Int(InName, i, InOffset);
            InOutData = static_cast<uint8>(i);
            EndTable(InName, InOffset);
        }
        return result; 
    }

    inline bool Edit(const String& InName, uint32& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            int i = static_cast<int>(InOutData);
            result = Int(InName, i, InOffset);
            InOutData = static_cast<uint32>(i);
            EndTable(InName, InOffset);
        }
        return result; 
    }

    inline bool Edit(const String& InName, uint64& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            int i = static_cast<int>(InOutData);
            result = Int(InName, i, InOffset);
            InOutData = static_cast<uint32>(i);
            EndTable(InName, InOffset);
        }
        return result; 
    }

    inline bool Edit(const String& InName, Vec2F& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            result = Float2(InName, InOutData, InOffset);
            EndTable(InName, InOffset);
        }
        return result;
    }

    inline bool Edit(const String& InName, Vec3F& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            result = Float3(InName, InOutData, InOffset);
            EndTable(InName, InOffset);
        }
        return result;
    }

    inline bool Edit(const String& InName, Vec4F& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            result = Float4(InName, InOutData, InOffset);
            EndTable(InName, InOffset);
        }
        return result;
    }

    inline bool Edit(const String& InName, Vec2I& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            result = Int2(InName, InOutData, InOffset);
            EndTable(InName, InOffset);
        }
        return result;
    }

    inline bool Edit(const String& InName, Vec3I& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            result = Int3(InName, InOutData, InOffset);
            EndTable(InName, InOffset);
        }
        return result;
    }

    inline bool Edit(const String& InName, Vec4I& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            result = Int4(InName, InOutData, InOffset);
            EndTable(InName, InOffset);
        }
        return result;
    }

    inline bool Edit(const String& InName, QuatF& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            Vec3F euler = InOutData.Euler();
            euler *= Math::RadiansToDegrees(1.0f);
            if (Float3(InName, euler, InOffset))
            {
                InOutData = QuatF::FromEuler(euler *= Math::DegreesToRadians(1.0f));
                result = true; 
            }
            EndTable(InName, InOffset);
        }
        return result;
    }

    inline bool Edit(const String& InName, Mat4F& InOutData, uint32 InOffset)
    {
        // TODO: Break down, edit individually
        return false; 
    }

    inline bool Edit(const String& InName, String& InOutData, const uint32 InOffset)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            const String copy = InOutData;
            Text(InName, InOutData, InOffset);
            result = InOutData != copy;
            EndTable(InName, InOffset);
        }
        return result;
    }

    template <class T>
    bool Edit(const String& InName, T& InOutData, uint32 InOffset = 0)
    {
        return InOutData.Edit(InName, InOffset);
    }
    
    template <class T>
    bool Edit(const String& InName, Vector<T>& InOutData, const uint32 InOffset = 0)
    {
        bool edited = false; 
        if (BeginSection(InName))
        {
            // TODO: Fix naming and format
            int off = 0;
            int moveUp = -1;
            int remove = -1;
            bool wasEmpty = InOutData.empty();
            for (auto& data : InOutData)
            {
                if (Button("-", InOffset + 1234 * off))
                    remove = off;
                SameLine();
                if (Button("^", InOffset + 1234 * off))
                    moveUp = off;
                SameLine();
                String offStr = std::to_string(off + 1);
                String name = offStr + "##" + InName + offStr; 
                if (Edit(name.c_str(), data, InOffset + 1234 * off))
                    edited = true;
                off++;
            }

            if (wasEmpty)
                SameLine();
            if (Button("+##" + InName, InOffset))
            {
                InOutData.emplace_back();
                edited = true;
            }
            
            if (!InOutData.empty())
            {
                SameLine();
                if (Button("Clear##" + InName, InOffset))
                {
                    InOutData.clear();
                    edited = true;
                }
            }
            
            if (moveUp > 0)
            {
                InOutData.insert(InOutData.begin() + moveUp - 1, InOutData.at(moveUp));
                InOutData.erase(InOutData.begin() + moveUp + 1);
                edited = true;
            }

            if (remove >= 0)
            {
                InOutData.erase(InOutData.begin() + remove);
                edited = true;
            }
            
        }
        EndSection();
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
        if (BeginSection(InName))
        {
            for (auto& data : InOutData)
                if (Edit(("##" + InName).c_str(), data, InOffset))
                    edited = true;
        }
        EndSection();
        return edited; 
    }

    template <class Key, class Val>
    bool Edit(const String& InName, Map<Key, Val>& InOutData, uint32 InOffset = 0)
    {
        bool edited = false; 
        if (BeginSection(InName + "##" + ToStr(InOffset)))
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
                const String idStr = InName + ToStr(id) + ToStr(InOffset);

                auto startPos = GetCursorPos();
                float width = GetWindowWidth() / 2;
                SetCursorPos({ startPos.x + width, startPos.y });
                if (Edit("##Val_" + idStr, data.second, InOffset + id))
                    edited = true;
                auto endPos = GetCursorPos();

                SetCursorPos(startPos);
                if (Button("-##Rem_" + idStr, InOffset + id))
                    removed.push_back(data.first);

                SameLine();
                SetNextItemWidth(width - GetCursorPos().x);
                Key k = data.first;
                if (Edit("##Key_" + idStr, k, 0))
                    remappings[data.first] = k;
                
                SetCursorPos(endPos);
            }

            for (auto& rem : removed)
            {
                CHECK_CONTINUE(!InOutData.contains(rem));
                InOutData.erase(rem);
                idMap.erase(rem);
                edited = true;
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
            
            if (Button("+##" + InName, InOffset))
            {
                if (!InOutData.contains(Key()))
                {
                    InOutData[Key()] = Val();
                    edited = true;
                }
            }

            if (!InOutData.empty())
            {
                SameLine();
                SetNextItemWidth(-1);
                if (Button("Clear##" + InName, InOffset))
                {
                    InOutData.clear();
                    idMap.clear();
                    edited = true;
                }
            }
        }
        EndSection();
        return edited;
    }

    template <class Key, class Val>
    bool Edit(const String& InName, OrderedMap<Key, Val>& InOutData, uint32 InOffset = 0)
    {
        Map<Key, Val> map = Map<Key, Val>(InOutData.begin(), InOutData.end());
        if (Edit(InName, map, InOffset))
        {
            InOutData = OrderedMap<Key, Val>(map.begin(), map.end());
            return true;
        }
        return false;
    }
}

#else

namespace Utility
{
    inline String GetEditName(const String& InName, uint32 InOffset) { return ""; }
    inline bool BeginTable(const String& InName, uint32 InOffset) { return false;}
    inline void EndTable(uint32 InOffset) {}
    
    inline bool MaybeCollapse(const String& InName, uint32 InOffset, bool& OutHeader) { return false;}
    inline bool Button(const String& InName, uint32 InOffset) { return false;}
    inline bool BeginSection(const String& InName) { return false;}
    inline void EndSection() {}
    inline void SameLine() {}
    inline bool AddButton(uint32 InOffset) { return false;}
    inline bool RemoveButton(uint32 InOffset) { return false;}
    inline void Separator() {}
    inline void Indent() {}
    inline void Unindent() {}
    inline void EditText(const String& InStr) {}

    template <class T>
    bool Edit(const String& InName, T& InOutData, const uint32 InOffset = 0) { return false; }
}

#endif

#endif