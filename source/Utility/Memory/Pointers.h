#pragma once

namespace Utility
{
    // This class is responsible for tracking refences
    struct BaseRef
    {
        uint32 Count;
        uint32 WeakCount; 
        BaseRef() : Count(1), WeakCount(0) {}
        virtual void destroy() = 0;
        virtual ~BaseRef() = default;
    };

    template <class T>
    struct RefImpl : BaseRef
    {
        T* Ptr = nullptr;
        RefImpl(T* InPtr) : Ptr(InPtr) {}
        void destroy() override { delete Ptr; }
    };
    
    // Reference counted
    template<class T>
    class ObjectPtr
    {
    public:

        ObjectPtr() = default;
        ObjectPtr(T* InPtr) : Ref(new RefImpl<T>(InPtr)), Ptr(InPtr) {}
        ObjectPtr(const ObjectPtr& s) : Ref(s.Ref), Ptr(s.Ptr) { Increment(); }
        ~ObjectPtr() { Decrement(); }

        ObjectPtr& operator=(const ObjectPtr& InPtr)
        {
            if (this!=&InPtr)
            {
                Decrement();
                Ptr = InPtr.Ptr;
                Ref = InPtr.Ref;
                Increment();
            }        
            return *this;
        }
        
        // Getters
        T* Get() const { return Ptr; }
        T* operator->() const { return Ptr; }
        T& operator*() const { return *Ptr; }
        
        uint32 RefCount() const { return Ref ? Ref->Count : 0; }
        uint32 WeakRefCount() const { return Ref ? Ref->WeakCount : 0; }
        
    private:

        void Increment() const
        {
            CHECK_ASSERT(!Ref, "Ref invalid on increment")
            Ref->Count++;
        }
        
        void Decrement() 
        {
            if (!Ref)
                return; 
            Ref->Count--;
            if (!Ref->Count && !Ref->WeakCount)
            {
                Ref->destroy();
                delete Ref;
                Ref = nullptr;
            }
        }
        
        T* Ptr = nullptr;
        RefImpl<T>* Ref = nullptr;
    };

    // Weak counted, requires ObjectPtr
    template <class T>
    class WeakPtr
    {
        friend class ObjectPtr<T>;
        
    public:

        // Construct
        WeakPtr() = default;
        WeakPtr(const ObjectPtr<T>& InPtr) : Ref(InPtr.Ref), Ptr(InPtr.Ptr) { Increment(); }
        WeakPtr(const WeakPtr& InPtr) : Ref(InPtr.Ref), Ptr(InPtr.Ptr) { Increment(); }
        ~WeakPtr() { Decrement(); }
        
        WeakPtr& operator=(const WeakPtr& InPtr)
        {
            if (this!=&InPtr)
            {
                Decrement();
                Ptr = InPtr.Ptr;
                Ref = InPtr.Ref;
                Increment();
            }        
            return *this;
        }

        // Getters
        T* Get() const
        {
            if (!Ref || Ref->Count == 0)
                return nullptr; 
            return Ptr;
        }
        T* operator->() const { return Get(); }

        uint32 RefCount() const { return Ref ? Ref->Count : 0; }
        uint32 WeakRefCount() const { return Ref ? Ref->WeakCount : 0; }
        
    private:

        void Increment() const
        {
            CHECK_ASSERT(!Ref, "Ref invalid on increment")
            Ref->WeakCount++;
        }
        
        void Decrement() 
        {
            CHECK_ASSERT(!Ref, "Ref invalid on decement")
            Ref->WeakCount--;
            if (!Ref->Count && !Ref->WeakCount)
            {
                Ref->destroy();
                delete Ref;
                Ref = nullptr;
            }
        }
        
        T* Ptr = nullptr;
        BaseRef* Ref = nullptr;
    };

    // TODO: Scoped pointer
}