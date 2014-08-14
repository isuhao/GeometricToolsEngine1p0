// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

// Applications
#include "GteCommand.h"
#include "GteEnvironment.h"
#include "GteWICFileIO.h"
#include "GteWindow.h"
#include "GteWindowSystem.h"

// Graphics/Common
#include "GteCamera.h"

// Graphics/Common/SceneGraph
#include "GteBoundingSphere.h"
#include "GteCuller.h"
#include "GteCullingPlane.h"
#include "GteNode.h"
#include "GtePicker.h"
#include "GtePickRecord.h"
#include "GteSpatial.h"
#include "GteVisibleSet.h"

// Graphics/DirectX11/CPU
#include "GteGraphicsObject.h"

// Graphics/DirectX11/CPU/Effects
#include "GteConstantColorEffect.h"
#include "GteFont.h"
#include "GteFontArialW400H18.h"
#include "GteLightAmbientEffect.h"
#include "GteLightDirectionPerPixelEffect.h"
#include "GteLightDirectionPerVertexEffect.h"
#include "GteLightPointPerPixelEffect.h"
#include "GteLightPointPerVertexEffect.h"
#include "GteLightSpotPerPixelEffect.h"
#include "GteLightSpotPerVertexEffect.h"
#include "GteOverlayEffect.h"
#include "GteTextEffect.h"
#include "GteTexture2Effect.h"
#include "GteTexture3Effect.h"
#include "GteVertexColorEffect.h"
#include "GteVisualEffect.h"

// Graphics/DirectX11/CPU/Resources
#include "GteDataFormat.h"
#include "GteResource.h"

// Graphics/DirectX11/CPU/Resources/Buffers
#include "GteBuffer.h"
#include "GteConstantBuffer.h"
#include "GteIndexBuffer.h"
#include "GteIndirectArgumentsBuffer.h"
#include "GteRawBuffer.h"
#include "GteStructuredBuffer.h"
#include "GteStructuredBuffer1.h"
#include "GteStructuredBuffer2.h"
#include "GteStructuredBuffer3.h"
#include "GteTextureBuffer.h"
#include "GteTypedBuffer.h"
#include "GteVertexBuffer.h"
#include "GteVertexFormat.h"

// Graphics/DirectX11/CPU/Resources/Textures
#include "GteDrawTarget.h"
#include "GteTexture.h"
#include "GteTexture1.h"
#include "GteTexture1Array.h"
#include "GteTexture2.h"
#include "GteTexture2Array.h"
#include "GteTexture3.h"
#include "GteTextureArray.h"
#include "GteTextureCube.h"
#include "GteTextureCubeArray.h"
#include "GteTextureDS.h"
#include "GteTextureRT.h"
#include "GteTextureSingle.h"

// Graphics/DirectX11/CPU/Resources/SceneGraph
#include "GteMeshFactory.h"
#include "GteVisual.h"

// Graphics/DirectX11/CPU/Resources/Shaders
#include "GteComputeShader.h"
#include "GteGeometryShader.h"
#include "GtePixelShader.h"
#include "GteShader.h"
#include "GteShaderFactory.h"
#include "GteVertexShader.h"

// Graphics/DirectX11/CPU/Resources/State
#include "GteBlendState.h"
#include "GteDepthStencilState.h"
#include "GteDrawingState.h"
#include "GteRasterizerState.h"
#include "GteSamplerState.h"

// Graphics/DirectX11/DX11
#include "GteDX11Engine.h"
#include "GteDX11GraphicsObject.h"
#include "GteDX11Include.h"
#include "GteDX11PerformanceCounter.h"

// Graphics/DirectX11/DX11/InputLayout
#include "GteDX11InputLayout.h"
#include "GteDX11InputLayoutManager.h"

// Graphics/DirectX11/DX11/Resources
#include "GteDX11Resource.h"

// Graphics/DirectX11/DX11/Resources/Buffers
#include "GteDX11Buffer.h"
#include "GteDX11ConstantBuffer.h"
#include "GteDX11IndexBuffer.h"
#include "GteDX11IndirectArgumentsBuffer.h"
#include "GteDX11RawBuffer.h"
#include "GteDX11StructuredBuffer.h"
#include "GteDX11TextureBuffer.h"
#include "GteDX11VertexBuffer.h"

