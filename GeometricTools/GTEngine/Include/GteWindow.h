// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include "GTEngineDEF.h"
#include "GteCamera.h"
#include "GteTimer.h"
#include "GteTransform.h"
#include "GteDX11Engine.h"

namespace gte
{

class GTE_IMPEXP Window
{
public:
    // All parameters for constructing windows are in this structure.  Derived
    // classes may define nested classes to derive from this.  The creation by
    // WindowSystem requires you to pass such a structure.
    struct GTE_IMPEXP Parameters
    {
        Parameters ()
            :
            title(L""),
            xOrigin(0), yOrigin(0), xSize(0), ySize(0),
            allowResize(false), hscrollBar(false), vscrollBar(false),
            created(false),
            deviceCreationFlags(0),
            featureLevel(D3D_FEATURE_LEVEL_11_0),
            handle(nullptr),
            engine(nullptr)
        {
        }

        std::wstring title;
        int xOrigin, yOrigin, xSize, ySize;
        bool allowResize, hscrollBar, vscrollBar, created;
        UINT deviceCreationFlags;
        D3D_FEATURE_LEVEL featureLevel;
        HWND handle;
        DX11Engine* engine;
    };

    // Abstract base class.  Only WindowSystem may create windows.
    virtual ~Window ();

    // Member access.
    inline HWND GetHandle () const;
    inline void SetTitle (std::wstring const& title);
    inline std::wstring GetTitle () const;
    inline int GetXOrigin () const;
    inline int GetYOrigin () const;
    inline int GetXSize () const;
    inline int GetYSize () const;
    inline bool IsMinimized () const;
    inline bool IsMaximized () const;
    inline float GetAspectRatio () const;

    // Display callbacks.
    virtual void OnMove (int x, int y);
    virtual bool OnResize (int xSize, int ySize);
    virtual void OnMinimize ();
    virtual void OnMaximize ();
    virtual void OnDisplay ();
    virtual void OnIdle ();

    // Keyboard callbacks.  OnCharPress allows you to distinguish between
    // upper-case and lower-case letters; OnKeyDown and OnKeyUp do not.
    virtual bool OnCharPress (unsigned char key, int x, int y);
    virtual bool OnKeyDown (int key, int x, int y);
    virtual bool OnKeyUp (int key, int x, int y);

    // Mouse callbacks and state information.
    enum GTE_IMPEXP MouseButton
    {
        MOUSE_NONE,
        MOUSE_LEFT,
        MOUSE_MIDDLE,
        MOUSE_RIGHT
    };

    enum GTE_IMPEXP MouseState
    {
        MOUSE_UP,
        MOUSE_DOWN
    };

    virtual bool OnMouseClick (MouseButton button, MouseState state,
        int x, int y, unsigned int modifiers);

    virtual bool OnMouseMotion (MouseButton button, int x, int y,
        unsigned int modifiers);

    virtual bool OnMouseWheel (int delta, int x, int y,
        unsigned int modifiers);

    void SetMousePosition (int x, int y);
    void GetMousePosition (int& x, int& y) const;

    // Scroll support.  The 'bar' value is 0 for horizontal scroll bars or 1
    // for vertical scroll bars.
    void SetScrollInterval (int bar, int minValue, int maxValue);
    void GetScrollInterval (int bar, int& minValue, int& maxValue) const;
    int SetScrollPosition (int bar, int value);
    int GetScrollPosition (int bar) const;

    // The return value of the increment/decrement functions is the delta of
    // the slider thumb.  If zero, the scroll state did not change.  For the
    // tracking functions, the return value is the current slider thumb
    // position.  A derived-class override must call the base-class function
    // first.
    virtual int OnScrollIncrementLoRes (int bar);
    virtual int OnScrollDecrementLoRes (int bar);
    virtual int OnScrollIncrementHiRes (int bar);
    virtual int OnScrollDecrementHiRes (int bar);
    virtual int OnScrollTracking (int bar);
    virtual int OnScrollEndTracking (int bar);

    // Performance measurements.
    inline void ResetTime ();
    void MeasureTime ();
    inline void UpdateFrameCount ();

    // Draw the current frame rate at window location (x,y) using the
    // specified color.
    void DrawFrameRate (unsigned int x, unsigned int y,
        Vector4<float> const& color);

    // Camera motion using key strokes.
    void EnableCameraMotion (float trnSpeed, float rotSpeed,
        float trnSpeedFactor, float rotSpeedFactor);
    inline void DisableCameraMotion ();
    virtual bool MoveCamera ();
    virtual void MoveForward ();
    virtual void MoveBackward ();
    virtual void MoveUp ();
    virtual void MoveDown ();
    virtual void MoveRight ();
    virtual void MoveLeft ();
    virtual void TurnRight ();
    virtual void TurnLeft ();
    virtual void LookUp ();
    virtual void LookDown ();

    // Object rotation using a virtual trackball.
    inline void EnableObjectMotion ();
    inline void DisableObjectMotion ();

    // Support for automatic updates of projection-view-world matrices
    // for objects attached to the virtual track ball and using the default
    // Window camera.  The sourceWMatrix is the application-managed transform
    // that must be combined with camera transforms.  FOR NOW, the assumption
    // is that the constant buffer's system memory starts with the pvwMatrix.
    // This design choice can be modified later.
    void Register(Matrix4x4<float> const& sourceWMatrix,
        std::shared_ptr<ConstantBuffer> const& cbuffer);
    void Unregister(Matrix4x4<float> const& sourceMatrix);
    void UpdateRegisteredTransforms();

protected:
    Window (Parameters& parameters);

    // Standard window information.
    HWND mHandle;
    std::wstring mTitle;
    int mXOrigin, mYOrigin, mXSize, mYSize;
    bool mAllowResize;
    bool mIsMinimized;
    bool mIsMaximized;

    // Scroll bar support.
    bool mHasScroll[2];
    mutable SCROLLINFO mScrollInfo[2];
    int mScrollLoResDelta[2];
    int mScrollHiResDelta[2];

    // Performance measurement parameters.
    double mLastTime, mAccumulatedTime, mFrameRate;
    int mFrameCount, mAccumulatedFrameCount, mTimer, mMaxTimer;
    Timer mClock;

    // Graphics device and camera.
    DX11Engine* mEngine;
    Camera mCamera;

    // Camera motion support.
    Vector4<float> mWorldAxis[3];
    float mTrnSpeed, mTrnSpeedFactor;
    float mRotSpeed, mRotSpeedFactor;
    bool mUpArrowPressed;
    bool mDownArrowPressed;
    bool mLeftArrowPressed;
    bool mRightArrowPressed;
    bool mPageUpPressed;
    bool mPageDownPressed;
    bool mHomePressed;
    bool mEndPressed;
    bool mInsertPressed;
    bool mDeletePressed;
    bool mCameraMovable;

    // Object motion support using a virtual trackball.
    void RotateTrackBall(float x0, float y0, float x1, float y1);
    Transform<float> mObjectTransform;
    float mXTrack0, mYTrack0, mXTrack1, mYTrack1;
    Matrix4x4<float> mSaveRotate;
    bool mUseTrackBall, mTrackBallDown;

    // Automatic transform update support.
    std::map<Matrix4x4<float> const*,std::shared_ptr<ConstantBuffer>>
        mRegistered;
};

#include "GteWindow.inl"

}
