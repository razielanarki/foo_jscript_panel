#pragma once

class ComArrayWriter
{
public:
	ComArrayWriter(auto count)
	{
		m_psa = SafeArrayCreateVector(VT_VARIANT, 0L, static_cast<ULONG>(count));
	}

	HRESULT add_item(const std::wstring& str)
	{
		auto var = _variant_t(str.data());
		return add_item(var);
	}

	HRESULT add_item(jstring str)
	{
		auto var = _variant_t(to_wide(str).data());
		return add_item(var);
	}

	HRESULT add_item(_variant_t& var)
	{
		const HRESULT hr = SafeArrayPutElement(m_psa, &idx, &var);
		idx++;

		if (FAILED(hr))
		{
			SafeArrayDestroy(m_psa);
			m_psa = nullptr;
		}
		return hr;
	}

	SAFEARRAY* data()
	{
		return m_psa;
	}

private:
	SAFEARRAY* m_psa = nullptr;
	LONG idx{};
};
