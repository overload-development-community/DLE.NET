#include "pch.h"
#include "Delegates.h"

using namespace DLEDotNet::ManagedWrappers;

gcroot<DelegateManager^> g_proxyDelegateManager = nullptr;

DelegateManager::DelegateManager()
{
	g_proxyDelegateManager = this;
}

void DelegateManager::AddTraceDelegate(TraceDelegate^ traceDelegate)
{
	m_traceDelegate += traceDelegate;
}

void DelegateManager::SetMineMoveRateDelegate(MineMoveRateDelegate^ mineMoveRateDelegate)
{
	// Only one is allowed
	m_mineMoveRateDelegate = mineMoveRateDelegate;
}

void DelegateManager::SetViewMoveRateDelegate(ViewMoveRateDelegate^ viewMoveRateDelegate)
{
	m_viewMoveRateDelegate = viewMoveRateDelegate;
}

void DelegateManager::SetD1PigPathDelegate(D1PigPathDelegate^ d1PigPathDelegate)
{
	m_d1PigPathDelegate = d1PigPathDelegate;
}

void DelegateManager::SetD2PigPathDelegate(D2PigPathDelegate^ d2PigPathDelegate)
{
	m_d2PigPathDelegate = d2PigPathDelegate;
}

void DelegateManager::SetChangeD2PigPathDelegate(ChangeD2PigPathDelegate^ changeD2PigPathDelegate)
{
	m_changeD2PigPathDelegate += changeD2PigPathDelegate;
}

void DelegateManager::SetMissionFolderDelegate(MissionFolderDelegate^ missionFolderDelegate)
{
	m_missionFolderDelegate = missionFolderDelegate;
}

void DelegateManager::SetProgressTracker(IProgressTracker^ progressTracker)
{
	m_progressTracker = progressTracker;
}

void DelegateManager::OnTrace(System::Diagnostics::TraceLevel level, System::String^ trace)
{
	m_traceDelegate(level, trace);
}

double DelegateManager::GetMineMoveRate()
{
	return m_mineMoveRateDelegate();
}

double DelegateManager::GetViewMoveRate()
{
	return m_viewMoveRateDelegate();
}

System::String^ DelegateManager::GetD1PigPath()
{
	return m_d1PigPathDelegate();
}

System::String^ DelegateManager::GetD2PigPath()
{
	return m_d2PigPathDelegate();
}

void DelegateManager::ChangeD2PigPath(System::String^ newPath)
{
	m_changeD2PigPathDelegate(newPath);
}

System::String^ DelegateManager::GetMissionFolder()
{
	return m_missionFolderDelegate();
}

IProgressTracker^ DelegateManager::GetProgressTracker()
{
	return m_progressTracker;
}
