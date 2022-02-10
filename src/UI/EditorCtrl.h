#pragma once
#include <ScintillaImpl.h>

class CDialogFindReplace;

class CEditorCtrl : public CScintillaImpl<CEditorCtrl>
{
public:
	BEGIN_MSG_MAP_EX(CEditorCtrl)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		REFLECTED_NOTIFY_CODE_HANDLER_EX(std::to_underlying(Notification::UpdateUI), OnUpdateUI)
		REFLECTED_NOTIFY_CODE_HANDLER_EX(std::to_underlying(Notification::CharAdded), OnCharAdded)
		REFLECTED_NOTIFY_CODE_HANDLER_EX(std::to_underlying(Notification::Zoom), OnZoom)
		REFLECTED_COMMAND_CODE_HANDLER_EX(std::to_underlying(FocusChange::Change), OnChange)
	END_MSG_MAP()

	bool Find(bool next);
	std::string GetContent();
	void Init();
	void Replace();
	void ReplaceAll();
	void SetContent(jstring text);

private:
	struct API
	{
		std::string text;
		size_t len = 0;
	};

	struct EditorStyle
	{
		bool bold = false, italic = false;
		int size = 0;
		std::optional<Colour> back{}, fore{};
		std::string font;
	};

	struct Range
	{
		Position start = 0;
		Position end = 0;
	};

	Line GetCurrentLineNumber();
	LRESULT OnChange(UINT, int, CWindow);
	LRESULT OnCharAdded(LPNMHDR);
	LRESULT OnKeyDown(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnUpdateUI(LPNMHDR);
	LRESULT OnZoom(LPNMHDR);
	Position GetCaretInLine();
	Range GetSelection();
	bool IsWordCharacter(char c);
	std::optional<Colour> ParseHex(const std::string& hex);
	std::string GetCurLineText();
	std::string GetLineText(Line line);
	std::string GetWordStart(const std::string& text, Position current);
	void AutoMarginWidth();
	void AutomaticIndentation(int ch);
	void ContinueCallTip();
	void OpenFindDialog();
	void OpenGotoDialog();
	void OpenReplaceDialog();
	void ReadAPIs();
	void SetIndentation(Line line, int indent);
	void SetStyle(const std::string& name, const std::string& value);
	void StartAutoComplete();
	void StartCallTip();
	void TrackWidth();

	CDialogFindReplace* m_dlg_find_replace = nullptr;
	Position m_word_start_pos = 0;
	int m_brace_count = 0;
	std::string m_function_definition;
	std::vector<API> m_apis;
};
