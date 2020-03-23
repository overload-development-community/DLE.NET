using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EditorUITest.Util
{
    static class MathUtilities
    {
        public static bool IsFinite(double f)
        {
            return !Double.IsInfinity(f) && !Double.IsNaN(f);
        }

        internal static int RoundDiv2(int v)
        {
            return (v >> 1) + (v & 1);
        }
    }
}
