namespace DLEDotNet.Util
{
    public static class StringUtil
    {
        /// <summary>
        /// Truncates a string to fit the maximum length.
        /// </summary>
        /// <param name="text">The string to truncate.</param>
        /// <param name="maximumLength">The maximum length to truncate to.</param>
        /// <returns></returns>
        public static string Truncate(string text, int maximumLength)
        {
            if (text.Length > maximumLength)
                return text.Substring(0, maximumLength);
            else
                return text;
        }
    }
}
