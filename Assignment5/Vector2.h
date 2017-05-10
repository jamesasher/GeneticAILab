
#pragma once
#include <vector>
#include <sstream>
#include <math.h>
#include "Vector3.h"

#define ZERO_VECTOR2 Vector2(0,0)
#define X_VECTOR2 Vector2(1,0)
#define INVERTED_X_VECTOR2 Vector2(-1,0)
#define Y_VECTOR2 Vector2(0,1)
#define INVERTED_Y_VECTOR2 Vector2(0,-1)
#define IDENTITY_VECTOR2 Vector2(1,1)

class Vector2
{
public:
	/*****	 Static Operations	 *****/

	static float dot(const Vector2 v1, const Vector2 v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}


	/*****	Memeber Variables	*****/
	//float axes[2];

	float x, y;


	/*****	Constructors	*****/

	Vector2(float xValue = 0.0, float yValue = 0.0)
	{
		x = xValue; y = yValue;
	}

	/*Vector2(Vector3* otherVector)
	{
		axes[0] = (*otherVector)[0]; axes[1] = (*otherVector)[1];
	}

	Vector2(Vector2* otherVector)
	{
		axes[0] = (*otherVector)[0]; axes[1] = (*otherVector)[1];
	}*/

	/*****	Operations	*****/

	float lengthSqrd() const
	{
		return x * x + y * y;
		//return dot(*this, *this);
	}

	float length() const
	{
		return sqrt(x * x + y * y);
		//return sqrt(dot(*this, *this))
	}

	void normalize()
	{
		float scalar = sqrt(x * x + y * y);
		x /= scalar; y /= scalar;
		//*this /= length();
	}

	Vector2 normalizedCopy() const
	{
		float scalar = sqrt(x * x + y * y);
		//float scalar = length();
		return Vector2(x / scalar, y / scalar);
	}


	/*****	Operator Overloads	*****/

	//Removed to enforce faster use of calling the public func directly
	//float operator[](const unsigned idx) const
	//{
	//Without check, slightly faster but may opens it up for unsafe use
	/*if (idx > 2)
	throw std::out_of_range("Vector3 idx is out of range (idx > 2)");*/
	//	return axes[idx];
	//}

	Vector2 operator+(const Vector2 other) const
	{
		return Vector2(x + other.x, y + other.y);
	}
	void operator+=(const Vector2 other)
	{
		x += other.x; y += other.y;
	}

	Vector2 operator-(const Vector2 other) const
	{
		return Vector2(x - other.x, y - other.y);
	}
	void operator-=(const Vector2 other)
	{
		x -= other.x; x -= other.x;
	}

	void operator=(const Vector2 other)
	{
		x = other.x; y = other.y;
	}

	bool operator==(const Vector2 other) const
	{
		if (x == other.x && y == other.y)
			return true;
		return false;
	}

	Vector2 operator*(float scalar) const
	{
		return Vector2(x * scalar, y * scalar);
	}
	Vector2 operator*(const Vector2 other) const
	{
		return Vector2(x * other.x, y * other.y);
	}
	void operator*=(float scalar)
	{
		x *= scalar; y *= scalar;
	}
	void operator*=(const Vector2 other)
	{
		x *= other.x; y *= other.y;
	}

	Vector2 operator/(float scalar) const
	{
		return Vector2(x / scalar, y / scalar);
	}
	Vector2 operator/(const Vector2 other) const
	{
		return Vector2(x / other.x, y / other.y);
	}
	void operator/=(float scalar)
	{
		x /= scalar; y /= scalar;
	}
	void operator/=(const Vector2 other)
	{
		x /= other.x; y /= other.y;
	}

	float operator[](int idx) const
	{
		switch (idx)
		{
		case 0:
			return x;
		case 1:
			return y;
		}
		return -1.0;
	}
	float & operator[](int idx)
	{
		switch (idx)
		{
		case 0:
			return x;
		case 1:
			return y;
		}
		float d = -1.0;
		return d;
	}


	/*****	Utillity Funtions	*****/

	std::string toString() const
	{
		std::stringstream ss;
		ss << "Vector2<" << x << ", " << y << ">";
		return ss.str();
	}

	void printMe() const
	{
		printf("%s\n", toString().c_str());
	}

	Vector2 copy() const
	{
		return Vector2(x, y);
	}
};
