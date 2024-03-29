ppt.tf_album_sort = fb.TitleFormat(window.GetProperty("SMOOTH.SORT.ALBUM", "%album artist% | %date% | %album% | %discnumber% | %tracknumber% | %title%"));
ppt.tf_artist_sort = fb.TitleFormat(window.GetProperty("SMOOTH.SORT.ARTIST", "$meta(artist,0) | %date% | %album% | %discnumber% | %tracknumber% | %title%"));
ppt.tf_album_artist_sort = fb.TitleFormat(window.GetProperty("SMOOTH.SORT.ALBUM.ARTIST", "%album artist% | %date% | %album% | %discnumber% | %tracknumber% | %title%"));
ppt.tf_groupkey_album = fb.TitleFormat("$if2(%album artist%,Unknown Artist) ^^ $if2(%album%,'('Singles')')");
ppt.tf_groupkey_artist = fb.TitleFormat("$if2($meta(artist,0),Unknown Artist)");
ppt.tf_groupkey_album_artist = fb.TitleFormat("%album artist%");
ppt.tf_crc_artist = fb.TitleFormat("$crc32(artists$meta(artist,0))");
ppt.tf_crc_album_artist = fb.TitleFormat("$crc32(artists%album artist%)");

ppt.panelMode = window.GetProperty("SMOOTH.DISPLAY.MODE", 2); // 0 = column, 1 = column + art, 2 = album art grid, 3 - album art grid + overlay text
ppt.showAllItem = window.GetProperty("SMOOTH.SHOW.ALL.ITEMS", true);
ppt.tagMode = window.GetProperty("SMOOTH.TAG.MODE", 0); // 0 = album, 1 = artist, 2 = album artist
ppt.tagText = ["album", "artist", "album artist"];

ppt.default_thumbnailWidthMin = window.GetProperty("SMOOTH.THUMB.MIN.WIDTH", 130);
ppt.thumbnailWidthMin = ppt.default_thumbnailWidthMin;
ppt.default_lineHeightMin = window.GetProperty("SMOOTH.LINE.MIN.HEIGHT", 90);
ppt.lineHeightMin = ppt.default_lineHeightMin;

var cFilterBox = {
	default_w: 300,
	default_h: 20,
	x: 5,
	y: 2,
	w: 300,
	h: 20
}

function oGroup(index, start, metadb, groupkey) {
	this.index = index;
	this.start = start;
	this.count = 1;
	this.metadb = metadb;
	this.groupkey = groupkey;

	var arr = this.groupkey.split(" ^^ ");
	this.artist = arr[0];
	this.album = arr[1] || "";

	if (metadb) {
		switch (ppt.tagMode) {
		case 0:
			this.cachekey = ppt.tf_crc_path.EvalWithMetadb(metadb);
			break;
		case 1:
			this.cachekey = ppt.tf_crc_artist.EvalWithMetadb(metadb);
			break;
		case 2:
			this.cachekey = ppt.tf_crc_album_artist.EvalWithMetadb(metadb);
			break;
		}
	} else {
		this.cachekey = null;
	}
	this.cover_image = null;
	this.image_requested = false;

	this.finalise = function (handles) {
		this.handles = handles.Clone();
		this.count = this.handles.Count;
	}
}

