#pragma once

delegate void TraceDelegate(TraceLevel level, System::String^ trace);

ref class ProxyDelegateManager
{
public:
	void AddTraceDelegate(TraceDelegate^ traceDelegate);

	void OnTrace(TraceLevel level, System::String^ trace);

private:
	TraceDelegate^ m_traceDelegate;
};

gcroot<ProxyDelegateManager^> g_proxyDelegateManager;