// Graphics/DirectX11/DX11/Resources/Textures
#include "GteDX11DrawTarget.h"
#include "GteDX11Texture.h"
#include "GteDX11Texture1.h"
#include "GteDX11Texture1Array.h"
#include "GteDX11Texture2.h"
#include "GteDX11Texture2Array.h"
#include "GteDX11Texture3.h"
#include "GteDX11TextureArray.h"
#include "GteDX11TextureCube.h"
#include "GteDX11TextureCubeArray.h"
#include "GteDX11TextureDS.h"
#include "GteDX11TextureRT.h"
#include "GteDX11TextureSingle.h"

// Graphics/DirectX11/DX11/Shaders
#include "GteDX11ComputeShader.h"
#include "GteDX11GeometryShader.h"
#include "GteDX11PixelShader.h"
#include "GteDX11Shader.h"
#include "GteDX11VertexShader.h"

// Graphics/DirectX11/DX11/State
#include "GteDX11BlendState.h"
#include "GteDX11DepthStencilState.h"
#include "GteDX11DrawingState.h"
#include "GteDX11RasterizerState.h"
#include "GteDX11SamplerState.h"

// Graphics/DirectX11/DXGI
#include "GteDXGIAdapter.h"
#include "GteDXGIOutput.h"

// Graphics/DirectX11/HLSL
#include "GteHLSLBaseBuffer.h"
#include "GteHLSLByteAddressBuffer.h"
#include "GteHLSLConstantBuffer.h"
#include "GteHLSLDefiner.h"
#include "GteHLSLFactory.h"
#include "GteHLSLParameter.h"
#include "GteHLSLResource.h"
#include "GteHLSLResourceBindInfo.h"
#include "GteHLSLSamplerState.h"
#include "GteHLSLShader.h"
#include "GteHLSLShaderFactory.h"
#include "GteHLSLShaderType.h"
#include "GteHLSLShaderVariable.h"
#include "GteHLSLStructuredBuffer.h"
#include "GteHLSLTexture.h"
#include "GteHLSLTextureArray.h"
#include "GteHLSLTextureBuffer.h"
#include "GteMemberLayout.h"

// Imagics/Extraction
#include "GteMarchingCubes.h"
#include "GteMarchingCubesTable.h"
#include "GteSurfaceExtractor.h"

// Imagics
#include "GteColorPixels.h"
#include "GteHistogram.h"
#include "GteImage.h"
#include "GteImage1.h"
#include "GteImage2.h"
#include "GteImage3.h"
#include "GteImageUtility2.h"
#include "GteImageUtility3.h"
#include "GteMarchingCubes.h"
#include "GteMarchingCubesTable.h"
#include "GteSurfaceExtractor.h"

// LowLevel/DataTypes
#include "GteArray2.h"
#include "GteAtomicMinMax.h"
#include "GteRangeIteration.h"
#include "GteThreadSafeMap.h"
#include "GteThreadSafeQueue.h"

// LowLevel/Graphs
#include "GteEdgeKey.h"
#include "GteETManifoldMesh.h"
#include "GteFeatureKey.h"
#include "GteTetrahedronKey.h"
#include "GteTriangleKey.h"
#include "GteTSManifoldMesh.h"
#include "GteVEManifoldMesh.h"

// LowLevel/Logger
#include "GteLogger.h"
#include "GteLogReporter.h"
#include "GteLogToFile.h"
#include "GteLogToMessageBox.h"
#include "GteLogToOutputWindow.h"
#include "GteLogToStdout.h"
#include "GteLogToStringArray.h"

// LowLevel/Memory
#include "GteAlignedMemory.h"
#include "GteMemory.h"

// LowLevel/Timer
#include "GteTimer.h"

// Mathematics/Algebra
#include "GteAxisAngle.h"
#include "GteBandedMatrix.h"
#include "GteEulerAngles.h"
#include "GteGMatrix.h"
#include "GteGVector.h"
#include "GteMatrix.h"
#include "GteMatrix2x2.h"
#include "GteMatrix3x3.h"
#include "GteMatrix4x4.h"
#include "GteQuaternion.h"
#include "GteRotation.h"
#include "GteTransform.h"
#include "GteVector.h"
#include "GteVector2.h"
#include "GteVector3.h"
#include "GteVector4.h"