function oBrowser() {
	this.groups = [];
	this.rowsCount = 0;
	this.scrollbar = new oScrollbar();
	this.selectedIndex = -1;

	window.SetTimeout(function () {
		brw.populate();
	}, 250);

	this.repaint = function () {
		need_repaint = true;
	}

	this.setSize = function () {
		this.x = 0;
		this.y = ppt.showHeaderBar ? ppt.headerBarHeight : 0;
		this.w = ww - cScrollBar.width;
		this.h = wh - this.y;

		switch (ppt.panelMode) {
		case 0:
		case 1:
			ppt.lineHeightMin = scale(ppt.default_lineHeightMin);
			this.totalColumns = 1;
			this.rowsCount = this.groups.length;
			this.thumbnailWidth = this.w;
			switch (ppt.tagMode) {
			case 0: // album
				this.rowHeight = (ppt.panelMode == 0 ? Math.ceil(g_fsize * 4.5) : ppt.lineHeightMin);
				break;
			case 1: // artist
			case 2: // album artist
				this.rowHeight = (ppt.panelMode == 0 ? Math.ceil(g_fsize * 2.5) : ppt.lineHeightMin);
				break;
			}
			break;
		case 2:
		case 3:
			ppt.thumbnailWidthMin = scale(ppt.default_thumbnailWidthMin);
			this.totalColumns = Math.floor(this.w / ppt.thumbnailWidthMin);
			if (this.totalColumns < 1) this.totalColumns = 1;
			this.thumbnailWidth = this.w / this.totalColumns;
			this.rowsCount = Math.ceil(this.groups.length / this.totalColumns);
			this.rowHeight = this.thumbnailWidth;
			if (ppt.panelMode == 2) this.rowHeight += (g_font_height * 3);
			break;
		}

		this.totalRows = Math.ceil(this.h / this.rowHeight);
		this.totalRowsVis = Math.floor(this.h / this.rowHeight);

		g_filterbox.setSize(cFilterBox.w, cFilterBox.h + 2, g_fsize + 2);

		this.scrollbar.setSize();

		scroll = Math.round(scroll / this.rowHeight) * this.rowHeight;
		scroll = check_scroll(scroll);
		scroll_ = scroll;

		// scrollbar update
		this.scrollbar.updateScrollbar();
	}

	this.showItemFromItemIndex = function (index) {
		if (ppt.showAllItem && index == 0)
			index += 1;
		switch (ppt.panelMode) {
		case 0:
		case 1:
			if (this.h / 2 > this.rowHeight) {
				var delta = Math.floor(this.h / 2);
			} else {
				var delta = 0
			}
			scroll = index * this.rowHeight - delta;
			scroll = check_scroll(scroll);
			break;
		case 2:
		case 3:
			var row = Math.floor(index / this.totalColumns);
			if (this.h / 2 > this.rowHeight) {
				var delta = Math.floor(this.h / 2);
			} else {
				var delta = 0
			}
			scroll = row * this.rowHeight - delta;
			scroll = check_scroll(scroll);
			break;
		}
		this.activateItem(index);
	}

	this.getItemIndexFromTrackIndex = function (tid) {
		var mediane = 0;
		var deb = 0;
		var fin = this.groups.length - 1;
		while (deb <= fin) {
			mediane = Math.floor((fin + deb) / 2);
			if (tid >= this.groups[mediane].start && tid < this.groups[mediane].start + this.groups[mediane].count) {
				return mediane;
			} else if (tid < this.groups[mediane].start) {
				fin = mediane - 1;
			} else {
				deb = mediane + 1;
			}
		}
		return -1;
	}

	this.init_groups = function () {
		this.groups = [];
		if (this.list.Count == 0) return;

		var previous = "";
		var g = 0;
		var handles = fb.CreateHandleList();

		switch (ppt.tagMode) {
		case 0: // album
			var tf = ppt.tf_groupkey_album;
			break;
		case 1: // artist
			var tf = ppt.tf_groupkey_artist;
			break;
		case 2: // album artist
			var tf = ppt.tf_groupkey_album_artist;
			break;
		}

		if (g_filter_text.length > 0) {
			try {
				this.list = fb.GetQueryItems(this.list, g_filter_text);
			} catch (e) {
				// invalid query
			}
		}

		var arr = tf.EvalWithMetadbs(this.list).toArray();

		for (var i = 0; i < this.list.Count; i++) {
			var handle = this.list.Item(i);
			var meta = arr[i];
			var current = meta.toLowerCase();

			if (current != previous) {
				if (g > 0) {
					this.groups[g - 1].finalise(handles);
					handles.RemoveAll();
				}
				this.groups.push(new oGroup(g + 1, i, handle, meta));
				handles.Add(handle);
				g++;
				previous = current;
			} else {
				handles.Add(handle);
			}
		}

		if (g > 0) {
			this.groups[g - 1].finalise(handles);

			if (g > 1 && ppt.showAllItem) {
				var meta = "All items";
				if (ppt.tagMode == 0) {
					var all_items = "(" + this.groups.length + " " + ppt.tagText[ppt.tagMode] + "s)"
					meta += " ^^ " + all_items;
				}
				this.groups.unshift(new oGroup(0, 0, null, meta));
				this.groups[0].finalise(this.list);
			}
		}
		handles.Dispose();
	}

	this.populate = function () {
		this.list = fb.GetLibraryItems();

		if (ppt.tagMode == 0) { //album
			this.list.OrderByFormat(ppt.tf_album_sort, 1);
		} else if (ppt.tagMode == 1) { // artist
			this.list.OrderByFormat(ppt.tf_artist_sort, 1);
		} else if (ppt.tagMode == 2) { // allbum artist
			this.list.OrderByFormat(ppt.tf_album_artist_sort, 1);
		}

		this.init_groups();
		get_metrics();
		this.repaint();
	}

	this.activateItem = function (index) {
		if (this.groups.length == 0)
			return;
		this.selectedIndex = index;
	}

	this.sendItemToPlaylist = function (index) {
		if (this.groups.length == 0)
			return;

		var pidx = plman.FindPlaylist("Library selection");
		var pfound = pidx > -1;
		var pidx_playing = plman.FindPlaylist("Library selection (playing)");
		var pfound_playing = pidx_playing > -1;

		if (fb.IsPlaying && plman.PlayingPlaylist == pidx) { // playing playlist is "Library selection"
			plman.RenamePlaylist(pidx, "Library selection (playing)");
			if (pfound_playing) {
				plman.RenamePlaylist(pidx_playing, "Library selection");
				plman.ClearPlaylist(pidx_playing);
			} else {
				pidx_playing = plman.CreatePlaylist(plman.PlaylistCount, "Library selection");
			}
			plman.InsertPlaylistItems(pidx_playing, 0, this.groups[index].handles, false);
			plman.MovePlaylist(pidx_playing, pidx);
			plman.MovePlaylist(pidx + 1, pidx_playing);
		} else {
			if (pfound) {
				plman.ClearPlaylist(pidx);
			} else {
				pidx = plman.CreatePlaylist(plman.PlaylistCount, "Library selection");
			}
			plman.InsertPlaylistItems(pidx, 0, this.groups[index].handles, false);
		}
		plman.ActivePlaylist = pidx;
	}

	this.getlimits = function () {
		if (this.groups.length <= this.totalRowsVis * this.totalColumns) {
			var start_ = 0;
			var end_ = this.groups.length;
		} else {
			var start_ = Math.round(scroll_ / this.rowHeight) * this.totalColumns;
			var end_ = Math.round((scroll_ + wh + this.rowHeight) / this.rowHeight) * this.totalColumns;
			// check values / limits
			end_ = (this.groups.length < end_) ? this.groups.length : end_;
			start_ = start_ > 0 ? start_ - this.totalColumns : (start_ < 0 ? 0 : start_);
		}
		g_start_ = start_;
		g_end_ = end_;
	}

	this.draw = function (gr) {
		this.getlimits();

		var total = this.groups.length;
		var cx = 0;
		var ax = 0;
		var ay = 0;
		var aw = this.thumbnailWidth;
		var ah = this.rowHeight;

		for (var i = g_start_; i < g_end_; i++) {
			var group = this.groups[i];

			var row = Math.floor(i / this.totalColumns);
			ax = this.x + (cx * this.thumbnailWidth);
			ay = Math.floor(this.y + (row * this.rowHeight) - scroll_);
			group.x = ax;
			group.y = ay;

			var normal_text = g_color_normal_txt;
			var fader_txt = blendColors(g_color_normal_txt, g_color_normal_bg, 0.25);

			if (ppt.panelMode != 0 && !group.cover_image && !group.image_requested && group.metadb) {
				group.image_requested = true;
				var id = ppt.tagMode == 0 ? AlbumArtId.front : AlbumArtId.artist
				var filename = generate_filename(group.cachekey, id);
				group.cover_image = get_art(group.metadb, filename, id);
			}

			switch (ppt.panelMode) {
			case 0:
			case 1:
				if (i % 2 != 0) {
					gr.FillSolidRect(ax, ay, aw, ah, g_color_normal_txt & 0x08ffffff);
				}

				// selected?
				if (i == this.selectedIndex) {
					gr.FillSolidRect(ax, ay, aw, ah, g_color_selected_bg & 0xb0ffffff);
					gr.DrawRect(ax + 1, ay + 1, aw - 2, ah - 2, 2.0, g_color_selected_bg);
					if (normal_text == g_color_selected_bg) {
						normal_text = g_color_normal_bg;
						fader_txt = blendColors(normal_text, g_color_normal_bg, 0.25);
					}
				} else if (i == g_rightClickedIndex) {
					gr.DrawRect(ax + 1, ay + 1, aw - 2, ah - 2, 2.0, g_color_selected_bg);
				}

				var text_left = 8;
				var text_width = aw - (text_left * 2);
				if (ppt.panelMode == 1) {
					var cover_size = ah - (text_left * 2);
					if (ppt.showAllItem && i == 0 && this.groups.length > 1) {
						gr.FillSolidRect(ax + text_left, ay + text_left, cover_size, cover_size, g_color_normal_bg);
						drawImage(gr, images.all, ax + text_left, ay + text_left, cover_size, cover_size, ppt.autoFill, normal_text & 0x25ffffff);
					} else {
						if (!group.cover_image) gr.FillSolidRect(ax + text_left, ay + text_left, cover_size, cover_size, g_color_normal_bg);
						drawImage(gr, group.cover_image || images.noart, ax + text_left, ay + text_left, cover_size, cover_size, ppt.autoFill, normal_text & 0x25ffffff);
					}
					text_left += cover_size + 8;
					text_width = aw - text_left - 16;
				}

				if (ppt.tagMode == 0) { // album, 2 lines
					gr.GdiDrawText(group.album, g_font_bold, normal_text, ax + text_left, ay + (ah / 2) - g_font_bold_height - 2, text_width, g_font_bold_height + 2, DT_LEFT | DT_VCENTER | DT_CALCRECT | DT_END_ELLIPSIS | DT_NOPREFIX);
					gr.GdiDrawText(group.artist, g_font, fader_txt, ax + text_left, ay + (ah / 2) + 2, text_width, g_font_height + 2, DT_LEFT | DT_VCENTER | DT_CALCRECT | DT_END_ELLIPSIS | DT_NOPREFIX);
				} else { // artist/album artist, 1 line
					gr.GdiDrawText(group.artist, g_font, normal_text, ax + text_left, ay, text_width, ah, DT_LEFT | DT_VCENTER | DT_CALCRECT | DT_END_ELLIPSIS | DT_NOPREFIX);
				}
				break;
			case 2:
				var text_left = 8;
				var cover_size = aw - (text_left * 2);

				// selected?
				if (i == this.selectedIndex) {
					gr.FillSolidRect(ax, ay, aw, ah, g_color_selected_bg & 0xb0ffffff);
					gr.DrawRect(ax + 1, ay + 1, aw - 2, ah - 2, 2.0, g_color_selected_bg);
					if (!group.cover_image) gr.FillSolidRect(ax + text_left, ay + text_left, cover_size, cover_size, g_color_normal_bg);
					if (normal_text == g_color_selected_bg) {
						normal_text = g_color_normal_bg;
						fader_txt = blendColors(normal_text, g_color_normal_bg, 0.25);
					}
				} else if (i == g_rightClickedIndex) {
					gr.DrawRect(ax + 1, ay + 1, aw - 2, ah - 2, 2.0, g_color_selected_bg);
				}

				if (ppt.showAllItem && i == 0 && this.groups.length > 1) {
					gr.FillSolidRect(ax + text_left, ay + text_left, cover_size, cover_size, g_color_normal_bg);
					drawImage(gr, images.all, ax + text_left, ay + text_left, cover_size, cover_size, ppt.autoFill, normal_text & 0x25ffffff);
				} else {
					drawImage(gr, group.cover_image || images.noart, ax + text_left, ay + text_left, cover_size, cover_size, ppt.autoFill, normal_text & 0x25ffffff);
				}

				if (ppt.tagMode == 0) {
					gr.GdiDrawText(group.album, g_font_bold, normal_text, ax + text_left, ay + cover_size + (text_left * 2), cover_size, g_font_bold_height + 2, DT_CENTER | DT_VCENTER | DT_CALCRECT | DT_END_ELLIPSIS | DT_NOPREFIX);
					gr.GdiDrawText(group.artist, g_font, fader_txt, ax + text_left, ay + cover_size + (text_left * 2) + (g_font_bold_height * 1.4), cover_size, g_font_height + 2, DT_CENTER | DT_VCENTER | DT_CALCRECT | DT_END_ELLIPSIS | DT_NOPREFIX);
				} else {
					gr.GdiDrawText(group.artist, g_font_bold, normal_text, ax + text_left, ay + cover_size + (text_left * 2) - 2, cover_size, g_font_bold_height * 3, DT_CENTER | DT_TOP | DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX);
				}

				break;
			case 3: // auto-fil setting is ignored here and forced on. if turned off, it looks terrible with non square artist images
				if (ppt.showAllItem && i == 0 && this.groups.length > 1) {
					drawImage(gr, images.all, ax, ay, aw, ah, true, normal_text & 0x25ffffff);
				} else {
					drawImage(gr, group.cover_image || images.noart, ax, ay, aw, ah, true, normal_text & 0x25ffffff);
					var h = g_font_height * 3;
					var hh = h / 2;
					gr.FillGradRect(ax, ay + ah - h, aw, h, 90, RGBA(0, 0, 0, 230), RGBA(0, 0, 0, 200));
					if (ppt.tagMode == 0) {
						gr.GdiDrawText(group.album, g_font_bold, RGB(240, 240, 240), ax + 8, ay + ah - h + 2, aw - 16, hh, DT_LEFT | DT_VCENTER | DT_CALCRECT | DT_END_ELLIPSIS | DT_NOPREFIX);
						gr.GdiDrawText(group.artist, g_font, RGB(230, 230, 230), ax + 8, ay + ah - hh - 2, aw - 16, hh, DT_LEFT | DT_VCENTER | DT_CALCRECT | DT_END_ELLIPSIS | DT_NOPREFIX);
					} else {
						gr.GdiDrawText(group.artist, g_font, RGB(230, 230, 230), ax + 8, ay + ah - h, aw - 16, h, DT_LEFT | DT_VCENTER | DT_CALCRECT | DT_END_ELLIPSIS | DT_NOPREFIX);
					}
				}
				if (i == this.selectedIndex || i == g_rightClickedIndex) {
					gr.DrawRect(ax + 1, ay + 1, aw - 3, ah - 3, 3.0, g_color_selected_bg);
				}
				break;
			}

			if (cx == this.totalColumns - 1) {
				cx = 0;
			} else {
				cx++;
			}
		}

		// draw scrollbar
		this.scrollbar && this.scrollbar.draw(gr);

		// draw top header bar
		if (ppt.showHeaderBar) {
			gr.FillSolidRect(0, 0, ww, this.y - 1, g_color_normal_bg);
			gr.FillSolidRect(this.x, 0, this.w + cScrollBar.width, ppt.headerBarHeight - 1, g_color_normal_bg & 0x20ffffff);
			gr.FillSolidRect(this.x, ppt.headerBarHeight - 2, this.w + cScrollBar.width, 1, g_color_normal_txt & 0x22ffffff);
			var nb_groups = (ppt.showAllItem && total > 1 ? total - 1 : total);
			var boxText = nb_groups + " " + ppt.tagText[ppt.tagMode];
			if (nb_groups > 1) boxText += "s";
			gr.GdiDrawText(boxText, g_font_box, blendColors(g_color_normal_txt, g_color_normal_bg, 0.4), 0, 0, ww - 5, ppt.headerBarHeight - 1, DT_RIGHT | DT_VCENTER | DT_CALCRECT | DT_NOPREFIX | DT_END_ELLIPSIS);
		}
	}

	this._isHover = function (x, y) {
		return (x >= this.x && x <= this.x + this.w && y >= this.y && y <= this.y + this.h);
	}

	this.on_mouse = function (event, x, y, delta) {
		this.ishover = this._isHover(x, y);

		// get active item index at x,y coords...
		this.activeIndex = -1;
		if (this.ishover) {
			this.activeRow = Math.ceil((y + scroll_ - this.y) / this.rowHeight) - 1;
			if (y > this.y && x > this.x && x < this.x + this.w) {
				this.activeColumn = Math.ceil((x - this.x) / this.thumbnailWidth) - 1;
				this.activeIndex = (this.activeRow * this.totalColumns) + this.activeColumn;
				this.activeIndex = this.activeIndex > this.groups.length - 1 ? -1 : this.activeIndex;
			}
		}
		if (this.activeIndex != this.activeIndexSaved) {
			this.activeIndexSaved = this.activeIndex;
			this.repaint();
		}

		switch (event) {
		case "down":
			if (this.ishover) {
				if (this.activeIndex > -1) {
					if (this.activeIndex != this.selectedIndex) {
						this.activateItem(this.activeIndex)
					}
					this.sendItemToPlaylist(this.activeIndex);
				}
				this.repaint();
			}
			break;
		case "dblclk":
			if (this.ishover && this.activeIndex > -1) {
				play(plman.ActivePlaylist, 0);
			}
			break;
		case "right":
			g_rightClickedIndex = this.activeIndex;
			if (this.ishover && this.activeIndex > -1) {
				this.context_menu(x, y, this.activeIndex);
			} else {
				if (!g_filterbox.inputbox.hover) {
					this.settings_context_menu(x, y);
				}
			}
			g_rightClickedIndex = -1;
			break;
		case "wheel":
			if (cScrollBar.visible) {
				this.scrollbar.updateScrollbar();
			}
			break;
		}
		if (cScrollBar.visible) {
			this.scrollbar.on_mouse(event, x, y);
		}
	}

	this.g_time = window.SetInterval(function () {
		if (!window.IsVisible) {
			need_repaint = true;
			return;
		}

		scroll = check_scroll(scroll);
		if (Math.abs(scroll - scroll_) >= 1) {
			scroll_ += (scroll - scroll_) / ppt.scrollSmoothness;
			isScrolling = true;
			need_repaint = true;
			if (scroll_prev != scroll)
				brw.scrollbar.updateScrollbar();
		} else {
			if (scroll_ != scroll) {
				scroll_ = scroll; // force to scroll_ value to fixe the 5.5 stop value for expanding album action
				need_repaint = true;
			}
			if (isScrolling) {
				if (scroll_ < 1)
					scroll_ = 0;
				isScrolling = false;
				need_repaint = true;
			}
		}

		if (need_repaint) {
			need_repaint = false;
			window.Repaint();
		}

		scroll_prev = scroll;

	}, ppt.refreshRate);

	this.context_menu = function (x, y, albumIndex) {
		var _menu = window.CreatePopupMenu();
		var _context = fb.CreateContextMenuManager();
		var _add = window.CreatePopupMenu();

		_add.AppendMenuItem(MF_STRING, 1, "New Playlist");
		if (plman.PlaylistCount > 1) {
			_add.AppendMenuSeparator();
		}
		for (var i = 0; i < plman.PlaylistCount; i++) {
			_add.AppendMenuItem(playlist_can_add_items(i) ? MF_STRING : MF_GRAYED, i + 2, plman.GetPlaylistName(i));

		}
		_add.AppendTo(_menu, MF_STRING, "Add to");

		_menu.AppendMenuSeparator();
		_context.InitContext(this.groups[albumIndex].handles);
		_context.BuildMenu(_menu, 1000);

		var idx = _menu.TrackPopupMenu(x, y);
		switch (true) {
		case idx == 0:
			break;
		case idx == 1:
			var pl = plman.CreatePlaylist();
			plman.ActivePlaylist = pl
			plman.InsertPlaylistItems(pl, 0, this.groups[albumIndex].handles, false);
			break;
		case idx < 1000:
			var target_playlist = idx - 2;
			plman.UndoBackup(target_playlist);
			var base = plman.PlaylistItemCount(target_playlist);
			plman.InsertPlaylistItems(target_playlist, base, this.groups[albumIndex].handles, false);
			plman.ActivePlaylist = target_playlist;
			break;
		default:
			_context.ExecuteByID(idx - 1000);
			break;
		}
		_add.Dispose();
		_context.Dispose();
		_menu.Dispose();
		return true;
	}

	this.settings_context_menu = function (x, y) {
		var _menu = window.CreatePopupMenu();
		var _menu1 = window.CreatePopupMenu();

		_menu.AppendMenuItem(MF_STRING, 1, "Header Bar");
		_menu.CheckMenuItem(1, ppt.showHeaderBar);
		_menu.AppendMenuSeparator();

		var colour_flag = ppt.enableCustomColors ? MF_STRING : MF_GRAYED;
		_menu1.AppendMenuItem(MF_STRING, 2, "Enable");
		_menu1.CheckMenuItem(2, ppt.enableCustomColors);
		_menu1.AppendMenuSeparator();
		_menu1.AppendMenuItem(colour_flag, 3, "Text");
		_menu1.AppendMenuItem(colour_flag, 4, "Background");
		_menu1.AppendMenuItem(colour_flag, 5, "Selected background");
		_menu1.AppendTo(_menu, MF_STRING, "Custom colours");
		_menu.AppendMenuSeparator();

		_menu.AppendMenuItem(MF_STRING, 20, "Album");
		_menu.AppendMenuItem(MF_STRING, 21, "Artist");
		_menu.AppendMenuItem(MF_STRING, 22, "Album Artist");
		_menu.CheckMenuRadioItem(20, 22, 20 + ppt.tagMode);
		_menu.AppendMenuSeparator();
		_menu.AppendMenuItem(MF_STRING, 30, "Column");
		_menu.AppendMenuItem(MF_STRING, 31, "Column + Album Art");
		_menu.AppendMenuItem(MF_STRING, 32, "Album Art Grid (Original style)");
		_menu.AppendMenuItem(MF_STRING, 33, "Album Art Grid (Overlayed text)");
		_menu.CheckMenuRadioItem(30, 33, 30 + ppt.panelMode);
		_menu.AppendMenuSeparator();
		_menu.AppendMenuItem(MF_STRING, 40, "Show all items");
		_menu.CheckMenuItem(40, ppt.showAllItem);
		_menu.AppendMenuItem(ppt.panelMode == 0 || ppt.panelMode == 3 ? MF_GRAYED : MF_STRING, 41, "Album Art: Auto-fill");
		_menu.CheckMenuItem(41, ppt.autoFill);

		_menu.AppendMenuSeparator();
		_menu.AppendMenuItem(MF_STRING, 50, "Panel Properties");
		_menu.AppendMenuItem(MF_STRING, 51, "Configure...");

		var idx = _menu.TrackPopupMenu(x, y);

		switch (idx) {
		case 1:
			ppt.showHeaderBar = !ppt.showHeaderBar;
			window.SetProperty("SMOOTH.SHOW.TOP.BAR", ppt.showHeaderBar);
			get_metrics();
			this.repaint();
			break;
		case 2:
			ppt.enableCustomColors = !ppt.enableCustomColors;
			window.SetProperty("SMOOTH.CUSTOM.COLOURS.ENABLED", ppt.enableCustomColors);
			on_colours_changed();
			break;
		case 3:
			g_color_normal_txt = utils.ColourPicker(window.ID, g_color_normal_txt);
			window.SetProperty("SMOOTH.COLOUR.TEXT", g_color_normal_txt);
			on_colours_changed();
			break;
		case 4:
			g_color_normal_bg = utils.ColourPicker(window.ID, g_color_normal_bg);
			window.SetProperty("SMOOTH.COLOUR.BACKGROUND.NORMAL", g_color_normal_bg);
			on_colours_changed();
			break;
		case 5:
			g_color_selected_bg = utils.ColourPicker(window.ID, g_color_selected_bg);
			window.SetProperty("SMOOTH.COLOUR.BACKGROUND.SELECTED", g_color_selected_bg);
			on_colours_changed();
			break;
		case 20:
		case 21:
		case 22:
			ppt.tagMode = idx - 20;
			window.SetProperty("SMOOTH.TAG.MODE", ppt.tagMode);
			this.populate();
			break;
		case 30:
		case 31:
		case 32:
		case 33:
			ppt.panelMode = idx - 30;
			window.SetProperty("SMOOTH.DISPLAY.MODE", ppt.panelMode);
			get_metrics();
			this.repaint();
			break;
		case 40:
			ppt.showAllItem = !ppt.showAllItem;
			window.SetProperty("SMOOTH.SHOW.ALL.ITEMS", ppt.showAllItem);
			this.populate();
			break;
		case 41:
			ppt.autoFill = !ppt.autoFill;
			window.SetProperty("SMOOTH.AUTO.FILL", ppt.autoFill);
			images.clear();
			this.populate();
			break;
		case 50:
			window.ShowProperties();
			break;
		case 51:
			window.ShowConfigure();
			break;
		}
		_menu1.Dispose();
		_menu.Dispose();
		return true;
	}
}

