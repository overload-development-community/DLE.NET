#pragma once

delegate void TraceDelegate(System::Diagnostics::TraceLevel level, System::String^ trace);

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
	void SetProgressTracker(IProgressTracker^ progressTracker);

	// Called by GlobalDataImpl

	void OnTrace(System::Diagnostics::TraceLevel level, System::String^ trace);
	IProgressTracker^ GetProgressTracker();

private:
	TraceDelegate^ m_traceDelegate;
	IProgressTracker^ m_progressTracker;
};

extern gcroot<ProxyDelegateManager^> g_proxyDelegateManager;