// Mathematics/Approximation
#include "GteApprCircle2.h"
#include "GteApprEllipse2.h"
#include "GteApprEllipsoid3.h"
#include "GteApprGaussian2.h"
#include "GteApprGaussian3.h"
#include "GteApprGreatCircle3.h"
#include "GteApprHeightLine2.h"
#include "GteApprHeightPlane3.h"
#include "GteApprOrthogonalLine2.h"
#include "GteApprOrthogonalLine3.h"
#include "GteApprOrthogonalPlane3.h"
#include "GteApprParaboloid3.h"
#include "GteApprPolynomial2.h"
#include "GteApprPolynomial3.h"
#include "GteApprPolynomial4.h"
#include "GteApprPolynomialSpecial2.h"
#include "GteApprPolynomialSpecial3.h"
#include "GteApprPolynomialSpecial4.h"
#include "GteApprQuadratic2.h"
#include "GteApprQuadratic3.h"
#include "GteApprSphere3.h"

// Mathematics/Arithmetic
#include "GteBitHacks.h"
#include "GteBSNumber.h"
#include "GteBSRational.h"
#include "GteBSUInteger.h"
#include "GteIEEEBinary.h"
#include "GteIEEEBinary16.h"

// Mathematics/ComputationalGeometry
#include "GteConstrainedDelaunay2.h"
#include "GteConvexHull2.h"
#include "GteConvexHull3.h"
#include "GteDelaunay2.h"
#include "GteDelaunay3.h"
#include "GteMinimumAreaBox2.h"
#include "GteMinimumAreaCircle2.h"
#include "GteMinimumVolumeBox3.h"
#include "GteMinimumVolumeSphere3.h"
#include "GteNearestNeighborQuery.h"
#include "GtePrimalQuery2.h"
#include "GtePrimalQuery3.h"
#include "GteSeparatePoints2.h"
#include "GteSeparatePoints3.h"

// Mathematics/Containment
#include "GteContCapsule3.h"
#include "GteContCircle2.h"
#include "GteContCylinder3.h"
#include "GteContEllipse2.h"
#include "GteContEllipse2MinCR.h"
#include "GteContEllipsoid3.h"
#include "GteContEllipsoid3MinCR.h"
#include "GteContOrientedBox2.h"
#include "GteContOrientedBox3.h"
#include "GteContPointInPolygon2.h"
#include "GteContPointInPolyhedron3.h"
#include "GteContScribeCircle2.h"
#include "GteContScribeCircle3Sphere3.h"
#include "GteContSphere3.h"

// Mathematics/CurvesSurfaces/Volumes
#include "GteBasisFunction.h"
#include "GteBezierCurve.h"
#include "GteBSplineCurve.h"
#include "GteBSplineSurface.h"
#include "GteBSplineVolume.h"
#include "GteNaturalSplineCurve.h"
#include "GteNURBSCurve.h"
#include "GteNURBSSurface.h"
#include "GteNURBSVolume.h"
#include "GteTCBSplineCurve.h"

// Mathematics/Distance
#include "GteDCPQuery.h"

// Mathematics/Distance/2D
#include "GteDistPoint2Line2.h"
#include "GteDistPoint2Ray2.h"
#include "GteDistPoint2Segment2.h"

#include "GteDistPoint2AlignedBox2.h"
#include "GteDistPoint2Ellipse2.h"
#include "GteDistPoint2OrientedBox2.h"

#include "GteDistLine2Line2.h"
#include "GteDistLine2Ray2.h"
#include "GteDistLine2Segment2.h"
#include "GteDistRay2Ray2.h"
#include "GteDistRay2Segment2.h"
#include "GteDistSegment2Segment2.h"

// Mathematics/Distance/3D
#include "GteDistPoint3Line3.h"
#include "GteDistPoint3Ray3.h"
#include "GteDistPoint3Segment3.h"

#include "GteDistPoint3Circle3.h"
#include "GteDistPoint3Plane3.h"
#include "GteDistPoint3Rectangle3.h"
#include "GteDistPoint3Triangle3.h"

#include "GteDistPoint3AlignedBox3.h"
#include "GteDistPoint3Ellipsoid3.h"
#include "GteDistPoint3Frustum3.h"
#include "GteDistPoint3OrientedBox3.h"
#include "GteDistPoint3Tetrahedron3.h"

#include "GteDistLine3Line3.h"
#include "GteDistLine3Ray3.h"
#include "GteDistLine3Segment3.h"
#include "GteDistRay3Ray3.h"
#include "GteDistRay3Segment3.h"
#include "GteDistSegment3Segment3.h"

