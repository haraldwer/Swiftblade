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
    
    // Reference counted
    template<class T>
    class ObjectPtr
    {
    public:

        ObjectPtr() = default;
        ObjectPtr(T* InPtr) : Ref(RefImpl<T>::Get(InPtr)), Ptr(InPtr) { }
        ObjectPtr(const ObjectPtr& s) : Ref(s.Ref), Ptr(s.Ptr) { Increment(); }
        ~ObjectPtr() { Decrement(); }

        ObjectPtr& operator=(const ObjectPtr& InPtr)
        {
            if (this != &InPtr)
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
                LOG("Increment, new count: " + std::to_string(Ref->Count));
            }
        }
        
        void Decrement() 
        {
            if (!Ref)
                return; 
            Ref->Count--;
            LOG("Decrement, new count: " + std::to_string(Ref->Count)); 
            CHECK_ASSERT(Ref->Count < 0, "Negative ref count");
            if (!Ref->Count && !Ref->WeakCount)
            {
                LOG("Destroying ref"); 
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
        ~WeakPtr() { Decrement(); }
        
        WeakPtr& operator=(const WeakPtr& InPtr)
        {
            if (this!=&InPtr)
            {
                Decrement();
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