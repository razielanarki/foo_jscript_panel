#pragma once

class ComArrayReader
{
public:
	bool convert(const VARIANT& v, pfc::bit_array_bittable& out)
	{
		if (!init(v, VT_UI4)) return false;
		if (m_data.empty()) out.resize(0);

		for (const auto& item : m_data)
		{
			out.set(item.ulVal, true);
		}
		return true;
	}

	bool convert(const VARIANT& v, pfc::string_list_impl& out)
	{
		if (!init(v, VT_BSTR)) return false;

		for (const auto& item : m_data)
		{
			const string8 str = from_wide(item.bstrVal);
			out.add_item(str);
		}
		return true;
	}

	bool convert(const VARIANT& v, std::vector<Gdiplus::PointF>& out)
	{
		if (!init(v, VT_R4)) return false;
		if (m_data.size() % 2 != 0) return false;

		const uint32_t count = m_data.size() >> 1;
		out.resize(count);

		for (const uint32_t i : std::views::iota(0U, count))
		{
			out[i] = { m_data[i * 2].fltVal, m_data[(i * 2) + 1].fltVal };
		}
		return true;
	}

private:
	bool get_property(IDispatch* pdisp, const std::wstring& name, VARTYPE vt, VARIANT& result)
	{
		LPOLESTR cname = const_cast<LPOLESTR>(name.data());
		DISPID dispId;
		DISPPARAMS params{};
		if (FAILED(pdisp->GetIDsOfNames(IID_NULL, &cname, 1, LOCALE_USER_DEFAULT, &dispId))) return false;
		if (FAILED(pdisp->Invoke(dispId, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &params, &result, nullptr, nullptr))) return false;
		if (FAILED(VariantChangeType(&result, &result, 0, vt))) return false;
		return true;
	}

	bool init(const VARIANT& v, VARTYPE vt)
	{
		if (v.vt != VT_DISPATCH || !v.pdispVal) return false;

		IDispatch* pdisp = v.pdispVal;
		_variant_t result;
		if (!get_property(pdisp, L"length", VT_UI4, result)) return false;
		const uint32_t count = result.ulVal;
		m_data.resize(count);

		for (const uint32_t i : std::views::iota(0U, count))
		{
			if (!get_property(pdisp, std::to_wstring(i), vt, result)) return false;
			m_data[i] = result;
		}
		return true;
	}

	VariantArgs m_data;
};

class ComArrayWriter
{
public:
	ComArrayWriter(ULONG count)
	{
		m_psa = SafeArrayCreateVector(VT_VARIANT, 0L, count);
	}

	SAFEARRAY* data()
	{
		return m_psa;
	}

	bool add_item(const std::wstring& str)
	{
		_variant_t var = SysAllocString(str.data());
		return add_item(var);
	}

	bool add_item(jstring str)
	{
		_variant_t var = to_bstr(str);
		return add_item(var);
	}

	bool add_item(_variant_t& var)
	{
		if (m_psa == nullptr) return false;
		if (FAILED(SafeArrayPutElement(m_psa, &idx, &var)))
		{
			SafeArrayDestroy(m_psa);
			m_psa = nullptr;
			return false;
		}
		idx++;
		return true;
	}

private:
	SAFEARRAY* m_psa = nullptr;
	LONG idx = 0L;
};
