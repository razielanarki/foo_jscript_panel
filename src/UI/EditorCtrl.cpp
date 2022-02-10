#include "stdafx.h"
#include "Config.h"
#include "DialogFindReplace.h"
#include "DialogGoto.h"
#include "EditorCtrl.h"

#include <ILexer.h>
#include <Lexilla.h>
#include <SciLexer.h>
#include <Scintilla.h>
#include <ScintillaStructures.h>

// Large portions taken from SciTE

namespace
{
	using StyleIDs = std::vector<int>;

	static const std::map<std::string, StyleIDs> style_map =
	{
		{ "style.default", { STYLE_DEFAULT } },
		{ "style.comment", { SCE_C_COMMENT, SCE_C_COMMENTLINE, SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, SCE_C_COMMENTDOCKEYWORD, SCE_C_COMMENTDOCKEYWORDERROR } },
		{ "style.linenumber", { STYLE_LINENUMBER } },
		{ "style.bracelight", { STYLE_BRACELIGHT } },
		{ "style.bracebad" , { STYLE_BRACEBAD } },
		{ "style.keyword", { SCE_C_WORD } },
		{ "style.identifier", { SCE_C_IDENTIFIER } },
		{ "style.number", { SCE_C_NUMBER } },
		{ "style.string", { SCE_C_STRING, SCE_C_CHARACTER } },
		{ "style.operator", { SCE_C_OPERATOR } },
	};
}

Line CEditorCtrl::GetCurrentLineNumber()
{
	return LineFromPosition(GetCurrentPos());
}

LRESULT CEditorCtrl::OnChange(UINT, int, CWindow)
{
	AutoMarginWidth();
	return 0;
}

LRESULT CEditorCtrl::OnCharAdded(LPNMHDR pnmh)
{
	const Range range = GetSelection();
	const NotificationData* notification = reinterpret_cast<NotificationData*>(pnmh);
	const int ch = notification->ch;

	if (range.start == range.end && range.start > 0)
	{
		if (CallTipActive())
		{
			switch (ch)
			{
			case ')':
				m_brace_count--;
				if (m_brace_count < 1)
					CallTipCancel();
				else
					StartCallTip();
				break;
			case '(':
				m_brace_count++;
				StartCallTip();
				break;
			default:
				ContinueCallTip();
				break;
			}
		}
		else if (AutoCActive())
		{
			if (ch == '(')
			{
				m_brace_count++;
				StartCallTip();
			}
			else if (ch == ')')
			{
				m_brace_count--;
			}
			else if (!IsWordCharacter(ch))
			{
				AutoCCancel();

				if (ch == '.')
					StartAutoComplete();
			}
		}
		else
		{
			if (ch == '(')
			{
				m_brace_count = 1;
				StartCallTip();
			}
			else
			{
				AutomaticIndentation(ch);

				if (IsWordCharacter(ch) || ch == '.')
					StartAutoComplete();
			}
		}
	}

	return 0;
}

LRESULT CEditorCtrl::OnKeyDown(UINT, WPARAM wParam, LPARAM, BOOL& bHandled)
{
	const KeyMod modifiers = (IsKeyPressed(VK_SHIFT) ? KeyMod::Shift : KeyMod::Norm) | (IsKeyPressed(VK_CONTROL) ? KeyMod::Ctrl : KeyMod::Norm) | (IsKeyPressed(VK_MENU) ? KeyMod::Alt : KeyMod::Norm);

	if (modifiers == KeyMod::Ctrl)
	{
		switch (wParam)
		{
		case 'F':
			OpenFindDialog();
			break;
		case 'H':
			OpenReplaceDialog();
			break;
		case 'G':
			fb2k::inMainThread([&]()
				{
					OpenGotoDialog();
				});
			break;
		case 'S':
			GetParent().PostMessageW(WM_COMMAND, MAKEWPARAM(IDC_BTN_APPLY, BN_CLICKED), reinterpret_cast<LPARAM>(m_hWnd));
			break;
		}
	}
	else if (wParam == VK_F3 && (modifiers == KeyMod::Norm || modifiers == KeyMod::Shift))
	{
		if (!m_dlg_find_replace || m_dlg_find_replace->m_find_text.is_empty())
		{
			OpenFindDialog();
		}
		else
		{
			Find(modifiers == KeyMod::Norm);
		}
	}

	bHandled = FALSE;
	return 1;
}

