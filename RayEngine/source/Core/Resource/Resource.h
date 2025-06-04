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

        String Identifier() const
        {
            return ptr ? ptr->identifier : ""; 
        }

        T* Get() const
        {
            if (!IsLoaded() && ptr)
                ptr->Load(); // Try load
            if (IsLoaded())
                return &ptr->data;
            return nullptr;
        }
        
        // Ref counting
        Ref() = default;
        Ref(const Ref& InOther) { *this = InOther; }
        Ref(Ref&& InOther)
        {
            ptr = InOther.ptr;
            InOther.ptr = nullptr;
        }
        
        virtual ~Ref() { Decrement(); }
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
            Utility::WriteValue(InOutObj, ptr->identifier);
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
            const String currID = ptr ? ptr->identifier : "";
            
            const String newID = Base::Pick(InName, currID);
            if (currID != newID)
            {
                *this = Ref(newID); 
                return true; 
            }
        
            // Inline editing
            if (Editable)
            {
                 if (T* res = Get())
                {
                    if (res->Edit(InName, InOffset))
                    {
                        res->Save(currID);
                        ptr->TryHotReload(); 
                    }
                }
            }
            
            // TODO: Duplicate resource
            return false; 
        }

        bool operator==(const Ref& InOther) const
        {
            return InOther.ptr == ptr;
        }
        
    private:
        
        void Increment() const { if (ptr) ptr->count++; }
        void Decrement() { if (ptr) ptr->count--; }
        
        Impl<T>* ptr = nullptr;
    };
}