function on_size() {
	ww = window.Width;
	wh = window.Height;
	brw.setSize();
}

function on_paint(gr) {
	if (ww < 10 || wh < 10)
		return;

	gr.FillSolidRect(0, 0, ww, wh, g_color_normal_bg);
	brw.draw(gr);

	if (ppt.showHeaderBar) {
		g_filterbox.draw(gr, 5, 2);
	}
}

function on_mouse_lbtn_down(x, y) {
	// stop inertia
	if (cTouch.timer) {
		window.ClearInterval(cTouch.timer);
		cTouch.timer = false;
		if (Math.abs(scroll - scroll_) > brw.rowHeight) {
			scroll = (scroll > scroll_ ? scroll_ + brw.rowHeight : scroll_ - brw.rowHeight);
			scroll = check_scroll(scroll);
		}
	}

	var is_scroll_enabled = brw.rowsCount > brw.totalRowsVis;
	if (ppt.enableTouchControl && is_scroll_enabled) {
		if (brw._isHover(x, y) && !brw.scrollbar._isHover(x, y)) {
			if (!timers.mouseDown) {
				cTouch.y_prev = y;
				cTouch.y_start = y;
				if (cTouch.t1) {
					cTouch.t1.Reset();
				} else {
					cTouch.t1 = fb.CreateProfiler("t1");
				}
				timers.mouseDown = window.SetTimeout(function () {
					timers.mouseDown = false;
					if (Math.abs(cTouch.y_start - m_y) > 015) {
						cTouch.down = true;
					} else {
						brw.on_mouse("down", x, y);
					}
				}, 50);
			}
		} else {
			brw.on_mouse("down", x, y);
		}
	} else {
		brw.on_mouse("down", x, y);
	}

	// inputBox
	if (ppt.showHeaderBar) {
		g_filterbox.on_mouse("lbtn_down", x, y);
	}
}