LRESULT CEditorCtrl::OnUpdateUI(LPNMHDR)
{
	const auto IsBraceChar = [](int ch)
	{
		return ch == '[' || ch == ']' || ch == '(' || ch == ')' || ch == '{' || ch == '}';
	};

	Position brace_at_caret = -1;
	Position brace_opposite = -1;
	const Position pos = GetCurrentPos();
	const int len = GetLength();

	if (len > 0)
	{
		if (pos > 0)
		{
			const Position pos_before = PositionBefore(pos);
			if (pos_before == pos - 1 && IsBraceChar(GetCharAt(pos_before)))
			{
				brace_at_caret = pos - 1;
			}
		}

		if (brace_at_caret < 0 && pos < len && IsBraceChar(GetCharAt(pos)))
		{
			brace_at_caret = pos;
		}

		if (brace_at_caret >= 0)
		{
			brace_opposite = BraceMatch(brace_at_caret, 0);
		}

		if (brace_at_caret != -1 && brace_opposite == -1)
		{
			BraceBadLight(brace_at_caret);
			SetHighlightGuide(0);
		}
		else
		{
			BraceHighlight(brace_at_caret, brace_opposite);
			SetHighlightGuide(std::min(GetColumn(brace_at_caret), GetColumn(brace_opposite)));
		}
	}
	return 0;
}

LRESULT CEditorCtrl::OnZoom(LPNMHDR)
{
	AutoMarginWidth();
	return 0;
}

CEditorCtrl::Range CEditorCtrl::GetSelection()
{
	Range range;
	range.start = GetSelectionStart();
	range.end = GetSelectionEnd();
	return range;
}

Position CEditorCtrl::GetCaretInLine()
{
	return GetCurrentPos() - PositionFromLine(GetCurrentLineNumber());
}

bool CEditorCtrl::Find(bool next)
{
	Position pos = 0;
	const FindOption flags = m_dlg_find_replace->m_flags;
	const string8 text = m_dlg_find_replace->m_find_text;

	if (next)
	{
		CharRight();
		SearchAnchor();
		pos = SearchNext(flags, text);
	}
	else
	{
		if (FlagSet(flags, FindOption::RegExp)) return false;

		SearchAnchor();
		pos = SearchPrev(flags, text);
	}

	if (pos != -1)
	{
		ScrollCaret();
		return true;
	}

	string8 msg;
	msg << "Cannot find \"" << text << "\"";
	::MessageBoxW(m_dlg_find_replace->m_hWnd, to_wide(msg).data(), to_wide(Component::name).data(), MB_SYSTEMMODAL | MB_ICONINFORMATION);
	return false;
}

bool CEditorCtrl::IsWordCharacter(char c)
{
	static const std::string word_characters = "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	return word_characters.contains(c);
}

std::optional<Colour> CEditorCtrl::ParseHex(const std::string& hex)
{
	const auto int_from_hex_digit = [](int ch)
	{
		if (ch >= '0' && ch <= '9')
		{
			return ch - '0';
		}
		else if (ch >= 'A' && ch <= 'F')
		{
			return ch - 'A' + 10;
		}
		else if (ch >= 'a' && ch <= 'f')
		{
			return ch - 'a' + 10;
		}
		return 0;
	};

	if (hex.length() == 7 && hex.at(0) == '#')
	{
		const int r = int_from_hex_digit(hex.at(1)) << 4 | int_from_hex_digit(hex.at(2));
		const int g = int_from_hex_digit(hex.at(3)) << 4 | int_from_hex_digit(hex.at(4));
		const int b = int_from_hex_digit(hex.at(5)) << 4 | int_from_hex_digit(hex.at(6));
		return RGB(r, g, b);
	}
	return std::nullopt;
}

std::string CEditorCtrl::GetContent()
{
	const int len = GetText(0, nullptr);
	std::string text(len, '\0');
	GetText(len, text.data());
	return text;
}

std::string CEditorCtrl::GetCurLineText()
{
	const int len = GetCurLine(0, nullptr);
	std::string text(len, '\0');
	GetCurLine(len, text.data());
	return text;
}

std::string CEditorCtrl::GetLineText(Line line)
{
	const int len = GetLine(line, nullptr);
	std::string text(len, '\0');
	GetLine(line, text.data());
	return text;
}

std::string CEditorCtrl::GetWordStart(const std::string& text, Position current)
{
	m_word_start_pos = current;
	for (const char c : text | std::views::take(current) | std::views::reverse)
	{
		if (IsWordCharacter(c) || c == '.')
			m_word_start_pos--;
		else
			break;
	}
	return text.substr(m_word_start_pos, current - m_word_start_pos);
}

