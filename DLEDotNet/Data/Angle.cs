using DLEDotNet.Util;

namespace DLEDotNet.Data
{
    public struct Angle
    {
        private double angle;

        public static Angle FromDegrees(double deg)
        {
            return FromRadians(MathUtil.ToRadians(deg));
        }

        public static Angle FromRadians(double rad)
        {
            Angle a = new Angle();
            a.angle = rad;
            return a;
        }

        public double Degrees
        {
            get => MathUtil.ToDegrees(angle);
        }

        public double Radians
        {
            get => angle;
        }

        public override string ToString()
        {
            return this.Degrees + "°";
        }
    }
}