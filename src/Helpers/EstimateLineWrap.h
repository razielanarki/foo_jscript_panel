#pragma once

class EstimateLineWrap
{
public:
	struct WrappedItem
	{
		std::wstring text;
		uint32_t width = 0;
	};

	using WrappedItems = std::vector<WrappedItem>;

	EstimateLineWrap(HDC hdc, uint32_t width) : m_hdc(hdc), m_width(width) {}

	WrappedItems wrap(const std::wstring& text)
	{
		const std::wstring delims = text.contains(CRLF_WIDE) ? CRLF_WIDE : LF_WIDE;
		auto view = std::views::split(text, delims);
		WStrings lines(view.begin(), view.end());
		WrappedItems items;

		for (const std::wstring& line : lines)
		{
			wrap_recur(line, items);
		}
		return items;
	}

private:
	bool not_wrap_char(wchar_t ch)
	{
		return iswalnum(ch) || iswpunct(ch);
	}

	uint32_t get_text_width(const std::wstring& text, uint32_t length)
	{
		SIZE size;
		GetTextExtentPoint32W(m_hdc, text.data(), to_int(length), &size);
		return to_uint(size.cx);
	}

	void wrap_recur(const std::wstring& text, WrappedItems& out)
	{
		const uint32_t text_width = get_text_width(text, text.length());

		if (text_width <= m_width)
		{
			WrappedItem item = { text, text_width };
			out.emplace_back(item);
		}
		else
		{
			uint32_t text_length = text.length() * m_width / text_width;

			if (get_text_width(text, text_length) < m_width)
			{
				while (get_text_width(text, std::min(text.length(), text_length + 1)) <= m_width)
				{
					++text_length;
				}
			}
			else
			{
				while (get_text_width(text, text_length) > m_width && text_length > 1)
				{
					--text_length;
				}
			}

			const uint32_t fallback_length = std::max(text_length, 1U);

			while (text_length > 0 && not_wrap_char(text.at(text_length - 1)))
			{
				--text_length;
			}

			if (text_length == 0) text_length = fallback_length;

			WrappedItem item = { text.substr(0, text_length), get_text_width(text, text_length) };
			out.emplace_back(item);

			if (text_length < text.length())
			{
				wrap_recur(text.substr(text_length), out);
			}
		}
	}

	static constexpr const wchar_t* CRLF_WIDE = L"\r\n";
	static constexpr const wchar_t* LF_WIDE = L"\n";

	HDC m_hdc;
	uint32_t m_width;
};
