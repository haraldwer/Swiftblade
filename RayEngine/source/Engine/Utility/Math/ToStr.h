
namespace Utility
{
    namespace Math
    {
        template <class T>
        String ToStr(const T& InValue)
        {
            return std::to_string(InValue);
        }

        template <class T>
        String ToStr(const Quaternion<T>& InValue)
        {
            return std::to_string(InValue.w) + " " + std::to_string(InValue.x) + " " + std::to_string(InValue.y) + " " + std::to_string(InValue.z);
        }

        template <class T, int A>
        String ToStr(const Vector4<T, A>& InValue)
        {
            return std::to_string(InValue.x) + " " + std::to_string(InValue.y) + " " + std::to_string(InValue.z) + " " + std::to_string(InValue.w);
        }

        template <class T, int A>
        String ToStr(const Vector3<T, A>& InValue)
        {
            return std::to_string(InValue.x) + " " + std::to_string(InValue.y) + " " + std::to_string(InValue.z);
        }
        
        template <class T, int A>
        String ToStr(const Vector2<T, A>& InValue)
        {
            return std::to_string(InValue.x) + " " + std::to_string(InValue.y);
        }

        template <class T>
        String ToStr(const Matrix3x3<T>& InValue)
        {
            return ToStr(InValue.right) + " " + ToStr(InValue.up) + " " + ToStr(InValue.forward);
        }
        
        template <class T>
        String ToStr(const Matrix4x4<T>& InValue)
        {
            auto p = InValue.GetPosition();
            auto s = InValue.GetScale();
            auto r = InValue.GetEuler();
            return "P: " + ToStr(p) + " S: " + ToStr(s) + " R: " + ToStr(r);
        }
    }
}
