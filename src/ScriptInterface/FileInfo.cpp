#include "stdafx.h"
#include "FileInfo.h"

FileInfo::FileInfo(const metadb_info_container::ptr& info) : m_info(info) {}

STDMETHODIMP FileInfo::get__ptr(void** out)
{
	if (!out) return E_POINTER;

	*out = m_info.get_ptr();
	return S_OK;
}

STDMETHODIMP FileInfo::InfoFind(BSTR name, int* out)
{
	if (m_info.is_empty() || !out) return E_POINTER;

	const string8 uname = from_wide(name);
	*out = to_int(m_info->info().info_find(uname));
	return S_OK;
}

STDMETHODIMP FileInfo::InfoName(UINT idx, BSTR* out)
{
	if (m_info.is_empty() || !out) return E_POINTER;
	if (idx >= m_info->info().info_get_count()) return E_INVALIDARG;

	*out = to_bstr(m_info->info().info_enum_name(idx));
	return S_OK;
}

STDMETHODIMP FileInfo::InfoValue(UINT idx, BSTR* out)
{
	if (m_info.is_empty() || !out) return E_POINTER;
	if (idx >= m_info->info().info_get_count()) return E_INVALIDARG;

	*out = to_bstr(m_info->info().info_enum_value(idx));
	return S_OK;
}

STDMETHODIMP FileInfo::MetaFind(BSTR name, int* out)
{
	if (m_info.is_empty() || !out) return E_POINTER;

	const string8 uname = from_wide(name);
	*out = to_int(m_info->info().meta_find(uname));
	return S_OK;
}

STDMETHODIMP FileInfo::MetaName(UINT idx, BSTR* out)
{
	if (m_info.is_empty() || !out) return E_POINTER;
	if (idx >= m_info->info().meta_get_count()) return E_INVALIDARG;

	*out = to_bstr(m_info->info().meta_enum_name(idx));
	return S_OK;
}

STDMETHODIMP FileInfo::MetaValue(UINT idx, UINT vidx, BSTR* out)
{
	if (m_info.is_empty() || !out) return E_POINTER;
	if (idx >= m_info->info().meta_get_count() || vidx >= m_info->info().meta_enum_value_count(idx)) return E_INVALIDARG;

	*out = to_bstr(m_info->info().meta_enum_value(idx, vidx));
	return S_OK;
}

STDMETHODIMP FileInfo::MetaValueCount(UINT idx, UINT* out)
{
	if (m_info.is_empty() || !out) return E_POINTER;
	if (idx >= m_info->info().meta_get_count()) return E_INVALIDARG;

	*out = m_info->info().meta_enum_value_count(idx);
	return S_OK;
}

STDMETHODIMP FileInfo::get_InfoCount(UINT* out)
{
	if (m_info.is_empty() || !out) return E_POINTER;

	*out = m_info->info().info_get_count();
	return S_OK;
}

STDMETHODIMP FileInfo::get_MetaCount(UINT* out)
{
	if (m_info.is_empty() || !out) return E_POINTER;

	*out = m_info->info().meta_get_count();
	return S_OK;
}

void FileInfo::FinalRelease()
{
	m_info.release();
}
