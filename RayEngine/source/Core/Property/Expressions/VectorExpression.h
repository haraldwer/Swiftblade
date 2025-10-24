#pragma once
#include "Expression.h"

// Abstraction
template <class T>
struct ExprVec : PropertyOwner<ExprVec<T>>
{
    bool Edit(const String& InName = "", uint32 InOffset = 0) override { return false; }
    
    virtual void Set(uint8 InKey, float InVal) const {}
    virtual void Declare(uint8 InKey, const String& InName) {}
    virtual void Remove(uint8 InKey) {}
    virtual void DeclareFunc(const String& InName, te_variant_type InFunc) {}
    virtual void RemoveFunc(const String& InName) {}
    virtual bool Compile() { return false; }
    virtual T Evaluate() const { return {}; }
};

struct ExprVec2 : ExprVec<Vec2F>
{
    ExprVec2() = default;
    ExprVec2(const String& InStr) : ExprVec()
    {
        x = InStr;
        y = InStr;
    }
    
    bool Edit(const String &InName, uint32 InOffset) override
    {
        ImGui::Text(InName.c_str());
        bool edited = false;
        edited |= x.Edit(InOffset);
        edited |= y.Edit(InOffset);
        return edited;
    }
    
    void Set(uint8 InKey, float InVal) const override
    {
        x.Get().Set(InKey, InVal);
        y.Get().Set(InKey, InVal);
    }
    
    void Declare(uint8 InKey, const String& InName) override
    {
        x.Get().Declare(InKey, InName);
        y.Get().Declare(InKey, InName);
    }
    
    void Remove(uint8 InKey) override
    {
        x.Get().Remove(InKey);    
        y.Get().Remove(InKey);    
    }
    
    void DeclareFunc(const String& InName, te_variant_type InFunc) override
    {
        x.Get().DeclareFunc(InName, InFunc);    
        y.Get().DeclareFunc(InName, InFunc);    
    }
    
    void RemoveFunc(const String& InName) override
    {
        x.Get().RemoveFunc(InName);    
        y.Get().RemoveFunc(InName);    
    }

    // Needs manual recompile after adding functions
    bool Compile() override
    {
        return
            x.Get().Compile() &&
            y.Get().Compile();
    }
    
    Vec2F Evaluate() const override
    {
        return {
            x.Get().Evaluate(),
            y.Get().Evaluate()
        };
    }
    
    PROPERTY(Expression, x);
    PROPERTY(Expression, y);
};

struct ExprVec3 : ExprVec<Vec3F>
{
    ExprVec3() = default;
    ExprVec3(const String& InStr) : ExprVec()
    {
        x = InStr;
        y = InStr;
        z = InStr;
    }

    bool Edit(const String &InName, uint32 InOffset) override
    {
        ImGui::Text(InName.c_str());
        bool edited = false;
        edited |= x.Edit(InOffset);
        edited |= y.Edit(InOffset);
        edited |= z.Edit(InOffset);
        return edited;
    }
    
    void Set(uint8 InKey, float InVal) const override
    {
        x.Get().Set(InKey, InVal);
        y.Get().Set(InKey, InVal);
        z.Get().Set(InKey, InVal);
    }
    
    void Declare(uint8 InKey, const String& InName) override
    {
        x.Get().Declare(InKey, InName);
        y.Get().Declare(InKey, InName);
        z.Get().Declare(InKey, InName);
    }
    
    void Remove(uint8 InKey) override
    {
        x.Get().Remove(InKey);    
        y.Get().Remove(InKey);    
        z.Get().Remove(InKey);    
    }
    
    void DeclareFunc(const String& InName, te_variant_type InFunc) override
    {
        x.Get().DeclareFunc(InName, InFunc);    
        y.Get().DeclareFunc(InName, InFunc);    
        z.Get().DeclareFunc(InName, InFunc);    
    }
    
    void RemoveFunc(const String& InName) override
    {
        x.Get().RemoveFunc(InName);    
        y.Get().RemoveFunc(InName);    
        z.Get().RemoveFunc(InName);    
    }

    // Needs manual recompile after adding functions
    bool Compile() override
    {
        return 
            x.Get().Compile() &&
            y.Get().Compile() &&
            z.Get().Compile();
    }
    
    Vec3F Evaluate() const override
    {
        return {
            x.Get().Evaluate(),
            y.Get().Evaluate(),
            z.Get().Evaluate()
        };
    }
    
    PROPERTY(Expression, x);
    PROPERTY(Expression, y);
    PROPERTY(Expression, z);
};

struct ExprVec4 : ExprVec<Vec4F>
{
    ExprVec4() = default;
    ExprVec4(const String& InStr) : ExprVec()
    {
        x = InStr;
        y = InStr;
        z = InStr;
        w = InStr;
    }

    bool Edit(const String &InName, uint32 InOffset) override
    {
        ImGui::Text(InName.c_str());
        bool edited = false;
        edited |= x.Edit(InOffset);
        edited |= y.Edit(InOffset);
        edited |= z.Edit(InOffset);
        edited |= w.Edit(InOffset);
        return edited;
    }
    
    void Set(uint8 InKey, float InVal) const override
    {
        x.Get().Set(InKey, InVal);
        y.Get().Set(InKey, InVal);
        z.Get().Set(InKey, InVal);
        w.Get().Set(InKey, InVal);
    }
    
    void Declare(uint8 InKey, const String& InName) override
    {
        x.Get().Declare(InKey, InName);
        y.Get().Declare(InKey, InName);
        z.Get().Declare(InKey, InName);
        w.Get().Declare(InKey, InName);
    }
    
    void Remove(uint8 InKey) override
    {
        x.Get().Remove(InKey);
        y.Get().Remove(InKey);
        z.Get().Remove(InKey);
        w.Get().Remove(InKey);
    }
    
    void DeclareFunc(const String& InName, te_variant_type InFunc) override
    {
        x.Get().DeclareFunc(InName, InFunc);    
        y.Get().DeclareFunc(InName, InFunc);    
        z.Get().DeclareFunc(InName, InFunc);    
        w.Get().DeclareFunc(InName, InFunc);    
    }
    
    void RemoveFunc(const String& InName) override
    {
        x.Get().RemoveFunc(InName);    
        y.Get().RemoveFunc(InName);    
        z.Get().RemoveFunc(InName);    
        w.Get().RemoveFunc(InName);    
    }

    // Needs manual recompile after adding functions
    bool Compile() override
    {
        return
            x.Get().Compile() &&
            y.Get().Compile() &&
            z.Get().Compile() &&
            w.Get().Compile();
    }
    
    Vec4F Evaluate() const override
    {
        return {
            x.Get().Evaluate(),
            y.Get().Evaluate(),
            z.Get().Evaluate(),
            w.Get().Evaluate()
        };
    }
    
    PROPERTY(Expression, x);
    PROPERTY(Expression, y);
    PROPERTY(Expression, z);
    PROPERTY(Expression, w);
};

