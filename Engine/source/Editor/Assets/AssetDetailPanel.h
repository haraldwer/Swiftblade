#pragma once

namespace Editor
{
    class AssetDetailPanel
    {
        BASE_CLASS_INFO(AssetDetailPanel);

        friend class AssetDetails;
        
    public:
        virtual ~AssetDetailPanel() = default;
        AssetDetailPanel(const String& InPath) : path(InPath) {}
        virtual void Init() {}
        virtual void Deinit() {}
        virtual void Draw();
        virtual bool AllowRawEdit() const { return false; }

        static bool Accept(const String& InPath) { return true; }

        void DrawFileInfo();
        bool RawTextEdit(bool InSubwindow = false);

    protected:
        String path;
        String content;
        bool rawEditing = false;

        bool infoSet = false;
        uint32 size;
        Utility::Timepoint lastWriteTime;
    };
}
