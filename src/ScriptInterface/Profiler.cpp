#include "stdafx.h"
#include "Profiler.h"

Profiler::Profiler(const std::wstring& name) : m_name(from_wide(name)) {}

STDMETHODIMP Profiler::Print()
{
	FB2K_console_formatter() << "FbProfiler (" << m_name << "): " << m_timer.query() << " ms";
	return S_OK;
}

STDMETHODIMP Profiler::Reset()
{
	m_timer.reset();
	return S_OK;
}

STDMETHODIMP Profiler::get_Time(__int64* out)
{
	if (!out) return E_POINTER;

	*out = m_timer.query();
	return S_OK;
}
