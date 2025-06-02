#pragma once

namespace Utility
{
    // This class is responsible for tracking refences
    struct BaseRef
    {
        uint32 count;
        uint32 weakCount; 
        BaseRef() : count(1), weakCount(0) {}
        virtual void destroy() = 0;
        virtual ~BaseRef() = default;

        // A map that tracks existing pointers
        inline static Map<void*, BaseRef*> refs;
    };

    template <class T>
    struct RefImpl : BaseRef
    {
        T* ptr = nullptr;
        RefImpl(T* InPtr) : ptr(InPtr) {}
        ~RefImpl() override = default;
        
        // Find in map or create
        static RefImpl* Get(T* InPtr)
        {
            auto find = refs.find(InPtr);
            if (find != refs.end() && find->second)
                return reinterpret_cast<RefImpl*>(find->second);
            RefImpl* ptr = new RefImpl(InPtr);
            refs[InPtr] = ptr;
            return ptr;
        }

        // Remove from map and destroy
        void destroy() override
        {
            refs.erase(ptr);
            delete ptr;
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
        ObjectPtr(T* InPtr) : ptr(InPtr), ref(RefImpl<T>::Get(InPtr)) { }
        ObjectPtr(const ObjectPtr& InS) : ptr(InS.Ptr), ref(InS.Ref) { Increment(); }
        ObjectPtr(ObjectPtr&& InS) noexcept
        {
            ptr = InS.Ptr;
            ref = InS.Ref;
            InS.Ptr = nullptr;
            InS.Ref = nullptr;
        }
        ~ObjectPtr() { Decrement(); }

        ObjectPtr& operator=(const ObjectPtr& InPtr)
        {
            if (this->Ref == InPtr.Ref)
                return *this;
            Decrement();
            ref = InPtr.Ref;
            ptr = InPtr.Ptr;
            Increment();
            return *this;
        }
        
        // Getters
        T* Get() const { return ptr; }
        T* operator->() const { return ptr; }
        T& operator*() const
        {
            CHECK_ASSERT(!ptr, "Dereference invalid ptr");
            return *ptr;
        }
        operator bool() const { return ptr; }
            
        uint32 RefCount() const { return ref ? ref->count : 0; }
        uint32 WeakRefCount() const { return ref ? ref->weakCount : 0; }
        
    private:

        void Increment() const
        {
            if (ref)
            {
                ref->count++;
            }
        }
        
        void Decrement() 
        {
            if (!ref)
                return; 
            CHECK_ASSERT(ref->count == 0, "Negative ref count");
            ref->count--; 
            if (!ref->count && !ref->weakCount)
            { 
                ref->destroy();
                delete ref;
                ref = nullptr;
            }
        }
        
        T* ptr = nullptr;
        BaseRef* ref = nullptr;
    };

    // Weak counted, requires ObjectPtr
    template <class T>
    class WeakPtr
    {
        friend ObjectPtr<T>;
        
    public:

        // Construct
        WeakPtr() = default;
        WeakPtr(T* InPtr) : ref(RefImpl<T>::Get(InPtr)) { Increment(); }
        WeakPtr(const WeakPtr& InPtr) : ref(InPtr.Ref) { Increment(); }
        WeakPtr(const ObjectPtr<T>& InPtr) : ref(InPtr.Ref) { Increment(); }
        WeakPtr(WeakPtr&& InPtr) noexcept : ref(InPtr.Ref)
        {
            ref = InPtr.Ref;
            InPtr.Ref = nullptr;
        }
        ~WeakPtr() { Decrement(); }
        
        WeakPtr& operator=(const WeakPtr& InPtr)
        {
            if (this->Ref == InPtr.Ref)
                return *this;
            Decrement();
            ref = InPtr.Ref;
            Increment();
            return *this;
        }

        // Getters
        T* Get() const
        {
            if (!ref || ref->count == 0)
                return nullptr; 
            return reinterpret_cast<RefImpl<T>*>(ref)->Ptr;
        }
        T* operator->() const { return Get(); }
        T& operator*() const
        {
            auto ptr = Get();
            CHECK_ASSERT(!ptr, "Dereference invalid ptr")
            return *ptr;
        }
        operator bool() const { return Get(); }

        uint32 RefCount() const { return ref ? ref->count : 0; }
        uint32 WeakRefCount() const { return ref ? ref->weakCount : 0; }
        
    private:

        void Increment() const
        {
            if (ref)
                ref->weakCount++;
        }
        
        void Decrement() 
        {
            if (!ref)
                return; 
            ref->weakCount--;
            CHECK_ASSERT(ref->weakCount < 0, "Negative weak ref count");
            if (!ref->count && !ref->weakCount)
            {
                ref->destroy();
                delete ref;
                ref = nullptr;
            }
        }
        
        BaseRef* ref = nullptr;
    };

    // TODO: Scoped pointer
}