void CEditorCtrl::AutoMarginWidth()
{
	const int line_count = GetLineCount() * 10;
	const int margin_width = TextWidth(STYLE_LINENUMBER, std::to_string(line_count).c_str());
	SetMarginWidthN(0, margin_width);
}

void CEditorCtrl::AutomaticIndentation(int ch)
{
	const Line current_line = GetCurrentLineNumber();
	const Line previous_line = current_line - 1;
	const Position line_start = PositionFromLine(current_line);
	const Position selection_start = GetSelectionStart();
	const int indent_size = GetIndent();
	int indent_block = GetLineIndentation(previous_line);

	if (current_line > 0)
	{
		const std::string text = GetLineText(previous_line);

		for (const char c : text | std::views::reverse)
		{
			if (isspace(c)) continue;
			if (c == '{') indent_block += indent_size;
			break;
		}
	}

	if (ch == '}')
	{
		auto rng = std::views::iota(line_start, selection_start - 1);
		const bool all_whitespace = std::ranges::all_of(rng, [&](Position i) { return isspace(GetCharAt(i)); });
		if (all_whitespace)
		{
			SetIndentation(current_line, indent_block - indent_size);
		}
	}
	else if ((ch == '\r' || ch == '\n') && selection_start == line_start)
	{
		SetIndentation(current_line, indent_block);
	}
}

void CEditorCtrl::ContinueCallTip()
{
	const std::string text = GetCurLineText();
	const Position current = GetCaretInLine();
	const size_t len = m_function_definition.length();
	int braces = 0;
	int commas = 0;

	for (Position i = m_word_start_pos; i < current; ++i)
	{
		if (text[i] == '(')
			braces++;
		else if (text[i] == ')' && braces > 0)
			braces--;
		else if (braces == 1 && text[i] == ',')
			commas++;
	}

	size_t start_highlight = 0;
	while (start_highlight < len && m_function_definition[start_highlight] != '(')
	{
		start_highlight++;
	}
	if (start_highlight < len && m_function_definition[start_highlight] == '(')
		start_highlight++;

	while (start_highlight < len && commas > 0)
	{
		if (m_function_definition[start_highlight] == ',')
			commas--;
		if (m_function_definition[start_highlight] == ')')
			commas = 0;
		else
			start_highlight++;
	}

	if (start_highlight < len && m_function_definition[start_highlight] == ',')
		start_highlight++;

	size_t end_highlight = start_highlight;
	while (end_highlight < len && m_function_definition[end_highlight] != ',' && m_function_definition[end_highlight] != ')')
	{
		end_highlight++;
	}

	CallTipSetHlt(start_highlight, end_highlight);
}

void CEditorCtrl::Init()
{
	SetFnPtr();
	SetTechnology(Technology::DirectWrite);

	SetCodePage(CpUtf8);
	SetEOLMode(EndOfLine::CrLf);
	SetModEventMask(ModificationFlags::InsertText | ModificationFlags::DeleteText | ModificationFlags::Undo | ModificationFlags::Redo);
	UsePopUp(PopUp::Text);

	static constexpr std::array<const int, 21> ctrlcodes = { 'Q', 'W', 'E', 'R', 'I', 'O', 'P', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'B', 'N', 'M', 186, 187, 226 };

	for (const int ctrlcode : ctrlcodes)
	{
		ClearCmdKey(MAKELONG(ctrlcode, KeyMod::Ctrl));
	}

	for (int i = 48; i < 122; ++i)
	{
		ClearCmdKey(MAKELONG(i, KeyMod::Ctrl | KeyMod::Shift));
	}

	AssignCmdKey(MAKELONG(Keys::Next, KeyMod::Ctrl), std::to_underlying(Message::ParaDown));
	AssignCmdKey(MAKELONG(Keys::Prior, KeyMod::Ctrl), std::to_underlying(Message::ParaUp));
	AssignCmdKey(MAKELONG(Keys::Next, (KeyMod::Ctrl | KeyMod::Shift)), std::to_underlying(Message::ParaDownExtend));
	AssignCmdKey(MAKELONG(Keys::Prior, (KeyMod::Ctrl | KeyMod::Shift)), std::to_underlying(Message::ParaUpExtend));
	AssignCmdKey(MAKELONG(Keys::Home, KeyMod::Norm), std::to_underlying(Message::VCHomeWrap));
	AssignCmdKey(MAKELONG(Keys::End, KeyMod::Norm), std::to_underlying(Message::LineEndWrap));
	AssignCmdKey(MAKELONG(Keys::Home, KeyMod::Shift), std::to_underlying(Message::VCHomeWrapExtend));
	AssignCmdKey(MAKELONG(Keys::End, KeyMod::Shift), std::to_underlying(Message::LineEndWrapExtend));

	SetUseTabs(true);
	SetTabIndents(true);
	SetBackSpaceUnIndents(true);
	SetTabWidth(4);
	SetIndent(4);

	SetEndAtLastLine(true);
	SetScrollWidthTracking(true);
	SetScrollWidth(1);

	AutoCSetIgnoreCase(true);

	static constexpr const char* js_keywords =
		"break case catch continue default delete do else false for function if in "
		"instanceof new null return switch this throw true try typeof var while "
		"Array Date Error Function JSON Math Number Object RegExp String "
		"console fb gdi plman utils window";

	SetILexer(CreateLexer("cpp"));
	SetKeyWords(0, js_keywords);
	ReadAPIs();

	SetMarginWidthN(1, 0);
	SetMarginWidthN(2, 0);
	SetMarginWidthN(3, 0);
	SetMarginWidthN(4, 0);
	SetMarginTypeN(0, MarginType::Number);

	ClearDocumentStyle();
	StyleResetDefault();

	for (const auto& [name, value] : g_config.m_data)
	{
		SetStyle(name, value);
	}
}

