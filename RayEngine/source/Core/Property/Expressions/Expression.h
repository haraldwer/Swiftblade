#pragma once

#include "Math/Expressions/tinyexpr.h"

struct Expression : PropertyOwner<Expression>
{
    ~Expression() override;
    Expression();
    Expression(const String& InExpression);
    Expression(const Expression& InOther);
    Expression(Expression&& InOther) noexcept;
    Expression& operator = (const Expression& InOther);
    bool operator==(const Expression& InOther) const;

    void Set(uint8 InKey, float InVal) const;
    void Declare(uint8 InKey, const String& InName);
    void Remove(uint8 InKey);
    void DeclareFunc(const String& InName, te_variant_type InFunc);
    void RemoveFunc(const String& InName);
    bool Compile(); // Needs manual recompile after adding functions
    float Evaluate() const;
    
    bool Edit(const String& InName, uint32 InOffset) override;
    bool CustomDeserialize(const DeserializeObj& InObj) override;

private:
    void CacheGraph();
    void DrawGraph();
    
    PROPERTY(String, Expr);

    // TODO: Provide a library of default functions
    
    te_parser* parser = nullptr;

    struct Var
    {
        float* val;
        String name;
    };
    Map<uint8, Var> vars;
    Map<String, te_variant_type> funcs;

    uint8 graphDefault = 0;
    Vector<float> cachedGraph;
    static constexpr int graphPrecision = 100;
};