#include "GteDistLine3Rectangle3.h"
#include "GteDistLine3Triangle3.h"
#include "GteDistRay3Rectangle3.h"
#include "GteDistRay3Triangle3.h"
#include "GteDistSegment3Rectangle3.h"
#include "GteDistSegment3Triangle3.h"

#include "GteDistLine3AlignedBox3.h"
#include "GteDistLine3OrientedBox3.h"
#include "GteDistRay3AlignedBox3.h"
#include "GteDistRay3OrientedBox3.h"
#include "GteDistSegment3AlignedBox3.h"
#include "GteDistSegment3OrientedBox3.h"

#include "GteDistCircle3Circle3.h"
#include "GteDistRectangle3Rectangle3.h"
#include "GteDistTriangle3Rectangle3.h"
#include "GteDistTriangle3Triangle3.h"

// Mathematics/Functions
#include "GteACosEstimate.h"
#include "GteASinEstimate.h"
#include "GteATanEstimate.h"
#include "GteConstants.h"
#include "GteCosEstimate.h"
#include "GteExp2Estimate.h"
#include "GteExpEstimate.h"
#include "GteInvSqrtEstimate.h"
#include "GteLog2Estimate.h"
#include "GteLogEstimate.h"
#include "GteSinEstimate.h"
#include "GteSqrtEstimate.h"
#include "GteTanEstimate.h"

// Mathematics/GeometricPrimitives
#include "GteGeometricPrimitive.h"

// Mathematics/GeometricPrimitives/2D
#include "GteAlignedBox2.h"
#include "GteArc2.h"
#include "GteCircle2.h"
#include "GteEllipse2.h"
#include "GteLine2.h"
#include "GteOrientedBox2.h"
#include "GtePolygon2.h"
#include "GteRay2.h"
#include "GteSegment2.h"
#include "GteTriangle2.h"

// Mathematics/GeometricPrimitives/3D
#include "GteAlignedBox3.h"
#include "GteCapsule3.h"
#include "GteCircle3.h"
#include "GteCone3.h"
#include "GteCylinder3.h"
#include "GteEllipse3.h"
#include "GteEllipsoid3.h"
#include "GteFrustum3.h"
#include "GteHalfspace3.h"
#include "GteLine3.h"
#include "GteOrientedBox3.h"
#include "GtePlane3.h"
#include "GtePolyhedron3.h"
#include "GteRay3.h"
#include "GteRectangle3.h"
#include "GteSegment3.h"
#include "GteSphere3.h"
#include "GteTetrahedron3.h"
#include "GteTorus3.h"
#include "GteTriangle3.h"

// Mathematics/Intersection
#include "GteFIQuery.h"
#include "GteTIQuery.h"
#include "GteIntrIntervals.h"

// Mathematics/Intersection/2D
#include "GteIntrLine2Line2.h"
#include "GteIntrLine2Ray2.h"
#include "GteIntrLine2Segment2.h"
#include "GteIntrRay2Ray2.h"
#include "GteIntrRay2Segment2.h"
#include "GteIntrSegment2Segment2.h"

#include "GteIntrArc2Arc2.h"
#include "GteIntrCircle2Arc2.h"
#include "GteIntrCircle2Circle2.h"
#include "GteIntrEllipse2Ellipse2.h"
#include "GteIntrLine2AlignedBox2.h"
#include "GteIntrLine2Arc2.h"
#include "GteIntrLine2Circle2.h"
#include "GteIntrLine2OrientedBox2.h"
#include "GteIntrLine2Triangle2.h"
#include "GteIntrRay2AlignedBox2.h"
#include "GteIntrRay2Arc2.h"
#include "GteIntrRay2Circle2.h"
#include "GteIntrRay2OrientedBox2.h"
#include "GteIntrRay2Triangle2.h"
#include "GteIntrSegment2AlignedBox2.h"
#include "GteIntrSegment2Arc2.h"
#include "GteIntrSegment2Circle2.h"
#include "GteIntrSegment2OrientedBox2.h"
#include "GteIntrSegment2Triangle2.h"

#include "GteIntrAlignedBox2AlignedBox2.h"
#include "GteIntrAlignedBox2OrientedBox2.h"
#include "GteIntrOrientedBox2Circle2.h"
#include "GteIntrOrientedBox2OrientedBox2.h"

