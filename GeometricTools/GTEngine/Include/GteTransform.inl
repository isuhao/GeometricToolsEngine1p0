// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <typename Real>
Transform<Real>::~Transform()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Transform<Real>::Transform()
{
    SetIdentity();
}
//----------------------------------------------------------------------------
template <typename Real>
Transform<Real>::Transform(Transform const& transform)
{
    *this = transform;
}
//----------------------------------------------------------------------------
template <typename Real>
Transform<Real>& Transform<Real>::operator=(Transform<Real> const& transform)
{
    Matrix4x4<Real>::operator=(transform);
    mQuaternion = transform.mQuaternion;
    mTranslation[0] = transform.mTranslation[0];
    mTranslation[1] = transform.mTranslation[1];
    mTranslation[2] = transform.mTranslation[2];
    mScale = transform.mScale;
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
void Transform<Real>::SetIdentity()
{
    MakeIdentity();
    mQuaternion = Quaternion<Real>::Identity();
    mTranslation[0] = (Real)0;
    mTranslation[1] = (Real)0;
    mTranslation[2] = (Real)0;
    mScale = (Real)1;
}
//----------------------------------------------------------------------------
template <typename Real>
void Transform<Real>::SetQuaternion(Quaternion<Real> const& q)
{
    mQuaternion = q;
    UpdateMatrix();
}
//----------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Transform<Real>::GetQuaternion() const
{
    return mQuaternion;
}
//----------------------------------------------------------------------------
template <typename Real>
void Transform<Real>::SetAxisAngle(AxisAngle<4,Real> const& axisAngle)
{
#if defined(GTE_ASSERT_ON_INVALID_TRANSFORM_INPUT)
    LogAssert(IsUnitLength(axisAngle.axis, (Real)1e-05),
        "Axis must be unit length.");
#endif

    mQuaternion = Rotation<4,Real>(axisAngle);
    UpdateMatrix();
}
//----------------------------------------------------------------------------
template <typename Real>
AxisAngle<4,Real> Transform<Real>::GetAxisAngle() const
{
    return Rotation<4,Real>(mQuaternion);
}
//----------------------------------------------------------------------------
template <typename Real>
void Transform<Real>::SetEulerAngles(EulerAngles<Real> const& eulerAngles)
{
    mQuaternion = Rotation<4,Real>(eulerAngles);
    UpdateMatrix();
}
//----------------------------------------------------------------------------
template <typename Real>
void Transform<Real>::GetEulerAngles(EulerAngles<Real>& eulerAngles) const
{
    eulerAngles = Rotation<4,Real>(mQuaternion);
}
//----------------------------------------------------------------------------
template <typename Real>
void Transform<Real>::SetRotation(Matrix4x4<Real> const& rotation)
{
#if defined(GTE_ASSERT_ON_INVALID_TRANSFORM_INPUT)
    LogAssert(IsRotation(rotation, (Real)1e-5),
        "The matrix is not a rotation.");
#endif

    mQuaternion = Rotation<4,Real>(rotation);
    UpdateMatrix();
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix4x4<Real> Transform<Real>::GetRotation() const
{
    return Rotation<4,Real>(mQuaternion);
}
//----------------------------------------------------------------------------
template <typename Real>
void Transform<Real>::SetScale(Real scale)
{
#if defined(GTE_ASSERT_ON_INVALID_TRANSFORM_INPUT)
    LogAssert(scale != (Real)0, "The scale must be nonzero.");
#endif

    mScale = scale;
    UpdateMatrix();
}
//----------------------------------------------------------------------------
template <typename Real>
Real Transform<Real>::GetScale() const
{
    return mScale;
}
//----------------------------------------------------------------------------
template <typename Real>
void Transform<Real>::SetTranslation(Real x0, Real x1, Real x2)
{
    mTranslation[0] = x0;
    mTranslation[1] = x1;
    mTranslation[2] = x2;
    UpdateMatrix();
}
//----------------------------------------------------------------------------
template <typename Real>
void Transform<Real>::SetTranslation(Vector3<Real> const& translation)
{
    mTranslation[0] = translation[0];
    mTranslation[1] = translation[1];
    mTranslation[2] = translation[2];
    UpdateMatrix();
}
//----------------------------------------------------------------------------
template <typename Real>
void Transform<Real>::SetTranslation(Vector4<Real> const& translation)
{
    mTranslation[0] = translation[0];
    mTranslation[1] = translation[1];
    mTranslation[2] = translation[2];
    UpdateMatrix();
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> Transform<Real>::GetTranslation() const
{
    return Vector3<Real>(mTranslation[0], mTranslation[1], mTranslation[2]);
}
//----------------------------------------------------------------------------
template <typename Real>
Vector4<Real> Transform<Real>::GetTranslationW0() const
{
    return Vector4<Real>(mTranslation[0], mTranslation[1], mTranslation[2],
        (Real)0);
}
//----------------------------------------------------------------------------
template <typename Real>
Vector4<Real> Transform<Real>::GetTranslationW1() const
{
    return Vector4<Real>(mTranslation[0], mTranslation[1], mTranslation[2],
        (Real)1);
}
//----------------------------------------------------------------------------
template <typename Real>
Transform<Real> Transform<Real>::operator*(Transform<Real> const& transform)
    const
{
    Transform<Real> M0M1;
    M0M1.mQuaternion = mQuaternion*transform.mQuaternion;
    M0M1.mScale = mScale*transform.mScale;
    Vector4<Real> trn = Rotate<Real>(mQuaternion,
        mScale*transform.GetTranslationW0()) + GetTranslationW0();
    M0M1.mTranslation[0] = trn[0];
    M0M1.mTranslation[1] = trn[1];
    M0M1.mTranslation[2] = trn[2];
    M0M1.UpdateMatrix();
    return M0M1;
}
//----------------------------------------------------------------------------
template <typename Real>
void Transform<Real>::UpdateMatrix()
{
    Matrix4x4<Real> rotation = Rotation<4,Real>(mQuaternion);
    Matrix4x4<Real>::operator=(mScale*rotation);
#if defined(GTE_USE_MAT_VEC)
    (*this)(0,3) = mTranslation[0];
    (*this)(1,3) = mTranslation[1];
    (*this)(2,3) = mTranslation[2];
#else
    (*this)(3,0) = mTranslation[0];
    (*this)(3,1) = mTranslation[1];
    (*this)(3,2) = mTranslation[2];
#endif
    (*this)(3,3) = (Real)1;
}
//----------------------------------------------------------------------------
#if defined(GTE_ASSERT_ON_INVALID_TRANSFORM_INPUT)
//----------------------------------------------------------------------------
template <typename Real>
bool Transform<Real>::IsRotation(Matrix4x4<Real> const& rotate, Real epsilon)
{
    Matrix4x4<Real> test =
        MultiplyATB(rotate, rotate) - Matrix4x4<Real>::Identity();

    Real maxError = (Real)0;
    for (int i = 0; i < 16; ++i)
    {
        Real error = test[i];
        if (error > maxError)
        {
            maxError = error;
        }
    }

    return maxError < epsilon;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Transform<Real>::IsUnitLength(Vector4<Real> const& axis,
    Real epsilon)
{
    Real error = std::abs(Length(axis) - (Real)1);
    return error < epsilon;
}
//----------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------
