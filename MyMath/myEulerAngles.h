#pragma once

class myVec3;

class myEulerAngles
{
public:
	myEulerAngles();
	myEulerAngles(float InPitch, float InYaw, float InRoll) : Pitch(InPitch), Yaw(InYaw), Roll(InRoll) {}

	void Clamp();

	float GetAxisClampValue(float InRotatorValue);

	const static myEulerAngles Identity;

	myVec3 ToVec3();

	myEulerAngles operator-() const { return myEulerAngles(-Pitch, -Yaw, -Roll); }

public: 
	float Pitch;
	float Yaw;
	float Roll;
};

