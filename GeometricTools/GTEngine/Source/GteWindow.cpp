// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "GTEnginePCH.h"
#include "GteWindow.h"
using namespace gte;

//----------------------------------------------------------------------------
Window::~Window ()
{
    delete mEngine;
}
//----------------------------------------------------------------------------
Window::Window (Parameters& parameters)
    :
    mHandle(parameters.handle),
    mTitle(parameters.title),
    mXOrigin(parameters.xOrigin),
    mYOrigin(parameters.yOrigin),
    mXSize(parameters.xSize),
    mYSize(parameters.ySize),
    mAllowResize(parameters.allowResize),
    mIsMinimized(false),
    mIsMaximized(false),
    mLastTime(-1.0),
    mAccumulatedTime(0.0),
    mFrameRate(0.0),
    mFrameCount(0),
    mAccumulatedFrameCount(0),
    mTimer(30),
    mMaxTimer(30),
    mEngine(parameters.engine),
    mTrnSpeed(0.0f),
    mTrnSpeedFactor(1.0f),
    mRotSpeed(0.0f),
    mRotSpeedFactor(1.0f),
    mUpArrowPressed(false),
    mDownArrowPressed(false),
    mLeftArrowPressed(false),
    mRightArrowPressed(false),
    mPageUpPressed(false),
    mPageDownPressed(false),
    mHomePressed(false),
    mEndPressed(false),
    mInsertPressed(false),
    mDeletePressed(false),
    mCameraMovable(false),
    mXTrack0(0.0f),
    mYTrack0(0.0f),
    mXTrack1(0.0f),
    mYTrack1(0.0f),
    mSaveRotate(Matrix4x4<float>::Identity()),
    mUseTrackBall(false),
    mTrackBallDown(false)
{
    // If scroll bars are requested, the Window-derived class constructor
    // should call the function Window::SetScrollInterval(...) to set the
    // scroll bar range.  Increments and decrements may be set separately
    // directly via class members.
    mHasScroll[0] = parameters.hscrollBar;
    mHasScroll[1] = parameters.vscrollBar;
    for (int i = 0; i < 2; ++i)
    {
        ZeroMemory(&mScrollInfo[i], sizeof(SCROLLINFO));
        mScrollInfo[i].cbSize = sizeof(SCROLLINFO);
        mScrollInfo[i].fMask = SIF_ALL;
        GetScrollInfo(mHandle, i, &mScrollInfo[i]);
        mScrollLoResDelta[i] = 1;
        mScrollHiResDelta[i] = 1;
    }

    // Initialize the world coordinate system to invalid vectors.  These are
    // used only when camera motion is enabled, in which case the enabling
    // function sets these properly.
    mWorldAxis[0] = Vector4<float>::Zero();
    mWorldAxis[1] = Vector4<float>::Zero();
    mWorldAxis[2] = Vector4<float>::Zero();
}
//----------------------------------------------------------------------------
void Window::SetTitle (std::wstring const& title)
{
    SetWindowText(mHandle, title.c_str());
    mTitle = title;
}
//----------------------------------------------------------------------------
void Window::OnMove (int x, int y)
{
    mXOrigin = x;
    mYOrigin = y;
}
//----------------------------------------------------------------------------
bool Window::OnResize (int xSize, int ySize)
{
    mIsMinimized = false;
    mIsMaximized = false;

    if (xSize != mXSize || ySize != mYSize)
    {
        mXSize = xSize;
        mYSize = ySize;

        if (mEngine)
        {
            mEngine->Resize(xSize, ySize);
        }

        float upFovDegrees, aspectRatio, dMin, dMax;
        mCamera.GetFrustum(upFovDegrees, aspectRatio, dMin, dMax);
        mCamera.SetFrustum(upFovDegrees, GetAspectRatio(), dMin, dMax);
        UpdateRegisteredTransforms();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void Window::OnMinimize ()
{
    mIsMinimized = true;
    mIsMaximized = false;
}
//----------------------------------------------------------------------------
void Window::OnMaximize ()
{
    mIsMinimized = false;
    mIsMaximized = true;
}
//----------------------------------------------------------------------------
void Window::OnDisplay ()
{
    // Stub for derived classes.
}
//----------------------------------------------------------------------------
void Window::OnIdle ()
{
    // Stub for derived classes.
}
//----------------------------------------------------------------------------
bool Window::OnCharPress (unsigned char key, int, int)
{
    switch (key)
    {
    case ' ':
        ResetTime();
        return true;

    case 't':  // Slower camera translation.
        mTrnSpeed /= mTrnSpeedFactor;
        return true;

    case 'T':  // Faster camera translation.
        mTrnSpeed *= mTrnSpeedFactor;
        return true;

    case 'r':  // Slower camera rotation.
        mRotSpeed /= mRotSpeedFactor;
        return true;

    case 'R':  // Faster camera rotation.
        mRotSpeed *= mRotSpeedFactor;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool Window::OnKeyDown (int key, int, int)
{
    if (mCameraMovable)
    {
        switch (key)
        {
        case VK_LEFT:
            mLeftArrowPressed = true;
            return true;

        case VK_RIGHT:
            mRightArrowPressed = true;
            return true;

        case VK_UP:
            mUpArrowPressed = true;
            return true;

        case VK_DOWN:
            mDownArrowPressed = true;
            return true;

        case VK_PRIOR:
            mPageUpPressed = true;
            return true;

        case VK_NEXT:
            mPageDownPressed = true;
            return true;

        case VK_HOME:
            mHomePressed = true;
            return true;

        case VK_END:
            mEndPressed = true;
            return true;

        case VK_INSERT:
            mInsertPressed = true;
            return true;

        case VK_DELETE:
            mDeletePressed = true;
            return true;
        };
    }

    return false;
}
//----------------------------------------------------------------------------
bool Window::OnKeyUp (int key, int, int)
{
    if (mCameraMovable)
    {
        switch (key)
        {
        case VK_LEFT:
            mLeftArrowPressed = false;
            return true;

        case VK_RIGHT:
            mRightArrowPressed = false;
            return true;

        case VK_UP:
            mUpArrowPressed = false;
            return true;

        case VK_DOWN:
            mDownArrowPressed = false;
            return true;

        case VK_PRIOR:
            mPageUpPressed = false;
            return true;

        case VK_NEXT:
            mPageDownPressed = false;
            return true;

        case VK_HOME:
            mHomePressed = false;
            return true;

        case VK_END:
            mEndPressed = false;
            return true;

        case VK_INSERT:
            mInsertPressed = false;
            return true;

        case VK_DELETE:
            mDeletePressed = false;
            return true;
        };
    }

    return false;
}
//----------------------------------------------------------------------------
bool Window::OnMouseClick (MouseButton button, MouseState state, int x, int y,
    unsigned int)
{
    if (mUseTrackBall && button == MOUSE_LEFT)
    {
        float mult = 1.0f/(mXSize >= mYSize ? mYSize : mXSize);

        if (state == MOUSE_DOWN)
        {
            // Get the starting point.
            mTrackBallDown = true;
            mSaveRotate = mObjectTransform.GetRotation();
            mXTrack0 = (2*x - mXSize)*mult;
            mYTrack0 = (2*(mYSize - 1 - y) - mYSize)*mult;
        }
        else
        {
            mTrackBallDown = false;
        }

        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool Window::OnMouseMotion (MouseButton button, int x, int y, unsigned int)
{
    if (mUseTrackBall && button == MOUSE_LEFT && mTrackBallDown)
    {
        // Get the ending point.
        float mult = 1.0f/(mXSize >= mYSize ? mYSize : mXSize);
        mXTrack1 = (2*x - mXSize)*mult;
        mYTrack1 = (2*(mYSize - 1 - y) - mYSize)*mult;

        // Update the object's local rotation.
        RotateTrackBall(mXTrack0, mYTrack0, mXTrack1, mYTrack1);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool Window::OnMouseWheel (int, int, int, unsigned int)
{
    // Stub for derived classes.
    return false;
}
//----------------------------------------------------------------------------
void Window::SetMousePosition (int x, int y)
{
    POINT point = { (LONG)x, (LONG)y };
    ClientToScreen(mHandle, &point);
    SetCursorPos(point.x, point.y);
}
//----------------------------------------------------------------------------
void Window::GetMousePosition (int& x, int& y) const
{
    POINT point;
    GetCursorPos(&point);
    ScreenToClient(mHandle, &point);
    x = (int)point.x;
    y = (int)point.y;
}
//----------------------------------------------------------------------------
void Window::SetScrollInterval (int bar, int minValue, int maxValue)
{
    mScrollInfo[bar].fMask = SIF_RANGE;
    mScrollInfo[bar].nMin = minValue;
    mScrollInfo[bar].nMax = maxValue;
    SetScrollInfo(mHandle, bar, &mScrollInfo[bar], FALSE);
}
//----------------------------------------------------------------------------
void Window::GetScrollInterval (int bar, int& minValue, int& maxValue) const
{
    mScrollInfo[bar].fMask = SIF_RANGE;
    GetScrollInfo(mHandle, bar, &mScrollInfo[bar]);
    minValue = mScrollInfo[bar].nMin;
    maxValue = mScrollInfo[bar].nMax;
}
//----------------------------------------------------------------------------
int Window::SetScrollPosition (int bar, int value)
{
    mScrollInfo[bar].fMask = SIF_POS;
    mScrollInfo[bar].nPos = value;
    return SetScrollInfo(mHandle, bar, &mScrollInfo[bar], FALSE);
}
//----------------------------------------------------------------------------
int Window::GetScrollPosition (int bar) const
{
    mScrollInfo[bar].fMask = SIF_POS;
    GetScrollInfo(mHandle, bar, &mScrollInfo[bar]);
    return mScrollInfo[bar].nPos;
}
//----------------------------------------------------------------------------
int Window::OnScrollIncrementLoRes (int bar)
{
    SCROLLINFO& info = mScrollInfo[bar];
    info.fMask = SIF_POS;
    GetScrollInfo(mHandle, bar, &info);
    int delta = info.nMax - info.nPos;
    if (delta > 0)
    {
        delta = std::min(delta, mScrollLoResDelta[bar]);
        info.nPos += delta;
        SetScrollInfo(mHandle, bar, &info, TRUE);
    }
    return delta;
}
//----------------------------------------------------------------------------
int Window::OnScrollDecrementLoRes (int bar)
{
    SCROLLINFO& info = mScrollInfo[bar];
    info.fMask = SIF_POS;
    GetScrollInfo(mHandle, bar, &info);
    int delta = info.nPos - info.nMin;
    if (delta > 0)
    {
        delta = std::min(delta, mScrollLoResDelta[bar]);
        info.nPos -= delta;
        SetScrollInfo(mHandle, bar, &info, TRUE);
    }
    return delta;
}
//----------------------------------------------------------------------------
int Window::OnScrollIncrementHiRes (int bar)
{
    SCROLLINFO& info = mScrollInfo[bar];
    info.fMask = SIF_POS;
    GetScrollInfo(mHandle, bar, &info);
    int delta = info.nMax - info.nPos;
    if (delta > 0)
    {
        delta = std::min(delta, mScrollHiResDelta[bar]);
        info.nPos += delta;
        SetScrollInfo(mHandle, bar, &info, TRUE);
    }
    return delta;
}
//----------------------------------------------------------------------------
int Window::OnScrollDecrementHiRes (int bar)
{
    SCROLLINFO& info = mScrollInfo[bar];
    info.fMask = SIF_POS;
    GetScrollInfo(mHandle, bar, &info);
    int delta = info.nPos - info.nMin;
    if (delta > 0)
    {
        delta = std::min(delta, mScrollHiResDelta[bar]);
        info.nPos -= delta;
        SetScrollInfo(mHandle, bar, &info, TRUE);
    }
    return delta;
}
//----------------------------------------------------------------------------
int Window::OnScrollTracking (int bar)
{
    SCROLLINFO& info = mScrollInfo[bar];
    info.fMask = SIF_TRACKPOS;
    GetScrollInfo(mHandle, bar, &info);
    return info.nTrackPos;
}
//----------------------------------------------------------------------------
int Window::OnScrollEndTracking (int bar)
{
    SCROLLINFO& info = mScrollInfo[bar];
    info.fMask = SIF_TRACKPOS;
    GetScrollInfo(mHandle, bar, &info);
    info.fMask = SIF_POS;
    info.nPos = info.nTrackPos;
    SetScrollInfo(mHandle, bar, &info, TRUE);
    return info.nTrackPos;
}
//----------------------------------------------------------------------------
void Window::MeasureTime ()
{
    // Start performance measurements.
    if (mLastTime == -1.0)
    {
        mLastTime = mClock.GetSeconds();
        mAccumulatedTime = 0.0;
        mFrameRate = 0.0;
        mFrameCount = 0;
        mAccumulatedFrameCount = 0;
        mTimer = mMaxTimer;
    }

    // Accumulate the time only when the mini-timer allows it.
    if (--mTimer == 0)
    {
        double dCurrentTime = mClock.GetSeconds();
        double dDelta = dCurrentTime - mLastTime;
        mLastTime = dCurrentTime;
        mAccumulatedTime += dDelta;
        mAccumulatedFrameCount += mFrameCount;
        mFrameCount = 0;
        mTimer = mMaxTimer;
    }

    if (mAccumulatedTime > 0.0)
    {
        mFrameRate = mAccumulatedFrameCount/mAccumulatedTime;
    }
    else
    {
        mFrameRate = 0.0;
    }
}
//----------------------------------------------------------------------------
void Window::DrawFrameRate (unsigned int x, unsigned int y,
    Vector4<float> const& color)
{
    std::string message = Environment::CreateString("fps: %.1lf", mFrameRate);
    mEngine->Draw(x, y, color, message);
}
//----------------------------------------------------------------------------
void Window::EnableCameraMotion (float trnSpeed, float rotSpeed,
    float trnSpeedFactor, float rotSpeedFactor)
{
    mCameraMovable = true;

    mTrnSpeed = trnSpeed;
    mRotSpeed = rotSpeed;
    mTrnSpeedFactor = trnSpeedFactor;
    mRotSpeedFactor = rotSpeedFactor;

    mWorldAxis[0] = mCamera.GetDVector();
    mWorldAxis[1] = mCamera.GetUVector();
    mWorldAxis[2] = mCamera.GetRVector();
}
//----------------------------------------------------------------------------
bool Window::MoveCamera ()
{
    if (mCameraMovable)
    {
        bool bMoved = false;

        if (mUpArrowPressed)
        {
            MoveForward();
            bMoved = true;
        }

        if (mDownArrowPressed)
        {
            MoveBackward();
            bMoved = true;
        }

        if (mHomePressed)
        {
            MoveUp();
            bMoved = true;
        }

        if (mEndPressed)
        {
            MoveDown();
            bMoved = true;
        }

        if (mLeftArrowPressed)
        {
            TurnLeft();
            bMoved = true;
        }

        if (mRightArrowPressed)
        {
            TurnRight();
            bMoved = true;
        }

        if (mPageUpPressed)
        {
            LookUp();
            bMoved = true;
        }

        if (mPageDownPressed)
        {
            LookDown();
            bMoved = true;
        }

        if (mInsertPressed)
        {
            MoveRight();
            bMoved = true;
        }

        if (mDeletePressed)
        {
            MoveLeft();
            bMoved = true;
        }

        if (bMoved)
        {
            UpdateRegisteredTransforms();
        }
        return bMoved;
    }

    return false;
}
//----------------------------------------------------------------------------
void Window::MoveForward ()
{
    mCamera.SetPosition(mCamera.GetPosition() + mTrnSpeed*mWorldAxis[0]);
}
//----------------------------------------------------------------------------
void Window::MoveBackward ()
{
    mCamera.SetPosition(mCamera.GetPosition() - mTrnSpeed*mWorldAxis[0]);
}
//----------------------------------------------------------------------------
void Window::MoveUp ()
{
    mCamera.SetPosition(mCamera.GetPosition() + mTrnSpeed*mWorldAxis[1]);
}
//----------------------------------------------------------------------------
void Window::MoveDown ()
{
    mCamera.SetPosition(mCamera.GetPosition() - mTrnSpeed*mWorldAxis[1]);
}
//----------------------------------------------------------------------------
void Window::MoveRight ()
{
    mCamera.SetPosition(mCamera.GetPosition() + mTrnSpeed*mWorldAxis[2]);
}
//----------------------------------------------------------------------------
void Window::MoveLeft ()
{
    mCamera.SetPosition(mCamera.GetPosition() - mTrnSpeed*mWorldAxis[2]);
}
//----------------------------------------------------------------------------
void Window::TurnRight ()
{
    Matrix4x4<float> incr = Rotation<4,float>(
        AxisAngle<4,float>(mWorldAxis[1], -mRotSpeed));

#if defined(GTE_USE_MAT_VEC)
    mWorldAxis[0] = incr*mWorldAxis[0];
    mWorldAxis[2] = incr*mWorldAxis[2];
    mCamera.SetAxes(
        incr*mCamera.GetDVector(),
        incr*mCamera.GetUVector(),
        incr*mCamera.GetRVector());
#else
    mWorldAxis[0] = mWorldAxis[0]*incr;
    mWorldAxis[2] = mWorldAxis[2]*incr;
    mCamera.SetAxes(
        mCamera.GetDVector()*incr,
        mCamera.GetUVector()*incr,
        mCamera.GetRVector()*incr);
#endif
}
//----------------------------------------------------------------------------
void Window::TurnLeft ()
{
    Matrix4x4<float> incr = Rotation<4,float>(
        AxisAngle<4,float>(mWorldAxis[1], +mRotSpeed));

#if defined(GTE_USE_MAT_VEC)
    mWorldAxis[0] = incr*mWorldAxis[0];
    mWorldAxis[2] = incr*mWorldAxis[2];
    mCamera.SetAxes(
        incr*mCamera.GetDVector(),
        incr*mCamera.GetUVector(),
        incr*mCamera.GetRVector());
#else
    mWorldAxis[0] = mWorldAxis[0]*incr;
    mWorldAxis[2] = mWorldAxis[2]*incr;
    mCamera.SetAxes(
        mCamera.GetDVector()*incr,
        mCamera.GetUVector()*incr,
        mCamera.GetRVector()*incr);
#endif
}
//----------------------------------------------------------------------------
void Window::LookUp ()
{
    Matrix4x4<float> incr = Rotation<4,float>(
        AxisAngle<4,float>(mWorldAxis[2], +mRotSpeed));

#if defined(GTE_USE_MAT_VEC)
    mCamera.SetAxes(
        incr*mCamera.GetDVector(),
        incr*mCamera.GetUVector(),
        incr*mCamera.GetRVector());
#else
    mCamera.SetAxes(
        mCamera.GetDVector()*incr,
        mCamera.GetUVector()*incr,
        mCamera.GetRVector()*incr);
#endif
}
//----------------------------------------------------------------------------
void Window::LookDown ()
{
    Matrix4x4<float> incr = Rotation<4,float>(
        AxisAngle<4,float>(mWorldAxis[2], -mRotSpeed));

#if defined(GTE_USE_MAT_VEC)
    mCamera.SetAxes(
        incr*mCamera.GetDVector(),
        incr*mCamera.GetUVector(),
        incr*mCamera.GetRVector());
#else
    mCamera.SetAxes(
        mCamera.GetDVector()*incr,
        mCamera.GetUVector()*incr,
        mCamera.GetRVector()*incr);
#endif
}
//----------------------------------------------------------------------------
void Window::RotateTrackBall (float x0, float y0, float x1, float y1)
{
    if (x0 == x1 && y0 == y1)
    {
        // Nothing to rotate.
        return;
    }

    // Get the first vector on the sphere.
    float length = sqrt(x0*x0 + y0*y0), invLength, z0, z1;
    if (length > 1.0f)
    {
        // Outside the unit disk, project onto it.
        invLength = 1.0f/length;
        x0 *= invLength;
        y0 *= invLength;
        z0 = 0.0f;
    }
    else
    {
        // Compute point (x0,y0,z0) on negative unit hemisphere.
        z0 = 1.0f - x0*x0 - y0*y0;
        z0 = (z0 <= 0.0f ? 0.0f : sqrt(z0));
    }
    z0 *= -1.0f;

    // Use camera world coordinates, order is (D,U,R), so point is (z,y,x).
    Vector4<float> vec0(z0, y0, x0, 0.0f);

    // Get the second vector on the sphere.
    length = sqrt(x1*x1 + y1*y1);
    if (length > 1.0f)
    {
        // Outside unit disk, project onto it.
        invLength = 1.0f/length;
        x1 *= invLength;
        y1 *= invLength;
        z1 = 0.0f;
    }
    else
    {
        // Compute point (x1,y1,z1) on negative unit hemisphere.
        z1 = 1.0f - x1*x1 - y1*y1;
        z1 = (z1 <= 0.0f ? 0.0f : sqrt(z1));
    }
    z1 *= -1.0f;

    // Use camera world coordinates, order is (D,U,R), so point is (z,y,x).
    Vector4<float> vec1(z1, y1, x1, 0.0f);

    // Create axis and angle for the rotation.
    Vector4<float> axis = Cross(vec0, vec1);
    float dot = Dot(vec0, vec1);
    float angle;
    if (Normalize(axis) > 0.0f)
    {
        angle = acos(std::min(std::max(dot, -1.0f), 1.0f)); 
    }
    else  // Vectors are parallel.
    {
        if (dot < 0.0f)
        {
            // Rotated pi radians.
            invLength = 1.0f/sqrt(x0*x0 + y0*y0);
            axis[0] = y0*invLength;
            axis[1] = -x0*invLength;
            axis[2] = 0.0f;
            angle = (float)GTE_C_PI;
        }
        else
        {
            // Rotation by zero radians.
            axis = Vector4<float>::Basis0();
            angle = 0.0f;
        }
    }

    // Compute the rotation matrix implied by trackball motion.  The axis
    // vector was computed in camera coordinates.  It must be converted
    // to world coordinates.  Once again, I use the camera ordering (D,U,R).
    Vector4<float> worldAxis =
        axis[0]*mCamera.GetDVector() +
        axis[1]*mCamera.GetUVector() +
        axis[2]*mCamera.GetRVector();

    Matrix4x4<float> trackRotate = Rotation<4,float>(
        AxisAngle<4,float>(worldAxis, angle));

    // Compute the new rotation, which is the incremental rotation of
    // the trackball appiled after the object has been rotated by its old
    // rotation.
#if defined(GTE_USE_MAT_VEC)
    Matrix4x4<float> rotate = trackRotate*mSaveRotate;
#else
    Matrix4x4<float> rotate = mSaveRotate*trackRotate;
#endif
    Vector4<float> v[3] =
    {
        rotate.GetCol(0),
        rotate.GetCol(1),
        rotate.GetCol(2)
    };
    Orthonormalize<4,float>(3, v);
    rotate.SetCol(0, v[0]);
    rotate.SetCol(1, v[1]);
    rotate.SetCol(2, v[2]);

    mObjectTransform.SetRotation(rotate);
    UpdateRegisteredTransforms();
}
//----------------------------------------------------------------------------
void Window::Register(Matrix4x4<float> const& sourceWMatrix,
    std::shared_ptr<ConstantBuffer> const& cbuffer)
{
    if (mRegistered.find(&sourceWMatrix) == mRegistered.end())
    {
        mRegistered.insert(std::make_pair(&sourceWMatrix, cbuffer));
    }
}
//----------------------------------------------------------------------------
void Window::Unregister(Matrix4x4<float> const& sourceWMatrix)
{
    mRegistered.erase(&sourceWMatrix);
}
//----------------------------------------------------------------------------
void Window::UpdateRegisteredTransforms()
{
    Matrix4x4<float> pvMatrix = mCamera.GetProjectionViewMatrix();
    for (auto& element : mRegistered)
    {
        // Compute the new projection-view-world matrix.
#if defined(GTE_USE_MAT_VEC)
        Matrix4x4<float> wMatrix = mObjectTransform * (*element.first);
        Matrix4x4<float> pvwMatrix = pvMatrix * wMatrix;
#else
        Matrix4x4<float> wMatrix = (*element.first) * mObjectTransform;
        Matrix4x4<float> pvwMatrix = wMatrix * pvMatrix;
#endif
        // Copy the source matrix into the system memory of the constant
        // buffer.
        *element.second->Get<Matrix4x4<float>>() = pvwMatrix;

        // Upload the new matrix into video memory.
        mEngine->Update(element.second);
    }
}
//----------------------------------------------------------------------------
