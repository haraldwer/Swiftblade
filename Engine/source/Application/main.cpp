
#include "Application.h"

int main()
{
    Application::App app;
    app.Init();
    app.Run();
    app.Deinit();
    return 0;
}
