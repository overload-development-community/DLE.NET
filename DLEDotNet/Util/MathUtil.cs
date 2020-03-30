using System;
using System.Drawing;

namespace DLEDotNet.Util
{
    static class MathUtil
    {
        private const double DEG_PER_RAD = 180 / Math.PI;

        /// <summary>
        /// Returns whether the given floating-point number is a normal, finite number, including zero, but not including infinities or NaN.
        /// </summary>
        /// <param name="f">The number to check.</param>
        /// <returns></returns>
        public static bool IsFinite(double f)
        {
            return !Double.IsInfinity(f) && !Double.IsNaN(f);
        }

        /// <summary>
        /// Returns the result of the integer divided by two rounded according to mathematical rules (thus, odd integers round up).
        /// </summary>
        /// <param name="v">The number to divide by two and round.</param>
        /// <returns></returns>
        public static int RoundDiv2(int v)
        {
            return (v >> 1) + (v & 1);
        }

        /// <summary>
        /// Returns whether number range A = [a0, a1[ is completely outside number range B = [b0, b1[.
        /// </summary>
        /// <param name="a0">The lesser end of range A.</param>
        /// <param name="a1">The greater end of range A. Must be greater than or equal to a0.</param>
        /// <param name="b0">The lesser end of range B.</param>
        /// <param name="b1">The greater end of range B. Must be greater than or equal to b0.</param>
        /// <returns></returns>
        public static bool RangeOutside(int a0, int a1, int b0, int b1)
        {
#if DEBUG
            System.Diagnostics.Debug.Assert(a0 <= a1 && b0 <= b1);
#endif
            return a1 <= b0 || b1 <= a0;
        }

        /// <summary>
        /// Returns whether the inner rectangle is completely outside the outer rectangle.
        /// </summary>
        /// <param name="inner">The inner rectangle.</param>
        /// <param name="outer">The outer rectangle.</param>
        /// <returns></returns>
        public static bool RectangleOutside(Rectangle inner, Rectangle outer)
        {
            return RangeOutside(inner.Left, inner.Right, outer.Left, outer.Right)
                || RangeOutside(inner.Top, inner.Bottom, outer.Top, outer.Bottom);
        }

        /// <summary>
        /// Returns the given angle (in degrees) converted into radians.
        /// </summary>
        /// <param name="angleDegrees">The angle in degrees.</param>
        /// <returns></returns>
        public static double ToRadians(double angleDegrees)
        {
            return angleDegrees / DEG_PER_RAD;
        }

        /// <summary>
        /// Returns the given angle (in radians) converted into degrees.
        /// </summary>
        /// <param name="angleRadians">The angle in radians.</param>
        /// <returns></returns>
        public static double ToDegrees(double angleRadians)
        {
            return angleRadians * DEG_PER_RAD;
        }

        /// <summary>
        /// Returns whether a bit at a given index is set in the value.
        /// </summary>
        /// <param name="value">The value to check.</param>
        /// <param name="bitIndex">The index of the bit. Bit 0 corresponds to the rightmost (least significant) bit of the number, and increasing indexes correspond to more significant bits.</param>
        /// <returns></returns>
        public static bool GetBit(long value, int bitIndex)
        {
            return 0 != (value & (1L << bitIndex));
        }

        /// <summary>
        /// Returns the value clamped to the [minimum, maximum] range.
        /// </summary>
        /// <param name="value"></param>
        /// <param name="minimum"></param>
        /// <param name="maximum"></param>
        /// <returns></returns>
        public static int Clamp(int value, int minimum, int maximum)
        {
            return Math.Min(maximum, Math.Max(minimum, value));
        }

        /// <summary>
        /// Returns the value clamped to the [minimum, maximum] range.
        /// </summary>
        /// <param name="value"></param>
        /// <param name="minimum"></param>
        /// <param name="maximum"></param>
        /// <returns></returns>
        public static double Clamp(double value, double minimum, double maximum)
        {
            return Math.Min(maximum, Math.Max(minimum, value));
        }

        /// <summary>
        /// Returns the value clamped to the [minimum, maximum] range.
        /// </summary>
        /// <param name="value"></param>
        /// <param name="minimum"></param>
        /// <param name="maximum"></param>
        /// <returns></returns>
        public static decimal Clamp(decimal value, decimal minimum, decimal maximum)
        {
            return Math.Min(maximum, Math.Max(minimum, value));
        }

        /// <summary>
        /// Returns the value clamped to the [minimum, maximum] range.
        /// </summary>
        /// <param name="value"></param>
        /// <param name="minimum"></param>
        /// <param name="maximum"></param>
        /// <returns></returns>
        public static LibDescent.Data.Fix Clamp(LibDescent.Data.Fix value, LibDescent.Data.Fix minimum, LibDescent.Data.Fix maximum)
        {
            return Math.Min(maximum, Math.Max(minimum, value));
        }
    }
}
