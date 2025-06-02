#pragma once

#include "Impl.h"
#include "Manager.h"

namespace Resource
{
    template <class T, bool Editable = false>
    class Ref
    {
        
    public:
        
        // Create resource from identifier
        Ref(const String& InIdentifier)
        {
            CHECK_RETURN(InIdentifier.empty());
            Manager& man = Manager::Get();
            auto* res = reinterpret_cast<Impl<T>*>(man.GetResource(InIdentifier));
            if (!res)
            {
                res = new Impl<T>(InIdentifier);
                man.Register(res, InIdentifier); 
            }
            ptr = res;
            Increment(); 
        }
        
        bool IsLoaded() const
        {
            return ptr && ptr->Loaded;    
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

        String Identifier() const
        {
            return ptr ? ptr->Identifier : ""; 
        }

        T* Get() const
        {
            if (!IsLoaded() && ptr)
                ptr->Load(); // Try load
            if (IsLoaded())
                return &ptr->Data;
            return nullptr;
        }
        
        // Ref counting
        Ref() = default;
        Ref(const Ref& InOther) { *this = InOther; }
        Ref(Ref&& InOther)
        {
            ptr = InOther.Ptr;
            InOther.Ptr = nullptr;
        }
        
        virtual ~Ref() { Decrement(); }
        Ref& operator=(const Ref& InPtr)
        {
            if (this != &InPtr)
            {
                Decrement();
                ptr = InPtr.Ptr;
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
            Utility::WriteValue(InOutObj, ptr->Identifier);
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
            const String currID = ptr ? ptr->Identifier : "";
            
            const String newID = Base::Pick(InName, currID);
            if (currID != newID)
            {
                *this = Ref(newID); 
                return true; 
            }
        
            // Inline editing
            if (Editable)
            {
                 if (T* ptr = Get())
                {
                    if (ptr->Edit(InName, InOffset))
                    {
                        ptr->Save(currID);
                        ptr->TryHotReload(); 
                    }
                }
            }
            
            // TODO: Duplicate resource
            return false; 
        }

        bool operator==(const Ref& InOther) const
        {
            return InOther.Ptr == ptr;
        }
        
    private:
        
        void Increment() const { if (ptr) ptr->Count++; }
        void Decrement() { if (ptr) ptr->Count--; }
        
        Impl<T>* ptr = nullptr;
    };
}
