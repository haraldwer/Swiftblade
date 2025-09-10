#pragma once

#include "Impl.h"

namespace Resource
{
    class BaseRef
    {
    public:
        virtual ~BaseRef() {}
    protected:
        Base* Register(const ID& InID);
        virtual Base* NewRes(const ID& InID) { return nullptr; };
    };
    
    template <class T, bool Editable = false>
    class Ref final : BaseRef
    {
    public:
        
        bool IsLoaded() const
        {
            return ptr && ptr->loaded;    
        }

        operator bool() const
        {
            return IsLoaded(); 
        }
        
        bool Unload()
        {
            CHECK_RETURN(!ptr, false);
            return ptr->Unload();  
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

        // Create resource from identifier
        Ref(const String& InID) : Ref(ID(InID)) { }

        // Create resource from identifier
        Ref(const ID& InID)
        {
            CHECK_RETURN(InID.Str().empty());
            ptr = reinterpret_cast<Impl<T, Editable>*>(Register(InID));
            Increment(); 
        }
        
        Ref() = default;
        
        virtual ~Ref()
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
            
            const String newID = Base::Pick(InName, currID);
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

        Base* NewRes(const ID& InID) override
        {
            return new Impl<T, Editable>(InID);
        }
        
        void Increment() const { if (ptr) ++ptr->count; }
        void Decrement() { if (ptr) --ptr->count; }
        
        Impl<T, Editable>* ptr = nullptr;
    };
}