void CEditorCtrl::OpenFindDialog()
{
	if (!m_dlg_find_replace) m_dlg_find_replace = fb2k::newDialogEx<CDialogFindReplace>(m_hWnd, this);
	m_dlg_find_replace->SetMode(CDialogFindReplace::Mode::find);
}

void CEditorCtrl::OpenGotoDialog()
{
	modal_dialog_scope scope(m_hWnd);
	CDialogGoto dlg(GetCurrentLineNumber() + 1);
	if (dlg.DoModal(m_hWnd) == IDOK)
	{
		const Line line = dlg.m_line_number - 1;
		GotoLine(line);
	}
}

void CEditorCtrl::OpenReplaceDialog()
{
	if (!m_dlg_find_replace) m_dlg_find_replace = fb2k::newDialogEx<CDialogFindReplace>(m_hWnd, this);
	m_dlg_find_replace->SetMode(CDialogFindReplace::Mode::replace);
}

void CEditorCtrl::ReadAPIs()
{
	const std::string content = Component::get_resource_text(IDR_API);

	for (const std::string& text : split_string(content, CRLF))
	{
		if (text.empty()) continue;
		API item = { text, std::min({text.find('('), text.find(' '), text.length()}) };
		m_apis.emplace_back(item);
	}

	std::ranges::sort(m_apis, [](const API& a, const API& b) -> bool
		{
			return _stricmp(a.text.c_str(), b.text.c_str()) < 0;
		});
}

void CEditorCtrl::Replace()
{
	const Range range = GetSelection();
	SetTargetStart(range.start);
	SetTargetEnd(range.end);
	const string8 replace = m_dlg_find_replace->m_replace_text;
	ReplaceTarget(replace.get_length(), replace);
	SetSel(range.start + replace.get_length(), range.end);
}

void CEditorCtrl::ReplaceAll()
{
	BeginUndoAction();
	SetTargetStart(0);
	SetTargetEnd(0);
	SetSearchFlags(m_dlg_find_replace->m_flags);
	const string8 find = m_dlg_find_replace->m_find_text;
	const string8 replace = m_dlg_find_replace->m_replace_text;

	while (true)
	{
		SetTargetStart(GetTargetEnd());
		SetTargetEnd(GetLength());

		const Position occurrence = SearchInTarget(find.get_length(), find);

		if (occurrence == -1)
		{
			MessageBeep(MB_ICONINFORMATION);
			break;
		}

		ReplaceTarget(replace.get_length(), replace);
		SetSel(occurrence + replace.get_length(), occurrence);
	}

	EndUndoAction();
}

void CEditorCtrl::SetContent(jstring text)
{
	SetText(text);
	ConvertEOLs(EndOfLine::CrLf);
	GrabFocus();
	TrackWidth();
}