function on_mouse_lbtn_up(x, y) {

	// inputBox
	if (ppt.showHeaderBar) {
		g_filterbox.on_mouse("lbtn_up", x, y);
	}

	brw.on_mouse("up", x, y);

	if (timers.mouseDown) {
		timers.mouseDown = false;
		if (Math.abs(cTouch.y_start - m_y) <= 030) {
			brw.on_mouse("down", x, y);
		}
	}

	// create scroll inertia on mouse lbtn up
	if (cTouch.down) {
		cTouch.down = false;
		cTouch.y_end = y;
		cTouch.scroll_delta = scroll - scroll_;
		if (Math.abs(cTouch.scroll_delta) > 015) {
			cTouch.multiplier = ((1000 - cTouch.t1.Time) / 20);
			cTouch.delta = Math.round((cTouch.scroll_delta) / 015);
			if (cTouch.multiplier < 1)
				cTouch.multiplier = 1;
			if (cTouch.timer)
				window.ClearInterval(cTouch.timer);
			cTouch.timer = window.SetInterval(function () {
				scroll += cTouch.delta * cTouch.multiplier;
				scroll = check_scroll(scroll);
				cTouch.multiplier = cTouch.multiplier - 1;
				cTouch.delta = cTouch.delta - (cTouch.delta / 10);
				if (cTouch.multiplier < 1) {
					window.ClearInterval(cTouch.timer);
					cTouch.timer = false;
				}
			}, 75);
		}
	}
}

