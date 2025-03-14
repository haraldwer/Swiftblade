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

        // A map that tracks existing pointers
        inline static Map<void*, BaseRef*> Refs;
    };

    template <class T>
    struct RefImpl : BaseRef
    {
        T* Ptr = nullptr;
        RefImpl(T* InPtr) : Ptr(InPtr) {}
        ~RefImpl() override = default;
        
        // Find in map or create
        static RefImpl* Get(T* InPtr)
        {
            auto find = Refs.find(InPtr);
            if (find != Refs.end() && find->second)
                return reinterpret_cast<RefImpl*>(find->second);
            RefImpl* ptr = new RefImpl(InPtr);
            Refs[InPtr] = ptr;
            return ptr;
        }

        // Remove from map and destroy
        void destroy() override
        {
            Refs.erase(Ptr);
            delete Ptr;
        }
    };

    template<class T>
    class WeakPtr;
    
    // Reference counted
    template<class T>
    class ObjectPtr
    {
        friend WeakPtr<T>;
    public:

        ObjectPtr() = default;
        ObjectPtr(T* InPtr) : Ref(RefImpl<T>::Get(InPtr)), Ptr(InPtr) { }
        ObjectPtr(const ObjectPtr& s) : Ptr(s.Ptr), Ref(s.Ref) { Increment(); }
        ObjectPtr(ObjectPtr&& s) noexcept
        {
            Ptr = s.Ptr;
            Ref = s.Ref;
            s.Ptr = nullptr;
            s.Ref = nullptr;
        }
        ~ObjectPtr() { Decrement(); }

        ObjectPtr& operator=(const ObjectPtr& InPtr)
        {
            if (this->Ref == InPtr.Ref)
                return *this;
            Decrement();
            Ref = InPtr.Ref;
            Ptr = InPtr.Ptr;
            Increment();
            return *this;
        }
        
        // Getters
        T* Get() const { return Ptr; }
        T* operator->() const { return Ptr; }
        T& operator*() const
        {
            CHECK_ASSERT(!Ptr, "Dereference invalid ptr");
            return *Ptr;
        }
        operator bool() const { return Ptr; }
            
        uint32 RefCount() const { return Ref ? Ref->Count : 0; }
        uint32 WeakRefCount() const { return Ref ? Ref->WeakCount : 0; }
        
    private:

        void Increment() const
        {
            if (Ref)
            {
                Ref->Count++;
            }
        }
        
        void Decrement() 
        {
            if (!Ref)
                return; 
            CHECK_ASSERT(Ref->Count == 0, "Negative ref count");
            Ref->Count--; 
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

    // Weak counted, requires ObjectPtr
    template <class T>
    class WeakPtr
    {
        friend ObjectPtr<T>;
        
    public:

        // Construct
        WeakPtr() = default;
        WeakPtr(T* InPtr) : Ref(RefImpl<T>::Get(InPtr)) { Increment(); }
        WeakPtr(const WeakPtr& InPtr) : Ref(InPtr.Ref) { Increment(); }
        WeakPtr(const ObjectPtr<T>& InPtr) : Ref(InPtr.Ref) { Increment(); }
        WeakPtr(WeakPtr&& InPtr) noexcept : Ref(InPtr.Ref)
        {
            Ref = InPtr.Ref;
            InPtr.Ref = nullptr;
        }
        ~WeakPtr() { Decrement(); }
        
        WeakPtr& operator=(const WeakPtr& InPtr)
        {
            if (this->Ref == InPtr.Ref)
                return *this;
            Decrement();
            Ref = InPtr.Ref;
            Increment();
            return *this;
        }

        // Getters
        T* Get() const
        {
            if (!Ref || Ref->Count == 0)
                return nullptr; 
            return reinterpret_cast<RefImpl<T>*>(Ref)->Ptr;
        }
        T* operator->() const { return Get(); }
        T& operator*() const
        {
            auto ptr = Get();
            CHECK_ASSERT(!ptr, "Dereference invalid ptr")
            return *ptr;
        }
        operator bool() const { return Get(); }

        uint32 RefCount() const { return Ref ? Ref->Count : 0; }
        uint32 WeakRefCount() const { return Ref ? Ref->WeakCount : 0; }
        
    private:

        void Increment() const
        {
            if (Ref)
                Ref->WeakCount++;
        }
        
        void Decrement() 
        {
            if (!Ref)
                return; 
            Ref->WeakCount--;
            CHECK_ASSERT(Ref->WeakCount < 0, "Negative weak ref count");
            if (!Ref->Count && !Ref->WeakCount)
            {
                Ref->destroy();
                delete Ref;
                Ref = nullptr;
            }
        }
        
        BaseRef* Ref = nullptr;
    };

    // TODO: Scoped pointer
}