#pragma once

struct CallbackData : public pfc::refcounted_object_root
{
	CallbackData(const VariantArgs& args) : m_args(args) {}

	VariantArgs m_args;
};

struct MetadbCallbackData : public pfc::refcounted_object_root
{
	MetadbCallbackData(const metadb_handle_ptr& handle) : m_handles(pfc::list_single_ref_t<metadb_handle_ptr>(handle)) {}
	MetadbCallbackData(metadb_handle_list_cref handles) : m_handles(handles) {}

	metadb_handle_list m_handles;
};

template <typename T>
class CallbackDataReleaser
{
public:
	template <typename TParam>
	CallbackDataReleaser(TParam data)
	{
		m_data = reinterpret_cast<T*>(data);
	}

	~CallbackDataReleaser()
	{
		m_data->refcount_release();
	}

	T* operator->()
	{
		return m_data;
	}

private:
	T* m_data;
};

struct AsyncArtData
{
	AsyncArtData(IMetadbHandle* handle, uint32_t art_id, IGdiBitmap* bitmap, BSTR path) : m_handle(handle), m_art_id(art_id), m_bitmap(bitmap), m_path(path) {}

	~AsyncArtData()
	{
		if (m_handle)
		{
			m_handle->Release();
		}

		if (m_bitmap)
		{
			m_bitmap->Release();
		}
	}

	IGdiBitmap* m_bitmap;
	IMetadbHandle* m_handle;
	_bstr_t m_path;
	uint32_t m_art_id = 0;
};

struct AsyncImageData
{
	AsyncImageData(uint32_t task_id, IGdiBitmap* bitmap, BSTR path) : m_task_id(task_id), m_bitmap(bitmap), m_path(path) {}

	~AsyncImageData()
	{
		if (m_bitmap)
		{
			m_bitmap->Release();
		}
	}

	IGdiBitmap* m_bitmap;
	_bstr_t m_path;
	uint32_t m_task_id = 0;
};
