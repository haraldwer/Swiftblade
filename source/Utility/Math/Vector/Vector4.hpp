#pragma once

#include "Swizzle.hpp"
#include "Utility/Math/DefaultInitialization.h"

namespace Utility
{
	namespace Math
	{
		template<typename Type, int alignment>
		struct alignas(alignment) Vector4
		{
			union
			{
				Type data[4];

				struct
				{

					union
					{
						Type x;
						Type width;
						Type red;
						Type r;
					};

					union
					{
						Type y;
						Type height;
						Type green;
						Type g;
					};

					union
					{
						Type z;
						Type depth;
						Type blue;
						Type b;
					};

					union
					{
						union
						{
							Type w;
							Type spissitude;
							Type alpha;
							Type a;
						};

						Vector2Swizzle<Type, -3, -3, alignment> xx;
						Vector2Swizzle<Type, -3, -2, alignment> xy;
						Vector2Swizzle<Type, -3, -1, alignment> xz;
						Vector2Swizzle<Type, -3, 0, alignment> xw;
						Vector2Swizzle<Type, -2, -3, alignment> yx;
						Vector2Swizzle<Type, -2, -2, alignment> yy;
						Vector2Swizzle<Type, -2, -1, alignment> yz;
						Vector2Swizzle<Type, -2, 0, alignment> yw;
						Vector2Swizzle<Type, -1, -3, alignment> zx;
						Vector2Swizzle<Type, -1, -2, alignment> zy;
						Vector2Swizzle<Type, -1, -1, alignment> zz;
						Vector2Swizzle<Type, -1, 0, alignment> zw;
						Vector2Swizzle<Type, 0, -3, alignment> wx;
						Vector2Swizzle<Type, 0, -2, alignment> wy;
						Vector2Swizzle<Type, 0, -1, alignment> wz;
						Vector2Swizzle<Type, 0, 0, alignment> ww;
						Vector3Swizzle<Type, -3, -3, -3, alignment> xxx;
						Vector3Swizzle<Type, -3, -3, -2, alignment> xxy;
						Vector3Swizzle<Type, -3, -3, -1, alignment> xxz;
						Vector3Swizzle<Type, -3, -3, 0, alignment> xxw;
						Vector3Swizzle<Type, -3, -2, -3, alignment> xyx;
						Vector3Swizzle<Type, -3, -2, -2, alignment> xyy;
						Vector3Swizzle<Type, -3, -2, -1, alignment> xyz;
						Vector3Swizzle<Type, -3, -2, 0, alignment> xyw;
						Vector3Swizzle<Type, -3, -1, -3, alignment> xzx;
						Vector3Swizzle<Type, -3, -1, -2, alignment> xzy;
						Vector3Swizzle<Type, -3, -1, -1, alignment> xzz;
						Vector3Swizzle<Type, -3, -1, 0, alignment> xzw;
						Vector3Swizzle<Type, -3, 0, -3, alignment> xwx;
						Vector3Swizzle<Type, -3, 0, -2, alignment> xwy;
						Vector3Swizzle<Type, -3, 0, -1, alignment> xwz;
						Vector3Swizzle<Type, -3, 0, 0, alignment> xww;
						Vector3Swizzle<Type, -2, -3, -3, alignment> yxx;
						Vector3Swizzle<Type, -2, -3, -2, alignment> yxy;
						Vector3Swizzle<Type, -2, -3, -1, alignment> yxz;
						Vector3Swizzle<Type, -2, -3, 0, alignment> yxw;
						Vector3Swizzle<Type, -2, -2, -3, alignment> yyx;
						Vector3Swizzle<Type, -2, -2, -2, alignment> yyy;
						Vector3Swizzle<Type, -2, -2, -1, alignment> yyz;
						Vector3Swizzle<Type, -2, -2, 0, alignment> yyw;
						Vector3Swizzle<Type, -2, -1, -3, alignment> yzx;
						Vector3Swizzle<Type, -2, -1, -2, alignment> yzy;
						Vector3Swizzle<Type, -2, -1, -1, alignment> yzz;
						Vector3Swizzle<Type, -2, -1, 0, alignment> yzw;
						Vector3Swizzle<Type, -2, 0, -3, alignment> ywx;
						Vector3Swizzle<Type, -2, 0, -2, alignment> ywy;
						Vector3Swizzle<Type, -2, 0, -1, alignment> ywz;
						Vector3Swizzle<Type, -2, 0, 0, alignment> yww;
						Vector3Swizzle<Type, -1, -3, -3, alignment> zxx;
						Vector3Swizzle<Type, -1, -3, -2, alignment> zxy;
						Vector3Swizzle<Type, -1, -3, -1, alignment> zxz;
						Vector3Swizzle<Type, -1, -3, 0, alignment> zxw;
						Vector3Swizzle<Type, -1, -2, -3, alignment> zyx;
						Vector3Swizzle<Type, -1, -2, -2, alignment> zyy;
						Vector3Swizzle<Type, -1, -2, -1, alignment> zyz;
						Vector3Swizzle<Type, -1, -2, 0, alignment> zyw;
						Vector3Swizzle<Type, -1, -1, -3, alignment> zzx;
						Vector3Swizzle<Type, -1, -1, -2, alignment> zzy;
						Vector3Swizzle<Type, -1, -1, -1, alignment> zzz;
						Vector3Swizzle<Type, -1, -1, 0, alignment> zzw;
						Vector3Swizzle<Type, -1, 0, -3, alignment> zwx;
						Vector3Swizzle<Type, -1, 0, -2, alignment> zwy;
						Vector3Swizzle<Type, -1, 0, -1, alignment> zwz;
						Vector3Swizzle<Type, -1, 0, 0, alignment> zww;
						Vector3Swizzle<Type, 0, -3, -3, alignment> wxx;
						Vector3Swizzle<Type, 0, -3, -2, alignment> wxy;
						Vector3Swizzle<Type, 0, -3, -1, alignment> wxz;
						Vector3Swizzle<Type, 0, -3, 0, alignment> wxw;
						Vector3Swizzle<Type, 0, -2, -3, alignment> wyx;
						Vector3Swizzle<Type, 0, -2, -2, alignment> wyy;
						Vector3Swizzle<Type, 0, -2, -1, alignment> wyz;
						Vector3Swizzle<Type, 0, -2, 0, alignment> wyw;
						Vector3Swizzle<Type, 0, -1, -3, alignment> wzx;
						Vector3Swizzle<Type, 0, -1, -2, alignment> wzy;
						Vector3Swizzle<Type, 0, -1, -1, alignment> wzz;
						Vector3Swizzle<Type, 0, -1, 0, alignment> wzw;
						Vector3Swizzle<Type, 0, 0, -3, alignment> wwx;
						Vector3Swizzle<Type, 0, 0, -2, alignment> wwy;
						Vector3Swizzle<Type, 0, 0, -1, alignment> wwz;
						Vector3Swizzle<Type, 0, 0, 0, alignment> www;
						Vector4Swizzle<Type, -3, -3, -3, -3, alignment> xxxx;
						Vector4Swizzle<Type, -3, -3, -3, -2, alignment> xxxy;
						Vector4Swizzle<Type, -3, -3, -3, -1, alignment> xxxz;
						Vector4Swizzle<Type, -3, -3, -3, 0, alignment> xxxw;
						Vector4Swizzle<Type, -3, -3, -2, -3, alignment> xxyx;
						Vector4Swizzle<Type, -3, -3, -2, -2, alignment> xxyy;
						Vector4Swizzle<Type, -3, -3, -2, -1, alignment> xxyz;
						Vector4Swizzle<Type, -3, -3, -2, 0, alignment> xxyw;
						Vector4Swizzle<Type, -3, -3, -1, -3, alignment> xxzx;
						Vector4Swizzle<Type, -3, -3, -1, -2, alignment> xxzy;
						Vector4Swizzle<Type, -3, -3, -1, -1, alignment> xxzz;
						Vector4Swizzle<Type, -3, -3, -1, 0, alignment> xxzw;
						Vector4Swizzle<Type, -3, -3, 0, -3, alignment> xxwx;
						Vector4Swizzle<Type, -3, -3, 0, -2, alignment> xxwy;
						Vector4Swizzle<Type, -3, -3, 0, -1, alignment> xxwz;
						Vector4Swizzle<Type, -3, -3, 0, 0, alignment> xxww;
						Vector4Swizzle<Type, -3, -2, -3, -3, alignment> xyxx;
						Vector4Swizzle<Type, -3, -2, -3, -2, alignment> xyxy;
						Vector4Swizzle<Type, -3, -2, -3, -1, alignment> xyxz;
						Vector4Swizzle<Type, -3, -2, -3, 0, alignment> xyxw;
						Vector4Swizzle<Type, -3, -2, -2, -3, alignment> xyyx;
						Vector4Swizzle<Type, -3, -2, -2, -2, alignment> xyyy;
						Vector4Swizzle<Type, -3, -2, -2, -1, alignment> xyyz;
						Vector4Swizzle<Type, -3, -2, -2, 0, alignment> xyyw;
						Vector4Swizzle<Type, -3, -2, -1, -3, alignment> xyzx;
						Vector4Swizzle<Type, -3, -2, -1, -2, alignment> xyzy;
						Vector4Swizzle<Type, -3, -2, -1, -1, alignment> xyzz;
						Vector4Swizzle<Type, -3, -2, -1, 0, alignment> xyzw;
						Vector4Swizzle<Type, -3, -2, 0, -3, alignment> xywx;
						Vector4Swizzle<Type, -3, -2, 0, -2, alignment> xywy;
						Vector4Swizzle<Type, -3, -2, 0, -1, alignment> xywz;
						Vector4Swizzle<Type, -3, -2, 0, 0, alignment> xyww;
						Vector4Swizzle<Type, -3, -1, -3, -3, alignment> xzxx;
						Vector4Swizzle<Type, -3, -1, -3, -2, alignment> xzxy;
						Vector4Swizzle<Type, -3, -1, -3, -1, alignment> xzxz;
						Vector4Swizzle<Type, -3, -1, -3, 0, alignment> xzxw;
						Vector4Swizzle<Type, -3, -1, -2, -3, alignment> xzyx;
						Vector4Swizzle<Type, -3, -1, -2, -2, alignment> xzyy;
						Vector4Swizzle<Type, -3, -1, -2, -1, alignment> xzyz;
						Vector4Swizzle<Type, -3, -1, -2, 0, alignment> xzyw;
						Vector4Swizzle<Type, -3, -1, -1, -3, alignment> xzzx;
						Vector4Swizzle<Type, -3, -1, -1, -2, alignment> xzzy;
						Vector4Swizzle<Type, -3, -1, -1, -1, alignment> xzzz;
						Vector4Swizzle<Type, -3, -1, -1, 0, alignment> xzzw;
						Vector4Swizzle<Type, -3, -1, 0, -3, alignment> xzwx;
						Vector4Swizzle<Type, -3, -1, 0, -2, alignment> xzwy;
						Vector4Swizzle<Type, -3, -1, 0, -1, alignment> xzwz;
						Vector4Swizzle<Type, -3, -1, 0, 0, alignment> xzww;
						Vector4Swizzle<Type, -3, 0, -3, -3, alignment> xwxx;
						Vector4Swizzle<Type, -3, 0, -3, -2, alignment> xwxy;
						Vector4Swizzle<Type, -3, 0, -3, -1, alignment> xwxz;
						Vector4Swizzle<Type, -3, 0, -3, 0, alignment> xwxw;
						Vector4Swizzle<Type, -3, 0, -2, -3, alignment> xwyx;
						Vector4Swizzle<Type, -3, 0, -2, -2, alignment> xwyy;
						Vector4Swizzle<Type, -3, 0, -2, -1, alignment> xwyz;
						Vector4Swizzle<Type, -3, 0, -2, 0, alignment> xwyw;
						Vector4Swizzle<Type, -3, 0, -1, -3, alignment> xwzx;
						Vector4Swizzle<Type, -3, 0, -1, -2, alignment> xwzy;
						Vector4Swizzle<Type, -3, 0, -1, -1, alignment> xwzz;
						Vector4Swizzle<Type, -3, 0, -1, 0, alignment> xwzw;
						Vector4Swizzle<Type, -3, 0, 0, -3, alignment> xwwx;
						Vector4Swizzle<Type, -3, 0, 0, -2, alignment> xwwy;
						Vector4Swizzle<Type, -3, 0, 0, -1, alignment> xwwz;
						Vector4Swizzle<Type, -3, 0, 0, 0, alignment> xwww;
						Vector4Swizzle<Type, -2, -3, -3, -3, alignment> yxxx;
						Vector4Swizzle<Type, -2, -3, -3, -2, alignment> yxxy;
						Vector4Swizzle<Type, -2, -3, -3, -1, alignment> yxxz;
						Vector4Swizzle<Type, -2, -3, -3, 0, alignment> yxxw;
						Vector4Swizzle<Type, -2, -3, -2, -3, alignment> yxyx;
						Vector4Swizzle<Type, -2, -3, -2, -2, alignment> yxyy;
						Vector4Swizzle<Type, -2, -3, -2, -1, alignment> yxyz;
						Vector4Swizzle<Type, -2, -3, -2, 0, alignment> yxyw;
						Vector4Swizzle<Type, -2, -3, -1, -3, alignment> yxzx;
						Vector4Swizzle<Type, -2, -3, -1, -2, alignment> yxzy;
						Vector4Swizzle<Type, -2, -3, -1, -1, alignment> yxzz;
						Vector4Swizzle<Type, -2, -3, -1, 0, alignment> yxzw;
						Vector4Swizzle<Type, -2, -3, 0, -3, alignment> yxwx;
						Vector4Swizzle<Type, -2, -3, 0, -2, alignment> yxwy;
						Vector4Swizzle<Type, -2, -3, 0, -1, alignment> yxwz;
						Vector4Swizzle<Type, -2, -3, 0, 0, alignment> yxww;
						Vector4Swizzle<Type, -2, -2, -3, -3, alignment> yyxx;
						Vector4Swizzle<Type, -2, -2, -3, -2, alignment> yyxy;
						Vector4Swizzle<Type, -2, -2, -3, -1, alignment> yyxz;
						Vector4Swizzle<Type, -2, -2, -3, 0, alignment> yyxw;
						Vector4Swizzle<Type, -2, -2, -2, -3, alignment> yyyx;
						Vector4Swizzle<Type, -2, -2, -2, -2, alignment> yyyy;
						Vector4Swizzle<Type, -2, -2, -2, -1, alignment> yyyz;
						Vector4Swizzle<Type, -2, -2, -2, 0, alignment> yyyw;
						Vector4Swizzle<Type, -2, -2, -1, -3, alignment> yyzx;
						Vector4Swizzle<Type, -2, -2, -1, -2, alignment> yyzy;
						Vector4Swizzle<Type, -2, -2, -1, -1, alignment> yyzz;
						Vector4Swizzle<Type, -2, -2, -1, 0, alignment> yyzw;
						Vector4Swizzle<Type, -2, -2, 0, -3, alignment> yywx;
						Vector4Swizzle<Type, -2, -2, 0, -2, alignment> yywy;
						Vector4Swizzle<Type, -2, -2, 0, -1, alignment> yywz;
						Vector4Swizzle<Type, -2, -2, 0, 0, alignment> yyww;
						Vector4Swizzle<Type, -2, -1, -3, -3, alignment> yzxx;
						Vector4Swizzle<Type, -2, -1, -3, -2, alignment> yzxy;
						Vector4Swizzle<Type, -2, -1, -3, -1, alignment> yzxz;
						Vector4Swizzle<Type, -2, -1, -3, 0, alignment> yzxw;
						Vector4Swizzle<Type, -2, -1, -2, -3, alignment> yzyx;
						Vector4Swizzle<Type, -2, -1, -2, -2, alignment> yzyy;
						Vector4Swizzle<Type, -2, -1, -2, -1, alignment> yzyz;
						Vector4Swizzle<Type, -2, -1, -2, 0, alignment> yzyw;
						Vector4Swizzle<Type, -2, -1, -1, -3, alignment> yzzx;
						Vector4Swizzle<Type, -2, -1, -1, -2, alignment> yzzy;
						Vector4Swizzle<Type, -2, -1, -1, -1, alignment> yzzz;
						Vector4Swizzle<Type, -2, -1, -1, 0, alignment> yzzw;
						Vector4Swizzle<Type, -2, -1, 0, -3, alignment> yzwx;
						Vector4Swizzle<Type, -2, -1, 0, -2, alignment> yzwy;
						Vector4Swizzle<Type, -2, -1, 0, -1, alignment> yzwz;
						Vector4Swizzle<Type, -2, -1, 0, 0, alignment> yzww;
						Vector4Swizzle<Type, -2, 0, -3, -3, alignment> ywxx;
						Vector4Swizzle<Type, -2, 0, -3, -2, alignment> ywxy;
						Vector4Swizzle<Type, -2, 0, -3, -1, alignment> ywxz;
						Vector4Swizzle<Type, -2, 0, -3, 0, alignment> ywxw;
						Vector4Swizzle<Type, -2, 0, -2, -3, alignment> ywyx;
						Vector4Swizzle<Type, -2, 0, -2, -2, alignment> ywyy;
						Vector4Swizzle<Type, -2, 0, -2, -1, alignment> ywyz;
						Vector4Swizzle<Type, -2, 0, -2, 0, alignment> ywyw;
						Vector4Swizzle<Type, -2, 0, -1, -3, alignment> ywzx;
						Vector4Swizzle<Type, -2, 0, -1, -2, alignment> ywzy;
						Vector4Swizzle<Type, -2, 0, -1, -1, alignment> ywzz;
						Vector4Swizzle<Type, -2, 0, -1, 0, alignment> ywzw;
						Vector4Swizzle<Type, -2, 0, 0, -3, alignment> ywwx;
						Vector4Swizzle<Type, -2, 0, 0, -2, alignment> ywwy;
						Vector4Swizzle<Type, -2, 0, 0, -1, alignment> ywwz;
						Vector4Swizzle<Type, -2, 0, 0, 0, alignment> ywww;
						Vector4Swizzle<Type, -1, -3, -3, -3, alignment> zxxx;
						Vector4Swizzle<Type, -1, -3, -3, -2, alignment> zxxy;
						Vector4Swizzle<Type, -1, -3, -3, -1, alignment> zxxz;
						Vector4Swizzle<Type, -1, -3, -3, 0, alignment> zxxw;
						Vector4Swizzle<Type, -1, -3, -2, -3, alignment> zxyx;
						Vector4Swizzle<Type, -1, -3, -2, -2, alignment> zxyy;
						Vector4Swizzle<Type, -1, -3, -2, -1, alignment> zxyz;
						Vector4Swizzle<Type, -1, -3, -2, 0, alignment> zxyw;
						Vector4Swizzle<Type, -1, -3, -1, -3, alignment> zxzx;
						Vector4Swizzle<Type, -1, -3, -1, -2, alignment> zxzy;
						Vector4Swizzle<Type, -1, -3, -1, -1, alignment> zxzz;
						Vector4Swizzle<Type, -1, -3, -1, 0, alignment> zxzw;
						Vector4Swizzle<Type, -1, -3, 0, -3, alignment> zxwx;
						Vector4Swizzle<Type, -1, -3, 0, -2, alignment> zxwy;
						Vector4Swizzle<Type, -1, -3, 0, -1, alignment> zxwz;
						Vector4Swizzle<Type, -1, -3, 0, 0, alignment> zxww;
						Vector4Swizzle<Type, -1, -2, -3, -3, alignment> zyxx;
						Vector4Swizzle<Type, -1, -2, -3, -2, alignment> zyxy;
						Vector4Swizzle<Type, -1, -2, -3, -1, alignment> zyxz;
						Vector4Swizzle<Type, -1, -2, -3, 0, alignment> zyxw;
						Vector4Swizzle<Type, -1, -2, -2, -3, alignment> zyyx;
						Vector4Swizzle<Type, -1, -2, -2, -2, alignment> zyyy;
						Vector4Swizzle<Type, -1, -2, -2, -1, alignment> zyyz;
						Vector4Swizzle<Type, -1, -2, -2, 0, alignment> zyyw;
						Vector4Swizzle<Type, -1, -2, -1, -3, alignment> zyzx;
						Vector4Swizzle<Type, -1, -2, -1, -2, alignment> zyzy;
						Vector4Swizzle<Type, -1, -2, -1, -1, alignment> zyzz;
						Vector4Swizzle<Type, -1, -2, -1, 0, alignment> zyzw;
						Vector4Swizzle<Type, -1, -2, 0, -3, alignment> zywx;
						Vector4Swizzle<Type, -1, -2, 0, -2, alignment> zywy;
						Vector4Swizzle<Type, -1, -2, 0, -1, alignment> zywz;
						Vector4Swizzle<Type, -1, -2, 0, 0, alignment> zyww;
						Vector4Swizzle<Type, -1, -1, -3, -3, alignment> zzxx;
						Vector4Swizzle<Type, -1, -1, -3, -2, alignment> zzxy;
						Vector4Swizzle<Type, -1, -1, -3, -1, alignment> zzxz;
						Vector4Swizzle<Type, -1, -1, -3, 0, alignment> zzxw;
						Vector4Swizzle<Type, -1, -1, -2, -3, alignment> zzyx;
						Vector4Swizzle<Type, -1, -1, -2, -2, alignment> zzyy;
						Vector4Swizzle<Type, -1, -1, -2, -1, alignment> zzyz;
						Vector4Swizzle<Type, -1, -1, -2, 0, alignment> zzyw;
						Vector4Swizzle<Type, -1, -1, -1, -3, alignment> zzzx;
						Vector4Swizzle<Type, -1, -1, -1, -2, alignment> zzzy;
						Vector4Swizzle<Type, -1, -1, -1, -1, alignment> zzzz;
						Vector4Swizzle<Type, -1, -1, -1, 0, alignment> zzzw;
						Vector4Swizzle<Type, -1, -1, 0, -3, alignment> zzwx;
						Vector4Swizzle<Type, -1, -1, 0, -2, alignment> zzwy;
						Vector4Swizzle<Type, -1, -1, 0, -1, alignment> zzwz;
						Vector4Swizzle<Type, -1, -1, 0, 0, alignment> zzww;
						Vector4Swizzle<Type, -1, 0, -3, -3, alignment> zwxx;
						Vector4Swizzle<Type, -1, 0, -3, -2, alignment> zwxy;
						Vector4Swizzle<Type, -1, 0, -3, -1, alignment> zwxz;
						Vector4Swizzle<Type, -1, 0, -3, 0, alignment> zwxw;
						Vector4Swizzle<Type, -1, 0, -2, -3, alignment> zwyx;
						Vector4Swizzle<Type, -1, 0, -2, -2, alignment> zwyy;
						Vector4Swizzle<Type, -1, 0, -2, -1, alignment> zwyz;
						Vector4Swizzle<Type, -1, 0, -2, 0, alignment> zwyw;
						Vector4Swizzle<Type, -1, 0, -1, -3, alignment> zwzx;
						Vector4Swizzle<Type, -1, 0, -1, -2, alignment> zwzy;
						Vector4Swizzle<Type, -1, 0, -1, -1, alignment> zwzz;
						Vector4Swizzle<Type, -1, 0, -1, 0, alignment> zwzw;
						Vector4Swizzle<Type, -1, 0, 0, -3, alignment> zwwx;
						Vector4Swizzle<Type, -1, 0, 0, -2, alignment> zwwy;
						Vector4Swizzle<Type, -1, 0, 0, -1, alignment> zwwz;
						Vector4Swizzle<Type, -1, 0, 0, 0, alignment> zwww;
						Vector4Swizzle<Type, 0, -3, -3, -3, alignment> wxxx;
						Vector4Swizzle<Type, 0, -3, -3, -2, alignment> wxxy;
						Vector4Swizzle<Type, 0, -3, -3, -1, alignment> wxxz;
						Vector4Swizzle<Type, 0, -3, -3, 0, alignment> wxxw;
						Vector4Swizzle<Type, 0, -3, -2, -3, alignment> wxyx;
						Vector4Swizzle<Type, 0, -3, -2, -2, alignment> wxyy;
						Vector4Swizzle<Type, 0, -3, -2, -1, alignment> wxyz;
						Vector4Swizzle<Type, 0, -3, -2, 0, alignment> wxyw;
						Vector4Swizzle<Type, 0, -3, -1, -3, alignment> wxzx;
						Vector4Swizzle<Type, 0, -3, -1, -2, alignment> wxzy;
						Vector4Swizzle<Type, 0, -3, -1, -1, alignment> wxzz;
						Vector4Swizzle<Type, 0, -3, -1, 0, alignment> wxzw;
						Vector4Swizzle<Type, 0, -3, 0, -3, alignment> wxwx;
						Vector4Swizzle<Type, 0, -3, 0, -2, alignment> wxwy;
						Vector4Swizzle<Type, 0, -3, 0, -1, alignment> wxwz;
						Vector4Swizzle<Type, 0, -3, 0, 0, alignment> wxww;
						Vector4Swizzle<Type, 0, -2, -3, -3, alignment> wyxx;
						Vector4Swizzle<Type, 0, -2, -3, -2, alignment> wyxy;
						Vector4Swizzle<Type, 0, -2, -3, -1, alignment> wyxz;
						Vector4Swizzle<Type, 0, -2, -3, 0, alignment> wyxw;
						Vector4Swizzle<Type, 0, -2, -2, -3, alignment> wyyx;
						Vector4Swizzle<Type, 0, -2, -2, -2, alignment> wyyy;
						Vector4Swizzle<Type, 0, -2, -2, -1, alignment> wyyz;
						Vector4Swizzle<Type, 0, -2, -2, 0, alignment> wyyw;
						Vector4Swizzle<Type, 0, -2, -1, -3, alignment> wyzx;
						Vector4Swizzle<Type, 0, -2, -1, -2, alignment> wyzy;
						Vector4Swizzle<Type, 0, -2, -1, -1, alignment> wyzz;
						Vector4Swizzle<Type, 0, -2, -1, 0, alignment> wyzw;
						Vector4Swizzle<Type, 0, -2, 0, -3, alignment> wywx;
						Vector4Swizzle<Type, 0, -2, 0, -2, alignment> wywy;
						Vector4Swizzle<Type, 0, -2, 0, -1, alignment> wywz;
						Vector4Swizzle<Type, 0, -2, 0, 0, alignment> wyww;
						Vector4Swizzle<Type, 0, -1, -3, -3, alignment> wzxx;
						Vector4Swizzle<Type, 0, -1, -3, -2, alignment> wzxy;
						Vector4Swizzle<Type, 0, -1, -3, -1, alignment> wzxz;
						Vector4Swizzle<Type, 0, -1, -3, 0, alignment> wzxw;
						Vector4Swizzle<Type, 0, -1, -2, -3, alignment> wzyx;
						Vector4Swizzle<Type, 0, -1, -2, -2, alignment> wzyy;
						Vector4Swizzle<Type, 0, -1, -2, -1, alignment> wzyz;
						Vector4Swizzle<Type, 0, -1, -2, 0, alignment> wzyw;
						Vector4Swizzle<Type, 0, -1, -1, -3, alignment> wzzx;
						Vector4Swizzle<Type, 0, -1, -1, -2, alignment> wzzy;
						Vector4Swizzle<Type, 0, -1, -1, -1, alignment> wzzz;
						Vector4Swizzle<Type, 0, -1, -1, 0, alignment> wzzw;
						Vector4Swizzle<Type, 0, -1, 0, -3, alignment> wzwx;
						Vector4Swizzle<Type, 0, -1, 0, -2, alignment> wzwy;
						Vector4Swizzle<Type, 0, -1, 0, -1, alignment> wzwz;
						Vector4Swizzle<Type, 0, -1, 0, 0, alignment> wzww;
						Vector4Swizzle<Type, 0, 0, -3, -3, alignment> wwxx;
						Vector4Swizzle<Type, 0, 0, -3, -2, alignment> wwxy;
						Vector4Swizzle<Type, 0, 0, -3, -1, alignment> wwxz;
						Vector4Swizzle<Type, 0, 0, -3, 0, alignment> wwxw;
						Vector4Swizzle<Type, 0, 0, -2, -3, alignment> wwyx;
						Vector4Swizzle<Type, 0, 0, -2, -2, alignment> wwyy;
						Vector4Swizzle<Type, 0, 0, -2, -1, alignment> wwyz;
						Vector4Swizzle<Type, 0, 0, -2, 0, alignment> wwyw;
						Vector4Swizzle<Type, 0, 0, -1, -3, alignment> wwzx;
						Vector4Swizzle<Type, 0, 0, -1, -2, alignment> wwzy;
						Vector4Swizzle<Type, 0, 0, -1, -1, alignment> wwzz;
						Vector4Swizzle<Type, 0, 0, -1, 0, alignment> wwzw;
						Vector4Swizzle<Type, 0, 0, 0, -3, alignment> wwwx;
						Vector4Swizzle<Type, 0, 0, 0, -2, alignment> wwwy;
						Vector4Swizzle<Type, 0, 0, 0, -1, alignment> wwwz;
						Vector4Swizzle<Type, 0, 0, 0, 0, alignment> wwww;
						
