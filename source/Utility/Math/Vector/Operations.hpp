
// Do not include this file manually.

namespace Utility
{
	namespace Math
	{

#ifdef ALIB_MATH_VECTOR2


#ifndef ALIB_MATH_VECTOR2_OPERATIONS
#define ALIB_MATH_VECTOR2_OPERATIONS


// Vector2 , Vector Arithmetic , Addition

template<typename TypeLeft, typename TypeRight>
strong_inline Vector2<TypeLeft> operator + (const Vector2<TypeLeft>& aLeft, const Vector2<TypeRight>& aRight)
{
	return Vector2<TypeLeft>(aLeft.x + aRight.x, aLeft.y + aRight.y);
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right>
strong_inline Vector2<TypeLeft> operator + (const Vector2<TypeLeft>& aLeft, const Vector2Swizzle<TypeRight, Offset1Right, Offset2Right>& aRight)
{
	return Vector2<TypeLeft>(aLeft.x + aRight.x, aLeft.y + aRight.y);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left>
strong_inline Vector2<TypeLeft> operator + (const Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const Vector2<TypeRight>& aRight)
{
	return Vector2<TypeLeft>(aLeft.x + aRight.x, aLeft.y + aRight.y);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset1Right, int Offset2Right>
strong_inline Vector2<TypeLeft> operator + (const Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const Vector2Swizzle<TypeRight, Offset1Right, Offset2Right>& aRight)
{
	return Vector2<TypeLeft>(aLeft.x + aRight.x, aLeft.y + aRight.y);
}

// Vector2 , Vector Arithmetic , Substraction

template<typename TypeLeft, typename TypeRight>
strong_inline Vector2<TypeLeft> operator - (const Vector2<TypeLeft>& aLeft, const Vector2<TypeRight>& aRight)
{
	return Vector2<TypeLeft>(aLeft.x - aRight.x, aLeft.y - aRight.y);
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right>
strong_inline Vector2<TypeLeft> operator - (const Vector2<TypeLeft>& aLeft, const Vector2Swizzle<TypeRight, Offset1Right, Offset2Right>& aRight)
{
	return Vector2<TypeLeft>(aLeft.x - aRight.x, aLeft.y - aRight.y);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left>
strong_inline Vector2<TypeLeft> operator - (const Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const Vector2<TypeRight>& aRight)
{
	return Vector2<TypeLeft>(aLeft.x - aRight.x, aLeft.y - aRight.y);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset1Right, int Offset2Right>
strong_inline Vector2<TypeLeft> operator - (const Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const Vector2Swizzle<TypeRight, Offset1Right, Offset2Right>& aRight)
{
	return Vector2<TypeLeft>(aLeft.x - aRight.x, aLeft.y - aRight.y);
}

// Vector2 , Vector Arithmetic , Multiplication

template<typename TypeLeft, typename TypeRight>
strong_inline Vector2<TypeLeft> operator * (const Vector2<TypeLeft>& aLeft, const Vector2<TypeRight>& aRight)
{
	return Vector2<TypeLeft>(aLeft.x * aRight.x, aLeft.y * aRight.y);
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right>
strong_inline Vector2<TypeLeft> operator * (const Vector2<TypeLeft>& aLeft, const Vector2Swizzle<TypeRight, Offset1Right, Offset2Right>& aRight)
{
	return Vector2<TypeLeft>(aLeft.x * aRight.x, aLeft.y * aRight.y);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left>
strong_inline Vector2<TypeLeft> operator * (const Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const Vector2<TypeRight>& aRight)
{
	return Vector2<TypeLeft>(aLeft.x * aRight.x, aLeft.y * aRight.y);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset1Right, int Offset2Right>
strong_inline Vector2<TypeLeft> operator * (const Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const Vector2Swizzle<TypeRight, Offset1Right, Offset2Right>& aRight)
{
	return Vector2<TypeLeft>(aLeft.x * aRight.x, aLeft.y * aRight.y);
}

// Vector2 , Vector Arithmetic , Division

template<typename TypeLeft, typename TypeRight>
strong_inline Vector2<TypeLeft> operator / (const Vector2<TypeLeft>& aLeft, const Vector2<TypeRight>& aRight)
{
	return Vector2<TypeLeft>(aLeft.x / aRight.x, aLeft.y / aRight.y);
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right>
strong_inline Vector2<TypeLeft> operator / (const Vector2<TypeLeft>& aLeft, const Vector2Swizzle<TypeRight, Offset1Right, Offset2Right>& aRight)
{
	return Vector2<TypeLeft>(aLeft.x / aRight.x, aLeft.y / aRight.y);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left>
strong_inline Vector2<TypeLeft> operator / (const Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const Vector2<TypeRight>& aRight)
{
	return Vector2<TypeLeft>(aLeft.x / aRight.x, aLeft.y / aRight.y);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset1Right, int Offset2Right>
strong_inline Vector2<TypeLeft> operator / (const Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const Vector2Swizzle<TypeRight, Offset1Right, Offset2Right>& aRight)
{
	return Vector2<TypeLeft>(aLeft.x / aRight.x, aLeft.y / aRight.y);
}


// Vector2 , Vector Arithmetic , Assign Addition

template<typename TypeLeft, typename TypeRight>
strong_inline Vector2<TypeLeft>& operator += (Vector2<TypeLeft>& aLeft, const Vector2<TypeRight> aRight)
{
	aLeft.x += aRight.x;
	aLeft.y += aRight.y;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right>
strong_inline Vector2<TypeLeft>& operator += (Vector2<TypeLeft>& aLeft, const Vector2Swizzle<TypeRight, Offset1Right, Offset2Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	aLeft.x += x;
	aLeft.y += y;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left>
strong_inline Vector2<TypeLeft>& operator += (Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const Vector2<TypeRight>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	aLeft.x += x;
	aLeft.y += y;
	return aLeft.vector;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset1Right, int Offset2Right>
strong_inline Vector2<TypeLeft>& operator += (Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const Vector2Swizzle<TypeRight, Offset1Right, Offset2Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	aLeft.x += x;
	aLeft.y += y;
	return aLeft.vector;
}


// Vector2 , Vector Arithmetic , Assign Substraction

template<typename TypeLeft, typename TypeRight>
strong_inline Vector2<TypeLeft>& operator -= (Vector2<TypeLeft>& aLeft, const Vector2<TypeRight> aRight)
{
	aLeft.x -= aRight.x;
	aLeft.y -= aRight.y;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right>
strong_inline Vector2<TypeLeft>& operator -= (Vector2<TypeLeft>& aLeft, const Vector2Swizzle<TypeRight, Offset1Right, Offset2Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	aLeft.x -= x;
	aLeft.y -= y;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left>
strong_inline Vector2<TypeLeft>& operator -= (Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const Vector2<TypeRight>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	aLeft.x -= x;
	aLeft.y -= y;
	return aLeft.vector;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset1Right, int Offset2Right>
strong_inline Vector2<TypeLeft>& operator -= (Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const Vector2Swizzle<TypeRight, Offset1Right, Offset2Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	aLeft.x -= x;
	aLeft.y -= y;
	return aLeft.vector;
}


// Vector2 , Vector Arithmetic , Assign Multiplication

template<typename TypeLeft, typename TypeRight>
strong_inline Vector2<TypeLeft>& operator *= (Vector2<TypeLeft>& aLeft, const Vector2<TypeRight> aRight)
{
	aLeft.x *= aRight.x;
	aLeft.y *= aRight.y;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right>
strong_inline Vector2<TypeLeft>& operator *= (Vector2<TypeLeft>& aLeft, const Vector2Swizzle<TypeRight, Offset1Right, Offset2Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	aLeft.x *= x;
	aLeft.y *= y;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left>
strong_inline Vector2<TypeLeft>& operator *= (Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const Vector2<TypeRight>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	aLeft.x *= x;
	aLeft.y *= y;
	return aLeft.vector;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset1Right, int Offset2Right>
strong_inline Vector2<TypeLeft>& operator *= (Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const Vector2Swizzle<TypeRight, Offset1Right, Offset2Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	aLeft.x *= x;
	aLeft.y *= y;
	return aLeft.vector;
}


// Vector2 , Vector Arithmetic , Assign Division

template<typename TypeLeft, typename TypeRight>
strong_inline Vector2<TypeLeft>& operator /= (Vector2<TypeLeft>& aLeft, const Vector2<TypeRight> aRight)
{
	aLeft.x /= aRight.x;
	aLeft.y /= aRight.y;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right>
strong_inline Vector2<TypeLeft>& operator /= (Vector2<TypeLeft>& aLeft, const Vector2Swizzle<TypeRight, Offset1Right, Offset2Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	aLeft.x /= x;
	aLeft.y /= y;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left>
strong_inline Vector2<TypeLeft>& operator /= (Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const Vector2<TypeRight>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	aLeft.x /= x;
	aLeft.y /= y;
	return aLeft.vector;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left = 0, int Offset2Left = 1, int Offset1Right = 0, int Offset2Right = 1>
strong_inline Vector2<TypeLeft>& operator /= (Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const Vector2Swizzle<TypeRight, Offset1Right, Offset2Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	aLeft.x /= x;
	aLeft.y /= y;
	return aLeft.vector;
}


// Vector2 , Scalar Arithmetic , Addition

template<typename TypeLeft>
strong_inline Vector2<TypeLeft> operator + (const Vector2<TypeLeft>& aLeft, const TypeLeft& aRight)
{
	return Vector2<TypeLeft>(aLeft.x + aRight, aLeft.y + aRight);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left>
strong_inline Vector2<TypeLeft> operator + (const Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const TypeLeft& aRight)
{
	return Vector2<TypeLeft>(aLeft.x + aRight, aLeft.y + aRight);
}

template<typename TypeLeft>
strong_inline Vector2<TypeLeft> operator + (const TypeLeft& aLeft, const Vector2<TypeLeft>& aRight)
{
	return Vector2<TypeLeft>(aLeft + aRight.x, aLeft + aRight.y);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left>
strong_inline Vector2<TypeLeft> operator + (const TypeLeft& aLeft, const Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aRight)
{
	return Vector2<TypeLeft>(aLeft + aRight.x, aLeft + aRight.y);
}

// Vector2 , Scalar Arithmetic , Substraction

template<typename TypeLeft>
strong_inline Vector2<TypeLeft> operator - (const Vector2<TypeLeft>& aLeft, const TypeLeft& aRight)
{
	return Vector2<TypeLeft>(aLeft.x - aRight, aLeft.y - aRight);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left>
strong_inline Vector2<TypeLeft> operator - (const Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const TypeLeft& aRight)
{
	return Vector2<TypeLeft>(aLeft.x - aRight, aLeft.y - aRight);
}

template<typename TypeLeft>
strong_inline Vector2<TypeLeft> operator - (const TypeLeft& aLeft, const Vector2<TypeLeft>& aRight)
{
	return Vector2<TypeLeft>(aLeft - aRight.x, aLeft - aRight.y);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left>
strong_inline Vector2<TypeLeft> operator - (const TypeLeft& aLeft, const Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aRight)
{
	return Vector2<TypeLeft>(aLeft - aRight.x, aLeft - aRight.y);
}


// Vector2 , Scalar Arithmetic , Multiplication

template<typename TypeLeft>
strong_inline Vector2<TypeLeft> operator * (const Vector2<TypeLeft>& aLeft, const TypeLeft& aRight)
{
	return Vector2<TypeLeft>(aLeft.x * aRight, aLeft.y * aRight);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left>
strong_inline Vector2<TypeLeft> operator * (const Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const TypeLeft& aRight)
{
	return Vector2<TypeLeft>(aLeft.x * aRight, aLeft.y * aRight);
}

template<typename TypeLeft>
strong_inline Vector2<TypeLeft> operator * (const TypeLeft& aLeft, const Vector2<TypeLeft>& aRight)
{
	return Vector2<TypeLeft>(aLeft * aRight.x, aLeft * aRight.y);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left>
strong_inline Vector2<TypeLeft> operator * (const TypeLeft& aLeft, const Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aRight)
{
	return Vector2<TypeLeft>(aLeft * aRight.x, aLeft * aRight.y);
}

// Vector2 , Scalar Arithmetic , Division

template<typename TypeLeft>
strong_inline Vector2<TypeLeft> operator / (const Vector2<TypeLeft>& aLeft, const TypeLeft& aRight)
{
	return Vector2<TypeLeft>(aLeft.x / aRight, aLeft.y / aRight);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left>
strong_inline Vector2<TypeLeft> operator / (const Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const TypeLeft& aRight)
{
	return Vector2<TypeLeft>(aLeft.x / aRight, aLeft.y / aRight);
}

template<typename TypeLeft>
strong_inline Vector2<TypeLeft> operator / (const TypeLeft& aLeft, const Vector2<TypeLeft>& aRight)
{
	return Vector2<TypeLeft>(aLeft / aRight.x, aLeft / aRight.y);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left>
strong_inline Vector2<TypeLeft> operator / (const TypeLeft& aLeft, const Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aRight)
{
	return Vector2<TypeLeft>(aLeft / aRight.x, aLeft / aRight.y);
}





// Vector2 , Scalar Arithmetic , Assign Addition

template<typename TypeLeft, typename TypeRight>
strong_inline Vector2<TypeLeft>& operator += (Vector2<TypeLeft>& aLeft, const TypeRight& aRight)
{
	aLeft.x += aRight;
	aLeft.y += aRight;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left>
strong_inline Vector2<TypeLeft>& operator += (Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const TypeRight& aRight)
{
	aLeft.x += aRight;
	aLeft.y += aRight;
	return aLeft.vector;
}

// Vector2 , Scalar Arithmetic , Assign Substraction

template<typename TypeLeft, typename TypeRight>
strong_inline Vector2<TypeLeft>& operator -= (Vector2<TypeLeft>& aLeft, const TypeRight& aRight)
{
	aLeft.x -= aRight;
	aLeft.y -= aRight;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left>
strong_inline Vector2<TypeLeft>& operator -= (Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const TypeRight& aRight)
{
	aLeft.x -= aRight;
	aLeft.y -= aRight;
	return aLeft.vector;
}


// Vector2 , Scalar Arithmetic , Assign Multiplication

template<typename TypeLeft, typename TypeRight>
strong_inline Vector2<TypeLeft>& operator *= (Vector2<TypeLeft>& aLeft, const TypeRight& aRight)
{
	aLeft.x *= aRight;
	aLeft.y *= aRight;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left>
strong_inline Vector2<TypeLeft>& operator *= (Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const TypeRight& aRight)
{
	aLeft.x *= aRight;
	aLeft.y *= aRight;
	return aLeft.vector;
}


// Vector2 , Scalar Arithmetic , Assign Division

template<typename TypeLeft>
strong_inline Vector2<TypeLeft>& operator /= (Vector2<TypeLeft>& aLeft, const TypeLeft& aRight)
{
	aLeft.x /= aRight;
	aLeft.y /= aRight;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left>
strong_inline Vector2<TypeLeft>& operator /= (Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft, const TypeRight& aRight)
{
	aLeft.x /= aRight;
	aLeft.y /= aRight;
	return aLeft.vector;
}


// Vector2 , Special , Negate

template<typename TypeLeft>
strong_inline Vector2<TypeLeft> operator ! (Vector2<TypeLeft>& aLeft)
{
	return Vector2<TypeLeft>(-aLeft.x, -aLeft.y);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left>
strong_inline Vector2<TypeLeft> operator ! (const Vector2Swizzle<TypeLeft, Offset1Left, Offset2Left>& aLeft)
{
	return Vector2<TypeLeft>(-aLeft.x, -aLeft.y);
}

#endif


#endif














#ifdef ALIB_MATH_VECTOR3

		
#ifndef ALIB_MATH_VECTOR3_OPERATIONS
#define ALIB_MATH_VECTOR3_OPERATIONS

// Vector3 , Vector Arithmetic , Addition

template<typename TypeLeft, typename TypeRight>
strong_inline Vector3<TypeLeft> operator + (const Vector3<TypeLeft>& aLeft, const Vector3<TypeRight>& aRight)
{
	return Vector3<TypeLeft>(aLeft.x + aRight.x, aLeft.y + aRight.y, aLeft.z + aRight.z);
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right, int Offset3Right>
strong_inline Vector3<TypeLeft> operator + (const Vector3<TypeLeft>& aLeft, const Vector3Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right>& aRight)
{
	return Vector3<TypeLeft>(aLeft.x + aRight.x, aLeft.y + aRight.y, aLeft.z + aRight.z);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left>
strong_inline Vector3<TypeLeft> operator + (const Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aLeft, const Vector3<TypeRight>& aRight)
{
	return Vector3<TypeLeft>(aLeft.x + aRight.x, aLeft.y + aRight.y, aLeft.z + aRight.z);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset1Right, int Offset2Right, int Offset3Right>
strong_inline Vector3<TypeLeft> operator + (const Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aLeft, const Vector3Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right>& aRight)
{
	return Vector3<TypeLeft>(aLeft.x + aRight.x, aLeft.y + aRight.y, aLeft.z + aRight.z);
}

// Vector3 , Vector Arithmetic , Substraction

template<typename TypeLeft, typename TypeRight>
strong_inline Vector3<TypeLeft> operator - (const Vector3<TypeLeft>& aLeft, const Vector3<TypeRight>& aRight)
{
	return Vector3<TypeLeft>(aLeft.x - aRight.x, aLeft.y - aRight.y, aLeft.z - aRight.z);
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right, int Offset3Right>
strong_inline Vector3<TypeLeft> operator - (const Vector3<TypeLeft>& aLeft, const Vector3Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right>& aRight)
{
	return Vector3<TypeLeft>(aLeft.x - aRight.x, aLeft.y - aRight.y, aLeft.z - aRight.z);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left>
strong_inline Vector3<TypeLeft> operator - (const Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aLeft, const Vector3<TypeRight>& aRight)
{
	return Vector3<TypeLeft>(aLeft.x - aRight.x, aLeft.y - aRight.y, aLeft.z - aRight.z);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset1Right, int Offset2Right, int Offset3Right>
strong_inline Vector3<TypeLeft> operator - (const Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aLeft, const Vector3Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right>& aRight)
{
	return Vector3<TypeLeft>(aLeft.x - aRight.x, aLeft.y - aRight.y, aLeft.z - aRight.z);
}

// Vector3 , Vector Arithmetic , Multiplication

template<typename TypeLeft, typename TypeRight>
strong_inline Vector3<TypeLeft> operator * (const Vector3<TypeLeft>& aLeft, const Vector3<TypeRight>& aRight)
{
	return Vector3<TypeLeft>(aLeft.x * aRight.x, aLeft.y * aRight.y, aLeft.z * aRight.z);
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right, int Offset3Right>
strong_inline Vector3<TypeLeft> operator * (const Vector3<TypeLeft>& aLeft, const Vector3Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right>& aRight)
{
	return Vector3<TypeLeft>(aLeft.x * aRight.x, aLeft.y * aRight.y, aLeft.z * aRight.z);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left>
strong_inline Vector3<TypeLeft> operator * (const Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aLeft, const Vector3<TypeRight>& aRight)
{
	return Vector3<TypeLeft>(aLeft.x * aRight.x, aLeft.y * aRight.y, aLeft.z * aRight.z);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset1Right, int Offset2Right, int Offset3Right>
strong_inline Vector3<TypeLeft> operator * (const Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aLeft, const Vector3Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right>& aRight)
{
	return Vector3<TypeLeft>(aLeft.x * aRight.x, aLeft.y * aRight.y, aLeft.z * aRight.z);
}

// Vector3 , Vector Arithmetic , Division

template<typename TypeLeft, typename TypeRight>
strong_inline Vector3<TypeLeft> operator / (const Vector3<TypeLeft>& aLeft, const Vector3<TypeRight>& aRight)
{
	return Vector3<TypeLeft>(aLeft.x / aRight.x, aLeft.y / aRight.y, aLeft.z / aRight.z);
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right, int Offset3Right>
strong_inline Vector3<TypeLeft> operator / (const Vector3<TypeLeft>& aLeft, const Vector3Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right>& aRight)
{
	return Vector3<TypeLeft>(aLeft.x / aRight.x, aLeft.y / aRight.y, aLeft.z / aRight.z);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left>
strong_inline Vector3<TypeLeft> operator / (const Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aLeft, const Vector3<TypeRight>& aRight)
{
	return Vector3<TypeLeft>(aLeft.x / aRight.x, aLeft.y / aRight.y, aLeft.z / aRight.z);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset1Right, int Offset2Right, int Offset3Right>
strong_inline Vector3<TypeLeft> operator / (const Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aLeft, const Vector3Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right>& aRight)
{
	return Vector3<TypeLeft>(aLeft.x / aRight.x, aLeft.y / aRight.y, aLeft.z / aRight.z);
}


// Vector3 , Vector Arithmetic , Assign Addition

template<typename TypeLeft, typename TypeRight>
strong_inline Vector3<TypeLeft>& operator += (Vector3<TypeLeft>& aLeft, const Vector3<TypeRight> aRight)
{
	aLeft.x += aRight.x;
	aLeft.y += aRight.y;
	aLeft.z += aRight.z;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right, int Offset3Right>
strong_inline Vector3<TypeLeft>& operator += (Vector3<TypeLeft>& aLeft, const Vector3Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	aLeft.x += aRight.x;
	aLeft.y += aRight.y;
	aLeft.z += aRight.z;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left>
strong_inline Vector3<TypeLeft>& operator += (Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aLeft, const Vector3<TypeRight>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	aLeft.x += aRight.x;
	aLeft.y += aRight.y;
	aLeft.z += aRight.z;
	return aLeft.vector;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset1Right, int Offset2Right, int Offset3Right>
strong_inline Vector3<TypeLeft>& operator += (Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aLeft, const Vector3Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	aLeft.x += aRight.x;
	aLeft.y += aRight.y;
	aLeft.z += aRight.z;
	return aLeft.vector;
}


// Vector3 , Vector Arithmetic , Assign Substraction

template<typename TypeLeft, typename TypeRight>
strong_inline Vector3<TypeLeft>& operator -= (Vector3<TypeLeft>& aLeft, const Vector3<TypeRight> aRight)
{
	aLeft.x -= aRight.x;
	aLeft.y -= aRight.y;
	aLeft.z -= aRight.z;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right, int Offset3Right>
strong_inline Vector3<TypeLeft>& operator -= (Vector3<TypeLeft>& aLeft, const Vector3Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	aLeft.x -= x;
	aLeft.y -= y;
	aLeft.z -= z;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left>
strong_inline Vector3<TypeLeft>& operator -= (Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aLeft, const Vector3<TypeRight>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	aLeft.x -= x;
	aLeft.y -= y;
	aLeft.z -= z;
	return aLeft.vector;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset1Right, int Offset2Right, int Offset3Right>
strong_inline Vector3<TypeLeft>& operator -= (Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aLeft, const Vector3Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	aLeft.x -= x;
	aLeft.y -= y;
	aLeft.z -= z;
	return aLeft.vector;
}


// Vector3 , Vector Arithmetic , Assign Multiplication

template<typename TypeLeft, typename TypeRight>
strong_inline Vector3<TypeLeft>& operator *= (Vector3<TypeLeft>& aLeft, const Vector3<TypeRight> aRight)
{
	aLeft.x *= aRight.x;
	aLeft.y *= aRight.y;
	aLeft.z *= aRight.z;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right, int Offset3Right>
strong_inline Vector3<TypeLeft>& operator *= (Vector3<TypeLeft>& aLeft, const Vector3Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	aLeft.x *= x;
	aLeft.y *= y;
	aLeft.z *= z;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left>
strong_inline Vector3<TypeLeft>& operator *= (Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aLeft, const Vector3<TypeRight>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	aLeft.x *= x;
	aLeft.y *= y;
	aLeft.z *= z;
	return aLeft.vector;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset1Right, int Offset2Right, int Offset3Right>
strong_inline Vector3<TypeLeft>& operator *= (Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aLeft, const Vector3Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	aLeft.x *= x;
	aLeft.y *= y;
	aLeft.z *= z;
	return aLeft.vector;
}


// Vector3 , Vector Arithmetic , Assign Division

template<typename TypeLeft, typename TypeRight>
strong_inline Vector3<TypeLeft>& operator /= (Vector3<TypeLeft>& aLeft, const Vector3<TypeRight> aRight)
{
	aLeft.x /= aRight.x;
	aLeft.y /= aRight.y;
	aLeft.z /= aRight.z;
	return aLeft;
}


template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right, int Offset3Right>
strong_inline Vector3<TypeLeft>& operator /= (Vector3<TypeLeft>& aLeft, const Vector3Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	aLeft.x /= x;
	aLeft.y /= y;
	aLeft.z /= z;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left>
strong_inline Vector3<TypeLeft>& operator /= (Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aLeft, const Vector3<TypeRight>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	aLeft.x /= x;
	aLeft.y /= y;
	aLeft.z /= z;
	return aLeft.vector;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left = 0, int Offset2Left, int Offset3Left, int Offset1Right, int Offset2Right, int Offset3Right>
strong_inline Vector3<TypeLeft>& operator /= (Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aLeft, const Vector3Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	aLeft.x /= x;
	aLeft.y /= y;
	aLeft.z /= z;
	return aLeft.vector;
}


// Vector3 , Scalar Arithmetic , Addition

template<typename TypeLeft>
strong_inline Vector3<TypeLeft> operator + (const Vector3<TypeLeft>& aLeft, const TypeLeft& aRight)
{
	return Vector3<TypeLeft>(aLeft.x + aRight, aLeft.y + aRight, aLeft.z + aRight);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left, int Offset3Left>
strong_inline Vector3<TypeLeft> operator + (const Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aLeft, const TypeLeft& aRight)
{
	return Vector3<TypeLeft>(aLeft.x + aRight, aLeft.y + aRight, aLeft.z + aRight);
}

template<typename TypeLeft>
strong_inline Vector3<TypeLeft> operator + (const TypeLeft& aLeft, const Vector3<TypeLeft>& aRight)
{
	return Vector3<TypeLeft>(aLeft + aRight.x, aLeft + aRight.y, aLeft + aRight.z);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left, int Offset3Left>
strong_inline Vector3<TypeLeft> operator + (const TypeLeft& aLeft, const Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aRight)
{
	return Vector3<TypeLeft>(aLeft + aRight.x, aLeft + aRight.y, aLeft + aRight.z);
}

// Vector3 , Scalar Arithmetic , Substraction

template<typename TypeLeft>
strong_inline Vector3<TypeLeft> operator - (const Vector3<TypeLeft>& aLeft, const TypeLeft& aRight)
{
	return Vector3<TypeLeft>(aLeft.x - aRight, aLeft.y - aRight, aLeft.z - aRight);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left, int Offset3Left>
strong_inline Vector3<TypeLeft> operator - (const Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aLeft, const TypeLeft& aRight)
{
	return Vector3<TypeLeft>(aLeft.x - aRight, aLeft.y - aRight, aLeft.z - aRight);
}

template<typename TypeLeft>
strong_inline Vector3<TypeLeft> operator - (const TypeLeft& aLeft, const Vector3<TypeLeft>& aRight)
{
	return Vector3<TypeLeft>(aLeft - aRight.x, aLeft - aRight.y, aLeft - aRight.z);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left, int Offset3Left>
strong_inline Vector3<TypeLeft> operator - (const TypeLeft& aLeft, const Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aRight)
{
	return Vector3<TypeLeft>(aLeft - aRight.x, aLeft - aRight.y, aLeft - aRight.z);
}


// Vector3 , Scalar Arithmetic , Multiplication

template<typename TypeLeft>
strong_inline Vector3<TypeLeft> operator * (const Vector3<TypeLeft>& aLeft, const TypeLeft& aRight)
{
	return Vector3<TypeLeft>(aLeft.x * aRight, aLeft.y * aRight, aLeft.z * aRight);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left, int Offset3Left>
strong_inline Vector3<TypeLeft> operator * (const Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aLeft, const TypeLeft& aRight)
{
	return Vector3<TypeLeft>(aLeft.x * aRight, aLeft.y * aRight, aLeft.z * aRight);
}

template<typename TypeLeft>
strong_inline Vector3<TypeLeft> operator * (const TypeLeft& aLeft, const Vector3<TypeLeft>& aRight)
{
	return Vector3<TypeLeft>(aLeft * aRight.x, aLeft * aRight.y, aLeft * aRight.z);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left, int Offset3Left>
strong_inline Vector3<TypeLeft> operator * (const TypeLeft& aLeft, const Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aRight)
{
	return Vector3<TypeLeft>(aLeft * aRight.x, aLeft * aRight.y, aLeft * aRight.z);
}


// Vector3 , Scalar Arithmetic , Division

template<typename TypeLeft>
strong_inline Vector3<TypeLeft> operator / (const Vector3<TypeLeft>& aLeft, const TypeLeft& aRight)
{
	return Vector3<TypeLeft>(aLeft.x / aRight, aLeft.y / aRight, aLeft.z / aRight);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left, int Offset3Left>
strong_inline Vector3<TypeLeft> operator / (const Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aLeft, const TypeLeft& aRight)
{
	return Vector3<TypeLeft>(aLeft.x / aRight, aLeft.y / aRight, aLeft.z / aRight);
}

template<typename TypeLeft>
strong_inline Vector3<TypeLeft> operator / (const TypeLeft& aLeft, const Vector3<TypeLeft>& aRight)
{
	return Vector3<TypeLeft>(aLeft / aRight.x, aLeft / aRight.y, aLeft / aRight.z);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left, int Offset3Left>
strong_inline Vector3<TypeLeft> operator / (const TypeLeft& aLeft, const Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aRight)
{
	return Vector3<TypeLeft>(aLeft / aRight.x, aLeft / aRight.y, aLeft / aRight.z);
}





// Vector3 , Scalar Arithmetic , Assign Addition

template<typename TypeLeft, typename TypeRight>
strong_inline Vector3<TypeLeft>& operator += (Vector3<TypeLeft>& aLeft, const TypeRight& aRight)
{
	aLeft.x += aRight;
	aLeft.y += aRight;
	aLeft.z += aRight;
	return aLeft;
}




// Vector3 , Scalar Arithmetic , Assign Substraction

template<typename TypeLeft, typename TypeRight>
strong_inline Vector3<TypeLeft>& operator -= (Vector3<TypeLeft>& aLeft, const TypeRight& aRight)
{
	aLeft.x -= aRight;
	aLeft.y -= aRight;
	aLeft.z -= aRight;
	return aLeft;
}


// Vector3 , Scalar Arithmetic , Assign Multiplication

template<typename TypeLeft, typename TypeRight>
strong_inline Vector3<TypeLeft>& operator *= (Vector3<TypeLeft>& aLeft, const TypeRight& aRight)
{
	aLeft.x *= aRight;
	aLeft.y *= aRight;
	aLeft.z *= aRight;
	return aLeft;
}


// Vector3 , Scalar Arithmetic , Assign Division

template<typename TypeLeft>
strong_inline Vector3<TypeLeft>& operator /= (Vector3<TypeLeft>& aLeft, const TypeLeft& aRight)
{
	aLeft.x /= aRight;
	aLeft.y /= aRight;
	aLeft.z /= aRight;
	return aLeft;
}


// Vector3 , Special , Negate

template<typename TypeLeft>
strong_inline Vector3<TypeLeft> operator ! (Vector3<TypeLeft>& aLeft)
{
	return Vector3<TypeLeft>(-aLeft.x, -aLeft.y, -aLeft.z);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left, int Offset3Left>
strong_inline Vector3<TypeLeft> operator ! (const Vector3Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left>& aLeft)
{
	return Vector3<TypeLeft>(-aLeft.x, -aLeft.y, -aLeft.z);
}

		
#endif


#endif




#ifdef ALIB_MATH_VECTOR4


#ifndef ALIB_MATH_VECTOR4_OPERATIONS
#define ALIB_MATH_VECTOR4_OPERATIONS

// Vector4 , Vector Arithmetic , Addition

template<typename TypeLeft, typename TypeRight>
strong_inline Vector4<TypeLeft> operator + (const Vector4<TypeLeft>& aLeft, const Vector4<TypeRight>& aRight)
{
	return Vector4<TypeLeft>(aLeft.x + aRight.x, aLeft.y + aRight.y, aLeft.z + aRight.z, aLeft.w + aRight.w);
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right, int Offset3Right, int Offset4Right>
strong_inline Vector4<TypeLeft> operator + (const Vector4<TypeLeft>& aLeft, const Vector4Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right, Offset4Right>& aRight)
{
	return Vector4<TypeLeft>(aLeft.x + aRight.x, aLeft.y + aRight.y, aLeft.z + aRight.z, aLeft.w + aRight.w);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left>
strong_inline Vector4<TypeLeft> operator + (const Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aLeft, const Vector4<TypeRight>& aRight)
{
	return Vector4<TypeLeft>(aLeft.x + aRight.x, aLeft.y + aRight.y, aLeft.z + aRight.z, aLeft.w + aRight.w);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left, int Offset1Right, int Offset2Right, int Offset3Right, int Offset4Right>
strong_inline Vector4<TypeLeft> operator + (const Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aLeft, const Vector4Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right, Offset4Right>& aRight)
{
	return Vector4<TypeLeft>(aLeft.x + aRight.x, aLeft.y + aRight.y, aLeft.z + aRight.z, aLeft.w + aRight.w);
}

// Vector4 , Vector Arithmetic , Substraction

template<typename TypeLeft, typename TypeRight>
strong_inline Vector4<TypeLeft> operator - (const Vector4<TypeLeft>& aLeft, const Vector4<TypeRight>& aRight)
{
	return Vector4<TypeLeft>(aLeft.x - aRight.x, aLeft.y - aRight.y, aLeft.z - aRight.z, aLeft.w - aRight.w);
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right, int Offset3Right, int Offset4Right>
strong_inline Vector4<TypeLeft> operator - (const Vector4<TypeLeft>& aLeft, const Vector4Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right, Offset4Right>& aRight)
{
	return Vector4<TypeLeft>(aLeft.x - aRight.x, aLeft.y - aRight.y, aLeft.z - aRight.z, aLeft.w - aRight.w);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left>
strong_inline Vector4<TypeLeft> operator - (const Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aLeft, const Vector4<TypeRight>& aRight)
{
	return Vector4<TypeLeft>(aLeft.x - aRight.x, aLeft.y - aRight.y, aLeft.z - aRight.z, aLeft.w - aRight.w);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left, int Offset1Right, int Offset2Right, int Offset3Right, int Offset4Right>
strong_inline Vector4<TypeLeft> operator - (const Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aLeft, const Vector4Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right, Offset4Right>& aRight)
{
	return Vector4<TypeLeft>(aLeft.x - aRight.x, aLeft.y - aRight.y, aLeft.z - aRight.z, aLeft.w - aRight.w);
}

// Vector4 , Vector Arithmetic , Multiplication

template<typename TypeLeft, typename TypeRight>
strong_inline Vector4<TypeLeft> operator * (const Vector4<TypeLeft>& aLeft, const Vector4<TypeRight>& aRight)
{
	return Vector4<TypeLeft>(aLeft.x * aRight.x, aLeft.y * aRight.y, aLeft.z * aRight.z, aLeft.w * aRight.w);
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right, int Offset3Right, int Offset4Right>
strong_inline Vector4<TypeLeft> operator * (const Vector4<TypeLeft>& aLeft, const Vector4Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right, Offset4Right>& aRight)
{
	return Vector4<TypeLeft>(aLeft.x * aRight.x, aLeft.y * aRight.y, aLeft.z * aRight.z, aLeft.w * aRight.w);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left>
strong_inline Vector4<TypeLeft> operator * (const Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aLeft, const Vector4<TypeRight>& aRight)
{
	return Vector4<TypeLeft>(aLeft.x * aRight.x, aLeft.y * aRight.y, aLeft.z * aRight.z, aLeft.w * aRight.w);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left, int Offset1Right, int Offset2Right, int Offset3Right, int Offset4Right>
strong_inline Vector4<TypeLeft> operator * (const Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aLeft, const Vector4Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right, Offset4Right>& aRight)
{
	return Vector4<TypeLeft>(aLeft.x * aRight.x, aLeft.y * aRight.y, aLeft.z * aRight.z, aLeft.w * aRight.w);
}

// Vector4 , Vector Arithmetic , Division

template<typename TypeLeft, typename TypeRight>
strong_inline Vector4<TypeLeft> operator / (const Vector4<TypeLeft>& aLeft, const Vector4<TypeRight>& aRight)
{
	return Vector4<TypeLeft>(aLeft.x / aRight.x, aLeft.y / aRight.y, aLeft.z / aRight.z, aLeft.w / aRight.w);
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right, int Offset3Right, int Offset4Right>
strong_inline Vector4<TypeLeft> operator / (const Vector4<TypeLeft>& aLeft, const Vector4Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right, Offset4Right>& aRight)
{
	return Vector4<TypeLeft>(aLeft.x / aRight.x, aLeft.y / aRight.y, aLeft.z / aRight.z, aLeft.w / aRight.w);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left>
strong_inline Vector4<TypeLeft> operator / (const Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aLeft, const Vector4<TypeRight>& aRight)
{
	return Vector4<TypeLeft>(aLeft.x / aRight.x, aLeft.y / aRight.y, aLeft.z / aRight.z, aLeft.w / aRight.w);
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left, int Offset1Right, int Offset2Right, int Offset3Right, int Offset4Right>
strong_inline Vector4<TypeLeft> operator / (const Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aLeft, const Vector4Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right, Offset4Right>& aRight)
{
	return Vector4<TypeLeft>(aLeft.x / aRight.x, aLeft.y / aRight.y, aLeft.z / aRight.z, aLeft.w / aRight.w);
}


// Vector4 , Vector Arithmetic , Assign Addition

template<typename TypeLeft, typename TypeRight>
strong_inline Vector4<TypeLeft>& operator += (Vector4<TypeLeft>& aLeft, const Vector4<TypeRight> aRight)
{
	aLeft.x += aRight.x;
	aLeft.y += aRight.y;
	aLeft.z += aRight.z;
	aLeft.w += aRight.w;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right, int Offset3Right, int Offset4Right>
strong_inline Vector4<TypeLeft>& operator += (Vector4<TypeLeft>& aLeft, const Vector4Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right, Offset4Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	const TypeLeft w = aRight.w;
	aLeft.x += aRight.x;
	aLeft.y += aRight.y;
	aLeft.z += aRight.z;
	aLeft.w += aRight.w;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left>
strong_inline Vector4<TypeLeft>& operator += (Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aLeft, const Vector4<TypeRight>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	const TypeLeft w = aRight.w;
	aLeft.x += aRight.x;
	aLeft.y += aRight.y;
	aLeft.z += aRight.z;
	aLeft.w += aRight.w;
	return aLeft.vector;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left, int Offset1Right, int Offset2Right, int Offset3Right, int Offset4Right>
strong_inline Vector4<TypeLeft>& operator += (Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aLeft, const Vector4Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right, Offset4Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	const TypeLeft w = aRight.w;
	aLeft.x += aRight.x;
	aLeft.y += aRight.y;
	aLeft.z += aRight.z;
	aLeft.w += aRight.w;
	return aLeft.vector;
}


// Vector4 , Vector Arithmetic , Assign Substraction

template<typename TypeLeft, typename TypeRight>
strong_inline Vector4<TypeLeft>& operator -= (Vector4<TypeLeft>& aLeft, const Vector4<TypeRight> aRight)
{
	aLeft.x -= aRight.x;
	aLeft.y -= aRight.y;
	aLeft.z -= aRight.z;
	aLeft.w -= aRight.w;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right, int Offset3Right, int Offset4Right>
strong_inline Vector4<TypeLeft>& operator -= (Vector4<TypeLeft>& aLeft, const Vector4Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right, Offset4Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	const TypeLeft w = aRight.w;
	aLeft.x -= x;
	aLeft.y -= y;
	aLeft.z -= z;
	aLeft.w -= w;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left>
strong_inline Vector4<TypeLeft>& operator -= (Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aLeft, const Vector4<TypeRight>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	const TypeLeft w = aRight.w;
	aLeft.x -= x;
	aLeft.y -= y;
	aLeft.z -= z;
	aLeft.w -= w;
	return aLeft.vector;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left, int Offset1Right, int Offset2Right, int Offset3Right, int Offset4Right>
strong_inline Vector4<TypeLeft>& operator -= (Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aLeft, const Vector4Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right, Offset4Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	const TypeLeft w = aRight.w;
	aLeft.x -= x;
	aLeft.y -= y;
	aLeft.z -= z;
	aLeft.w -= w;
	return aLeft.vector;
}


// Vector4 , Vector Arithmetic , Assign Multiplication

template<typename TypeLeft, typename TypeRight>
strong_inline Vector4<TypeLeft>& operator *= (Vector4<TypeLeft>& aLeft, const Vector4<TypeRight> aRight)
{
	aLeft.x *= aRight.x;
	aLeft.y *= aRight.y;
	aLeft.z *= aRight.z;
	aLeft.w *= aRight.w;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right, int Offset3Right, int Offset4Right>
strong_inline Vector4<TypeLeft>& operator *= (Vector4<TypeLeft>& aLeft, const Vector4Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right, Offset4Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	const TypeLeft w = aRight.w;
	aLeft.x *= x;
	aLeft.y *= y;
	aLeft.z *= z;
	aLeft.w *= w;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left>
strong_inline Vector4<TypeLeft>& operator *= (Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aLeft, const Vector4<TypeRight>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	const TypeLeft w = aRight.w;
	aLeft.x *= x;
	aLeft.y *= y;
	aLeft.z *= z;
	aLeft.w *= w;
	return aLeft.vector;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left, int Offset1Right, int Offset2Right, int Offset3Right, int Offset4Right>
strong_inline Vector4<TypeLeft>& operator *= (Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aLeft, const Vector4Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right, Offset4Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	const TypeLeft w = aRight.w;
	aLeft.x *= x;
	aLeft.y *= y;
	aLeft.z *= z;
	aLeft.w *= w;
	return aLeft.vector;
}


// Vector4 , Vector Arithmetic , Assign Division

template<typename TypeLeft, typename TypeRight>
strong_inline Vector4<TypeLeft>& operator /= (Vector4<TypeLeft>& aLeft, const Vector4<TypeRight> aRight)
{
	aLeft.x /= aRight.x;
	aLeft.y /= aRight.y;
	aLeft.z /= aRight.z;
	aLeft.w /= aRight.w;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Right, int Offset2Right, int Offset3Right, int Offset4Right>
strong_inline Vector4<TypeLeft>& operator /= (Vector4<TypeLeft>& aLeft, const Vector4Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right, Offset4Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	const TypeLeft w = aRight.w;
	aLeft.x /= x;
	aLeft.y /= y;
	aLeft.z /= z;
	aLeft.w /= w;
	return aLeft;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left>
strong_inline Vector4<TypeLeft>& operator /= (Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aLeft, const Vector4<TypeRight>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	const TypeLeft w = aRight.w;
	aLeft.x /= x;
	aLeft.y /= y;
	aLeft.z /= z;
	aLeft.w /= w;
	return aLeft.vector;
}

template<typename TypeLeft, typename TypeRight, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left, int Offset1Right, int Offset2Right, int Offset3Right, int Offset4Right>
strong_inline Vector4<TypeLeft>& operator /= (Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aLeft, const Vector4Swizzle<TypeRight, Offset1Right, Offset2Right, Offset3Right, Offset4Right>& aRight)
{
	const TypeLeft x = aRight.x;
	const TypeLeft y = aRight.y;
	const TypeLeft z = aRight.z;
	const TypeLeft w = aRight.w;
	aLeft.x /= x;
	aLeft.y /= y;
	aLeft.z /= z;
	aLeft.w /= w;
	return aLeft.vector;
}


// Vector4 , Scalar Arithmetic , Addition

template<typename TypeLeft>
strong_inline Vector4<TypeLeft> operator + (const Vector4<TypeLeft>& aLeft, const TypeLeft& aRight)
{
	return Vector4<TypeLeft>(aLeft.x + aRight, aLeft.y + aRight, aLeft.z + aRight, aLeft.w + aRight);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left>
strong_inline Vector4<TypeLeft> operator + (const Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aLeft, const TypeLeft& aRight)
{
	return Vector4<TypeLeft>(aLeft.x + aRight, aLeft.y + aRight, aLeft.z + aRight, aLeft.w + aRight);
}

template<typename TypeLeft>
strong_inline Vector4<TypeLeft> operator + (const TypeLeft& aLeft, const Vector4<TypeLeft>& aRight)
{
	return Vector4<TypeLeft>(aLeft + aRight.x, aLeft + aRight.y, aLeft + aRight.z, aLeft + aRight.w);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left>
strong_inline Vector4<TypeLeft> operator + (const TypeLeft& aLeft, const Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aRight)
{
	return Vector4<TypeLeft>(aLeft + aRight.x, aLeft + aRight.y, aLeft + aRight.z, aLeft + aRight.w);
}


// Vector4 , Scalar Arithmetic , Substraction

template<typename TypeLeft>
strong_inline Vector4<TypeLeft> operator - (const Vector4<TypeLeft>& aLeft, const TypeLeft& aRight)
{
	return Vector4<TypeLeft>(aLeft.x - aRight, aLeft.y - aRight, aLeft.z - aRight, aLeft.w - aRight);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left>
strong_inline Vector4<TypeLeft> operator - (const Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aLeft, const TypeLeft& aRight)
{
	return Vector4<TypeLeft>(aLeft.x - aRight, aLeft.y - aRight, aLeft.z - aRight, aLeft.w - aRight);
}

template<typename TypeLeft>
strong_inline Vector4<TypeLeft> operator - (const TypeLeft& aLeft, const Vector4<TypeLeft>& aRight)
{
	return Vector4<TypeLeft>(aLeft - aRight.x, aLeft - aRight.y, aLeft - aRight.z, aLeft - aRight.w);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left>
strong_inline Vector4<TypeLeft> operator - (const TypeLeft& aLeft, const Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aRight)
{
	return Vector4<TypeLeft>(aLeft - aRight.x, aLeft - aRight.y, aLeft - aRight.z, aLeft - aRight.w);
}


// Vector4 , Scalar Arithmetic , Multiplication

template<typename TypeLeft>
strong_inline Vector4<TypeLeft> operator * (const Vector4<TypeLeft>& aLeft, const TypeLeft& aRight)
{
	return Vector4<TypeLeft>(aLeft.x * aRight, aLeft.y * aRight, aLeft.z * aRight, aLeft.w * aRight);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left>
strong_inline Vector4<TypeLeft> operator * (const Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aLeft, const TypeLeft& aRight)
{
	return Vector4<TypeLeft>(aLeft.x * aRight, aLeft.y * aRight, aLeft.z * aRight, aLeft.w * aRight);
}

template<typename TypeLeft>
strong_inline Vector4<TypeLeft> operator * (const TypeLeft& aLeft, const Vector4<TypeLeft>& aRight)
{
	return Vector4<TypeLeft>(aLeft * aRight.x, aLeft * aRight.y, aLeft * aRight.z, aLeft * aRight.w);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left>
strong_inline Vector4<TypeLeft> operator * (const TypeLeft& aLeft, const Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aRight)
{
	return Vector4<TypeLeft>(aLeft * aRight.x, aLeft * aRight.y, aLeft * aRight.z, aLeft * aRight.w);
}


// Vector4 , Scalar Arithmetic , Division

template<typename TypeLeft>
strong_inline Vector4<TypeLeft> operator / (const Vector4<TypeLeft>& aLeft, const TypeLeft& aRight)
{
	return Vector4<TypeLeft>(aLeft.x / aRight, aLeft.y / aRight, aLeft.z / aRight, aLeft.w / aRight);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left>
strong_inline Vector4<TypeLeft> operator / (const Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aLeft, const TypeLeft& aRight)
{
	return Vector4<TypeLeft>(aLeft.x / aRight, aLeft.y / aRight, aLeft.z / aRight, aLeft.w / aRight);
}

template<typename TypeLeft>
strong_inline Vector4<TypeLeft> operator / (const TypeLeft& aLeft, const Vector4<TypeLeft>& aRight)
{
	return Vector4<TypeLeft>(aLeft / aRight.x, aLeft / aRight.y, aLeft / aRight.z, aLeft / aRight.w);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left>
strong_inline Vector4<TypeLeft> operator / (const TypeLeft& aLeft, const Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aRight)
{
	return Vector4<TypeLeft>(aLeft / aRight.x, aLeft / aRight.y, aLeft / aRight.z, aLeft / aRight.w);
}





// Vector4 , Scalar Arithmetic , Assign Addition

template<typename TypeLeft, typename TypeRight>
strong_inline Vector4<TypeLeft>& operator += (Vector4<TypeLeft>& aLeft, const TypeRight& aRight)
{
	aLeft.x += aRight;
	aLeft.y += aRight;
	aLeft.z += aRight;
	aLeft.w += aRight;
	return aLeft;
}


// Vector4 , Scalar Arithmetic , Assign Substraction

template<typename TypeLeft, typename TypeRight>
strong_inline Vector4<TypeLeft>& operator -= (Vector4<TypeLeft>& aLeft, const TypeRight& aRight)
{
	aLeft.x -= aRight;
	aLeft.y -= aRight;
	aLeft.z -= aRight;
	aLeft.w -= aRight;
	return aLeft;
}


// Vector4 , Scalar Arithmetic , Assign Multiplication

template<typename TypeLeft, typename TypeRight>
strong_inline Vector4<TypeLeft>& operator *= (Vector4<TypeLeft>& aLeft, const TypeRight& aRight)
{
	aLeft.x *= aRight;
	aLeft.y *= aRight;
	aLeft.z *= aRight;
	aLeft.w *= aRight;
	return aLeft;
}


// Vector4 , Scalar Arithmetic , Assign Division

template<typename TypeLeft>
strong_inline Vector4<TypeLeft>& operator /= (Vector4<TypeLeft>& aLeft, const TypeLeft& aRight)
{
	aLeft.x /= aRight;
	aLeft.y /= aRight;
	aLeft.z /= aRight;
	aLeft.w /= aRight;
	return aLeft;
}


// Vector4 , Special , Negate

template<typename TypeLeft>
strong_inline Vector4<TypeLeft> operator ! (Vector4<TypeLeft>& aLeft)
{
	return Vector4<TypeLeft>(-aLeft.x, -aLeft.y, -aLeft.z, -aLeft.w);
}

template<typename TypeLeft, int Offset1Left, int Offset2Left, int Offset3Left, int Offset4Left>
strong_inline Vector4<TypeLeft> operator ! (const Vector4Swizzle<TypeLeft, Offset1Left, Offset2Left, Offset3Left, Offset4Left>& aLeft)
{
	return Vector4<TypeLeft>(-aLeft.x, -aLeft.y, -aLeft.z, -aLeft.w);
}


#endif


#endif



	}
}
