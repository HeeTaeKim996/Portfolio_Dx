#pragma once

#include "myVec3.h"
#include "myQuaternion.h"

class myVec4;
class myMatrix3x3;
class myMatrix4x4;
class myEulerAngles;

class TransformBase
{
public:
	// »ý¼ºÀÚ
	TransformBase();
	TransformBase(const myVec3& InPosition);
	TransformBase(const myVec3& InPosition, const myQuaternion& InRotation);
	TransformBase(const myVec3& InPosition, const myQuaternion& InRotation, const myVec3& InScale);
	TransformBase(const myMatrix4x4& InMatrix);


public:
	// Æ®·£½ºÆû ¼³Á¤ ÇÔ¼ö
	void SetPosition(const myVec3& InPosition) { _position = InPosition; }
	void AddPosition(const myVec3& InDeltaPosition) { _position += InDeltaPosition; }

	void SetRotation(const myEulerAngles& InEulerAngles) { _rotation = myQuaternion(InEulerAngles); }
	void SetRotation(const myMatrix3x3& InMatrix) { _rotation = myQuaternion(InMatrix); }
	void SetRotation(const myQuaternion& InQuaternion) { _rotation = InQuaternion; }

	void AddYawRotation(float InDegree);
	void AddPitchRotation(float InDegree);
	void AddRollRotation(float InDegree);

	void AddUnitXRotation(float InDegree);
	void AddUnitYRotation(float InDegree);
	void AddUnitZRotation(float InDegree);

	void SetScale(const myVec3& InScale) { _scale = InScale; }
	

	myVec3 GetXAxis() const { return _rotation * myVec3::UnitX; }
	myVec3 GetYAxis() const { return _rotation * myVec3::UnitY; }
	myVec3 GetZAxis() const { return _rotation * myVec3::UnitZ; }
	myMatrix4x4 GetSRT() const;
	myEulerAngles GetEulerAngles();

	myVec3 GetPosition() const { return _position; }
	myQuaternion GetRotation() const { return _rotation; }
	myVec3 GetScale() const { return _scale; }

	// Æ®·»½ºÆû º¯È¯
	TransformBase Inverse() const;
	TransformBase LocalToWorld(const TransformBase& InParentWorldTransform) const;
	TransformBase WorldToLocal(const TransformBase& InParentWorldTransform) const;
	myVec3 WorldToLocalPos(const myVec3& InWorldVector) const;


private:
	// ¸â¹ö
	myVec3 _position;
	myQuaternion _rotation;
	myVec3 _scale;
};

