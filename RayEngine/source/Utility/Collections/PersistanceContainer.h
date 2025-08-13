#pragma once

namespace Utility
{
    template <class IDType>
    class PersistanceContainer
    {
    public:
        
        void Begin()
        {
            removed.clear();
            removed.insert(modified.begin(), modified.end());
            removed.insert(unchanged.begin(), unchanged.end());
            
            modified.clear();
            unchanged.clear();
            added.clear();
        }
        
        void Touch(IDType InID)
        {
            Update(InID);
            if (!modified.contains(InID))
                unchanged.insert(InID);
        }

        void Modify(IDType InID)
        {
            Update(InID);
            unchanged.erase(InID);
            modified.insert(InID);
        }

        const Set<IDType>& GetRemoved() const { return removed; }
        const Set<IDType>& GetModified() const { return modified; }
        const Set<IDType>& GetUnchanged() const { return modified; }
        const Set<IDType>& GetAdded() const { return added; }

        bool WasRemoved(IDType InType) { return removed.contains(InType); }
        bool WasModified(IDType InType) { return modified.contains(InType); }
        bool WasUnchanged(IDType InType) { return unchanged.contains(InType); }
        bool WasAdded(IDType InType) { return added.contains(InType); }

        bool Changed() const
        {
            return
                removed.empty() &&
                modified.empty() &&
                added.empty();
        }

    private:

        void Update(IDType InID)
        {
            if (modified.contains(InID) ||
                unchanged.contains(InID))
                return;
            
            if (removed.contains(InID))
                removed.erase(InID);
            else
                added.insert(InID);
        }
        
        Set<IDType> removed;
        Set<IDType> modified;
        Set<IDType> unchanged;
        Set<IDType> added;
    };
}
