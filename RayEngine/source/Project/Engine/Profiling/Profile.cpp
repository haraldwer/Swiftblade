#include "Profile.h"

#include "Manager.h"

void Profiling::BeginScope(const String& InName, const String& InFunction)
{
    Manager::Get().Begin(InName, InFunction);
}

void Profiling::EndScope()
{
    Manager::Get().End();
}