function on_mouse_lbtn_dblclk(x, y, mask) {
	if (y >= brw.y) {
		brw.on_mouse("dblclk", x, y);
	}
}

function on_mouse_rbtn_up(x, y) {
	if (ppt.showHeaderBar) {
		g_filterbox.on_mouse("rbtn_up", x, y);
	}

	brw.on_mouse("right", x, y);
	return true;
}

function on_mouse_move(x, y) {
	if (m_x == x && m_y == y)
		return;

	if (ppt.showHeaderBar) {
		g_filterbox.on_mouse("move", x, y);
	}

	if (cTouch.down) {
		cTouch.y_current = y;
		cTouch.y_move = (cTouch.y_current - cTouch.y_prev);
		if (x < brw.w) {
			scroll -= cTouch.y_move;
			cTouch.scroll_delta = scroll - scroll_;
			if (Math.abs(cTouch.scroll_delta) < 030)
				cTouch.y_start = cTouch.y_current;
			cTouch.y_prev = cTouch.y_current;
		}
	} else {
		brw.on_mouse("move", x, y);
	}

	m_x = x;
	m_y = y;
}

function on_mouse_wheel(step) {
	if (cTouch.timer) {
		window.ClearInterval(cTouch.timer);
		cTouch.timer = false;
	}

	if (utils.IsKeyPressed(VK_CONTROL)) { // zoom all elements)
		var zoomStep = 1;
		var previous = ppt.extra_font_size;
		if (!timers.mouseWheel) {
			if (step > 0) {
				ppt.extra_font_size += zoomStep;
				if (ppt.extra_font_size > 10)
					ppt.extra_font_size = 10;
			} else {
				ppt.extra_font_size -= zoomStep;
				if (ppt.extra_font_size < 0)
					ppt.extra_font_size = 0;
			}
			if (previous != ppt.extra_font_size) {
				timers.mouseWheel = window.SetTimeout(function () {
					timers.mouseWheel = false;
					window.SetProperty("SMOOTH.EXTRA.FONT.SIZE", ppt.extra_font_size);
					get_font();
					get_metrics();
					get_images();
					brw.repaint();
				}, 100);
			}
		}
	} else {
		scroll -= step * brw.rowHeight * ppt.rowScrollStep;
		scroll = check_scroll(scroll)
		brw.on_mouse("wheel", m_x, m_y, step);
	}
}