void CEditorCtrl::SetIndentation(Line line, int indent)
{
	if (indent < 0)
		return;

	Range range = GetSelection();
	const Position pos_before = GetLineIndentPosition(line);
	SetLineIndentation(line, indent);
	const Position pos_after = GetLineIndentPosition(line);
	const Position pos_difference = pos_after - pos_before;
	if (pos_after > pos_before)
	{
		if (range.start >= pos_before)
		{
			range.start += pos_difference;
		}

		if (range.end >= pos_before)
		{
			range.end += pos_difference;
		}
	}
	else if (pos_after < pos_before)
	{
		if (range.start >= pos_after)
		{
			if (range.start >= pos_before)
				range.start += pos_difference;
			else
				range.start = pos_after;
		}

		if (range.end >= pos_after)
		{
			if (range.end >= pos_before)
				range.end += pos_difference;
			else
				range.end = pos_after;
		}
	}

	SetSel(range.start, range.end);
}

void CEditorCtrl::SetStyle(const std::string& name, const std::string& value)
{
	if (value.empty()) return;

	const auto it = style_map.find(name);
	if (it != style_map.end())
	{
		EditorStyle style;

		for (const std::string& str : split_string(value, ","))
		{
			Strings tmp = split_string(str, ":");
			const std::string primary = tmp[0];
			const std::string secondary = tmp.size() == 2 ? tmp[1] : "";

			if (primary == "font") style.font = secondary;
			else if (primary == "size" && pfc::string_is_numeric(secondary.c_str())) style.size = std::stoi(secondary);
			else if (primary == "fore") style.fore = ParseHex(secondary);
			else if (primary == "back") style.back = ParseHex(secondary);
			else if (primary == "bold") style.bold = true;
			else if (primary == "italics") style.italic = true;
		}

		for (const int id : it->second)
		{
			if (style.font.length()) StyleSetFont(id, style.font.c_str());
			if (style.size > 0) StyleSetSize(id, style.size);
			if (style.fore) StyleSetFore(id, style.fore.value());
			if (style.back) StyleSetBack(id, style.back.value());
			if (style.bold) StyleSetBold(id, style.bold);
			if (style.italic) StyleSetItalic(id, style.italic);
			StyleSetCheckMonospaced(id, true);
			if (id == STYLE_DEFAULT) StyleClearAll();
		}
	}
	else
	{
		auto tmp = ParseHex(value);
		if (tmp)
		{
			const ColourAlpha colour = 0xff000000 | tmp.value();
			if (name == "style.caret.fore")
			{
				SetElementColour(Element::Caret, colour);
			}
			else if (name == "style.selection.back")
			{
				SetSelectionLayer(Layer::UnderText);
				SetElementColour(Element::SelectionBack, colour);
				SetElementColour(Element::SelectionAdditionalBack, colour);
				SetElementColour(Element::SelectionInactiveBack, colour);
				SetElementColour(Element::SelectionSecondaryBack, colour);
			}
		}
	}
}

void CEditorCtrl::StartAutoComplete()
{
	const Position current = GetCaretInLine();
	const std::string text = GetCurLineText();
	const std::string word_start = GetWordStart(text, current);
	std::string words;

	auto filter = [word_start](const API& item) { return item.text.starts_with(word_start); };
	auto transform = [](const API& item) { return item.text.substr(0, item.len); };

	for (const std::string& word : m_apis | std::views::filter(filter) | std::views::transform(transform))
	{
		if (words.length()) words.append(" ");
		words.append(word);
	}

	if (words.length())
	{
		AutoCShow(word_start.length(), words.c_str());
	}
}

void CEditorCtrl::StartCallTip()
{
	static constexpr int min_length = 4; // Date is the shortest word before opening brace
	const Position current = GetCaretInLine() - 1;
	if (current < min_length) return;

	const std::string text = GetCurLineText();
	const std::string current_calltip_word = GetWordStart(text, current);
	const size_t len = current_calltip_word.length();
	if (len < min_length) return;

	const auto it = std::ranges::find_if(m_apis, [current_calltip_word, len](const API& item) { return item.text.length() > len && current_calltip_word == item.text.substr(0, item.len); });
	if (it != m_apis.end())
	{
		m_function_definition = it->text;
		CallTipShow(GetCurrentPos() - len, m_function_definition.c_str());
		ContinueCallTip();
	}
}

void CEditorCtrl::TrackWidth()
{
	const int count = GetLineCount();
	int max_width = 1;

	for (const int i : std::views::iota(0, count))
	{
		const Position pos = GetLineEndPosition(i);
		const int width = PointXFromPosition(pos);

		if (width > max_width)
			max_width = width;
	}

	SetScrollWidth(max_width);
}
