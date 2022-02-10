#include "stdafx.h"
#include "PanelProperties.h"

bool PanelProperties::get_property(jstring key, VARIANT& out)
{
	const auto it = m_data.find(key);
	if (it == m_data.end()) return false;
	return SUCCEEDED(VariantCopy(&out, &it->second));
}

bool PanelProperties::set_property(jstring key, const VARIANT& val)
{
	if (g_sizeof(val.vt) == -1)
	{
		m_data.erase(key);
		return false;
	}
	m_data[key] = val;
	return true;
}

int PanelProperties::g_sizeof(VARTYPE vt)
{
	switch (vt)
	{
	case VT_BSTR:
		return 0;
	case VT_I1:
	case VT_UI1:
		return sizeof(BYTE);
	case VT_I2:
	case VT_UI2:
	case VT_BOOL:
		return sizeof(short);
	case VT_I4:
	case VT_UI4:
	case VT_R4:
		return sizeof(LONG);
	case VT_I8:
	case VT_UI8:
		return sizeof(LONGLONG);
	case VT_R8:
		return sizeof(double);
	default:
		return -1;
	}
}

void PanelProperties::g_get(stream_writer* writer, const PropertyData& data, abort_callback& abort)
{
	try
	{
		writer->write_lendian_t(data.size(), abort);

		for (const auto& [key, value] : data)
		{
			writer->write_string(key.c_str(), abort);
			writer->write_lendian_t(value.vt, abort);
			const int cbWrite = g_sizeof(value.vt);

			if (cbWrite > 0)
			{
				writer->write(&value.bVal, cbWrite, abort);
			}
			else if (cbWrite == 0)
			{
				const string8 str = from_wide(value.bstrVal);
				writer->write_string(str, abort);
			}
		}
	}
	catch (...) {}
}

void PanelProperties::g_set(stream_reader* reader, PropertyData& data, abort_callback& abort)
{
	data.clear();

	try
	{
		uint32_t count = 0;
		reader->read_lendian_t(count, abort);

		for (const uint32_t i : std::views::iota(0U, count))
		{
			string8 key;
			_variant_t value;
			reader->read_string(key, abort);
			reader->read_lendian_t(value.vt, abort);
			const int cbRead = g_sizeof(value.vt);

			if (cbRead > 0)
			{
				reader->read(&value.bVal, cbRead, abort);
			}
			else
			{
				string8 str;
				reader->read_string(str, abort);
				value.bstrVal = to_bstr(str);
			}

			data.emplace(key, value);
		}
	}
	catch (...) {}
}

void PanelProperties::get(stream_writer* writer, abort_callback& abort) const
{
	g_get(writer, m_data, abort);
}

void PanelProperties::set(stream_reader* reader, abort_callback& abort)
{
	g_set(reader, m_data, abort);
}
