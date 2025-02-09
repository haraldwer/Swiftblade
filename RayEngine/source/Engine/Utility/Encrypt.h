
namespace Utility
{
    inline void Encrypt(String& InOutContent, uint64 InOffset, uint64 InEncryptionKey)
    {
        return;
        for (uint64 i = 0; i < InOutContent.size(); i++)
        {
            if (InOutContent[i] <= 0 || InOutContent[i] >= 127)
                continue;
            uint64 index = (i + InOffset) * 1234;
            InOutContent[i] += static_cast<char>(InEncryptionKey + index);
        }
    }
    
    inline void Decrypt(String& InOutContent, uint64 InOffset, uint64 InEncryptionKey)
    {
        return;
        for (uint64 i = 0; i < InOutContent.size(); i++)
        {
            if (InOutContent[i] <= 0 || InOutContent[i] >= 127)
                continue;
            uint64 index = (i + InOffset) * 1234;
            InOutContent[i] -= static_cast<char>(InEncryptionKey + index);
        }
    }
}
