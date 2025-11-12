#include "pch.h"
#include "myEulerAngles.h"
#include "myVec3.h"

const myEulerAngles myEulerAngles::Identity;

myEulerAngles::myEulerAngles() : Yaw(0.f), Pitch(0.f), Roll(0.f)
{
}

void myEulerAngles::Clamp()
{
	Yaw = GetAxisClampValue(Yaw);
	Pitch = GetAxisClampValue(Pitch);
	Roll = GetAxisClampValue(Roll);
}

float myEulerAngles::GetAxisClampValue(float InRotatorValue)
{
	float angle = Math::FMod(InRotatorValue, 360.f);
	if (angle < 0.f)
	{
		angle += 360.f;
	}

	return angle;
}

myVec3 myEulerAngles::ToVec3()
{
	return myVec3(Pitch, Yaw, Roll);
}
