#include "pch.h"
#include "Delegates.h"

gcroot<ProxyDelegateManager^> g_proxyDelegateManager = gcnew ProxyDelegateManager();

void ProxyDelegateManager::AddTraceDelegate(TraceDelegate^ traceDelegate)
{
	m_traceDelegate += traceDelegate;
}

void ProxyDelegateManager::OnTrace(TraceLevel level, System::String^ trace)
{
	m_traceDelegate(level, trace);
}
