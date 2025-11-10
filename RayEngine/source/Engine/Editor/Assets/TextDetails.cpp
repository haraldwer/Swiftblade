#include "TextDetails.h"

#include "File/File.h"

void TextDetails::Init()
{
    content = Utility::ReadFile(path);
}

void TextDetails::Draw()
{
    DrawFileInfo();
    RawTextEdit();
}

bool TextDetails::Accept(const String &InPath)
{
    if (InPath.ends_with(".txt") ||
        InPath.ends_with(".json") ||
        InPath.ends_with(".ini"))
        return true;
    if (Utility::Filename(InPath).starts_with("SH_") &&
            (InPath.ends_with(".fs") ||
            InPath.ends_with(".ds") ||
            InPath.ends_with(".vs") ||
            InPath.ends_with(".si")))
        return true;
    return false;
}
