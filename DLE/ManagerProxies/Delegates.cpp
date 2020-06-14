#include "pch.h"
#include "Delegates.h"

using namespace DLEDotNet::ManagerProxies;

gcroot<ProxyDelegateManager^> g_proxyDelegateManager = gcnew ProxyDelegateManager();

void ProxyDelegateManager::AddTraceDelegate(TraceDelegate^ traceDelegate)
{
	m_traceDelegate += traceDelegate;
}

void ProxyDelegateManager::SetMineMoveRateDelegate(MineMoveRateDelegate^ mineMoveRateDelegate)
{
	// Only one is allowed
	m_mineMoveRateDelegate = mineMoveRateDelegate;
}

void ProxyDelegateManager::SetD1PigPathDelegate(D1PigPathDelegate^ d1PigPathDelegate)
{
	m_d1PigPathDelegate = d1PigPathDelegate;
}

void ProxyDelegateManager::SetD2PigPathDelegate(D2PigPathDelegate^ d2PigPathDelegate)
{
	m_d2PigPathDelegate = d2PigPathDelegate;
}

void ProxyDelegateManager::SetChangeD2PigPathDelegate(ChangeD2PigPathDelegate^ changeD2PigPathDelegate)
{
	m_changeD2PigPathDelegate += changeD2PigPathDelegate;
}

void ProxyDelegateManager::SetMissionFolderDelegate(MissionFolderDelegate^ missionFolderDelegate)
{
	m_missionFolderDelegate = missionFolderDelegate;
}

void ProxyDelegateManager::SetProgressTracker(IProgressTracker^ progressTracker)
{
	m_progressTracker = progressTracker;
}

void ProxyDelegateManager::OnTrace(System::Diagnostics::TraceLevel level, System::String^ trace)
{
	m_traceDelegate(level, trace);
}

double ProxyDelegateManager::GetMineMoveRate()
{
	return m_mineMoveRateDelegate();
}

System::String^ ProxyDelegateManager::GetD1PigPath()
{
	return m_d1PigPathDelegate();
}

System::String^ ProxyDelegateManager::GetD2PigPath()
{
	return m_d2PigPathDelegate();
}

void ProxyDelegateManager::ChangeD2PigPath(System::String^ newPath)
{
	m_changeD2PigPathDelegate(newPath);
}

System::String^ ProxyDelegateManager::GetMissionFolder()
{
	return m_missionFolderDelegate();
}

IProgressTracker^ ProxyDelegateManager::GetProgressTracker()
{
	return m_progressTracker;
}
