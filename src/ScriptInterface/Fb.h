#pragma once

class Fb : public JSDispatchImpl<IFb>
{
public:
	STDMETHODIMP AcquireUiSelectionHolder(ISelectionHolder** out) override;
	STDMETHODIMP AddDirectory() override;
	STDMETHODIMP AddFiles() override;
	STDMETHODIMP AddLocationsAsync(UINT window_id, VARIANT locations, UINT* out) override;
	STDMETHODIMP CheckClipboardContents(UINT /* FFS */, VARIANT_BOOL* out) override;
	STDMETHODIMP ClearPlaylist() override;
	STDMETHODIMP CopyHandleListToClipboard(IMetadbHandleList* handles, VARIANT_BOOL* out) override;
	STDMETHODIMP CreateContextMenuManager(IContextMenuManager** out) override;
	STDMETHODIMP CreateHandleList(VARIANT handle, IMetadbHandleList** out) override;
	STDMETHODIMP CreateMainMenuManager(IMainMenuManager** out) override;
	STDMETHODIMP CreateProfiler(BSTR name, IProfiler** out) override;
	STDMETHODIMP DoDragDrop(IMetadbHandleList* handles, UINT okEffects, UINT* out) override;
	STDMETHODIMP Exit() override;
	STDMETHODIMP GetClipboardContents(UINT /* FFS */, IMetadbHandleList** out) override;
	STDMETHODIMP GetDSPPresets(BSTR* out) override;
	STDMETHODIMP GetFocusItem(IMetadbHandle** out) override;
	STDMETHODIMP GetLibraryItems(IMetadbHandleList** out) override;
	STDMETHODIMP GetNowPlaying(IMetadbHandle** out) override;
	STDMETHODIMP GetOutputDevices(BSTR* out) override;
	STDMETHODIMP GetQueryItems(IMetadbHandleList* handles, BSTR query, IMetadbHandleList** out) override;
	STDMETHODIMP GetSelection(IMetadbHandle** out) override;
	STDMETHODIMP GetSelections(UINT flags, IMetadbHandleList** out) override;
	STDMETHODIMP GetSelectionType(UINT* out) override;
	STDMETHODIMP IsLibraryEnabled(VARIANT_BOOL* out) override;
	STDMETHODIMP IsMetadbInMediaLibrary(IMetadbHandle* handle, VARIANT_BOOL* out) override;
	STDMETHODIMP LoadPlaylist() override;
	STDMETHODIMP Next() override;
	STDMETHODIMP Pause() override;
	STDMETHODIMP Play() override;
	STDMETHODIMP PlayOrPause() override;
	STDMETHODIMP Prev() override;
	STDMETHODIMP Random() override;
	STDMETHODIMP RunContextCommand(BSTR command, VARIANT_BOOL* out) override;
	STDMETHODIMP RunContextCommandWithMetadb(BSTR command, VARIANT handle_or_handles, VARIANT_BOOL* out) override;
	STDMETHODIMP RunMainMenuCommand(BSTR command, VARIANT_BOOL* out) override;
	STDMETHODIMP SavePlaylist() override;
	STDMETHODIMP SetDSPPreset(UINT idx) override;
	STDMETHODIMP SetOutputDevice(BSTR output, BSTR device) override;
	STDMETHODIMP ShowConsole() override;
	STDMETHODIMP ShowLibrarySearchUI(BSTR query) override;
	STDMETHODIMP ShowPopupMessage(BSTR msg, BSTR title) override;
	STDMETHODIMP ShowPreferences() override;
	STDMETHODIMP Stop() override;
	STDMETHODIMP TitleFormat(BSTR pattern, ITitleFormat** out) override;
	STDMETHODIMP VolumeDown() override;
	STDMETHODIMP VolumeMute() override;
	STDMETHODIMP VolumeUp() override;
	STDMETHODIMP get_AlwaysOnTop(VARIANT_BOOL* out) override;
	STDMETHODIMP get_ComponentPath(BSTR* out) override;
	STDMETHODIMP get_CursorFollowPlayback(VARIANT_BOOL* out) override;
	STDMETHODIMP get_FoobarPath(BSTR* out) override;
	STDMETHODIMP get_IsPaused(VARIANT_BOOL* out) override;
	STDMETHODIMP get_IsPlaying(VARIANT_BOOL* out) override;
	STDMETHODIMP get_PlaybackFollowCursor(VARIANT_BOOL* out) override;
	STDMETHODIMP get_PlaybackLength(double* out) override;
	STDMETHODIMP get_PlaybackTime(double* out) override;
	STDMETHODIMP get_ProfilePath(BSTR* out) override;
	STDMETHODIMP get_ReplaygainMode(UINT* out) override;
	STDMETHODIMP get_StopAfterCurrent(VARIANT_BOOL* out) override;
	STDMETHODIMP get_Volume(float* out) override;
	STDMETHODIMP put_AlwaysOnTop(VARIANT_BOOL b) override;
	STDMETHODIMP put_CursorFollowPlayback(VARIANT_BOOL b) override;
	STDMETHODIMP put_PlaybackFollowCursor(VARIANT_BOOL b) override;
	STDMETHODIMP put_PlaybackTime(double time) override;
	STDMETHODIMP put_ReplaygainMode(UINT mode) override;
	STDMETHODIMP put_StopAfterCurrent(VARIANT_BOOL b) override;
	STDMETHODIMP put_Volume(float value) override;

private:
	int m_task_id = 0;
};
