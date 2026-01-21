#include "LoadingSpinner.h"

void UI::LoadingSpinner::Update(Container &InOwner)
{
    Image::Update(InOwner);

    transform.rotation = Utility::Time::Get().Total() * 50;
}
