using DLEDotNet.ManagedWrappers;
using System.Diagnostics;

namespace DLEDotNet.Data.Proxies
{
    internal class ManagerProxyBinder
    {
        private DelegateManager proxyDelegateManager = new DelegateManager();
        private EditorState editorState;

        internal ManagerProxyBinder(EditorState editorState)
        {
            this.editorState = editorState;
            proxyDelegateManager.AddTraceDelegate(TraceMessage);
            proxyDelegateManager.SetD1PigPathDelegate(() => editorState.Prefs.D1PIGPath);
            proxyDelegateManager.SetD2PigPathDelegate(() => editorState.Prefs.D2PIGPath);
            proxyDelegateManager.SetChangeD2PigPathDelegate((newPath) => editorState.Prefs.D2PIGPath = newPath);
            proxyDelegateManager.SetMissionFolderDelegate(() => editorState.Prefs.LevelsPath);
            proxyDelegateManager.SetMineMoveRateDelegate(() => editorState.Prefs.MoveRate);
            proxyDelegateManager.SetViewMoveRateDelegate(() => editorState.Prefs.ViewRate);
        }

        private void TraceMessage(TraceLevel level, string message)
        {
            // TODO Rethink this... some of these need to be displayed to the user. Prompts? Status messages?
            switch (level)
            {
                case TraceLevel.Error:
                    Trace.TraceError(message);
                    break;
                case TraceLevel.Warning:
                    Trace.TraceWarning(message);
                    break;
                case TraceLevel.Info:
                    if (editorState.Prefs.MessageLevel >= InformationLevel.Normal)
                    {
                        Trace.TraceInformation(message);
                    }
                    break;
                case TraceLevel.Verbose:
                    if (editorState.Prefs.MessageLevel >= InformationLevel.Verbose)
                    {
                        Trace.TraceInformation(message);
                    }
                    break;
                default:
                    break;
            }
        }
    }
}