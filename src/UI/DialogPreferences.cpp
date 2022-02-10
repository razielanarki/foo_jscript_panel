#include "stdafx.h"
#include "Config.h"

#include <libPPUI/CListControlOwnerData.h>
#include <libPPUI/Controls.h>

namespace
{
	class CDialogPreferences : public CDialogImpl<CDialogPreferences>, public preferences_page_instance, private IListControlOwnerDataSource
	{
	public:
		CDialogPreferences(preferences_page_callback::ptr callback) : m_callback(callback), m_list(this)
		{
			CSeparator::Register();
		}

		BEGIN_MSG_MAP_EX(CDialogPreferences)
			MSG_WM_INITDIALOG(OnInitDialog)
			COMMAND_HANDLER_EX(IDC_BTN_IMPORT, BN_CLICKED, OnImportBnClicked)
			COMMAND_HANDLER_EX(IDC_BTN_EXPORT, BN_CLICKED, OnExportBnClicked)
			COMMAND_HANDLER_EX(IDC_BTN_PRESETS, BN_CLICKED, OnPresetsBnClicked)
		END_MSG_MAP()

		enum { IDD = IDD_PREFERENCES };

		BOOL OnInitDialog(CWindow, LPARAM)
		{
			CreatePreferencesHeaderFont(m_font, *this);
			GetDlgItem(IDC_HEADER).SetFont(m_font.m_hFont);

			m_list.CreateInDialog(*this, IDC_LIST_PREFERENCES);
			m_list.SetSelectionModeNone();
			const SIZE DPI = m_list.GetDPI();
			m_list.AddColumn("Name", MulDiv(150, DPI.cx, 96));
			m_list.AddColumn("Value", MulDiv(310, DPI.cx, 96));

			m_btn_presets = GetDlgItem(IDC_BTN_PRESETS);
			return FALSE;
		}

		HWND get_wnd() override
		{
			return m_hWnd;
		}

		uint32_t get_state() override
		{
			return preferences_state::resettable;
		}

		void OnExportBnClicked(UINT, int, CWindow)
		{
			string8 path;
			if (uGetOpenFileName(m_hWnd, "Configuration files|*.cfg", 0, "cfg", "Save as", nullptr, path, TRUE))
			{
				g_config.save(path);
			}
		}

		void OnImportBnClicked(UINT, int, CWindow)
		{
			string8 path;
			if (uGetOpenFileName(m_hWnd, "Configuration files|*.cfg|All files|*.*", 0, "cfg", "Import from", nullptr, path, FALSE))
			{
				g_config.load(FileHelper(path).read());
				m_list.ReloadData();
			}
		}

		void OnPresetsBnClicked(UINT, int, CWindow)
		{
			auto menu = wil::unique_hmenu(CreatePopupMenu());
			AppendMenuW(menu.get(), MF_STRING, IDR_BRIGHT, L"Bright");
			AppendMenuW(menu.get(), MF_STRING, IDR_DARK, L"Dark");
			AppendMenuW(menu.get(), MF_STRING, IDR_RUBY, L"Ruby Blue");

			CRect rect;
			m_btn_presets.GetWindowRect(&rect);
			const int idx = TrackPopupMenuEx(menu.get(), TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, rect.left, rect.bottom, m_hWnd, nullptr);
			if (idx > 0)
			{
				g_config.load(Component::get_resource_text(idx));
				m_list.ReloadData();
			}
		}

		void apply() override
		{
			m_callback->on_state_changed();
		}

		void reset() override
		{
			g_config.init_data();
			m_list.ReloadData();
		}

	private:
		bool listIsColumnEditable(ctx_t, size_t column) override
		{
			return column == 1;
		}

		size_t listGetItemCount(ctx_t) override
		{
			return g_config.m_data.size();
		}

		string8 listGetSubItemText(ctx_t, size_t row, size_t column) override
		{
			switch (column)
			{
			case 0:
				return g_config.m_data[row].first;
			case 1:
				return g_config.m_data[row].second;
			}
			return "";
		}

		void listSetEditField(ctx_t, size_t row, size_t column, const char* value) override
		{
			if (column == 1)
			{
				g_config.m_data[row].second = value;
			}
		}

		void listSubItemClicked(ctx_t, size_t row, size_t column) override
		{
			if (column == 1)
			{
				m_list.TableEdit_Start(row, column);
			}
		}

		CButton m_btn_presets;
		CFont m_font;
		CListControlOwnerData m_list;
		preferences_page_callback::ptr m_callback;
	};

	class PreferencesPageImpl : public preferences_page_impl<CDialogPreferences>
	{
	public:
		GUID get_guid() override
		{
			return guids::preferences_page;
		}

		GUID get_parent_guid() override
		{
			return preferences_page::guid_tools;
		}

		bool get_help_url(pfc::string_base& out) override
		{
			out = Component::home_page;
			return true;
		}

		const char* get_name() override
		{
			return Component::name.c_str();
		}
	};

	FB2K_SERVICE_FACTORY(PreferencesPageImpl);
}
