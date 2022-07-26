#pragma once

static wil::com_ptr_t<ITypeLib> g_type_lib;

struct TypeInfoCache
{
	wil::com_ptr_t<ITypeInfo> type_info;
	std::map<ULONG, DISPID> cache;
};

template <typename T>
class JSDispatchImplBase : public T
{
protected:
	JSDispatchImplBase()
	{
		if (!g_type_lib)
		{
			const HINSTANCE ins = core_api::get_my_instance();
			const auto path = wil::GetModuleFileNameW(ins);
			LoadTypeLibEx(path.get(), REGKIND_NONE, &g_type_lib);
		}

		if (!s_type_info_cache.type_info)
		{
			g_type_lib->GetTypeInfoOfGuid(__uuidof(T), &s_type_info_cache.type_info);
		}
	}

	virtual ~JSDispatchImplBase() {}

public:
	STDMETHODIMP GetIDsOfNames(REFIID, OLECHAR** names, UINT, LCID, DISPID* dispids) override
	{
		RETURN_HR_IF_NULL(E_POINTER, dispids);

		const ULONG hash = LHashValOfName(LANG_NEUTRAL, names[0]);
		const auto it = s_type_info_cache.cache.find(hash);
		if (it != s_type_info_cache.cache.end())
		{
			dispids[0] = it->second;
		}
		else
		{
			RETURN_IF_FAILED(s_type_info_cache.type_info->GetIDsOfNames(&names[0], 1, &dispids[0]));
			s_type_info_cache.cache.emplace(hash, dispids[0]);
		}
		return S_OK;
	}

	STDMETHODIMP GetTypeInfo(UINT i, LCID, ITypeInfo** out) override
	{
		RETURN_HR_IF_NULL(E_POINTER, out);
		if (i != 0) return DISP_E_BADINDEX;

		s_type_info_cache.type_info->AddRef();
		*out = s_type_info_cache.type_info.get();
		return S_OK;
	}

	STDMETHODIMP GetTypeInfoCount(UINT* n) override
	{
		RETURN_HR_IF_NULL(E_POINTER, n);

		*n = 1;
		return S_OK;
	}

	STDMETHODIMP Invoke(DISPID dispid, REFIID, LCID, WORD flags, DISPPARAMS* params, VARIANT* result, EXCEPINFO* excep, UINT* err) override
	{
		const HRESULT hr = s_type_info_cache.type_info->Invoke(this, dispid, flags, params, result, excep, err);
		PFC_ASSERT(hr != RPC_E_WRONG_THREAD);
		return hr;
	}

private:
	static TypeInfoCache s_type_info_cache;
};

template <typename T>
FOOGUIDDECL TypeInfoCache JSDispatchImplBase<T>::s_type_info_cache;

template <typename T>
class JSDispatchImpl : public JSDispatchImplBase<T>
{
protected:
	JSDispatchImpl() {}
	~JSDispatchImpl() {}

public:
	COM_QI_BEGIN()
		COM_QI_ENTRY(IUnknown)
		COM_QI_ENTRY(IDispatch)
		COM_QI_ENTRY(T)
	COM_QI_END()
};

template <typename T>
class JSDisposableImpl : public JSDispatchImplBase<T>
{
protected:
	JSDisposableImpl() {}
	~JSDisposableImpl() {}

	virtual void FinalRelease() = 0;

public:
	COM_QI_BEGIN()
		COM_QI_ENTRY(IUnknown)
		COM_QI_ENTRY(IDispatch)
		COM_QI_ENTRY(IDisposable)
		COM_QI_ENTRY(T)
	COM_QI_END()

	STDMETHODIMP Dispose() override
	{
		this->FinalRelease();
		return S_OK;
	}
};

template <typename Base>
class ComObjectImpl : public Base
{
public:
	template <typename... Args>
	ComObjectImpl(Args&&... args) : Base(std::forward<Args>(args)...) {}

	ULONG STDMETHODCALLTYPE AddRef() override
	{
		return ++m_counter;
	}

	ULONG STDMETHODCALLTYPE Release() override
	{
		const long n = --m_counter;
		if (n == 0)
		{
			if constexpr (requires { this->FinalRelease(); })
			{
				this->FinalRelease();
			}
			delete this;
		}
		return n;
	}

protected:
	virtual ~ComObjectImpl() {}

private:
	pfc::refcounter m_counter = 1;
};

template <typename T>
class ComObjectSingleton
{
public:
	static T* get()
	{
		if (!instance)
		{
			instance = new ImplementCOMRefCounter<T>();
		}

		return reinterpret_cast<T*>(instance.get());
	}

private:
	static wil::com_ptr_t<IDispatch> instance;

	PFC_CLASS_NOT_COPYABLE_EX(ComObjectSingleton)
};

template <typename T>
FOOGUIDDECL wil::com_ptr_t<IDispatch> ComObjectSingleton<T>::instance;
