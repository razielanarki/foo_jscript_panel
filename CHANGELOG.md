## v2.8.6
- Font handling fixes.
- Various `JS Smooth Browser` fixes.

## v2.8.5.1
- Fix a `JS Smooth Playlist` script error when the `Delete` key was used to the remove the last item(s) in a playlist.

## v2.8.5
- Hopefully fix memory leak in various methods that return a `VBArray` containing strings.

## v2.8.4.1
- Fix `JS Smooth Playlist / JS Smooth Playlist Manager` background wallpaper not updating when displaying art from radio streams.

## v2.8.4
- Fix bug introduced in `v2.8.2` which caused the `IMetadbHandleList` `AttachImage / RemoveAttachedImage / RemoveAttachedImages` methods to silently fail.

## v2.8.3
- Hopefully fix timer related crashes.
- Fix `JS Smooth` custom text colour not sticking.

## v2.8.2
- Various sample fixes.

## v2.8.1.2
- Add `properties + other info` sample which makes use of the `GetOtherInfo` method added in `v2.8.1.1`.
- The original `properties` sample no longer displays `ReplayGain` or `Playback Statistics` info. Use the new addition above if you want to display them.

## v2.8.1.1
- Add `IMetadbHandleList` `GetOtherInfo` method. This returns a `JSON` object which is in string form so you need to use `JSON.parse` on the result. This dumps all the information that is viewable on the `Details` tab of the `Properties` dialog. Content is divided in to sections such as `General`, `Location` etc. Other sections like `ReplayGain` and database fields from 3rd party components will be provided here if present.
- The `Track Info + Seekbar` samples have have been tweaked to allow more width for artist/title text. Also, the colour of the stop button automatically changes to reflect `Stop After Current` status. Existing users will need to reload the script from the [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window) samples menu.

## v2.8.0.1
- Fix `JS Playlist` item selection bug when the `Cover` column was enabled.
- Fix issue with `JS Playlist / JS Smooth` scrollbar sizes if DPI was increased.

## v2.8.0
- Add `utils.GetRequestAsync` and `utils.PostRequestAsync`. These are for fetching plain text from a webserver and the result is sent to a new `on_http_request_done` callback.
- Add `utils.DownloadFileAsync` and `on_download_file_done` callback. Use this for downloading images or other binary files.
- The `last.fm lover` sample has been removed.
- The `IPlaybackQueueItem` interface has been removed. Also, the following methods have also been removed.

```js
plman.AddItemToPlaybackQueue
plman.FindPlaybackQueueItemIndex
plman.GetPlaybackQueueContents
```

  Full management of the playback queue and display of queue indexes in scripted playlists is still possible. See the following page for full details.

  https://github.com/marc2k3/foo_jscript_panel/wiki/Breaking-Changes

- `JS Playlist` no longer shows a light coloured placeholder image as it could flash briefly on dark layouts waiting for the requested album art to load. Configure a stub image in the main `foobar2000` album art preferences if this leaves a blank space where an image used to be.
- Fix bug with `JS Smooth Playlist Manager` where the selected playlist (as opposed to active playlist) was not highlighted.
- Refactor internal timer handling and management of multiple threads for async methods.

## v2.7.5.1
- Attempt to fix some weird `JS Playlist / JS Smooth Playlist` initialisation/refresh issues when contained within `Panel Stack Splitter`, specifically when `Forced layout` is enabled.
- `JS Smooth Browser` can now show `Media Library` contents only. The playlist option has been removed.

## v2.7.5
- Add `plman.InvertSelection`. The included `JSPlaylist` and `JS Smooth Playlist` scripts have this option added to their context menus.
- The `Spectrogram Seekbar` samples now support files encoded with `exhale` if you use a special build of `ffmpeg` with `libfdk-aac` support. See here:

  https://hydrogenaud.io/index.php?topic=118888.msg1006624#msg1006624

  The script should auto-detect these files and update the command line it passes automatically. The only user action necessary is replacing the existing `ffmpeg` executable.
- `IMetadbHandle` `GetAlbumArt` may return the url of remote images when playing certain radio streams. Requires `foobar2000` `v1.6.6` or later. The included `Album Art` sample has been updated and tested with `radioparadise`.

## v2.7.4
- The `Spectrogram Seekbar` samples no longer require `SoX` and use `ffmpeg` exclusively. 

  For existing users using default settings, the difference should not be noticeable. However, if changes were made to the `SoX` options, these are no longer supported and you can use the `ffmpeg` options documented here instead:

  https://ffmpeg.org/ffmpeg-filters.html#showspectrumpic

  All previously cached images are now obsolete and can be removed using the relevant menu item when right clicking the panel.

