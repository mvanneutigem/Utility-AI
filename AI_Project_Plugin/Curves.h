#pragma once
#include <cmath>

//polymorphism for different curves here:
struct Curve
{
	Curve(){};
	~Curve() {};
	float a = 1;
	float b = 0;
	virtual float Evaluate(float x) const = 0;
};

struct LinearCurve : Curve
{
	float a = 1;
	float b = 0;
	float Evaluate(float x) const override
	{
		return a*x + b;
	}
};

struct StepCurve : Curve
{
	float a = 0.5f;
	float b = 1.0;
	float Evaluate(float x) const override
	{
		return x > a ? b : -b;
	}
};

struct ExpCurve : Curve
{
	float a = 2.0f;
	float b = 0;
	float Evaluate(float x) const override
	{
		return pow(x, a);
	}
};

struct SigmCurve : Curve
{
	float a = 1.0f;
	float b = 0;
	float Evaluate(float x) const override
	{
		return 1.0f / (1.0f + pow(exp(1.0f), x)) + a;
	}
};