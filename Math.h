#pragma once
namespace Math
{
	extern float NormalizeYaw(float value);
	extern void VectorAngles(const Vector&vecForward, Vector&vecAngles);
	extern void AngleVectors(const Vector angles, Vector& forward, Vector& right, Vector& up);
	extern void VectorMA(const Vector & start, float scale, const Vector & direction, Vector & dest);
	extern void NormalizeVector(Vector & vecIn);
	extern void AngleVectors(const QAngle &angles, Vector* forward);
	extern void AngleVectors(const QAngle & angles, Vector & forward);
	extern void ClampAngles(QAngle& angles);
	extern float RandomFloat(float min, float max);
	extern float RandomFloat2(float min, float max);
	extern void AngleVectors(const Vector &angles, Vector *forward, Vector *right, Vector *up);
	extern void CalcAngle(Vector src, Vector dst, Vector& angles);
}

extern bool enabledtp;