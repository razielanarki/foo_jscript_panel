#pragma once

extern wil::com_ptr_t<ITypeLib> g_type_lib;
struct IDisposable;

#define QI_HELPER_BEGIN(first) \
	STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override \
	{ \
		if (!ppv) return E_POINTER; \
		*ppv = nullptr; \
		IUnknown* temp = nullptr; \
		if (riid == __uuidof(IUnknown)) temp = static_cast<IUnknown*>(static_cast<first*>(this)); \
		else if (riid == __uuidof(first)) temp = static_cast<first*>(this);

#define QI_HELPER_ENTRY(entry) \
		else if (riid == __uuidof(entry)) temp = static_cast<entry*>(this);

#define QI_HELPER_END() \
		else return E_NOINTERFACE; \
		temp->AddRef(); \
		*ppv = temp; \
		return S_OK; \
	}

#define QI_HELPER(what) QI_HELPER_BEGIN(what) QI_HELPER_END()
#define QI_HELPER_DISPATCH(what) QI_HELPER_BEGIN(IDispatch) QI_HELPER_ENTRY(what) QI_HELPER_END()
#define QI_HELPER_DISPOSABLE(what) QI_HELPER_BEGIN(IDispatch) QI_HELPER_ENTRY(IDisposable) QI_HELPER_ENTRY(what) QI_HELPER_END()

struct TypeInfoCache
{
	wil::com_ptr_t<ITypeInfo> type_info;
	std::map<ULONG, DISPID> cache;
};

template <typename T>
class JSDispatchImplBase : public T
{
protected:
	JSDispatchImplBase<T>()
	{
		if (!s_type_info_cache.type_info)
		{
			g_type_lib->GetTypeInfoOfGuid(__uuidof(T), &s_type_info_cache.type_info);
		}
	}

	virtual ~JSDispatchImplBase<T>() {}

public:
	STDMETHODIMP GetIDsOfNames(REFIID, OLECHAR** names, UINT cNames, LCID, DISPID* dispids) override
	{
		if (!dispids) return E_POINTER;

		const ULONG hash = LHashValOfName(LANG_NEUTRAL, names[0]);
		const auto it = s_type_info_cache.cache.find(hash);
		if (it != s_type_info_cache.cache.end())
		{
			dispids[0] = it->second;
		}
		else
		{
			const HRESULT hr = s_type_info_cache.type_info->GetIDsOfNames(&names[0], 1, &dispids[0]);
			if (FAILED(hr)) return hr;
			s_type_info_cache.cache.emplace(hash, dispids[0]);
		}
		return S_OK;
	}

	STDMETHODIMP GetTypeInfo(UINT i, LCID, ITypeInfo** out) override
	{
		if (!out) return E_POINTER;
		if (i != 0) return DISP_E_BADINDEX;
		s_type_info_cache.type_info->AddRef();
		*out = s_type_info_cache.type_info.get();
		return S_OK;
	}

	STDMETHODIMP GetTypeInfoCount(UINT* n) override
	{
		if (!n) return E_POINTER;
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
	JSDispatchImpl<T>() {}
	~JSDispatchImpl<T>() {}

public:
	QI_HELPER_DISPATCH(T)
};

template <typename T>
class JSDisposableImpl : public JSDispatchImplBase<T>
{
protected:
	JSDisposableImpl<T>() {}
	~JSDisposableImpl<T>() {}

	virtual void FinalRelease() = 0;

public:
	QI_HELPER_DISPOSABLE(T)

	STDMETHODIMP Dispose() override
	{
		this->FinalRelease();
		return S_OK;
	}
};

template <typename _Base>
class ComObjectImpl : public _Base
{
public:
	template <typename... Args>
	ComObjectImpl(Args&&... args) : _Base(std::forward<Args>(args)...) {}

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

private:
	~ComObjectImpl() {}

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
