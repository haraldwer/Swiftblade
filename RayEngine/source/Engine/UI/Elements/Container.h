#pragma once
#include "Element.h"

namespace UI
{
    // A container that contains other UI elements
    class Container : public Element
    {
        friend class Builder;
        
        TYPE_INFO(Container, Element)
        
    public:
        Container() = default;
        Container(const Transform& InTransform) : Element(InTransform) {}

        void Init();
        void Update();
        void Draw();

        void Init(Container& InOwner) override;
        void Update(Container& InOwner) override;
        void Draw(Container& InOwner) override;
        bool Invalidated() const override;
        void RefreshRect(Container& InInstance, const Rect& InContainer) override;

        bool Contains(const String& InIdentifier) const { return TryGet<Element>(InIdentifier); }

        template <class T>
        ElementID Add(const T& InElement, const String& InIdentifier);
        void Remove(ElementID InID);

        template <class T>
        T& Get(ElementID InID);

        template <class T>
        const T& Get(ElementID InID) const;

        template <class T>
        T& Get(const String& InName);

        template <class T>
        const T& Get(const String& InName) const;

        Element& operator [](const String& InName) { return Get<Element>(InName); }
        const Element& operator [](const String& InName) const { return Get<Element>(InName); }

        template <class T>
        T* TryGet(const ElementID& InID);

        template <class T>
        const T* TryGet(const ElementID& InID) const;
        
        template <class T>
        T* TryGet(const String& InName);
        
        template <class T>
        const T* TryGet(const String& InName) const;
        
        ElementID GetID(const String& InName) const;

    protected:

        // Sorted list of children
        Vector<ElementID> children = {};
        Rect cachedRefRect = {};

    private:
        
        Map<ElementID, Object<Element>> elements = {};
        Map<String, ElementID> namedElements = {};
        ElementID idCounter = 0;
    };

    
    template <class T>
    ElementID Container::Add(const T& InElement, const String& InIdentifier)
    {
        idCounter++;
        CHECK_ASSERT(elements.contains(idCounter), "ID already exists");
        elements[idCounter] = InElement;
        auto& elem = elements[idCounter].Get<>();
        elem.id = idCounter;
        elem.parent = id;
        children.push_back(idCounter);
        if (!InIdentifier.empty())
        {
            CHECK_ASSERT(Contains(InIdentifier), "Element with this identifier already exists");
            namedElements[InIdentifier] = idCounter;
        }
        return idCounter;
    }

    template <class T>
    T& Container::Get(const ElementID InID)
    {
        if (InID == 0 && IsA(Type::Get<T>()))
            return *reinterpret_cast<T*>(this);
        auto find = elements.find(InID);
        CHECK_ASSERT(find == elements.end(), "Failed to find element");
        return find->second.Get<T>();
    }

    template <class T>
    const T& Container::Get(const ElementID InID) const
    {
        if (InID == 0 && IsA(Type::Get<T>()))
            return *reinterpret_cast<const T*>(this);
        auto find = elements.find(InID);
        CHECK_ASSERT(find == elements.end(), "Failed to find element");
        return find->second.Get<T>();
    }
    
    template <class T>
    T* Container::TryGet(const ElementID& InID)
    {
        if (InID == 0 && IsA(Type::Get<T>()))
            return reinterpret_cast<T*>(this);
        auto find = elements.find(InID);
        CHECK_RETURN(find == elements.end(), nullptr);
        CHECK_RETURN(!find->second.IsA<T>(), nullptr);
        return &find->second.Get<T>();
    }

    template <class T>
    const T* Container::TryGet(const ElementID& InID) const
    {
        if (InID == 0 && IsA(Type::Get<T>()))
            return reinterpret_cast<const T*>(this);
        const auto find = elements.find(InID);
        CHECK_RETURN(find == elements.end(), nullptr);
        CHECK_RETURN(!find->second.IsA<T>(), nullptr);
        return &find->second.Get<T>();
    }

    template <class T>
    T& Container::Get(const String& InName)
    {
        T* ptr = TryGet<T>(InName);
        CHECK_ASSERT(!ptr, "Failed to find element");
        return *ptr;
    }

    template <class T>
    const T& Container::Get(const String& InName) const
    {
        const T* ptr = TryGet<T>(InName);
        CHECK_ASSERT(!ptr, "Failed to find element");
        return *ptr;
    }

    template <class T>
    T* Container::TryGet(const String& InName)
    {
        ElementID elID = GetID(InName);
        if (elID != static_cast<ElementID>(-1))
            return TryGet<T>(elID);
        for (auto& elem : elements)
            if (elem.second.IsA<Container>())
                if (T* ptr = elem.second.Get<Container>().TryGet<T>(InName))
                    return ptr;
        return nullptr;
    }

    template <class T>
    const T* Container::TryGet(const String& InName) const
    {
        ElementID elID = GetID(InName);
        if (elID != static_cast<ElementID>(-1))
            return TryGet<T>(elID);
        for (const auto& elem : elements)
            if (elem.second.IsA<Container>())
                if (const T* ptr = elem.second.Get<Container>().TryGet<T>(InName))
                    return ptr;
        return nullptr;
    }
}