// Mathematics/Intersection/3D
#include "GteIntrLine3Plane3.h"
#include "GteIntrLine3Triangle3.h"
#include "GteIntrRay3Plane3.h"
#include "GteIntrRay3Triangle3.h"
#include "GteIntrSegment3Plane3.h"
#include "GteIntrSegment3Triangle3.h"

#include "GteIntrHalfspace3Segment3.h"
#include "GteIntrLine3AlignedBox3.h"
#include "GteIntrLine3Capsule3.h"
#include "GteIntrLine3Cone3.h"
#include "GteIntrLine3Cylinder3.h"
#include "GteIntrLine3Ellipsoid3.h"
#include "GteIntrLine3OrientedBox3.h"
#include "GteIntrLine3Sphere3.h"
#include "GteIntrRay3AlignedBox3.h"
#include "GteIntrRay3Capsule3.h"
#include "GteIntrRay3Cone3.h"
#include "GteIntrRay3Cylinder3.h"
#include "GteIntrRay3Ellipsoid3.h"
#include "GteIntrRay3OrientedBox3.h"
#include "GteIntrRay3Sphere3.h"
#include "GteIntrSegment3AlignedBox3.h"
#include "GteIntrSegment3Capsule3.h"
#include "GteIntrSegment3Cone3.h"
#include "GteIntrSegment3Cylinder3.h"
#include "GteIntrSegment3Ellipsoid3.h"
#include "GteIntrSegment3OrientedBox3.h"
#include "GteIntrSegment3Sphere3.h"

#include "GteIntrPlane3Circle3.h"
#include "GteIntrPlane3Plane3.h"
#include "GteIntrPlane3Triangle3.h"

#include "GteIntrHalfspace3Triangle3.h"
#include "GteIntrPlane3Capsule3.h"
#include "GteIntrPlane3Cylinder3.h"
#include "GteIntrPlane3Ellipsoid3.h"
#include "GteIntrPlane3OrientedBox3.h"
#include "GteIntrPlane3Sphere3.h"

#include "GteIntrAlignedBox3AlignedBox3.h"
#include "GteIntrAlignedBox3OrientedBox3.h"
#include "GteIntrCapsule3Capsule3.h"
#include "GteIntrEllipsoid3Ellipsoid3.h"
#include "GteIntrHalfspace3Capsule3.h"
#include "GteIntrHalfspace3Cylinder3.h"
#include "GteIntrHalfspace3Ellipsoid3.h"
#include "GteIntrHalfspace3OrientedBox3.h"
#include "GteIntrHalfspace3Sphere3.h"
#include "GteIntrOrientedBox3Frustum3.h"
#include "GteIntrOrientedBox3OrientedBox3.h"
#include "GteIntrOrientedBox3Sphere3.h"
#include "GteIntrSphere3Cone3.h"
#include "GteIntrSphere3Frustum3.h"
#include "GteIntrSphere3Sphere3.h"

// Mathematics/NumericalMethods
#include "GteGaussianElimination.h"
#include "GteIntegration.h"
#include "GteLinearSystem.h"
#include "GteMinimize1.h"
#include "GteMinimizeN.h"
#include "GteOdeEuler.h"
#include "GteOdeImplicitEuler.h"
#include "GteOdeMidpoint.h"
#include "GteOdeRungeKutta4.h"
#include "GteOdeSolver.h"
#include "GteRootsBisection.h"
#include "GteRootsBrentsMethod.h"
#include "GteRootsPolynomial.h"
#include "GteSingularValueDecomposition.h"
#include "GteSymmetricEigensolver.h"

// Mathematics/Projection
#include "GteProjection.h"

// Physics/Fluid2
#include "GteFluid2.h"
#include "GteFluid2AdjustVelocity.h"
#include "GteFluid2ComputeDivergence.h"
#include "GteFluid2EnforceStateBoundary.h"
#include "GteFluid2InitializeSource.h"
#include "GteFluid2InitializeState.h"
#include "GteFluid2Parameters.h"
#include "GteFluid2SolvePoisson.h"
#include "GteFluid2UpdateState.h"

// Physics/Fluid3
#include "GteFluid3.h"
#include "GteFluid3AdjustVelocity.h"
#include "GteFluid3ComputeDivergence.h"
#include "GteFluid3EnforceStateBoundary.h"
#include "GteFluid3InitializeSource.h"
#include "GteFluid3InitializeState.h"
#include "GteFluid3Parameters.h"
#include "GteFluid3SolvePoisson.h"
#include "GteFluid3UpdateState.h"
