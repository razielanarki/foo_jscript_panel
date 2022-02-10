#include "stdafx.h"
#include "Config.h"
#include "DialogProperties.h"

namespace
{
	static const CDialogResizeHelper::Param resize_data[] =
	{
		{ IDC_LIST_PROPERTIES, 0, 0, 1, 1 },
		{ IDC_BTN_IMPORT, 0, 1, 0, 1 },
		{ IDC_BTN_EXPORT, 0, 1, 0, 1 },
		{ IDC_BTN_CLEAR, 0, 1, 0, 1 },
		{ IDOK, 1, 1, 1, 1 },
		{ IDCANCEL, 1, 1, 1, 1 },
		{ IDC_BTN_APPLY, 1, 1, 1, 1 },
	};

	static const CRect resize_min_max(620, 400, 0, 0);
}

CDialogProperties::CDialogProperties(PanelWindow* panel)
	: m_panel(panel)
	, m_dup_data(panel->m_config.m_properties.m_data)
	, m_resizer(resize_data, resize_min_max) {}

BOOL CDialogProperties::OnInitDialog(CWindow, LPARAM)
{
	pfc::setWindowText(m_hWnd, PFC_string_formatter() << Component::name << " Properties (id:" << m_panel->m_id << ")");

	g_config.set_window_placement(m_hWnd);

	m_btn_clear = GetDlgItem(IDC_BTN_CLEAR);
	m_btn_export = GetDlgItem(IDC_BTN_EXPORT);
	m_list.CreateInDialog(*this, IDC_LIST_PROPERTIES);
	LoadProperties();
	return FALSE;
}

void CDialogProperties::LoadProperties()
{
	m_list.m_items.clear();

	for (const auto& [key, value] : m_dup_data)
	{
		PropertyList::ListItem item;
		item.key = key;

		if (value.vt == VT_BOOL)
		{
			item.is_bool = true;
			item.bool_value = to_bool(value.boolVal);
		}
		else
		{
			if (value.vt == VT_BSTR) item.is_string = true;

			_variant_t var;
			var.ChangeType(VT_BSTR, &value);
			item.value = from_wide(var.bstrVal);
		}

		m_list.m_items.emplace_back(item);
	}

	m_list.ReloadData();
	m_btn_clear.EnableWindow(m_list.m_items.size());
	m_btn_export.EnableWindow(m_list.m_items.size());
}

void CDialogProperties::OnApplyOrOK(UINT, int nID, CWindow)
{
	m_panel->m_config.m_properties.m_data.clear();

	for (const PropertyList::ListItem& item : m_list.m_items)
	{
		if (item.is_bool)
		{
			m_panel->m_config.m_properties.m_data.emplace(item.key, _variant_t(item.bool_value));
		}
		else
		{
			_variant_t source(to_wide(item.value).data());
			_variant_t dest;

			if (!item.is_string && SUCCEEDED(VariantChangeType(&dest, &source, 0, VT_R8)))
			{
				m_panel->m_config.m_properties.m_data.emplace(item.key, dest);
			}
			else
			{
				m_panel->m_config.m_properties.m_data.emplace(item.key, source);
			}
		}
	}

	m_panel->update_script();
	m_dup_data = m_panel->m_config.m_properties.m_data;
	LoadProperties();

	g_config.get_window_placement(m_hWnd);

	if (nID == IDC_BTN_APPLY) return;
	EndDialog(nID);
}

void CDialogProperties::OnCancel(UINT, int nID, CWindow)
{
	EndDialog(nID);
}

void CDialogProperties::OnClear(UINT, int, CWindow)
{
	m_list.SelectAll();
	m_list.RequestRemoveSelection();
}

void CDialogProperties::OnExport(UINT, int, CWindow)
{
	string8 path;

	if (uGetOpenFileName(m_hWnd, "Property files|*.wsp", 0, "wsp", "Save as", nullptr, path, TRUE))
	{
		try
		{
			file_ptr io;
			filesystem::g_open_write_new(io, path, fb2k::noAbort);
			PanelProperties::g_get(io.get_ptr(), m_dup_data, fb2k::noAbort);
		}
		catch (...) {}
	}
}

void CDialogProperties::OnImport(UINT, int, CWindow)
{
	string8 path;

	if (uGetOpenFileName(m_hWnd, "Property files|*.wsp|All files|*.*", 0, "wsp", "Import from", nullptr, path, FALSE))
	{
		try
		{
			file_ptr io;
			filesystem::g_open_read(io, path, fb2k::noAbort);
			PanelProperties::g_set(io.get_ptr(), m_dup_data, fb2k::noAbort);
			LoadProperties();
		}
		catch (...) {}
	}
}