- Fix `JS Smooth Playlist` bug where info bar didn't update when the active playlist was renamed.
- Remove `preprocessors` and `notes + hints` from the `Docs` folder. The content is now on the [wiki](https://github.com/marc2k3/foo_jscript_panel/wiki).
- Add `GitHub` menu to the [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window).
- More autocomplete fixes.

## v2.7.3.2
- Fix `JS Smooth Playlist` error when re-ordering playlists.

## v2.7.3.1
- Fix minor autocomplete regressions in `v2.7.3`.
- `JS Smooth Playlist` and `JS Smooth Playlist Manager` now have support for custom background images in addition to the previous front cover support. `JS Smooth Browser` no longer supports background images of any kind.

## v2.7.3
- Minor keyword highlighting/autocomplete improvements in the [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window).
- `Find`/`Replace` dialog fixes.

## v2.7.2.2
- The `thumbs` sample now supports multiple folders when in custom folder mode. Just separate each one with a `|`.
- Fix bug with `Text Display` sample where it used `utils.ReadUTF8` internally meaning `ANSI` and `UCS2-LE` files were not handled properly. It now uses `utils.ReadTextFile` which supports them.

## v2.7.2.1
- `gdi.Font` now throws an error if the supplied size is zero.
- Fix `JSPlaylist` drag/drop bugs caused by a change in `v2.7.1`.

## v2.7.2
- Add `IThemeManager` `GetThemeColour`. See docs and the `basic\SimpleThemedButton` sample which now works properly with the `Windows` high contrast themes instead of having a fixed text colour which could clash with the background.
- The `spectrogram seekbar` and `track info + spectrogram seekbar + buttons` samples now display the cached spectrogram image for the selected track when playback is stopped. Existing users will need to update the scripts in their panels from the [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window) `Samples` menu.

## v2.7.1
- `IMetadbHandle` `GetFileInfo` now takes an optional `full_info` argument which defaults to `false` if omitted to preserve original behaviour. If `true` the file is opened for reading and you can access tags that contain large chunks of text such as `LYRICS` which usually have their contents hidden behind the period character ever since `foobar2000` `v1.3` was released. When `full_info` is `true`, the return value must be checked before using. See docs for full details.
- Because of the above change, the `Text Reader` sample has been renamed to `Text Display` because it now can display the content of tags in full in addition to the previous text file read mode it had before. Use the right click menu to switch modes, customise the file path or tag etc.
- Restore `Last.fm lover` sample which was removed without notice in `v2.7.0`. Apologies for any inconvenience.
- Fix various `JSPlaylist` bugs.

## v2.7.0.1
- Fix minor issue with `JSPlaylist` `RATING` display.
- Fix typo which may have distorted the background wallpaper in `JSPlaylist` with non square images.
- Fix minor issues with the `Properties` sample.

## v2.7.0

### Component changes

- The `GdiRawBitmap` interface and associated `gr.GdiAlphaBlend` and `gr.GdiDrawBitmap` methods have been removed.
- The `MeasureStringInfo` interface and associated `gr.MeasureString` method have been removed.
- Add `plman.MovePlaylistSelectionV2`. While updating `JSPlaylist` with improved drag/drop support, I discovered the existing `plman.MovePlaylistSelection` method inherited from `WSH Panel mod` wasn't really suited for the task because it couldn't handle non-contiguous selections without hacky workarounds. This updated method just requires a new position index and does the rest automatically.
- Add `ITitleFormat` `EvalActivePlaylistItem`. This provides easy access for end users to playlist specific fields such as `%list_index%`, `%list_total%`, `%isplaying%` etc. Full details [here](https://wiki.hydrogenaud.io/index.php?title=Foobar2000:Title_Formatting_Reference#Playlist-only_fields).
- Update `plman.CreatePlaylist` so the `playlistIndex` and `name` are both optional. See docs.
- More `plman` methods now check the `playlistIndex` and may throw errors if it's invalid. Many existing methods already did so this just adds a bit more consistency. Non destructive methods that only retrieve info are unaffected.
- Methods that that take an `Album Art ID` now verify it and throw errors if out of bounds. Omitting it is still fine as the default of `0` is assumed for the `Front Cover`.
- The handling of front cover images from radio streams has been improved so they always take precedence over any patterns set in the main `Preferences`. This aligns with how the `Default UI` and `Columns UI` artwork panels behave.

### Sample changes

- The internal file structure for all samples has changed meaning existing users will see blank panels or script errors on their first run. The scripts inside the panels must be updated from the [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window) `Samples` menu.
- All included samples have been updated to use [FontAwesome](https://github.com/FortAwesome/Font-Awesome/blob/fa-4/fonts/fontawesome-webfont.ttf?raw=true). `wingdings2/3` and `GuifxV2 Transports` are no longer used.

- `Complete` sample changes
  * Although reloading the `Samples` from the menu is easy for un-modified scripts, if you have put time and effort in to customising what's in your panel, you'll need to update them to be compatible with the new file structure and renamed methods from the helper scripts. See the [Breaking Changes](https://github.com/marc2k3/foo_jscript_panel/wiki/Breaking-Changes) page for details.
  * Update `track info + seekbar` samples so that if you click the album art thumbnail, the popup album art viewer built in to `foobar2000` `v1.6.2` and newer is opened.
  * Fix bug in `Properties` sample where line breaks in tech info fields were not handled correctly. It was never an issue in normal files but 3rd party input components like `foo_youtube` can add them.

- `JSPlaylist` changes
  * Best efforts have been made to retain previous settings but some may need to be set again via the settings or right click menu.
  * `JSPlaylist` properly responds to font size changes made in the main `Default UI` / `Columns UI` preferences. This fixes a regression made in `Beta.1`. Support for zoom using the mouse wheel has been removed.
  * Drag/drop support has been improved so you can drag out on to other panels and the playlist auto-scrolls if you drag near the top/bottom of the panel.
  * Previously, there were text replacement hacks that allowed the display of `%list_index%`, `%list_total%` and `%isplaying%`. Now the component has full support for all specific playlist fields detailed above, these should all be supported in any custom columns.
  * The `historic` and `Media Library` playlist options have been removed.
  * The built in playback stats was horribly broken so the option has been removed.
  * The group filter/sort options based on playlist name has been removed.
  * The custom colour widgets have been removed from the `Appearance tab` in the `Panel Settings`. Colours are now set via the right click menu.
  * Improved compatibility with default UI colour themes. It's not perfect but it's better than it was.
  * You can now access the `Panel Settings` and `Paste` items when right clicking empty playlists or blank areas.
  * If the background wallpaper is enabled and set to `Album` mode, images from radio streams are displayed if using `foobar2000` `v1.6.6` or later.
  * It's now possible to love/unlove tracks on `Last.fm` directly from clicking on the hearts in the `MOOD` column. This feature can be enabled in the `Properties` window. Hold `Shift` + `Winkey` before right clicking. The option is named `Love tracks with foo_lastfm_playcount_sync`.

    Prerequisites:

    You must have [foo_lastfm_playcount_sync](https://github.com/marc2k3/foo_jscript_panel/wiki/Other-Components) installed, configured and authenticated with your `Last.fm` account.

    You must have imported your `Last.fm` loved tracks via the main `Library` menu because it works as a toggle for the current value.

    You must manually configure the `MOOD` column in the settings to display `%lfm_loved%`. Ensure this is working as expected before clicking anything!

    Usage:

    Now when you click the heart, it loves/unloves tracks directly on `Last.fm`. Only a succesful response from `Last.fm` will make the icon update. Check the `foobar2000` `Console` for messages if not working as expected.

  * Massive internal code cleanup/removal.

- `JS Smooth` changes
  * All previous settings have been reset because the internal names have changed. All options are now set via right clicking the info bar or blank areas with no content.
  * Fix various `JS Smooth Browser` display glitches such as the word `undefined` appearing when tracks have no `Album` tags.
  * Fix various `JS Smooth Browser` image alignment issues.
  * The `JS Smooth Playlist` info bar now displays a track count and total duration. If wallpaper is enabled, double clicking the info bar will open the image in the new popup Album Art viewer built in to `foobar2000` `v1.6.2` and later.
  * The `JS Smooth Playlist` group header height is now configurable via the right click menu and the display for when the row height is set to 1 has been fixed.
  * Updated the image cache because previously it was invalidated far too often meaning cached images were read from disk over and over again. This may result in increased memory usage so please report any issues if you have them.
  * If the background wallpaper is enabled in `JS Smooth Playlist`, images from radio streams are displayed if using `foobar2000` `v1.6.6` or later.
  * Improved compatibility with default UI colour themes. It's not perfect but it's better than it was.
  * You can now right click and paste items in `JS Smooth Playlist` when the playlist is empty.
  * You can now browse `JS Smooth Browser` by `Album Artist` in additon to the previous `Album` and `Artist` modes.
  * The `JS Smooth Browser` filter box has been removed when browsing by playlist as it wasn't functional.
  * The incremental search feature has been removed. It never worked on playlists and the `JS Smooth Browser` library mode filter box has proper `Media Library` query support (added in previous release).
  * Massive internal code cleanup/removal.

## v2.6.2.3
- The `JS Smooth Browser` filter box now supports proper `Media Library` queries.
- The filter box has been removed from `JS Smooth Playlist` as it was functionally useless. It's not possible to select/play from a filtered playlist without modifying playlist content which is beyond the scope of what any playlist viewer should do.
- `JS Smooth Playlist Manager` now has some new features added such as the ability to restore/purge deleted playlists, edit/remove playlist locks that belong to `JScript Panel` and playlists can be sorted by name. All options are available on the right click menu although may be hidden if the filter is active.

## v2.6.2.2
- Add `spectrogram seekbar` & `track info + spectrogram seekbar + buttons` samples.
- `JS Smooth Playlist` now displays dynamic artist/title info from radio streams.
- `JS Smooth Browser/Playlist` now correctly caches files as `jpg`. Previously they were saved as `png` with a `jpg` extension. Existing users may consider deleting their existing `js_smooth_cache` folder.

## v2.6.2.1
- Fix drag/drop bug introduced when fixing callback bug in `v2.6.2`.

## v2.6.2
- Fix bug introduced in `v2.6.0.2` where callbacks that receive multiple args may not have received them in the correct order causing buggy behaviour in certain scripts.
- Fix various `JS Smooth` script issues.

## v2.6.1
- `window.GetFontCUI` and `window.GetFontDUI` have been updated so they never return null on failure. `Segoe UI` will be used instead.
- `gdi.Font` has also been updated so if it fails, the fallback font is `Segoe UI`. Previous fallbacks were not always predictable and there could be mismatches between the `.Name` property and what was actually used. In theory, that should not happen now.
- Add `utils.ListFonts`.

## v2.6.0.2
- Fixes a regression in `v2.6.0` where selected text in the [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window) may have become hard to read when the window lost focus.
- Because of the above change, colour/font changes have been reset. Also, the `Editor Properties` found under `File>Preferences>Tools>JScript Panel` have been simplified.
- Doc fixes.

## v2.6.0.1
- Fix regression in `v2.6.0` where the [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window) line numbers were not shown.

## v2.6.0
- Add `fb.AddLocationsAsync`. Similar to `plman.AddLocations` except rather than specifiying a target playlist, you get the processed handles to a new `on_locations_added` callback.
- Add `plman.AddPlaylistLock` / `plman.RemovePlaylistLock`. Use in conjunction with the existing `plman.IsPlaylistLocked`, `plman.GetPlaylistLockFilterMask` and `plman.GetPlaylistLockName` methods. The `on_playlists_changed` callback will be triggered when locks are added/removed. See docs for full details.
- Add `plman.ShowPlaylistLockUI`. This only works if the playlist is unlocked or the lock is owned by `JScript Panel`. Provides a popup dialog with checkboxes for toggling the various options on/off.
- Update the playlist manager built in to `JSPlaylist` so it can manage playlist locks from the context menu. Additionally, full support for restoring/purging deleted playlists has been added using the `plman` `recycler` methods. Although this functionality has been in the component for a very long time, it's not been exposed in any included sample before.
- The `JScript Panel` stats section on the `Properties` dialog, `Details` tab is now hidden unless there is actual data to display.

## v2.5.6
- Fix crash with `utils.GetAlbumArtAsync` when it's asked to query art from radio streams. Update docs to clarify only `IMetadbHandle` `GetAlbumArt` and `utils.GetAlbumArtV2` can retrieve that type of art.
- Fix `track info + seekbar + buttons` sample to properly update on stream title/art changes. Note this requires replacing the text in any existing panel by using the `Samples` menu.

## v2.5.5.2
- Fix double click action in `Album Art` sample.

## v2.5.5.1
- The `on_playback_dynamic_info_track` callback now receives a `type` as the first argument. It will have a value of `0` for stream title updates and a value of `1` for stream album art updates.

## v2.5.5
- The included `Album art` sample now displays album art (front cover only) from radio streams. Requires `foobar2000` `v1.6.6` or above. Tested with [RadioParadise](https://radioparadise.com/listen/stream-links) as per this thread: https://hydrogenaud.io/index.php?topic=120855.0

## v2.5.4
- Fix bug where the `cookie` returned by `gdi.LoadImageAsync` wasn't always unique.
- A recent website update broke the `Allmusic` sample. This is now fixed.

## v2.5.3
- Add `IMetadbHandle` `ShowAlbumArtViewer` which uses the new internal viewer added in `foobar2000` `v1.6.2`. You will see a popup text message if you try and use it on earlier versions. Check the docs for options.
- The included `Album art` sample now has 3 choices available for the `double click` action. Use the right click menu to choose.
  * Open using external viewer
  * Open using new internal viewer mentioned above
  * Open containing folder

## v2.5.2
- Add `IMetadbHandleList` `SaveAs` method which saves using the native `foobar2000` `fpl` format. The docs also give an example of how to save an `m3u8` playlist using `utils.WriteTextFile`.
- `IUiSelectionHolder` `SetSelection` now takes an optional `type` argument.
- Fix `utils.ReadUTF8` so it now strips the `BOM` if present. `utils.ReadTextFile` already did this. Although its presence didn't affect display, it could cause `JSON.parse` to fail. It never affected included samples because files are always written without `BOM`.

## v2.5.1
- Add `IMetadbHandleList` `Randomise`.
- Add `IMetadbHandleList` `RemoveDuplicatesByFormat`.
- Update `JS Smooth Browser` / `JS Smooth Playlist`.
  * The context menu no longer has a `Settings` option. Right click the `Album count` or `Playlist name` in the top right corner to access the same settings.
  * The disk cache is always enabled and now supports embedded art. Start up should be faster once the cache has been generated.
  * `JS Smooth Browser` has had the browse by `Genre` option removed.

## v2.5.0.2
- Fix editor bug that caused crashes if `foo_wave_seekbar` was installed.

## v2.5.0.1
- Fix `utils.Glob` bug introduced in `v2.4.3.1` where each item in the returned array had no separator between the filename and parent folder.

## v2.5.0
- The internal mechanism for saving per panel settings has changed. All scripts/settings from previous versions will be kept as you upgrade but the changes mean you can no longer downgrade to earlier versions without losing scripts embedded in each panel. Exporting your current layout before upgrading is highly recommended. Then if the worst happens, you can always re-import your theme with an earlier version.
- The `Album Art` sample now has a menu option to customise the double click action. It can either open the image in the default viewer as before or it can open the containing folder.
- Add `utils.ReplaceIllegalChars(str[, modern])`. If `modern` is set to `true`, characters are replaced with similar loooking unicode characters that are valid for use in file paths. If `false`, legacy character replacements are used. The behaviour should match the same advanced `Preferences` that are available for `File Operations` and the `Converter` built-in to `foobar2000` `v1.6` and later.
- Add `IMetadbHandleList` `RemoveDuplicates` method. The original order of the handle list is preserved.
- Remove `IMetadbHandleList` `MakeUnion` and `Sort` methods. `MakeDifference`, `MakeIntersection` and `BSearch` now perform the required sorting internally so any calls to `Sort` before using them must be removed. More details can be found on the `Breaking Changes` wiki page.
- (Beta.3) Fix bug in `IMetadbHandleList` `BSearch` introduced in `Beta.1`.
- (Beta.4) Properly fix `IMetadbHandleList` `BSearch`. The attempted fix in `Beta.3` was incorrect because it made a copy of the handle list internally which meant you couldn't remove the handle by the id of the result. Now the original order is not preserved which matches the behaviour of older versions when you had to manually `Sort()` first.

  https://github.com/marc2k3/foo_jscript_panel/wiki/Breaking-Changes

## v2.4.3.1
- Fix bug introduced in `v.2.4.3` which caused a crash on startup when `foo_whatsnew` is present. Apologies for any inconvenience.
- The `Preferences` for the `Editor Properties` have been reset to default. If you used one of the built in themes, you'll need to select it again. Any manual changes will need to be re-applied. `Properties` that used placeholders like `$(style.default)` are no longer supported so you'd have to duplicate those values manually.

## v2.4.3
- Add `window.SetTooltipFont`. Since it's not permitted to call `window.CreateTooltip` more than once in a panel, this method has been added for changing the tooltip font without reloading the panel.

## v2.4.2
- Internal changes only, no new features.

## v2.4.1.2
- Fix `Thumbs` sample so [Last.fm](https://last.fm) artist art downloads work again.

## v2.4.1.1
- `utils.ListFolders` now has a `recursive` mode. Like `utils.ListFiles`, the default is `false`.

## v2.4.1
- Now requires `foobar2000` `v1.5` or later.
- All album art methods and `gdi.Image` / `gdi.LoadImageAsync` now have full `WebP` support.
- Add `plman.RemovePlaylists`. Accepts an array like `[1,3,4]`.
- Update docs to make it clear that only a single call to `window.CreateTooltip` is permitted per panel instance. Errors will now be thrown on any subsequent calls. Additionally, the `ITooltip` interface no longer has a `Dispose` method.
- `%fb2k_path%` is no longer expanded when used in the `PREPROCESSOR`. Use `%fb2k_profile_path%`.
- The default editor font is now `Consolas`. Existing users won't see this change unless they edit the font names under `File>Preferences>Tools>JScript Panel`. Choosing a new `Preset` or using `Reset page` will also use the new default.
- Remove `ListenBrainz` sample.
- Various `JS Smooth` sample fixes.

## v2.4.0
- Fix crash when using `utils.CheckFont` on `foobar2000` `v1.6`. This affected many of the included scripts. Apologies for any inconvenience.
- Now requires `Windows 7` or later.

## v2.3.7 [last version compatible with Windows XP / Windows Vista]
- Fix crash caused by `utils.CheckFont` on `foobar2000` `v1.6` and later.
- Fix crash caused by using editor if `foo_wave_seekbar` was present.

## v2.3.6.1 [released by kbuffington aka MordredKLB]
- Fix bug where `on_focus` was not always triggered.

## v2.3.6 [released by kbuffington aka MordredKLB]
- Add `IGdiBitmap` `InvertColours`.
- Add `clear_properties` option to `window.Reload`.

## v2.3.5
- Add `plman.GetPlaylistLockFilterMask`. It's recommended that you replace any usage of `plman.IsPlaylistLocked` with this as you can now determine the type of locks in place.
- Add `plman.GetPlaylistLockName`.
- Restore compatibility with `foo_popup_panels` when used from within `Default UI`. This was broken in `v2.3.4`.
- `fb.CheckClipboardContents` and `fb.GetClipboardContents` no longer require a `window_id` parameter. If supplied, it will be silently ignored.
- Minor sample fixes.

## v2.3.4
- The size and position of the `Properties Window` is now remembered. In addition, the size and position of the [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window) is now shared as a global setting rather than per panel instance like it was previously.
- `on_size` calls are now suppressed when the panel width or height is zero.
- Improve behaviour of `Pseudo Transparent` mode to be like older versions. Some changes were made during the `2.2.x` series that made panels not configured with the `Panel Stack Splitter` `Forced layout` option glitch more than they should.
- The project is now [MIT licensed](https://github.com/marc2k3/foo_jscript_panel/blob/HEAD/LICENSE.md).

## v2.3.3.1
- Add `utils.ReadUTF8`. It's preferable to use this when you know the file is `UTF8` encoded... such as ones written by `utils.WriteTextFile`. Continue to use `utils.ReadTextFile` if the files are `UCS2-LE`, `ANSI` or unknown.

## v2.3.3
- Fix rare crash with `utils.ReadTextFile`.

## v2.3.2
- Add `IMetadbHandleList` `OptimiseFileLayout`. Also takes an optional `minimise` argument. With `minimise` set to `false`, provides the functionality of `Utilities>Optimize file layout` or if `minimise` is `true` then `Utilities>Optimize file layout + minimize file size`. Unlike the context menu versions, there is no prompt.
- Properly fix `Return`/`Tab` key usage in the `Find`/`Replace` dialog.

## v2.3.1
- Fix a nasty editor bug that caused the component to crash while typing. Versions `2.3.0`-`2.3.0.2` were all affected so upgrading immediately is highly recommended!

## v2.3.0.2
- `Find`/`Replace` dialog fixes.
- Fix `Thumbs` sample so [Last.fm](https://last.fm) artist art downloads work again.
- Restore `Clear` button to the `Properties` dialog.

## v2.3.0.1
- Make `utils.ColourPicker` remember `Custom colours` for the lifetime of `foobar2000` being open.

## v2.3.0
- Drop support for `foobar2000` `v1.3.x`. Now requires `v1.4` or later.
- Add `utils.DateStringFromTimestamp` and `utils.TimestampFromDateString`.
- The `Properties` dialog has been rewritten and there is no longer any `Clear` or `Delete` buttons. Now you can multi-select using your mouse and combinations of `Ctrl`/`Shift` keys. There is a right click menu to `Select All`/`Select None`/`Invert selection` and `Remove`. `Ctrl+A` and the `Delete` keyboard shortcuts are also supported.
- The list view under `File>Preferences>Tools>JScript Panel` now supports inline editing from a single click on the value. You no longer double click items to open a new dialog.
- The following methods no longer support the previously optional `force` parameter.

```js
window.Repaint();
window.RepaintRect(x, y, w, h);
ITitleFormat Eval() // returns an empty string when not playing
```

- The following methods no longer support the previously optional `flags` parameter. All commands are ran if they exist. It no longer matters if they are hidden or not.

```js
fb.RunContextCommand(command);
fb.RunContextCommandWithMetadb(command, handle_or_handle_list);
```

- These previously optional parameters have been removed:

```
IContextMenuManager BuildMenu "max_id"
IMainMenuManager BuildMenu "count"
utils.CheckComponent "is_dll"
utils.WriteTextFile "write_bom" (writing a `BOM` is no longer supported)
```

- `plman.PlayingPlaylist` is now a read-only property.
- The `IPlaylistRecyclerManager` interface has been removed and replaced with fixed `plman` methods. All previous functionality remains. Look at the docs for `plman.RecyclerCount`.
- `fb.GetLibraryRelativePath` has been removed. Use `IMetadbHandleList` `GetLibraryRelativePaths` instead.
- `utils.FileTest` has been removed and replaced with the following 4 new methods. There is no replacement for `split` mode.

```js
utils.Chardet(filename)
utils.GetFileSize(filename)
utils.IsFile(filename)
utils.IsFolder(folder)
```

- The `Grab focus` checkbox in the [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window) and the `dragdrop` `PREPROCESSOR` have both been removed. Both features are disabled by default but are automatically enabled if relevant callbacks like `on_drag_drop` or `on_key_up` are present in the script.

## v2.2.2.4
- Fix bug introduced in `v2.2.2.1` which prevented images loaded with `gdi.Image` / `gdi.LoadImageAsync` being deleted/overwritten after `Dispose()` had been called.

## v2.2.2.3
- Fix `IMetadbHandleList` `MakeDifference` method.

## v2.2.2.2
- Fix broken descriptions for main menu items in the keyboard shortcut preferences. Thanks to [TheQwertiest](https://github.com/TheQwertiest) for spotting and providing the fix.

## v2.2.2.1
- Remove optional `force` argument from `fb.GetFocusItem` as it had no effect.
- Various sample fixes. Volume now scales at the same rate as the `Default UI` volume slider. Thumbs has been fixed to work with [Last.fm](https://last.fm) site update.

## v2.2.2
- Fix broken colour handling in `IGdiBitmap` `GetColourSchemeJSON`. The original code by [MordredKLB](https://github.com/kbuffington) was good but I broke it so apologies for that.
- `IGdiBitmap` `GetColourScheme` has been removed. See this page for how to use the better `JSON` method mentioned above.

  https://github.com/marc2k3/foo_jscript_panel/wiki/Breaking-Changes

## v2.2.1.1
- Fix various sample problems reported on the `foobar2000` forums such as main menu `Edit` items not always being available and some `JS Smooth` `overflow` errors.

## v2.2.1
- Revert all timer related changes made in `v2.2.0.2`-`v2.2.0.3`. Although tested and working fine on my main machine, it caused crashes on `Windows XP` and `foobar2000` not to exit properly when running inside a `Windows 7` virtual machine. Apologies for any inconvenience!

## v2.2.0.3
- Fix potential freeze on shutdown caused by timer threads. Thanks to [TheQwertiest](https://github.com/TheQwertiest).
- Some minor sample bugs have been fixed.

## v2.2.0.2
- Revert some changes made in `v2.2.0` which may have broken various scripts that made use of `plman` (Playlist Manager) methods/properties. It would typically be noticeable if you had no playlists on startup or removed all playlists.

## v2.2.0.1
- Fix issue introduced in `v2.2.0` where some colours with certain levels of alpha transparency were not handled properly.

## v2.2.0
- Remove `window.DlgCode`. The docs were wrong, behaviour was inconsistent between `Default UI` and `Columns UI` and it's just not needed. Support for all keys will be enabled if you check `Grab focus` in the [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window). All instances will need removing from scripts. If you want to support this and older components with the same script, do something like:

```js
if ('DlgCode' in window) { window.DlgCode = 4; }
```

- `fb.RunMainMenuCommand`, `fb.RunContextCommand`, `fb.RunContextCommandWithMetadb` have all been rewritten and must be supplied with the full path to their commands. Case is not important. You should use forward slashes with no spaces. eg: `fb.RunMainMenuCommand("Library/Search")`.
- All files imported to the [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window), Preferences>Tools or included in a script via the `import` `PREPROCESSOR` directive must be `UTF8` encoded (with or without `BOM`). `utils.ReadTextFile` is unaffected although it should have better `UTF8` detection than before.
- The default `write_bom` argument for `utils.WriteTextFile` when omitted is now `false`.
- The code that parses the `PREPROCESSOR` section has been rewritten from scratch. The only difference is that double quotes in the name/author no longer need to be escaped so they will be duplicated if you did this previously.
- `window.GetColourCUI` and `window.GetFontCUI` no longer accept the previously optional `client_guid` argument.
- Add menu toolbar to [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window) which provides quick access to all included samples and docs. You can now Import/Export scripts via the `File` menu. The `Reset` option now properly resets all settings.
- The preset colour schemes previously bundled inside the `Colour Schemes` folder can now be loaded directly from the main preferences under `Tools>JScript Panel`.
- Add `utils.ListFiles` and `utils.ListFolders`.
- Update `fb.CreateHandleList` to take an optional handle argument to create a list containing a single item.
- Add `IMetadbHandle` `GetAlbumArt` method which returns the image and path without the need for using `utils.GetAlbumArtAsync` and `on_get_album_art_done`.
- `utils.FileTest` can now query files over 4GB in size when in `s` mode.
- Various sample fixes.
- Lots of internal code refactoring and external dependency updates.
- If you browse the component folder and notice the `jscript.api` and `interface.api` files are missing, it's intentional. The contents are now included inside the component.

## v2.1.8
- Add `IMetadbHandleList` `RemoveAttachedImages` method.
- There are no longer any limitations managing attached images. Working with the playing file is now supported. Thanks to Peter for giving me the help I needed!

## v2.1.7.2
- Fix various sample bugs.

## v2.1.7.1
- Images loaded by `gdi.LoadImageAsync` are no longer locked by the process.
- Correctly bump `utils.Version` - it wasn't updated in `v2.1.7`.

## v2.1.7
- Add `IMetadbHandleList` `AttachImage` / `RemoveAttachedImage` methods. There are some limitations!!

## v2.1.6
- Add `IContextMenuManager` `InitContextPlaylist` method which shows playlist specific options not available when passing a handle list to `InitContext`.
- Update `JSPlaylist` and `JS Smooth Playlist` samples with the above method.

## v2.1.5.2
- `utils.InputBox` is now centred when you open it plus you can now detect when the user presses `Cancel`.
- Fix bug with `Autoplaylists` sample which prevent existing queries from being edited.
- `gr.GdiDrawText` no longer has any return value.

## v2.1.5.1
- Fix incorrect spelling on the `Cancel` button in `utils.InputBox`.

## v2.1.5
- Add `utils.InputBox` method.
- Update samples in `complete` folder with above method - input is no longer truncated at 254 characters.
- Remove `fromhook` argument from `on_metadb_changed` callback. From now on, it will always be `undefined`.
- Ensure `JSPlaylist` doesn't load the same font more than once. Previously it was using `gdi.Font` inside `on_paint` which is bad practice.

## v2.1.4
- Add `on_dsp_preset_changed` callback.
- Add `fb.GetDSPPresets` and `fb.SetDSPPreset` methods.
- Add `IMetadbHandleList` `GetLibraryRelativePaths` method.
- Add `IMetadbHandleList` `Convert` method which converts a handle list to an array of handles.

## v2.1.3
- Add `fb.GetOutputDevices` and `fb.SetOutputDevice` methods.
- Add `on_output_device_changed` callback.
- `JSPlaylist` / `JS Smooth Browser/Playlist`, make right click behaviour consistent with other playlist/library viewers. Holding `Shift` should now show hidden context menu items.
- Playback now restarts when changing replaygain mode with `fb.ReplaygainMode`.

## v2.1.2
- `foobar2000` `v1.3.x` is now supported.

## v2.1.1
- A new `IGdiBitmap` `GetColourSchemeJSON` method has been added. Thanks to [MordredKLB](https://github.com/kbuffington) for the contribution.
- Fix script error when dragging items on to `JSPlaylist` / `JS Smooth Playlist` with no active playlist.

## v2.1.0.2
- Fix crash when supplying `plman.IsAutoPlaylist`/`plman.IsPlaylistLocked` with an invalid `playlistIndex`.

## v2.1.0.1
- Add `plman.RemovePlaylistSwitch` method. Unlike `plman.RemovePlaylist`, this automatically sets another playlist as active if removing the active playlist.
- Update `JS Smooth Playlist Manager` and `JSPlaylist` to make use of the above method which should fix some previous buggy behaviour whem removing playlists.

## v2.1.0
- Requires `foobar2000` `v1.4` `Beta 8` or later.
- The drag/drop functionality has been completely rewritten by [TheQwertiest](https://github.com/TheQwertiest). This will break all existing scripts which allow dragging in files from external sources. The ability to drag handle lists from `JScript Panel` to other panels has been added. The included playlist samples have been updated so they're compatible but they do not make use of the new functionality.
- Add `on_replaygain_mode_changed` callback.
- The behaviour of `plman.AddLocations` with the `select` argument set to `true` has been slightly modified.

  https://github.com/marc2k3/foo_jscript_panel/wiki/Drag-and-Drop

  https://github.com/marc2k3/foo_jscript_panel/wiki/Breaking-Changes

## v2.0.6
- Add `fb.CopyHandleListToClipboard` method. Contents can then be pasted in other components or as files in `Windows Explorer`.
- Add `fb.CheckClipboardContents` / `fb.GetClipboardContents`. Contents can be handles copied to the clipboard in other components or a file selection from `Windows Explorer`.
- `JSPlaylist` has been updated for full clipboard functionality with updated context menu items and keyboard shortcut support (`CTRL+C`, `CTRL+X` and `CTRL+V`).

## v2.0.5
- Add `ITitleFormat` `EvalWithMetadbs` method that takes a handle list as an argument. Returns a VBArray.
- Add `plman.SortPlaylistsByName`.
- The `IMetadbHandleList` `OrderByRelativePath` method now takes subsong index in to account. Thanks to WilB for reporting.
- `plman.GetPlaybackQueueContents` has been restored after being removed in the `v2` cleanse. It's the only way to determine if a single playlist item has been queued more than once. Note that each `IPlaybackQueueItem` is read-only.
- The `Properties` dialog size and layout have been tweaked.

## v2.0.4
- New `plman.FindPlaylist` and `plman.FindOrCreatePlaylist` methods have been addded.
- `on_library_items_added`, `on_library_items_changed`, `on_library_items_removed` now return a handle list of affected items.
- Various doc tidy up/fixes. Some `plman` methods have been updated to return `-1` on failure.

## v2.0.3
- `utils.WriteTextFile` now takes an optional `write_bom` argument. It defaults to `true` if omitted. If `false`, then the resulting file should be `UTF8` without `BOM`.
- Tidy up docs and `IMenuObj` interface. See the [Breaking Changes](https://github.com/marc2k3/foo_jscript_panel/wiki/Breaking-Changes) page for details.

## v2.0.2
- Fix random component crash caused by `utils.WriteTextFile`.
- Fix `JS Smooth` script image caching feature choking on some images. The breakage was introduced by me in `v2.0.1.2` as I attempted to update some old code. Thanks to always.beta for reporting.

## v2.0.1.2
- As part of the `v2` update, I accidentally broke the ability to drag tracks within `JSPlaylist`. This has now been fixed.

## v2.0.1.1
- Add usage notes to `Rating` and `Last.fm Lover` samples. `Rating` can now tag files or use the `JScript Panel` `Playback Stats` database in addition to using `foo_playcount` like it did before.
- A `Queue Viewer` script has been added to the `samples\complete` folder. It has a couple of basic options on the right click menu.

## v2.0.1
- Add `Last.fm Lover` sample. Imports all loved tracks from [Last.fm](https://last.fm) in to the new `JScript Panel` `Playback Stats` database. All loved tracks will have the value of `%JSP_LOVED%` set to `1` which is available in all components/search. You can then love/unlove tracks by clicking the heart icon. Obviously this requires a [Last.fm](https://last.fm) account. Use the right click menu to set your username and authorise the script.
- The timer mechanism behind `window.SetInterval` and `window.SetTimeout` has been completely rewritten by [TheQwertiest](https://github.com/TheQwertiest). Previously, it was limited to 16 instances and then it would fail causing random buggy behaviour in some advanced scripts.
- Fix `ListenBrainz` not clearing its cache after a successful submission. Thanks to zeremy for reporting.
- Fix `Thumbs` sample not deleting images.

## v2.0.0
- Requires `foobar2000` `v1.4`. It will not load with anything earlier.
- `fb.Trace` has been removed and replaced with a native `console.log` method. It takes multiple arguments as before.
- All callbacks/methods with `Color` in the name have been renamed with `Colour`. This will break most scripts!

```
on_colors_changed -> on_colours_changed
utils.ColorPicker -> utils.ColourPicker
utils.GetSysColor -> utils.GetSysColour
window.GetColorCUI -> window.GetColourCUI
window.GetColorDUI -> window.GetColourDUI
IGdiBitmap GetColorScheme -> GetColourScheme
```

- The `IPlaybackQueueItem` interface and `plman.CreatePlaybackQueueItem` method have been removed.
- `plman.GetPlaybackQueueContents` has been replaced with `plman.GetPlaybackQueueHandles` which returns a handle list. You can check the `Count` property so there is no longer any need for `plman.GetPlaybackQueueCount` and `plman.IsPlaybackQueueActive` which have been removed.
- `on_refresh_background_done` and `window.GetBackgroundImage` have both been removed.
- The `IGdiBitmap` `BoxBlur` method has been removed. Use `StackBlur` instead.
- `IContextMenuManager` `InitContext` only accepts a handle list as an argument.
- Anyone who has a `js_settings` folder in their `foobar2000` profile folder from using my previous samples should move the contents in to the `js_data` folder. Typically, this would be `autoplaylists.json`, `thumbs.ini`, `listenbrainz.ini`.
- Enable `Windows XP` support. Also, support for `Windows Vista`, `Windows 7` and `WINE` with `IE7`/`IE8` has been restored. `ES5` features are still supported for those with `IE9` or later. If your scripts crash after upgrading and making the changes outlined above, make sure to check the `Script Engine` setting in the [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window).
- Add `utils.WriteTextFile`. Always creates a `UTF8-BOM` file.
- Add `window.Name` property.
- Add a custom `Playback Stats` backend and `handle` methods for writing values.

  https://github.com/marc2k3/foo_jscript_panel/wiki/Breaking-Changes

  https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window

  https://github.com/marc2k3/foo_jscript_panel/wiki/Playback-Stats
