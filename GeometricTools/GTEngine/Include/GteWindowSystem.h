// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include "GTEngineDEF.h"
#include "GteWindow.h"

namespace gte
{

class GTE_IMPEXP WindowSystem
{
public:
    // Construction and destruction. This is a singleton class.
    ~WindowSystem ();
    WindowSystem ();

    // Get the true window size for the specified client size.  The true size
    // includes extra space for window decorations (window border, menu bar,
    // and so on).  This information is useful to know before creating a
    // window to ensure the to-be-created window fits within the monitor
    // resolution.
    static bool GetWindowRectangle (int xClientSize, int yClientSize,
        DWORD style, RECT& windowRectangle);

    // Create and destroy windows.  Derived classes may extend the inputs
    // using a nested class derived from Window::Parameters
    template <typename WindowType>
    WindowType* Create (typename WindowType::Parameters& parameters);

    template <typename WindowType>
    void Destroy (WindowType*& window);

private:
    // Window creation and destruction.
    void CreateFrom (Window::Parameters& parameters);

    // Extraction of cursor location, avoiding the extraction in <windows.h>
    // that does not work when you have dual monitors.
    static void Extract (LPARAM lParam, int& x, int& y);
    static void Extract (WPARAM wParam, int& x, int& y);

    // The event handler.
    static LRESULT CALLBACK WindowProcedure (HWND handle, UINT message,
        WPARAM wParam, LPARAM lParam);

    wchar_t const* mWindowClassName;
    ATOM mAtom;
    std::map<HWND,Window*> mHandleMap;
};

#include "GteWindowSystem.inl"
extern GTE_IMPEXP WindowSystem TheWindowSystem;

}
