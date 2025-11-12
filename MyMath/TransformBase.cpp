#include "pch.h"
#include "TransformBase.h"

#include "myVec4.h"
#include "myMatrix3x3.h"
#include "myMatrix4x4.h"
#include "myEulerAngles.h"


TransformBase::TransformBase() : _position(myVec3::Zero), _rotation(myQuaternion::Identity), _scale(myVec3::One)
{
}

TransformBase::TransformBase(const myVec3& InPosition)
	:_position(InPosition), _rotation(myQuaternion::Identity), _scale(myVec3::One)
{
}

TransformBase::TransformBase(const myVec3& InPosition, const myQuaternion& InRotation)
	: _position(InPosition), _rotation(InRotation), _scale(myVec3::One)
{
}

TransformBase::TransformBase(const myVec3& InPosition, const myQuaternion& InRotation, const myVec3& InScale)
	: _position(InPosition), _rotation(InRotation), _scale(InScale)
{
}

TransformBase::TransformBase(const myMatrix4x4& InMatrix)
{
	_position = InMatrix[3].ToVec3();

	myMatrix3x3 scaleRotMatrix(InMatrix[0].ToVec3(), InMatrix[1].ToVec3(), InMatrix[2].ToVec3());

	const float squareSumX = scaleRotMatrix[0].SizeSquared();
	const float squareSumY = scaleRotMatrix[1].SizeSquared();
	const float squareSumZ = scaleRotMatrix[2].SizeSquared();

	_scale.X = squareSumX > SMALL_NUMBER ? sqrtf(squareSumX) : SMALL_NUMBER;
	_scale.Y = squareSumY > SMALL_NUMBER ? sqrtf(squareSumY) : SMALL_NUMBER;
	_scale.Z = squareSumZ > SMALL_NUMBER ? sqrtf(squareSumZ) : SMALL_NUMBER;

	scaleRotMatrix[0] /= _scale.X;
	scaleRotMatrix[1] /= _scale.Y;
	scaleRotMatrix[2] /= _scale.Z;

	_rotation = myQuaternion(scaleRotMatrix);
}




//#define THIS_IS_NOT_QUATERNION_ROTATION_THIS_IS_SAME_AS_MATRIX_ROTATION
void TransformBase::AddYawRotation(float InDegree)
{
#ifdef THIS_IS_NOT_QUATERNION_ROTATION_THIS_IS_SAME_AS_MATRIX_ROTATION 
	myEulerAngles r = _rotation.ToEulerAngles();
	r.Yaw += InDegree;
	r.Clamp();
	_rotation = myQuaternion(r);
#else
	myQuaternion addingYawQuat(GetYAxis().GetNormalize(), InDegree);
	_rotation *= addingYawQuat;
#endif
}

void TransformBase::AddPitchRotation(float InDegree)
{
#ifdef THIS_IS_NOT_QUATERNION_ROTATION_THIS_IS_SAME_AS_MATRIX_ROTATION
	myEulerAngles r = _rotation.ToEulerAngles();
	r.Pitch += InDegree;
	r.Clamp();
	_rotation = myQuaternion(r);
#else
	myQuaternion addingPitchQuat(GetXAxis().GetNormalize(), InDegree);
	_rotation *= addingPitchQuat;
#endif
}

void TransformBase::AddRollRotation(float InDegree)
{
#ifdef THIS_IS_NOT_QUATERNION_ROTATION_THIS_IS_SAME_AS_MATRIX_ROTATION
	myEulerAngles r = _rotation.ToEulerAngles();
	r.Roll += InDegree;
	r.Clamp();
	_rotation = myQuaternion(r);
#else
	myQuaternion addingRollQuat(GetZAxis().GetNormalize(), InDegree);
	_rotation *= addingRollQuat;
#endif
}

void TransformBase::AddUnitXRotation(float InDegree)
{
	myQuaternion addingPitchQuat(myVec3::UnitX, InDegree);
	_rotation *= addingPitchQuat;
}

void TransformBase::AddUnitYRotation(float InDegree)
{
	myQuaternion addingYawQuat(myVec3::UnitY, InDegree);
	_rotation *= addingYawQuat;
}

void TransformBase::AddUnitZRotation(float InDegree)
{
	myQuaternion addingRollQuat(myVec3::UnitZ, InDegree);
	_rotation *= addingRollQuat;
}


myMatrix4x4 TransformBase::GetSRT() const
{
	return myMatrix4x4(
		myVec4(GetXAxis() * _scale.X, 0),
		myVec4(GetYAxis() * _scale.Y, 0),
		myVec4(GetZAxis() * _scale.Z, 0),
		myVec4(_position, 1)
	);
}

myEulerAngles TransformBase::GetEulerAngles()
{
	return _rotation.ToEulerAngles();
}

TransformBase TransformBase::Inverse() const
{
	myVec3 reciprocalScale = myVec3::Zero;
	if (!Math::EqualsInTolerance(_scale.X, 0.f)) reciprocalScale.X = 1.f / _scale.X;
	if (!Math::EqualsInTolerance(_scale.Y, 0.f)) reciprocalScale.Y = 1.f / _scale.Y;
	if (!Math::EqualsInTolerance(_scale.Z, 0.f)) reciprocalScale.Z = 1.f / _scale.Z;

	TransformBase ret;
	ret.SetScale(reciprocalScale);
	myQuaternion invQuat = _rotation.Inverse();
	ret.SetRotation(invQuat);
	ret.SetPosition((invQuat * (-_position)) * reciprocalScale);

	return ret;
}

TransformBase TransformBase::LocalToWorld(const TransformBase& InParentWorldTransform) const
{
	TransformBase ret;

	ret.SetScale(InParentWorldTransform.GetScale() * GetScale());

	ret.SetRotation(GetRotation() * InParentWorldTransform.GetRotation());

	ret.SetPosition(
		InParentWorldTransform.GetPosition()
		+ InParentWorldTransform.GetRotation() * (GetPosition() * InParentWorldTransform.GetScale())
	);

	return ret;
}

TransformBase TransformBase::WorldToLocal(const TransformBase& InParentWorldTransform) const
{
	TransformBase invParent = InParentWorldTransform.Inverse();
	TransformBase ret;

	ret.SetScale(GetScale() * invParent.GetScale());

	ret.SetRotation(GetRotation() * invParent.GetRotation());
	ret.SetPosition(
		invParent.GetPosition()
		+ (invParent.GetRotation() * GetPosition()) * invParent.GetScale()
	);

	return ret;
}

myVec3 TransformBase::WorldToLocalPos(const myVec3& InWorldVector) const
{
	TransformBase ownsInverse = Inverse();

	return ownsInverse.GetPosition() + ((ownsInverse.GetRotation() * InWorldVector) * ownsInverse.GetScale());
}
