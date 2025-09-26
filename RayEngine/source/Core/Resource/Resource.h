#pragma once

#include "Impl.h"

namespace Resource
{
    class BaseRef
    {
    public:
        virtual ~BaseRef() {}
    protected:
        ImplBase* Register(const ID& InID);
        virtual ImplBase* New(const ID& InID) { return nullptr; }
    };
    
    template <class T>
    class Ref final : BaseRef
    {
    public:
        
        // Create resource from identifier
        Ref(const String& InID) : Ref(ID(InID)) { }

        // Create resource from identifier
        Ref(const ID& InID)
        {
            CHECK_RETURN(InID.Str().empty());
            ptr = reinterpret_cast<Impl<T>*>(Register(InID));
            Increment(); 
        }
        
        Ref() = default;

        ~Ref() override
        {
            Decrement();
        }

        Ref(Ref&& InOther) noexcept
        {
            if (InOther.ptr == ptr)
                return;
            Decrement();
            ptr = InOther.ptr;
            InOther.ptr = nullptr;
        }
        
        Ref(const Ref& InOther)
        {
            *this = InOther;
        }
        
        Ref& operator=(const Ref& InPtr)
        {
            if (this != &InPtr)
            {
                Decrement();
                ptr = InPtr.ptr;
                Increment();
            }        
            return *this;
        }

        bool IsLoaded() const { return ptr && ptr->loaded; }
        operator bool() const { return IsLoaded(); }

        bool Reload()
        {
            if (ptr->loaded)
                ptr->Unload();
            return ptr->Load();
        }
        
        bool Unload()
        {
            if (ptr && ptr->loaded)
                return ptr->Unload();
            return false;
        }

        ID Identifier() const
        {
            return ptr ? ptr->id : ID(); 
        }

        T* Get() const
        {
            if (!IsLoaded() && ptr)
                ptr->Load(); // Try load
            if (IsLoaded())
                return &ptr->data;
            return nullptr;
        }

        T* GetData() const
        {
            CHECK_RETURN(!ptr, nullptr);
            return ptr->data;
        }
        
        void Serialize(SerializeObj& InOutObj) const
        {
            if (!ptr)
            {
                Utility::WriteValue(InOutObj, String());
                return;
            }
            Utility::WriteValue(InOutObj, ptr->id.Str());
        }
        
        bool Deserialize(const GenericVal& InObj)
        {
            String identifier;
            if (!Utility::ReadValue(InObj, identifier))
                return false;
            CHECK_RETURN(identifier.empty(), false)
            *this = Ref(identifier);
            return true; 
        }

        bool Edit(const String& InName, uint32 InOffset = 0)
        {
            // File picker
            const String currID = ptr ? ptr->id.Str() : "";
            const String newID = Pick(InName, currID);
            if (currID != newID)
            {
                *this = Ref(newID); 
                return true; 
            }
        
            // Inline editing
            if (IsLoaded())
                ptr->Edit(InName, InOffset);
            
            // TODO: Duplicate resource
            return false; 
        }

        bool operator==(const Ref& InOther) const
        {
            return InOther.ptr == ptr;
        }
        
    private:

        ImplBase* New(const ID& InID) override { return new Impl<T>(InID); }
        void Increment() const { if (ptr) ++ptr->count; }
        void Decrement() { if (ptr) --ptr->count; }
        
        Impl<T>* ptr = nullptr;
    };
}
