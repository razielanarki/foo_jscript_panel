#pragma once
#include <ScintillaTypes.h>
#include <ScintillaMessages.h>

#pragma comment(lib, "Imm32.lib")

using namespace Scintilla;

template <class T>
class CScintillaImpl : public CWindowImpl<T, CWindow, CControlWinTraits>
{
public:
	DECLARE_WND_SUPERCLASS2(L"CScintillaImpl", CScintillaImpl, CWindow::GetWndClassName())

REPLACEME

	void SetFnPtr()
	{
		ATLASSERT(::IsWindow(this->m_hWnd));
		fn = reinterpret_cast<FunctionDirect>(::SendMessageW(this->m_hWnd, static_cast<uint32_t>(Message::GetDirectFunction), 0, 0));
		ptr = ::SendMessageW(this->m_hWnd, static_cast<uint32_t>(Message::GetDirectPointer), 0, 0);
	}

private:
	using FunctionDirect = intptr_t(*)(intptr_t ptr, uint32_t msg, uintptr_t wParam, intptr_t lParam);

	intptr_t Call(Message msg, uintptr_t wParam = 0, intptr_t lParam = 0)
	{
		return fn(ptr, static_cast<uint32_t>(msg), wParam, lParam);
	}

	intptr_t CallPointer(Message msg, uintptr_t wParam, void* s)
	{
		return Call(msg, wParam, reinterpret_cast<intptr_t>(s));
	}
	
	intptr_t CallString(Message msg, uintptr_t wParam, const char* s)
	{
		return Call(msg, wParam, reinterpret_cast<intptr_t>(s));
	}

	FunctionDirect fn;
	intptr_t ptr;
};
