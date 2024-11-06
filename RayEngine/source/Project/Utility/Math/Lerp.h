
namespace Utility
{
    namespace Math
    {
        template<class T, class F>
        T Lerp(const T a, const T b, const F t)
        {
            return a + (b - a) * Saturate(t);
        }

        // TODO: Write interp functions
    }
}
