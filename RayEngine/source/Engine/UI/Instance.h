#pragma once

namespace UI
{
    class Container;

    // The instance that the builder will produce
    class Instance
    {
        friend class Builder; 
    public:
        void Init();
        void Update(); 
        void Draw();
    private:
        ObjectPtr<Container> Root;
    };
}
