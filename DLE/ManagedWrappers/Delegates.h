#pragma once

namespace DLEDotNet
{
    namespace ManagedWrappers
    {
        public delegate void TraceDelegate(System::Diagnostics::TraceLevel level, System::String^ trace);
        public delegate double MineMoveRateDelegate();
        public delegate System::String^ D1PigPathDelegate();
        public delegate System::String^ D2PigPathDelegate();
        public delegate void ChangeD2PigPathDelegate(System::String^ newPath);
        public delegate System::String^ MissionFolderDelegate();

        public interface struct IProgressTracker
        {
        public:
            void InitializeProgress(int numSteps) = 0;
            void SetProgress(int stepNum) = 0;
            void IncrementProgress() = 0;
            void CleanupProgress() = 0;
        };

        public ref class DelegateManager
        {
        public:
            DelegateManager();

            // Called by UI

            void AddTraceDelegate(TraceDelegate^ traceDelegate);
            void SetMineMoveRateDelegate(MineMoveRateDelegate^ mineMoveRateDelegate);
            void SetD1PigPathDelegate(D1PigPathDelegate^ d1PigPathDelegate);
            void SetD2PigPathDelegate(D2PigPathDelegate^ d2PigPathDelegate);
            void SetChangeD2PigPathDelegate(ChangeD2PigPathDelegate^ changeD2PigPathDelegate);
            void SetMissionFolderDelegate(MissionFolderDelegate^ missionFolderDelegate);
            void SetProgressTracker(IProgressTracker^ progressTracker);

            // Called by GlobalDataImpl

            void OnTrace(System::Diagnostics::TraceLevel level, System::String^ trace);
            double GetMineMoveRate();
            System::String^ GetD1PigPath();
            System::String^ GetD2PigPath();
            void ChangeD2PigPath(System::String^ newPath);
            System::String^ GetMissionFolder();
            IProgressTracker^ GetProgressTracker();

        private:
            TraceDelegate^ m_traceDelegate;
            MineMoveRateDelegate^ m_mineMoveRateDelegate;
            D1PigPathDelegate^ m_d1PigPathDelegate;
            D2PigPathDelegate^ m_d2PigPathDelegate;
            ChangeD2PigPathDelegate^ m_changeD2PigPathDelegate;
            MissionFolderDelegate^ m_missionFolderDelegate;
            IProgressTracker^ m_progressTracker;
        };
    }
}

extern gcroot<DLEDotNet::ManagedWrappers::DelegateManager^> g_proxyDelegateManager;
