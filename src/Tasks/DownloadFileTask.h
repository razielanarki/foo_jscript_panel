#pragma once
#include "ThreadPool.h"

class DownloadFileTask : public SimpleThreadTask
{
public:
	DownloadFileTask(CWindow hwnd, const std::wstring& url, const std::wstring& path)
		: m_hwnd(hwnd)
		, m_url(from_wide(url))
		, m_path(from_wide(path)) {}

	void run() override
	{
		try
		{
			auto request = http_client::get()->create_request("GET");
			auto response = request->run(m_url, fb2k::noAbort);
			auto size = response->get_size_ex(fb2k::noAbort); // ignoring the return value, throwing on unknown size is good enough

			pfc::array_t<uint8_t> data;
			response->read_till_eof(data, fb2k::noAbort);

			auto f = fileOpenWriteNew(m_path, fb2k::noAbort, 0.5);
			f->write(data.get_ptr(), data.get_size(), fb2k::noAbort);
			m_success = true;
		}
		catch (const std::exception& e)
		{
			m_error_text = e.what();
		}

		auto data = new CallbackData({ to_bstr(m_path), m_success, to_bstr(m_error_text) });
		data->refcount_add_ref();
		m_hwnd.SendMessageW(std::to_underlying(CallbackID::on_download_file_done), reinterpret_cast<WPARAM>(data));
	}

private:
	bool m_success = false;
	CWindow m_hwnd;
	string8 m_error_text, m_path, m_url;
};
