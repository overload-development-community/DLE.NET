using DLEDotNet.ManagedWrappers;
using LibDescent.Edit;

namespace DLEDotNet.Data.Proxies
{
    public class LevelProxy : ChangeableStateProxy<Level>
    {
        public void Open(string fileName)
        {
            // TODO ChangeableStateProxy needs a protected constructor to initialize Host.
            // It is currently not writeable from here.
            /*Host*/ new Level().Open(fileName);
        }
    }
}