						struct
						{
							operator Type() const
							{
								Vector4& vector = Memory::Cast<Vector4>(this, (-((int)sizeof(Type) * 3)));
								return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z + vector.w * vector.w;
							}

							Type operator()() const
							{
								//Type result = (*this);
								return (*this);
							}

							Vector4& operator = (Type aDesiredLengthSquared)
							{
								Vector4& vector = Memory::Cast<Vector4>(this, (-((int)sizeof(Type) * 3)));
								Type currentLength = vector.Length();
								vector *= (SquareRoot(aDesiredLengthSquared) / currentLength);
								return vector;
							}

						}LengthSqr, LengthSquared, MagnitudeSquared, lengthSquared, magnitudeSquared;

						struct
						{
							operator Type() const
							{
								Vector4& vector = Memory::Cast<Vector4>(this, (-((int)sizeof(Type) * 3)));
								return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z + vector.w * vector.w;
							}

							Type operator()(const Vector4& second_vector) const
							{
								Vector4& vector = Memory::Cast<Vector4>(this, (-((int)sizeof(Type) * 3)));
								return vector.x * second_vector.x + vector.y * second_vector.y + vector.z * second_vector.z + vector.w * second_vector.w;
							}

							Type operator()() const
							{
								Type result = (*this);
								return result;
							}

