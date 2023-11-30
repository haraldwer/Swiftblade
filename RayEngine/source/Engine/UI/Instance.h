#pragma once

namespace UI
{
    class Element;
    class Container;

    class Instance
    {
        friend class Builder; 
    public:
        void Init();
        void Update(); 
        void Draw();

        template <class T = UI::Element>
        T& Get(const String& InIdentifier)
        {
            const auto find = NamedElements.find(InIdentifier);
            CHECK_ASSERT(find == NamedElements.end(), "Entry does not exist")
            const auto ptr = find->second.Get();
            CHECK_ASSERT(!ptr, "Invalid ptr");
            return *reinterpret_cast<T*>(ptr); 
        }
        
    private:
        ObjectPtr<Container> Root;
        Map<String, WeakPtr<Element>> NamedElements;
    };
}
