#include "Expression.h"

#include "Math/Expressions/tinyexpr.h"

Expression::~Expression()
{
    if (parser)
    {
        delete parser;
        parser = nullptr;
    }
}

Expression::Expression()
{
    Compile();
}

Expression::Expression(const String& InExpression)
{
    Expr = InExpression;
    Compile();
}

Expression::Expression(const Expression& InOther)
{
    *this = InOther;
}

Expression::Expression(Expression&& InOther) noexcept
{
    Expr = InOther.Expr;
    parser = InOther.parser;
    vars = InOther.vars;
    funcs = InOther.funcs;

    InOther.Expr = String();
    InOther.parser = nullptr;
    InOther.vars.clear();
    InOther.funcs.clear();
}

Expression& Expression::operator=(const Expression& InOther)
{
    Expr = InOther.Expr;
    for (auto& var : InOther.vars)
    {
        Declare(var.first, var.second.name);
        Set(var.first, *var.second.val);
    }
    for (auto& func : InOther.funcs)
        DeclareFunc(func.first, func.second);
    Compile();
    return *this;
}

bool Expression::operator==(const Expression& InOther) const
{
    return Expr == InOther.Expr;
}

void Expression::Set(const uint8 InKey, const float InVal) const
{
    const auto find = vars.find(InKey);
    CHECK_ASSERT(find == vars.end(), "Unknown key");
    *find->second.val = InVal;
}

void Expression::Declare(const uint8 InKey, const String& InName)
{
    if (!vars.contains(InKey))
    {
        if (vars.empty())
            graphDefault = InKey;
        Var var = {
            .val = new float(0.0f),
            .name = InName
        }; 
        vars[InKey] = var;
        if (parser)
        {
            parser->add_variable_or_function({
                .m_name = var.name,
                .m_value = var.val
            });
        }
    }
}

void Expression::Remove(const uint8 InKey)
{
    if (vars.contains(InKey))
    {
        Var& var = vars.at(InKey);
        if (parser)
            parser->remove_variable_or_function(var.name);
        delete (var.val);
        var.val = nullptr;
        vars.erase(InKey);
    }
}

void Expression::DeclareFunc(const String& InName, const te_variant_type InFunc)
{
    funcs[InName] = InFunc;
    if (parser)
    {
        parser->add_variable_or_function({
            .m_name = InName,
            .m_value = InFunc
        });
    }
}

void Expression::RemoveFunc(const String& InName)
{
    funcs.erase(InName);
    parser->remove_variable_or_function(InName);
}

float Expression::Evaluate() const
{
    CHECK_RETURN(!parser, 0.0f)
    return parser->evaluate();
}

bool Expression::Edit(const String& InName, uint32 InOffset)
{
    bool comp = false;
    if (Utility::Edit(InName, Expr.Get(), InOffset))
        comp = Compile();
    DrawGraph();
    return comp;
}

void Expression::CacheGraph()
{
    cachedGraph.clear();
    CHECK_RETURN(!vars.contains(graphDefault));

    const auto& find = vars.at(graphDefault);
    float prevVal = *find.val;
    cachedGraph.resize(graphPrecision);
    for (int i = 0; i < graphPrecision; i++)
    {
        float alpha = static_cast<float>(i) / static_cast<float>(graphPrecision);
        *find.val = alpha;
        cachedGraph[i] = Evaluate();
    }
    *find.val = prevVal;
}

void Expression::DrawGraph()
{
    if (cachedGraph.empty())
        CacheGraph();

    ImGui::PushItemWidth(-FLT_MIN);
    constexpr ImVec2 plotSize = ImVec2(0, 50);

    ImGui::PlotLines(("##Freq" + Expr.Get()).c_str(),
        cachedGraph.data(),
        static_cast<int>(cachedGraph.size()),
        0,
        0,
        FLT_MAX,
        FLT_MAX,
        plotSize);

    ImGui::PopItemWidth();
}

bool Expression::Compile()
{
    cachedGraph.clear();
    CHECK_RETURN(Expr.Get().empty(), false);
    
    if (!parser)
        parser = new te_parser();

    std::set<te_variable> variables;
    for (const auto& var : vars)
        variables.insert({ var.second.name, var.second.val });
    for (const auto& func : funcs)
        variables.insert({ func.first, func.second });

    parser->set_variables_and_functions(variables);
    parser->compile(Expr.Get());
    if (!parser->success() && !parser->get_last_error_message().empty())
    {
        LOG("Error: " + parser->get_last_error_message() + " Expr: " + Expr.Get());
        return false;
    }
    return true;
}
