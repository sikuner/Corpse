#include "StdAfx.h"
#include "FloatWindow.h"

CEventMainWnd CFloatWindow::m_sHookMainWnd;

CFloatWindow::CFloatWindow()
{
	m_hEventHook = NULL;
}

CFloatWindow::~CFloatWindow()
{
	ReleaseHookWinEvent();
}

BOOL CFloatWindow::SetWndStickToDesktop(HWND hMainWnd)
{	
	if (m_hEventHook != NULL)
		return FALSE;

	m_hEventHook = SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_HIDE, NULL, WinEventHookProc, 0, 0, WINEVENT_OUTOFCONTEXT|WINEVENT_SKIPOWNPROCESS);
	if (m_hEventHook != NULL)
	{
		EventHookWnd HookWnd;
		HookWnd.hMainWnd = hMainWnd;
		HookWnd.hEventHook = m_hEventHook;

		m_sHookMainWnd.AddTail(HookWnd);
	}
	return m_hEventHook != NULL;
}

void CFloatWindow::CannelWndSticked(HWND hMainWnd)
{
	POSITION pos = m_sHookMainWnd.GetHeadPosition();
	while(pos)
	{
		POSITION posOld = pos;
		const EventHookWnd& HookWnd = m_sHookMainWnd.GetNext(pos);
		if (HookWnd.hMainWnd == hMainWnd)
			m_sHookMainWnd.RemoveAt(posOld);
	}
}

void CFloatWindow::ReleaseHookWinEvent()
{
	if (m_hEventHook != NULL)
	{
		UnhookWinEvent(m_hEventHook);
		POSITION pos = m_sHookMainWnd.GetHeadPosition();
		while(pos)
		{
			POSITION posOld = pos;
			const EventHookWnd HookWnd = m_sHookMainWnd.GetNext(pos);
			if (HookWnd.hEventHook == m_hEventHook)
				m_sHookMainWnd.RemoveAt(posOld);
		}
		m_hEventHook = NULL;
	}
}

void CFloatWindow::WinEventHookProc(HWINEVENTHOOK hEventHook, DWORD dwEvent, HWND hWnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
	//�жϵ�ǰ�����Ƿ���Ч
	if (IsWindow(hWnd))
	{
		TCHAR szClassName[MAX_PATH];
		GetClassName(hWnd, szClassName, _countof(szClassName));

		POSITION pos = m_sHookMainWnd.GetHeadPosition();
		HWND hWnd = NULL;
		while(pos)
		{
			const EventHookWnd& HookWnd = m_sHookMainWnd.GetNext(pos);
			if (HookWnd.hEventHook == hEventHook)
				hWnd = HookWnd.hMainWnd;
		}

		//���������Ч������
		if (hWnd == NULL || IsWindow(hWnd) == FALSE)
			return;

		if (dwEvent == EVENT_OBJECT_HIDE)
		{
			//�ָ����洰�ڣ����簴����ʾ����/Win+D
			if (_tcsicmp(szClassName, _T("WorkerW")) == 0)
			{
				//ȡ�������ö�����
				SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
				//ȡ������ǰ�����ԣ������ڷ��������д������棬����֮��
				SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
			}
		}
		else if (dwEvent == EVENT_OBJECT_SHOW)
		{
			//��ʾ���洰�ڣ����簴����ʾ����/Win+D
			if (_tcsicmp(szClassName, _T("WorkerW")) == 0)
			{
				SetWindowPos(hWnd, HWND_TOPMOST,0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
			}
		}
	}
}