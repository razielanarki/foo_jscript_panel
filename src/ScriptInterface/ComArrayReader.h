#pragma once

class ComArrayReader
{
public:
	HRESULT convert(const VARIANT& v, pfc::bit_array_bittable& out)
	{
		RETURN_IF_FAILED(init(v, VT_UI4));
		if (m_data.empty()) out.resize(0);

		for (const auto& item : m_data)
		{
			out.set(item.ulVal, true);
		}
		return S_OK;
	}

	HRESULT convert(const VARIANT& v, pfc::string_list_impl& out)
	{
		RETURN_IF_FAILED(init(v, VT_BSTR));

		for (const auto& item : m_data)
		{
			const string8 str = from_wide(item.bstrVal);
			out.add_item(str);
		}
		return S_OK;
	}

	HRESULT convert(const VARIANT& v, std::vector<Gdiplus::PointF>& out)
	{
		RETURN_IF_FAILED(init(v, VT_R4));
		if (m_data.size() % 2 != 0) return E_INVALIDARG;

		const uint32_t count = m_data.size() >> 1;
		out.resize(count);

		for (const uint32_t i : std::views::iota(0U, count))
		{
			out[i] = { m_data[i * 2].fltVal, m_data[(i * 2) + 1].fltVal };
		}
		return S_OK;
	}

private:
	HRESULT get_property(IDispatch* pdisp, const std::wstring& name, VARTYPE vt, VARIANT& result)
	{
		LPOLESTR cname = const_cast<LPOLESTR>(name.data());
		DISPID dispId;
		DISPPARAMS params{};
		RETURN_IF_FAILED(pdisp->GetIDsOfNames(IID_NULL, &cname, 1, LOCALE_USER_DEFAULT, &dispId));
		RETURN_IF_FAILED(pdisp->Invoke(dispId, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &params, &result, nullptr, nullptr));
		RETURN_IF_FAILED(VariantChangeType(&result, &result, 0, vt));
		return S_OK;
	}

	HRESULT init(const VARIANT& v, VARTYPE vt)
	{
		if (v.vt != VT_DISPATCH || !v.pdispVal) return E_INVALIDARG;

		IDispatch* pdisp = v.pdispVal;
		_variant_t result;
		RETURN_IF_FAILED(get_property(pdisp, L"length", VT_UI4, result));
		const size_t count = result.ulVal;
		m_data.resize(count);

		for (const size_t i : std::views::iota(0U, count))
		{
			RETURN_IF_FAILED(get_property(pdisp, std::to_wstring(i), vt, result));
			m_data[i] = result;
		}
		return S_OK;
	}

	VariantArgs m_data;
};
