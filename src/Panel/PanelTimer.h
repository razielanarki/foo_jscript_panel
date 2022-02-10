#pragma once

class PanelTimer
{
public:
	PanelTimer(CWindow hwnd, IDispatch* pdisp, DWORD delay, bool execute_once, uint32_t id)
		: m_hwnd(hwnd)
		, m_pdisp(pdisp)
		, m_delay(delay)
		, m_execute_once(execute_once)
		, m_id(id)
	{
		m_pdisp->AddRef();
	}

	~PanelTimer()
	{
		m_pdisp->Release();
	}

	static VOID CALLBACK timerProc(PVOID lpParameter, BOOLEAN)
	{
		PanelTimer* timer = static_cast<PanelTimer*>(lpParameter);
		timer->m_hwnd.SendMessageW(uwm::invoke_timer, timer->m_id);
	}

	BOOL start(HANDLE timer_queue)
	{
		return CreateTimerQueueTimer(
			&m_handle,
			timer_queue,
			PanelTimer::timerProc,
			this,
			m_delay,
			m_execute_once ? 0 : m_delay,
			WT_EXECUTEINTIMERTHREAD | (m_execute_once ? WT_EXECUTEONLYONCE : WT_EXECUTEDEFAULT));
	}

	void invoke()
	{
		DISPPARAMS params{};
		m_pdisp->Invoke(DISPID_VALUE, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, nullptr, nullptr, nullptr);
	}

	HANDLE m_handle = nullptr;
	bool m_execute_once = false;

private:
	CWindow m_hwnd;
	DWORD m_delay{};
	IDispatch* m_pdisp = nullptr;
	uint32_t m_id{};
};
