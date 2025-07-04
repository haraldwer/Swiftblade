﻿#ifndef EDIT_H
#define EDIT_H

//#pragma once

#include "ImGui/imgui.h"
#include "ImGui/imgui_stdlib.h"

namespace Utility
{
    String GetEditName(const String& InName, uint32 InOffset);

    bool MaybeCollapse(const String& InName, uint32 InOffset, bool& OutHeader);
    bool Button(const String& InName, uint32 InOffset);
    bool BeginSection(const String& InName);
    void EndSection();
    void SameLine();
    bool AddButton(uint32 InOffset);
    bool RemoveButton(uint32 InOffset);
    void Separator();

    inline bool Edit(const String& InName, bool& InOutData, const uint32 InOffset = 0)
    {
        return ImGui::Checkbox(GetEditName(InName, InOffset).c_str(), &InOutData);
    }

    inline bool Edit(const String& InName, float& InOutData, const uint32 InOffset = 0)
    {
        return ImGui::InputFloat(GetEditName(InName, InOffset).c_str(), &InOutData);
    }

    inline bool Edit(const String& InName, int32& InOutData, const uint32 InOffset = 0)
    {
        return ImGui::InputInt(GetEditName(InName, InOffset).c_str(), &InOutData);
    }

    inline bool Edit(const String& InName, uint8& InOutData, const uint32 InOffset = 0)
    {
        int i = InOutData;
        const bool result = ImGui::InputInt(GetEditName(InName, InOffset).c_str(), &i);
        InOutData = static_cast<uint8>(i); 
        return result; 
    }

    inline bool Edit(const String& InName, uint32& InOutData, const uint32 InOffset = 0)
    {
        int i = static_cast<int>(InOutData);
        const bool result = ImGui::InputInt(GetEditName(InName, InOffset).c_str(), &i);
        InOutData = static_cast<uint32>(i); 
        return result; 
    }

    inline bool Edit(const String& InName, uint64& InOutData, const uint32 InOffset = 0)
    {
        int i = static_cast<int>(InOutData);
        const bool result = ImGui::InputInt(GetEditName(InName, InOffset).c_str(), &i);
        InOutData = static_cast<uint32>(i); 
        return result; 
    }

    inline bool Edit(const String& InName, Vec2F& InOutData, const uint32 InOffset = 0)
    {
        return ImGui::InputFloat2(GetEditName(InName, InOffset).c_str(), &InOutData.data[0]);
    }

    inline bool Edit(const String& InName, Vec3F& InOutData, const uint32 InOffset = 0)
    {
        return ImGui::InputFloat3(GetEditName(InName, InOffset).c_str(), &InOutData.data[0]);
    }

    inline bool Edit(const String& InName, Vec4F& InOutData, const uint32 InOffset = 0)
    {
        return ImGui::InputFloat4(GetEditName(InName, InOffset).c_str(), &InOutData.data[0]);
    }

    inline bool Edit(const String& InName, QuatF& InOutData, const uint32 InOffset = 0)
    {
        Vec3F euler = InOutData.Euler();
        euler *= Math::RadiansToDegrees(1.0f);
        if (ImGui::InputFloat3(GetEditName(InName, InOffset).c_str(), &euler[0]))
        {
            InOutData = QuatF::FromEuler(euler *= Math::DegreesToRadians(1.0f));
            return true; 
        } 
        return false;  
    }

    inline bool Edit(const String& InName, Mat4F& InOutData, uint32 InOffset)
    {
        // TODO: Break down, edit individually
        return false; 
    }

    inline bool Edit(const String& InName, String& InOutData, const uint32 InOffset)
    {
        const String copy = InOutData;
        ImGui::InputText(GetEditName(InName, InOffset).c_str(), &InOutData);
        return InOutData != copy;
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
}

#endif