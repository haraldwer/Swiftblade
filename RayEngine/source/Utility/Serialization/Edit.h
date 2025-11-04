#ifndef EDIT_H
#define EDIT_H

#ifdef IMGUI_ENABLE

#include "ImGui/imgui.h"
#include "ImGui/imgui_stdlib.h"

namespace Utility
{
    String GetEditName(const String& InName, uint32 InOffset);
    bool BeginTable(const String& InName, uint32 InOffset);
    void EndTable(uint32 InOffset);
    
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
    void EditText(const String& InStr);

    inline bool Edit(const String& InName, bool& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            result = ImGui::Checkbox(GetEditName(InName, InOffset).c_str(), &InOutData);
            EndTable(InOffset);
        }
        return result;
    }

    inline bool Edit(const String& InName, float& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            result = ImGui::InputFloat(GetEditName(InName, InOffset).c_str(), &InOutData);
            EndTable(InOffset);
        }
        return result;
    }

    inline bool Edit(const String& InName, int32& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            result = ImGui::InputInt(GetEditName(InName, InOffset).c_str(), &InOutData);
            EndTable(InOffset);
        }
        return result;
    }

    inline bool Edit(const String& InName, uint8& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            int i = InOutData;
            result = ImGui::InputInt(GetEditName(InName, InOffset).c_str(), &i);
            InOutData = static_cast<uint8>(i);
            EndTable(InOffset);
        }
        return result; 
    }

    inline bool Edit(const String& InName, uint32& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            int i = static_cast<int>(InOutData);
            result = ImGui::InputInt(GetEditName(InName, InOffset).c_str(), &i);
            InOutData = static_cast<uint32>(i);
            EndTable(InOffset);
        }
        return result; 
    }

    inline bool Edit(const String& InName, uint64& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            int i = static_cast<int>(InOutData);
            result = ImGui::InputInt(GetEditName(InName, InOffset).c_str(), &i);
            InOutData = static_cast<uint32>(i);
            EndTable(InOffset);
        }
        return result; 
    }

    inline bool Edit(const String& InName, Vec2F& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            result = ImGui::InputFloat2(GetEditName(InName, InOffset).c_str(), &InOutData.data[0]);
            EndTable(InOffset);
        }
        return result;
    }

    inline bool Edit(const String& InName, Vec3F& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            result = ImGui::InputFloat3(GetEditName(InName, InOffset).c_str(), &InOutData.data[0]);
            EndTable(InOffset);
        }
        return result;
    }

    inline bool Edit(const String& InName, Vec4F& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            result = ImGui::InputFloat4(GetEditName(InName, InOffset).c_str(), &InOutData.data[0]);
            EndTable(InOffset);
        }
        return result;
    }

    inline bool Edit(const String& InName, Vec2I& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            result = ImGui::InputInt2(GetEditName(InName, InOffset).c_str(), &InOutData.data[0]);
            EndTable(InOffset);
        }
        return result;
    }

    inline bool Edit(const String& InName, Vec3I& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            result = ImGui::InputInt3(GetEditName(InName, InOffset).c_str(), &InOutData.data[0]);
            EndTable(InOffset);
        }
        return result;
    }

    inline bool Edit(const String& InName, Vec4I& InOutData, const uint32 InOffset = 0)
    {
        bool result = false;
        if (BeginTable(InName, InOffset))
        {
            result = ImGui::InputInt4(GetEditName(InName, InOffset).c_str(), &InOutData.data[0]);
            EndTable(InOffset);
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
            if (ImGui::InputFloat3(GetEditName(InName, InOffset).c_str(), &euler[0]))
            {
                InOutData = QuatF::FromEuler(euler *= Math::DegreesToRadians(1.0f));
                result = true; 
            }
            EndTable(InOffset);
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
            ImGui::InputText(GetEditName(InName, InOffset).c_str(), &InOutData);
            result = InOutData != copy;
            EndTable(InOffset);
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
                if (Button("^", InOffset + 1234 * off))
                    moveUp = off;
                SameLine();
                if (Button("-", InOffset + 1234 * off))
                    remove = off;
                SameLine();
                String offStr = std::to_string(off + 1);
                String name = offStr + "##" + InName + offStr; 
                if (Edit(name.c_str(), data, InOffset + 1234 * off))
                    edited = true;
                off++;
            }

            if (wasEmpty)
                ImGui::SameLine();
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
            
            EndSection();
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
        if (BeginSection(InName))
        {
            for (auto& data : InOutData)
                if (Edit(("##" + InName).c_str(), data, InOffset))
                    edited = true;
            EndSection();
        }
        return edited; 
    }

    template <class Key, class Val>
    bool Edit(const String& InName, Map<Key, Val>& InOutData, uint32 InOffset = 0)
    {
        bool edited = false; 
        if (BeginSection(InName))
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
                if (Edit("##Val_" + InName + idStr, data.second, InOffset))
                    edited = true;
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
            SameLine();
            if (Button("Clear##" + InName, InOffset))
            {
                InOutData.clear();
                idMap.clear();
                edited = true;
            }
            
            EndSection();
        }
        return edited;
    }

    template <class Key, class Val>
    bool Edit(const String& InName, OrderedMap<Key, Val>& InOutData, uint32 InOffset = 0)
    {
        bool edited = false; 
        if (BeginSection(InName))
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
                if (Edit("##Val_" + InName + idStr, data.second, InOffset))
                    edited = true;
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
            SameLine();
            if (Button("Clear##" + InName, InOffset))
            {
                InOutData.clear();
                idMap.clear();
                edited = true;
            }
            
            EndSection();
        }
        return edited;
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