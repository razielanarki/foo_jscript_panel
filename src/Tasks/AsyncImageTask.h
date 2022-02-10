#pragma once
#include "ImageHelpers.h"
#include "ThreadPool.h"

class AsyncImageTask : public SimpleThreadTask
{
public:
	AsyncImageTask(CWindow hwnd, const std::wstring& path, uint32_t task_id) : m_hwnd(hwnd), m_path(path), m_task_id(task_id) {}

	void run() override
	{
		IGdiBitmap* bitmap = ImageHelpers::load(m_path);
		AsyncImageData data(m_task_id, bitmap, SysAllocString(m_path.data()));
		m_hwnd.SendMessageW(std::to_underlying(CallbackID::on_load_image_done), reinterpret_cast<WPARAM>(&data));
	}

private:
	CWindow m_hwnd;
	std::wstring m_path;
	uint32_t m_task_id = 0;
};