							Vector4& operator = (Type aDesiredDotProduct)
							{
								Vector4& vector = Memory::Cast<Vector4>(this, (-((int)sizeof(Type) * 3)));
								Type currentLength = vector.Length();
								vector *= (SquareRoot(aDesiredDotProduct) / currentLength);
								return vector;
							}

						}Dot, DotProduct;

						struct
						{
							void operator()(const Vector4& aFirstVector, const Vector4& aSecondVector, float aInterpolation) const
							{
								Vector4& vector = Memory::Cast<Vector4>(this, (-((int)sizeof(Type))));
								vector = ((aSecondVector - aFirstVector) * aInterpolation) + aFirstVector;
							}

							Vector4 operator()(const Vector4& aSecondVector, float aInterpolation) const
							{
								Vector4& vector = Memory::Cast<Vector4>(this, (-((int)sizeof(Type))));
								return ((aSecondVector - vector) * aInterpolation) + vector;
							}

							Vector4 operator()(float aInterpolation) const
							{
								Vector4& vector = Memory::Cast<Vector4>(this, (-((int)sizeof(Type))));
								return vector * aInterpolation;
							}

						}Lerp;

						struct
						{
							operator Type() const
							{
								Vector4& vector = Memory::Cast<Vector4>(this, (-((int)sizeof(Type) * 3)));
								return SquareRoot(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z + vector.w * vector.w);
							}

