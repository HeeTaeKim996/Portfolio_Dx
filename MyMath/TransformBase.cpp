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

	// 스케일 회전 행렬만 분리
	myMatrix3x3 scaleRotMatrix(InMatrix[0].ToVec3(), InMatrix[1].ToVec3(), InMatrix[2].ToVec3());


	// 스케일 구하기
	const float squareSumX = scaleRotMatrix[0].SizeSquared();
	const float squareSumY = scaleRotMatrix[1].SizeSquared();
	const float squareSumZ = scaleRotMatrix[2].SizeSquared();

	_scale.X = squareSumX > SMALL_NUMBER ? sqrtf(squareSumX) : SMALL_NUMBER;
	_scale.Y = squareSumY > SMALL_NUMBER ? sqrtf(squareSumY) : SMALL_NUMBER;
	_scale.Z = squareSumZ > SMALL_NUMBER ? sqrtf(squareSumZ) : SMALL_NUMBER;

	scaleRotMatrix[0] /= _scale.X;
	scaleRotMatrix[1] /= _scale.Y;
	scaleRotMatrix[2] /= _scale.Z;

	// 사원수로 변환
	_rotation = myQuaternion(scaleRotMatrix);
}




//#define THIS_IS_NOT_QUATERNION_ROTATION_THIS_IS_SAME_AS_MATRIX_ROTATION
void TransformBase::AddYawRotation(float InDegree)
{
#ifdef THIS_IS_NOT_QUATERNION_ROTATION_THIS_IS_SAME_AS_MATRIX_ROTATION // 기존 강의 코드인데 왜 이렇게 했던 걸까
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
	/*	- 주의. 현재 LH 이므로, q2* q1* v q1 q2 이므로, 로테이션을 사원수로 연산할 때, 기존 q1이 좌측, 새로운 q2가 우측
		- 기존 강의의 경우, RH로 작성됐으므로, q2 q1 v q1* q2* 이므로, 강의 코드에서는 기존 q1가 우측, 새로운 q2가 좌측 */


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
	/*	- 주의. 현재 LH 이므로, q2* q1* v q1 q2 이므로, 로테이션을 사원수로 연산할 때, 기존 q1이 좌측, 새로운 q2가 우측
		- 기존 강의의 경우, RH로 작성됐으므로, q2 q1 v q1* q2* 이므로, 강의 코드에서는 기존 q1가 우측, 새로운 q2가 좌측 */

	ret.SetPosition(
		invParent.GetPosition()
		+ (invParent.GetRotation() * GetPosition()) * invParent.GetScale()
	);
	/*	- 강의 원본 코드에는, posW 를 scaleP 부터 처리후에, rotP 를 처리함.
		- 로컬 -> 월드를 posL 에 scaleP 를 하고, rotP 를 했으니, 월드 -> 로컬에서는 역순으로 해야 구할 수 있는 건데,
		  강의 코드에 오류가 있는 건지, 내가 이해를 잘 못했던건지.. 모르겠다.
		- 돌려보니 잘 작동한다. 강의 코드가 오류인듯
	*/


	return ret;
}

myVec3 TransformBase::WorldToLocalPos(const myVec3& InWorldVector) const
{
	TransformBase ownsInverse = Inverse();

	return ownsInverse.GetPosition() + ((ownsInverse.GetRotation() * InWorldVector) * ownsInverse.GetScale());
	// 마찬가지로 잘 작동한다. 강의 코드가 오류인듯
}
