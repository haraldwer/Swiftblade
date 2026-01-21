#pragma once

namespace Utility
{
    // Base type for obj ptr
    class ObjPtrBase
    {
    public:
        // Virtual destructor for type overriding
        virtual ~ObjPtrBase()
        {
            CHECK_ASSERT(ptr, "Object not destroyed properly, call Destroy()");
        }
        
        virtual void Destroy()
        {
            CHECK_ASSERT(true, "Virtual destroy function not resolved");
        }
        
        virtual ObjPtrBase* Copy()
        {
            CHECK_ASSERT(true, "Virtual copy function not resolved");
            return nullptr;
        }
        
        virtual Type GetType()
        {
            CHECK_ASSERT(true, "Virtual type function not resolved");
            return Type();
        }

        virtual bool IsA(Type InType)
        {
            CHECK_ASSERT(true, "Virtual type checking not resolved")
            return false;
        }
        
        void* Get() const { return ptr; }

    protected:
        void* ptr = nullptr;
    };

    // Wrapper for destructor overriding
    template <class T>
    class ObjPtr final : ObjPtrBase
    {
    public:
        ObjPtr() { ptr = new T(); }
        ObjPtr(const T& InData) { ptr = new T(InData); }
        
        ~ObjPtr() override
        {
            CHECK_ASSERT(ptr, "Object not destroyed properly, call Destroy()");
        }
        
        void Destroy() override
        {
            CHECK_ASSERT(!ptr, "Invalid ptr");
            T* tPtr = static_cast<T*>(ptr);
            delete tPtr;
            ptr = nullptr;
        }

        ObjPtrBase* Copy() override
        {
            CHECK_ASSERT(!ptr, "Invalid ptr")
            T* tPtr = static_cast<T*>(ptr);
            return new ObjPtr(*tPtr);
        }

        Type GetType() override { return T::GetType(); }
        bool IsA(Type InType) override { return T::IsType(InType); }
    };

    // This scoped object can own an abstract type
    // And will be destroyed when out of scope
    // Basically just a normal object + inheritance!
    // The wrapped object has to include TYPE_INFO().
    template <class BaseT>
    class Object
    {
    public:
        Object()
        {
            // Default constructor, create default object
            auto ptr = new ObjPtr<BaseT>();
            obj = reinterpret_cast<ObjPtrBase*>(ptr);
        }

        template <class T>
        Object(const T& InData)
        {
            // Specialized constructor
            auto ptr = new ObjPtr<T>(InData);
            obj = reinterpret_cast<ObjPtrBase*>(ptr);
        }

        
        Object(const Object& InOther)
        {
            // Copy from another object
            CHECK_ASSERT(!InOther.obj, "Invalid object");
            obj = InOther.obj->Copy();
        }

        Object& operator = (const Object& InOther)
        {
            obj = InOther.obj->Copy();
            return *this;
        }

        Object(Object&& InOther) noexcept
        {
            // Move from another object
            obj = InOther.obj;
            InOther.obj = nullptr;
        }

        ~Object()
        {
            if (obj)
            {
                obj->Destroy();
                delete obj;
                obj = nullptr;
            }
        }

        template <class T = BaseT>
        T& Get()
        {
            CHECK_ASSERT(!IsA<T>(), "Invalid type");
            auto* basePtr = obj->Get();
            CHECK_ASSERT(!basePtr, "Invalid ptr");
            return *static_cast<T*>(basePtr);
        }

        template <class T = BaseT>
        const T& Get() const
        {
            CHECK_ASSERT(!IsA<T>(), "Invalid type");
            auto* basePtr = obj->Get();
            CHECK_ASSERT(!basePtr, "Invalid ptr");
            return *static_cast<T*>(basePtr);
        }

        template <class T = BaseT>
        operator T&() { return Get<T>(); }

        template <class T = BaseT>
        operator const T&() const { return Get<T>(); }

        template <class T>
        bool IsA() const
        {
            CHECK_ASSERT(!obj, "Invalid object")
            return obj->IsA(Type::Get<T>());
        }

        Type GetType() const
        {
            CHECK_ASSERT(!obj, "Invalid object");
            return obj->GetType();
        } 

    private:
        ObjPtrBase* obj = nullptr;
    };
}