							Type operator()() const
							{
								Type result = (*this);
								return result;
							}

							Vector4& operator = (Type aDesiredLength)
							{
								Vector4& vector = Memory::Cast<Vector4>(this, (-((int)sizeof(Type) * 3)));
								Type currentLength = (*this);
								vector *= (aDesiredLength / currentLength);
								return vector;
							}

						}Length, Magnitude, length, magnitude;

						struct
						{
							operator Vector4() const
							{
								Vector4& vector = Memory::Cast<Vector4>(this, (-((int)sizeof(Type) * 3)));
								Type length = vector.length;
								if (length > 0.0f)
								{
									return Vector4(vector.x / length, vector.y / length, vector.z / length, vector.w / length);
								}
								return vector;
							}

							Vector4 operator()() const
							{
								Vector4 result = (*this);
								return result;
							}

						}GetNormalized, unit, Unit, Normalized, normalized;

						struct
						{
							operator Vector4&()
							{
								Vector4& vector = Memory::Cast<Vector4>(this, (-((int)sizeof(Type) * 3)));
								Type length = vector.length;
								if (length > 0.0f)
								{
									vector.x /= length;
									vector.y /= length;
									vector.z /= length;
									vector.w /= length;
								}
								return vector;
							}

							Vector4& operator()() 
							{
								Vector4& result = (*this);
								return result;
							}

						}Normalize, normalize;
					};
				};
			};