function get_metrics() {
	switch (ppt.panelMode) {
	case 0:
	case 1:
		ppt.rowScrollStep = 3;
		break;
	case 2:
	case 3:
		ppt.rowScrollStep = 1;
		break;
	}

	if (ppt.showHeaderBar) {
		ppt.headerBarHeight = scale(ppt.defaultHeaderBarHeight);
	} else {
		ppt.headerBarHeight = 0;
	}
	cScrollBar.width = scale(cScrollBar.defaultWidth);
	cScrollBar.minCursorHeight = scale(cScrollBar.defaultMinCursorHeight);

	cFilterBox.w = scale(cFilterBox.default_w);
	cFilterBox.h = scale(cFilterBox.default_h);

	if (brw) brw.setSize();
}

function on_key_up(vkey) {
	cScrollBar.timerCounter = -1;
	if (cScrollBar.timerID) {
		window.ClearTimeout(cScrollBar.timerID);
		cScrollBar.timerID = false;
	}
	brw.repaint();
}

function on_key_down(vkey) {
	if (ppt.showHeaderBar) {
		g_filterbox.inputbox.on_key_down(vkey);
	}

	if (GetKeyboardMask() == KMask.ctrl) {
		if (vkey == 84) { // CTRL+T
			ppt.showHeaderBar = !ppt.showHeaderBar;
			window.SetProperty("SMOOTH.SHOW.TOP.BAR", ppt.showHeaderBar);
			get_metrics();
			brw.repaint();
		}
		if (vkey == 48 || vkey == 96) { // CTRL+0
			if (ppt.extra_font_size > 0) {
				ppt.extra_font_size = 0;
				window.SetProperty("SMOOTH.EXTRA.FONT.SIZE", ppt.extra_font_size);
				get_font();
				get_metrics();
				get_images();
				brw.repaint();
			}
		}
	}
}

