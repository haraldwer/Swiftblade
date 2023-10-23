#pragma once

#include "ResourceImpl.h"
#include "ResourceManager.h"

template <class T>
class Resource
{
    
public:
    
    // Create resource from identifier
    Resource(const String& InIdentifier)
    {
        auto& man = ResourceManager::Get();
        auto res = man.GetResource(InIdentifier);
        if (!res)
        {
            res = new ResourceImpl<T>();
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
    Resource() = default;
    Resource(const Resource& s) : Ptr(s.Ptr) { Increment(); }
    virtual ~Resource() { Decrement(); }
    Resource& operator=(const Resource& InPtr)
    {
        if (this != &InPtr)
        {
            Decrement();
            Ptr = InPtr.Ptr;
            Increment();
        }        
        return *this;
    }
    
private:
    
    void Increment() const { if (Ptr) Ptr->Count++; }
    void Decrement() { if (Ptr) Ptr->Count--; }
    
    ResourceImpl<T>* Ptr = nullptr;
};
