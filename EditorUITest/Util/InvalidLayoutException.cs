﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EditorUITest.Util
{
    class InvalidLayoutException : Exception
    {
        public InvalidLayoutException() : base()
        {
        }

        public InvalidLayoutException(string message) : base(message)
        {
        }
    }
}