function on_char(code) {
	if (ppt.showHeaderBar && g_filterbox.inputbox.edit) {
		g_filterbox.inputbox.on_char(code);
	}
}

function on_library_items_added() {
	brw.populate();
}

function on_library_items_removed() {
	brw.populate();
}

function on_library_items_changed() {
	brw.populate();
}

function on_metadb_changed() {
	brw.populate();
}

function on_focus(is_focused) {
	if (!is_focused) {
		brw.repaint();
	}
}

function check_scroll(scroll___) {
	if (scroll___ < 0)
		scroll___ = 0;
	var g1 = brw.h - (brw.totalRowsVis * brw.rowHeight);
	var end_limit = (brw.rowsCount * brw.rowHeight) - (brw.totalRowsVis * brw.rowHeight) - g1;
	if (scroll___ != 0 && scroll___ > end_limit) {
		scroll___ = end_limit;
	}
	return scroll___;
}

function g_sendResponse() {
	if (g_filterbox.inputbox.text.length == 0) {
		g_filter_text = "";
	} else {
		g_filter_text = g_filterbox.inputbox.text;
	}
	brw.populate();
}

var g_rightClickedIndex = -1;

get_font();
get_colors();
get_metrics();

var brw = new oBrowser();
var g_filterbox = new oFilterBox();
