//Tim Bader

#pragma once
#include <vector>
#include <sstream>
#include <math.h>
#include "Vector2.h"

#define ZERO_VECTOR3 Vector3(0,0,0)
#define X_VECTOR3 Vector3(1,0,0)
#define INVERTED_X_VECTOR3 Vector3(-1,0,0)
#define Y_VECTOR3 Vector3(0,1,0)
#define INVERTED_Y_VECTOR3 Vector3(0,-1,0)
#define Z_VECTOR3 Vector3(0,0,1)
#define INVERTED_Z_VECTOR3 Vector3(0,0,-1)
#define IDENTITY_VECTOR3 Vector3(1,1,1)

/*
Author: Tim Bader
*/
class Vector3
{
public:
	/*****	 Static Operations	 *****/

	static float dot(const Vector3 v1,const Vector3 v2)
	{
		return v1.axes[0]*v2.axes[0] + v1.axes[1] * v2.axes[1] + v1.axes[2] * v2.axes[2];
	}

	static Vector3 cross(const Vector3 v1, const Vector3 v2)
	{
		return Vector3(v1.axes[1]*v2.axes[2] - v1.axes[2]*v2.axes[1], v1.axes[2]*v2.axes[0] - v1.axes[0]*v2.axes[2], v1.axes[0]*v2.axes[1] - v1.axes[1]*v2.axes[0]);
	}


	/*****	Memeber Variables	*****/
	float axes[3];


	/*****	Constructors	*****/

	Vector3(const float x = 0.0, const float y = 0.0, const float z = 0.0)
	{
		axes[0] = x; axes[1] = y; axes[2] = z;
	}

	/*Vector3(Vector3* otherVector)
	{
		axes[0] = (*otherVector)[0]; axes[1] = (*otherVector)[1]; axes[3] = (*otherVector)[3];
	}

	Vector3(Vector2* otherVector)
	{
		axes[0] = (*otherVector)[0]; axes[1] = (*otherVector)[1]; axes[3] = 0.0;
	}*/


	/*****	Operations	*****/

	float lengthSqrd() const
	{
		return axes[0]*axes[0] + axes[1]*axes[1] + axes[2]*axes[2];
		//return dot(*this, *this);
	}

	float length() const
	{
		return sqrt(axes[0]*axes[0] + axes[1]*axes[1] + axes[2]*axes[2]);
		//return sqrt(dot(*this, *this))
	}

	void normalize()
	{
		float scalar = sqrt(axes[0] * axes[0] + axes[1] * axes[1] + axes[2] * axes[2]);
		axes[0] /= scalar; axes[1] /= scalar; axes[2] /= scalar;
		//*this /= length();
	}

	Vector3 normalizedCopy() const
	{
		float scalar = sqrt(axes[0] * axes[0] + axes[1] * axes[1] + axes[2] * axes[2]);
		//float scalar = length();
		return Vector3(axes[0] / scalar, axes[1] / scalar, axes[2] / scalar);
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

	Vector3 operator+(const Vector3 other) const
	{
		return Vector3(axes[0] + other.axes[0], axes[1] + other.axes[1], axes[2] + other.axes[2]);
	}
	void operator+=(const Vector3 other)
	{
		axes[0] += other.axes[0]; axes[1] += other.axes[1]; axes[2] += other.axes[2];
	}

	Vector3 operator-(const Vector3 other) const
	{
		return Vector3(axes[0] - other.axes[0], axes[1] - other.axes[1], axes[2] - other.axes[2]);
	}
	void operator-=(const Vector3 other)
	{
		axes[0] -= other.axes[0]; axes[1] -= other.axes[1]; axes[2] -= other.axes[2];
	}

	void operator=(const Vector3 other)
	{
		axes[0] = other.axes[0]; axes[1] = other.axes[1]; axes[2] = other.axes[2];
	}

	bool operator==(const Vector3 other) const
	{
		if (axes[0] == other.axes[0] && axes[1] == other.axes[1] && axes[2] == other.axes[2])
			return true;
		return false;
	}

	Vector3 operator*(const float scalar) const
	{
		return Vector3(axes[0] * scalar, axes[1] * scalar, axes[2] * scalar);
	}
	Vector3 operator*(const Vector3 other) const
	{
		return Vector3(axes[0] * other.axes[0], axes[1] * other.axes[1], axes[2] * other.axes[2]);
	}
	void operator*=(const float scalar)
	{
		axes[0] *= scalar; axes[1] *= scalar; axes[2] *= scalar;
	}
	void operator*=(const Vector3 other)
	{
		axes[0] *= other.axes[0]; axes[1] *= other.axes[1]; axes[2] *= other.axes[2];
	}

	Vector3 operator/(const float scalar) const
	{
		return Vector3(axes[0] / scalar, axes[1] / scalar, axes[2] / scalar);
	}
	Vector3 operator/(const Vector3 other) const
	{
		return Vector3(axes[0] / other.axes[0], axes[1] / other.axes[1], axes[2] / other.axes[2]);
	}
	void operator/=(const float scalar)
	{
		axes[0] /= scalar; axes[1] /= scalar; axes[2] /= scalar;
	}
	void operator/=(const Vector3 other)
	{
		axes[0] /= other.axes[0]; axes[1] /= other.axes[1]; axes[2] /= other.axes[2];
	}

	//Will not do any safe checking for preformance
	float operator[](int idx) const
	{
		return axes[idx];
	}
	float & operator[](int idx)
	{
		return axes[idx];
	}

	/*****	Utillity Funtions	*****/

	std::string toString() const
	{
		std::stringstream ss;
		ss << "Vector3<" << axes[0] << ", " << axes[1] << ", " << axes[2] << ">";
		return ss.str();
	}

	void printMe() const
	{
		printf("%s\n", toString().c_str());
	}

	Vector3 copy() const
	{
		return Vector3(axes[0], axes[1], axes[2]);
	}
};
