#pragma once

delegate void TraceDelegate(System::Diagnostics::TraceLevel level, System::String^ trace);
delegate double MineMoveRateDelegate();
delegate System::String^ D1PigPathDelegate();
delegate System::String^ D2PigPathDelegate();
delegate void ChangeD2PigPathDelegate(System::String^ newPath);
delegate System::String^ MissionFolderDelegate();

interface struct IProgressTracker
{
public:
    void InitializeProgress(int numSteps) = 0;
    void SetProgress(int stepNum) = 0;
    void IncrementProgress() = 0;
    void CleanupProgress() = 0;
};

ref class ProxyDelegateManager
{
public:
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

extern gcroot<ProxyDelegateManager^> g_proxyDelegateManager;
