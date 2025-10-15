#pragma once
#include "Resource/Base.h"

namespace Rendering
{
    class ComputeShaderResourceBase : Resource::Base
    {
        CLASS_INFO(ComputeShaderResourceBase, Resource::Base)
        
    public:
        bool Load() override;

    protected:

        // Dispatch an array of structs
        void Dispatch(void* InPtr, int InSize);

        // Collect the same array!
        // Do some kind of type checking...
        void Collect(void* InPtr, int& InSize);
    
    private:

#ifdef GRAPHICS_API_OPENGL_43
        uint32 program = 0;
#endif
        
    };


    template <class T>
    class ComputeShaderResource : ComputeShaderResourceBase
    {
        CLASS_INFO(ComputeShaderResource, ComputeShaderResourceBase)

    public:
        void Dispatch(const Vector<T>& InData) {}
        void Collect(Vector<T>& OutData) {}
    };
}
