#pragma once

#include "Impl.h"
#include "Manager.h"

#include "Utility/Serialization.h"

namespace Resource
{
    template <class T>
    class Ref
    {
        
    public:
        
        // Create resource from identifier
        Ref(const String& InIdentifier)
        {
            Manager& man = Manager::Get();
            auto* res = static_cast<Impl<T>*>(man.GetResource(InIdentifier));
            if (!res)
            {
                res = new Impl<T>();
                res->Load(InIdentifier);
                man.Register(res, InIdentifier); 
            }
            Ptr = res;
            Increment(); 
        }
        
        bool IsLoaded() const
        {
            return Ptr && Ptr->Loaded;    
        }

        T* Get() const
        {
            if (IsLoaded())
                return &Ptr->Data;
            return nullptr;
        }
        
        // Ref counting
        Ref() = default;
        Ref(const Ref& s) : Ptr(s.Ptr) { Increment(); }
        virtual ~Ref() { Decrement(); }
        Ref& operator=(const Ref& InPtr)
        {
            if (this != &InPtr)
            {
                Decrement();
                Ptr = InPtr.Ptr;
                Increment();
            }        
            return *this;
        }

        void Serialize(SerializeObj& InOutObj, const String& InName) const
        {
            CHECK_RETURN(!Ptr); 
            Utility::Serialize(InOutObj, InName, Ptr->Identifier);
        }
        
        bool Deserialize(const DeserializeObj& InObj, const String& InName)
        {
            String identifier;
            if (!Utility::Deserialize(InObj, InName, identifier))
                return false;
            CHECK_RETURN(identifier.empty(), false)
            (*this) = Ref(identifier);
            return true; 
        }
        
    private:
        
        void Increment() const { if (Ptr) Ptr->Count++; }
        void Decrement() { if (Ptr) Ptr->Count--; }
        
        Impl<T>* Ptr = nullptr;
    };
}
