#pragma once

#include "Impl.h"
#include "Manager.h"

#include "Utility/Serialization/Serialize.h"
#include "Utility/Serialization/Deserialize.h"

namespace Resource
{
    template <class T>
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
            Ptr = res;
            Increment(); 
        }
        
        bool IsLoaded() const
        {
            return Ptr && Ptr->Loaded;    
        }

        String Identifier() const
        {
            return Ptr ? Ptr->Identifier : ""; 
        }

        T* Get() const
        {
            if (!IsLoaded() && Ptr)
                Ptr->Load(); // Try load
            if (IsLoaded())
                return &Ptr->Data;
            return nullptr;
        }
        
        // Ref counting
        Ref() = default;
        Ref(const Ref& InOther) { *this = InOther; }
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

        void Serialize(SerializeObj& InOutObj) const
        {
            CHECK_RETURN(!Ptr); 
            Utility::WriteValue(InOutObj, Ptr->Identifier);
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

        bool Edit(const String& InName)
        {
            // File picker?
            const String currID = Ptr ? Ptr->Identifier : "";
            const String newID = Base::Pick(InName, currID);
            if (currID != newID)
            {
                *this = Ref(newID); 
                return true; 
            }
            return false; 
        }

        bool operator==(const Ref& InOther) const
        {
            return InOther.Ptr == Ptr;
        }
        
    private:
        
        void Increment() const { if (Ptr) Ptr->Count++; }
        void Decrement() { if (Ptr) Ptr->Count--; }
        
        Impl<T>* Ptr = nullptr;
    };
}
