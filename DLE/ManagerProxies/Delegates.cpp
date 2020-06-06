#include "pch.h"
#include "Delegates.h"

gcroot<ProxyDelegateManager^> g_proxyDelegateManager = gcnew ProxyDelegateManager();

void ProxyDelegateManager::AddTraceDelegate(TraceDelegate^ traceDelegate)
{
	m_traceDelegate += traceDelegate;
}

void ProxyDelegateManager::SetProgressTracker(IProgressTracker^ progressTracker)
{
	m_progressTracker = progressTracker;
}

void ProxyDelegateManager::OnTrace(System::Diagnostics::TraceLevel level, System::String^ trace)
{
	m_traceDelegate(level, trace);
}

IProgressTracker^ ProxyDelegateManager::GetProgressTracker()
{
	return m_progressTracker;
}
