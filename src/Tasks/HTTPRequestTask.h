#pragma once
#include "ThreadPool.h"

class HTTPRequestTask : public SimpleThreadTask
{
public:
	enum class Type
	{
		GET,
		POST,
	};

	HTTPRequestTask(Type type, CWindow hwnd, uint32_t task_id, const std::wstring& url, const std::wstring& user_agent_or_headers, const std::wstring& post_data = std::wstring(), const std::wstring& content_type = std::wstring())
		: m_type(type)
		, m_hwnd(hwnd)
		, m_task_id(task_id)
		, m_url(from_wide(url))
		, m_user_agent_or_headers(from_wide(user_agent_or_headers))
		, m_post_data(from_wide(post_data))
		, m_content_type(from_wide(content_type)) {}

	void run() override
	{
		try
		{
			if (m_type == Type::GET)
			{
				auto request = http_client::get()->create_request("GET");
				add_headers(request);
				parse_response(request->run_ex(m_url, fb2k::noAbort));
			}
			else if (m_type == Type::POST)
			{
				http_request_post_v2::ptr request;
				http_client::get()->create_request("POST")->cast(request);
				request->set_post_data(reinterpret_cast<const void*>(m_post_data.get_ptr()), m_post_data.length(), m_content_type);
				add_headers(request);
				parse_response(request->run_ex(m_url, fb2k::noAbort));
			}
		}
		catch (const std::exception& e)
		{
			m_buffer = e.what();
		}

		auto data = new CallbackData({ m_task_id, m_success, to_bstr(m_buffer) });
		data->refcount_add_ref();
		m_hwnd.SendMessageW(std::to_underlying(CallbackID::on_http_request_done), reinterpret_cast<WPARAM>(data));
	}

private:
	void add_headers(auto& request)
	{
		if (m_user_agent_or_headers.is_empty()) return;

		json j = json::parse(m_user_agent_or_headers.get_ptr(), nullptr, false);
		if (j.is_object())
		{
			for (const auto& [name, value] : j.items())
			{
				if (name.empty()) continue;
				request->add_header(name.c_str(), string_from_json(value).c_str());
			}
		}
		else
		{
			request->add_header("User-Agent", m_user_agent_or_headers);
		}
	}

	void parse_response(file::ptr response)
	{
		http_reply::ptr reply;
		if (response->cast(reply))
		{
			string8 content_type;
			reply->get_http_header_multi("Content-Type", content_type);
			if (content_type.contains("text") || content_type.contains("json"))
			{
				response->read_string_raw(m_buffer, fb2k::noAbort);
				m_success = true;
			}
			else
			{
				m_buffer << "Invalid content type: " << content_type;
			}
		}
		else
		{
			m_buffer << "Unable to determine content type";
		}
	}

	CWindow m_hwnd;
	Type m_type = Type::GET;
	bool m_success = false;
	string8 m_buffer, m_url, m_user_agent_or_headers, m_post_data, m_content_type;
	uint32_t m_task_id = 0;
};
