// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "TextureArraysWindow.h"

int main(int, char const*[])
{
    LogReporter reporter(
        "LogReport.txt",
        Logger::Listener::LISTEN_FOR_ALL,
        Logger::Listener::LISTEN_FOR_ALL,
        Logger::Listener::LISTEN_FOR_ALL,
        Logger::Listener::LISTEN_FOR_ALL);

    Window::Parameters parameters;
    parameters.title = L"TextureArraysWindow";
    parameters.xOrigin = 0;
    parameters.yOrigin = 0;
    parameters.xSize = 512;
    parameters.ySize = 512;
    parameters.allowResize = true;
    parameters.deviceCreationFlags = D3D11_CREATE_DEVICE_DEBUG;

    TextureArraysWindow* window =
        TheWindowSystem.Create<TextureArraysWindow>(parameters);

    if (window)
    {
        HWND handle = window->GetHandle();
        ShowWindow(handle, SW_SHOW);
        UpdateWindow(handle);

        for (;;)
        {
            MSG msg;
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    break;
                }

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                if (!window->IsMinimized())
                {
                    window->OnIdle();
                }
            }
        }

        TheWindowSystem.Destroy<TextureArraysWindow>(window);
    }

    return 0;
}

