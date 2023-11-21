#pragma once
#include <string>

namespace ImGui
{
    class FileBrowser
    {
    public:
        enum class PickType
        {
            DIRECTORY,
            FILE
        };

        FileBrowser() = default; 
        inline static FileBrowser* Instance = nullptr;
        bool OpenInternal(const std::string& InPath, PickType InType, const std::set<std::string>& InExt);
        bool FetchInternal(std::string& OutPath);
    
    private:

        std::string GetLabel() const;
        static std::string GetRelative(const std::string& InPath);
        static std::string GetLocal(const std::string& InPath); 
        static std::string GetEditedPath(const std::string& InPath); 
        
        void Refresh();
        void RefreshHint();
        
        bool TryPopPath();
        bool TryApplyPath(const std::string& InString);
        
        void EditNavigation();
        void EditContent();
        
        bool IsOpen = false; 
        std::string OriginalPath;
        std::string Path;
        std::string NavigationGuess;
        std::string EditedPath;
        std::string Selected; 
        std::set<std::string> Ext;
        PickType Type = PickType::FILE;
        
        const float Width = 500.0f;
        const float Height = 400.0f;

        std::vector<std::string> Directories;
        std::vector<std::string> Files;
    };
    
    bool OpenFileBrowser(const std::string& InPath, FileBrowser::PickType InType = FileBrowser::PickType::FILE, const std::set<std::string>& InExt = {});
    bool FetchFileBrowserResult(std::string& OutPath);
}