			Type& operator [](int index)
			{
				return data[index];
			}

			void Iterate(std::function<void(Type)> aElementFunction)
			{
				for (int index = 0; index < 4; ++index)
				{
					aElementFunction(data[index]);
				}
			}

			Vector4& operator = (const Vector4 & aAssignment)
			{
				x = aAssignment.x;
				y = aAssignment.y;
				z = aAssignment.z;
				w = aAssignment.w;
				return (*this);
			}

			template<typename SecondaryType, int Offset1, int Offset2, int Offset3, int Offset4>
			Vector4& operator = (const Vector4Swizzle<SecondaryType, Offset1, Offset2, Offset3, Offset4> & aAssignment)
			{
				x = aAssignment.x;
				y = aAssignment.y;
				z = aAssignment.z;
				w = aAssignment.w;
				return (*this);
			}

			Vector4& operator = (const Type aAssignment)
			{
				x = aAssignment.x;
				y = aAssignment.y;
				z = aAssignment.z;
				w = aAssignment.w;
				return (*this);
			}

			Vector4& operator = (const float* aAssignment)
			{
				x = aAssignment[0];
				y = aAssignment[1];
				z = aAssignment[2];
				w = aAssignment[3];
				return (*this);
			}

			Vector4(const Type & a_x, const Type & a_y, const Type & a_z, const Type & a_w) : data{ a_x, a_y, a_z, a_w } {}
			Vector4(const Type & a_v) : data{ a_v, a_v, a_v, a_v } {}
			Vector4() : data{
				DefaultInitializationValue<Type>(),
				DefaultInitializationValue<Type>(),
				DefaultInitializationValue<Type>() ,
				DefaultInitializationValue<Type>() } {}
		};
	}
}

#include "operations.hpp"
