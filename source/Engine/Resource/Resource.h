#pragma once

#include "Impl.h"
#include "Manager.h"

#include "Utility/Serialization/Serialize.h"
#include "Utility/Serialization/Deserialize.h"
#include "Utility/Serialization/Edit.h"

namespace Resource
{
    template <class T>
    class Ref
    {
        
    public:
        
        // Create resource from identifier
        Ref(const String& InIdentifier)
        {
            CHECK_RETURN(InIdentifier == "");
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
            *this = Ref(identifier);
            return true; 
        }

        bool Edit(const String& InName)
        {
            // File picker
            String id = Ptr ? Ptr->Identifier : "";
            const bool result = Utility::Edit(InName, id);
            if (id != "" || IsLoaded())
                *this = Ref(id);
            return result; 
        }
        
    private:
        
        void Increment() const { if (Ptr) Ptr->Count++; }
        void Decrement() { if (Ptr) Ptr->Count--; }
        
        Impl<T>* Ptr = nullptr;
    };
}
