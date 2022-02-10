function oPlaylist(idx, parent) {
	this.idx = idx;
	this.name = plman.GetPlaylistName(idx);
	this.bt_remove = new button(parent.bt_remove_normal, parent.bt_remove_hover, parent.bt_remove_down);
	this.y = -1;
}

function oPlaylistManager() {
	this.h = p.list.h;
	this.woffset = 0;
	this.border = 2;
	this.playlists = [];
	this.offset = 0;
	this.totalRows = Math.floor(this.area_h / cPlaylistManager.rowHeight);
	this.scrollbar = new oScrollBar("p.playlistManager", 0, 0, cScrollBar.width, 0, 0, cPlaylistManager.rowHeight);
	this.scrollbarWidth = 0;
	this.inputbox = null;
	this.inputboxID = -1;

	this.sortNameAsc = function (obj_a, obj_b) {
		return (obj_a.name.toLowerCase() > obj_b.name.toLowerCase() ? 1 : -1);
	}
	this.sortNameDesc = function (obj_a, obj_b) {
		return (obj_a.name.toLowerCase() < obj_b.name.toLowerCase() ? 1 : -1);
	}

	this.setButtons = function () {
		var bt_w = g_z16;
		var bt_h = g_z16;

		// Az & zA vars
		var Az_h = cPlaylistManager.statusBarHeight;
		var Az_w = Math.floor(Az_h * 1.9);
		var left_padding = g_z2;
		var right_padding = 4;

		// normal sort Az playlist Image
		this.bt_sortAz_normal = gdi.CreateImage(Az_w, Az_h);
		var gb = this.bt_sortAz_normal.GetGraphics();
		gb.SetTextRenderingHint(3);
		gb.FillSolidRect(0, 0, 1, Az_h, blendColors(g_color_normal_bg, g_color_normal_txt, 0.35));
		gb.DrawString(chars.up, g_font_awesome_12, blendColors(g_color_normal_bg, g_color_normal_txt, 0.5), left_padding, 1, Az_w, Az_h, lc_stringformat);
		gb.DrawString("Az", g_font_12, blendColors(g_color_normal_bg, g_color_normal_txt, 0.5), 0, 0, Az_w - right_padding, Az_h, rc_stringformat);
		gb.FillSolidRect(Az_w - 1, 0, 1, Az_h, blendColors(g_color_normal_bg, g_color_normal_txt, 0.35));
		this.bt_sortAz_normal.ReleaseGraphics(gb);

		// hover sort Az playlist Image
		this.bt_sortAz_hover = gdi.CreateImage(Az_w, Az_h);
		var gb = this.bt_sortAz_hover.GetGraphics();
		gb.SetTextRenderingHint(3);
		gb.FillSolidRect(0, 0, 1, Az_h, blendColors(g_color_normal_bg, g_color_normal_txt, 0.35));
		gb.DrawString(chars.up, g_font_awesome_12, g_color_normal_txt, left_padding, 1, Az_w, Az_h, lc_stringformat);
		gb.DrawString("Az", g_font_12, g_color_normal_txt, 0, 0, Az_w - right_padding, Az_h, rc_stringformat);
		gb.FillSolidRect(Az_w - 1, 0, 1, Az_h, blendColors(g_color_normal_bg, g_color_normal_txt, 0.35));
		this.bt_sortAz_hover.ReleaseGraphics(gb);

		this.sortAz_button = new button(this.bt_sortAz_normal, this.bt_sortAz_hover, this.bt_sortAz_hover);

		// normal sort Za playlist Image
		this.bt_sortZa_normal = gdi.CreateImage(Az_w, Az_h);
		var gb = this.bt_sortZa_normal.GetGraphics();
		gb.SetTextRenderingHint(3);
		gb.DrawString(chars.down, g_font_awesome_12, blendColors(g_color_normal_bg, g_color_normal_txt, 0.5), left_padding, 1, Az_w, Az_h, lc_stringformat);
		gb.DrawString("Za  ", g_font_12, blendColors(g_color_normal_bg, g_color_normal_txt, 0.5), 0, 0, Az_w - right_padding, Az_h, rc_stringformat);
		gb.FillSolidRect(Az_w - 1, 0, 1, Az_h, blendColors(g_color_normal_bg, g_color_normal_txt, 0.35));
		this.bt_sortZa_normal.ReleaseGraphics(gb);

		// hover sort Za playlist Image
		this.bt_sortZa_hover = gdi.CreateImage(Az_w, Az_h);
		var gb = this.bt_sortZa_hover.GetGraphics();
		gb.SetTextRenderingHint(3);
		gb.DrawString(chars.down, g_font_awesome_12, g_color_normal_txt, left_padding, 1, Az_w, Az_h, lc_stringformat);
		gb.DrawString("Za  ", g_font_12, g_color_normal_txt, 0, 0, Az_w - right_padding, Az_h, rc_stringformat);
		gb.FillSolidRect(Az_w - 1, 0, 1, Az_h, blendColors(g_color_normal_bg, g_color_normal_txt, 0.35));
		this.bt_sortZa_hover.ReleaseGraphics(gb);

		this.sortZa_button = new button(this.bt_sortZa_normal, this.bt_sortZa_hover, this.bt_sortZa_hover);

		// normal remove playlist Image
		this.bt_remove_normal = gdi.CreateImage(bt_w, bt_h);
		var gb = this.bt_remove_normal.GetGraphics();
		gb.SetTextRenderingHint(4);
		gb.DrawString(chars.close, g_font_awesome_12, blendColors(g_color_normal_bg, g_color_normal_txt, 0.5), 0, 0, bt_w, bt_h, cc_stringformat);
		this.bt_remove_normal.ReleaseGraphics(gb);

		// hover remove playlist Image
		this.bt_remove_hover = gdi.CreateImage(bt_w, bt_h);
		gb = this.bt_remove_hover.GetGraphics();
		gb.SetTextRenderingHint(4);
		gb.DrawString(chars.close, g_font_awesome_12, RGB(255, 0, 0), 0, 0, bt_w, bt_h, cc_stringformat);
		this.bt_remove_hover.ReleaseGraphics(gb);

		// down remove playlist Image
		this.bt_remove_down = gdi.CreateImage(bt_w, bt_h);
		gb = this.bt_remove_down.GetGraphics();
		gb.SetTextRenderingHint(4);
		gb.DrawString(chars.close, g_font_awesome_12, g_color_normal_txt, 0, 0, bt_w, bt_h, cc_stringformat);
		this.bt_remove_down.ReleaseGraphics(gb);
	}
	this.setButtons();

	this.repaint = function () {
		window.RepaintRect(this.x - this.woffset, this.y, this.w, this.h);
	}

	this.setSize = function (x, y, w, h) {
		this.x = x;
		this.y = y;
		this.w = w;
		this.h = h;

		if (cPlaylistManager.visible) {
			this.woffset = this.w;
		} else {
			this.woffset = 0;
		}

		this.totalRows = Math.floor((h - cPlaylistManager.rowHeight - (cPlaylistManager.showStatusBar ? cPlaylistManager.statusBarHeight : 0)) / cPlaylistManager.rowHeight);
		this.offset = 0;

		// scrollbar resize
		this.scrollbar.reSize(this.x - this.woffset + this.w - cScrollBar.width, this.y + cPlaylistManager.rowHeight, cScrollBar.width, this.h - cPlaylistManager.rowHeight - (cPlaylistManager.showStatusBar ? cPlaylistManager.statusBarHeight : 0), 0, cPlaylistManager.rowHeight, this.offset);
		if (this.scrollbar.visible) {
			this.scrollbarWidth = this.scrollbar.w;
		} else {
			this.scrollbarWidth = 0;
		}
	}

	this.refresh = function () {
		this.playlists = [];
		this.rowTotal = plman.PlaylistCount;
		for (var i = 0; i < this.rowTotal; i++) {
			this.playlists.push(new oPlaylist(i, this));
		}
		this.max = this.rowTotal > this.totalRows ? this.totalRows : this.rowTotal;
		this.offset = 0;
		this.scrollbar.reSet(this.rowTotal, cPlaylistManager.rowHeight, this.offset);
		if (this.scrollbar.visible) {
			this.scrollbarWidth = this.scrollbar.w;
		} else {
			this.scrollbarWidth = 0;
		}
	}

	this.draw = function (gr) {
		if (this.woffset == 0) return;

		var cx = this.x - this.woffset + this.border + 5;
		var cw = this.w - this.border - this.scrollbarWidth - 10;
		var ch = cPlaylistManager.rowHeight;
		var row_idx = 0;
		var bt_w = this.bt_remove_normal.Width;

		// panel bg
		gr.FillSolidRect(this.x - this.woffset + 1, this.y + this.border, this.w, this.h - this.border, SetAlpha(g_color_normal_bg, 230));
		gr.DrawRect(this.x - this.woffset + this.border, this.y, this.w - this.border, this.h, 1, SetAlpha(g_color_normal_txt, 150));

		// panel header
		if (this.ishoverHeader) {
			if (g_drag_drop_playlist_manager_hover) {
				gr.FillSolidRect(this.x - this.woffset + this.border, this.y, this.w - this.border, ch - 1, g_color_normal_txt & 0x25ffffff);
				gr.DrawRect(this.x - this.woffset + this.border + 1, this.y + 1, this.w - this.border - 2, ch - 1 - 2, 2, g_color_normal_txt);
				gr.GdiDrawText("TO A NEW PLAYLIST", g_font_12, blendColors(g_color_normal_txt, g_color_normal_bg, 0.25), cx + g_z5, this.y, cw - g_z10, ch, DT_LEFT | DT_CALCRECT | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
			} else {
				gr.FillGradRect(this.x - this.woffset + this.border, this.y, this.w - this.border, ch - 1, 90, g_color_normal_txt & 0x15ffffff, 0, 0.96);
				gr.GdiDrawText("PLAYLISTS", g_font_12, blendColors(g_color_normal_txt, g_color_normal_bg, 0.25), cx, this.y, cw - g_z5, ch, DT_LEFT | DT_CALCRECT | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
			}
		} else {
			if (g_drag_drop_playlist_manager_hover) {
				gr.FillGradRect(this.x - this.woffset + this.border, this.y, this.w - this.border, ch - 1, 90, g_color_normal_txt & 0x15ffffff, 0, 0.96);
				gr.GdiDrawText("TO A NEW PLAYLIST", g_font_12, blendColors(g_color_normal_txt, g_color_normal_bg, 0.25), cx + g_z5, this.y, cw - g_z10, ch, DT_LEFT | DT_CALCRECT | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
			} else {
				gr.FillGradRect(this.x - this.woffset + this.border, this.y, this.w - this.border, ch - 1, 90, g_color_normal_txt & 0x15ffffff, 0, 0.96);
				gr.GdiDrawText("PLAYLISTS", g_font_12, blendColors(g_color_normal_txt, g_color_normal_bg, 0.25), cx, this.y, cw - g_z5, ch, DT_LEFT | DT_CALCRECT | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
			}
		}

		gr.FillSolidRect(this.x - this.woffset + this.border, this.y + ch - 2, this.w - this.border, 1, g_color_normal_txt & 0x30ffffff);
		// draw flashing header on lbtn_up after a drag'n drop
		if (cPlaylistManager.blink_counter > -1) {
			if (cPlaylistManager.blink_id == -1) {
				if (cPlaylistManager.blink_counter <= 5 && Math.floor(cPlaylistManager.blink_counter / 2) == Math.ceil(cPlaylistManager.blink_counter / 2)) {
					gr.DrawRect(this.x - this.woffset + this.border + 1, this.y + 1, this.w - this.border - 2, ch - 3, 2, g_color_normal_txt);
				}
			}
		}

		cx = this.x - this.woffset + this.border;
		for (var i = this.offset; i < this.playlists.length; i++) {
			var cy = this.y + cPlaylistManager.rowHeight + row_idx * ch;
			this.playlists[i].y = cy;
			var txt_color = g_color_normal_txt;
			var is_hovering = g_drag_drop_playlist_manager_hover && i == this.hoverId && playlist_can_add_items(this.playlists[i].idx);

			// draw selected bg if active or being hovered over
			if (this.playlists[i].idx == g_active_playlist || is_hovering) {
				txt_color = g_color_normal_txt == g_color_selected_bg ? RGB(255, 255, 255) : g_color_normal_txt; // update txt_color if it clashes with bg
				gr.FillSolidRect(cx + 1, cy + 1, this.w - this.border - this.scrollbarWidth - 2, ch - 2, SetAlpha(g_color_selected_bg, 150));
				if (is_hovering) { // only draw outline if hovering
					gr.DrawRect(cx + 1, cy + 1, this.w - this.border - this.scrollbarWidth - 2, ch - 2, 1, SetAlpha(txt_color, 190));
				}
			}

			// right clicked item
			if (cPlaylistManager.rightClickedId == i) {
				gr.DrawRect(cx + 1, cy + 1, this.w - this.border - this.scrollbarWidth - 2, ch - 2, 2, g_color_normal_txt);
			}

			// draw flashing item on lbtn_up after a drag'n drop
			if (cPlaylistManager.blink_counter > -1) {
				if (i == cPlaylistManager.blink_id) {
					if (cPlaylistManager.blink_counter <= 5 && Math.floor(cPlaylistManager.blink_counter / 2) == Math.ceil(cPlaylistManager.blink_counter / 2)) {
						gr.DrawRect(cx + 1, cy + 1, this.w - this.border - this.scrollbarWidth - 3, ch - 3, 2, txt_color);
					}
				}
			}

			gr.SetTextRenderingHint(5);
			gr.DrawString(plman.IsPlaylistLocked(this.playlists[i].idx) ? chars.lock : chars.list, g_font_awesome_20, txt_color, cx, cy, ch, ch, cc_stringformat);

			// draw INPUTBOX if rename requested
			if (this.inputboxID == i) {
				this.inputbox.draw(gr, this.x - this.woffset + this.border + scale(30), cy + 5);
			} else {
				// playlist total items
				var count = plman.PlaylistItemCount(this.playlists[i].idx);
				gr.GdiDrawText(count, g_font_12, txt_color, cx + g_z10, cy, cw - g_z5 - bt_w, ch, DT_RIGHT | DT_CALCRECT | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);

				var count_width = gr.CalcTextWidth("99999", g_font_12);
				// draw playlist name
				gr.GdiDrawText(this.playlists[i].name, g_font_12, txt_color, cx + g_z10 + g_font_12.Height, cy, cw - (g_z10 * 3) - bt_w - count_width, ch, DT_LEFT | DT_CALCRECT | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);

				// draw remove button
				this.playlists[i].bt_remove.draw(gr, cx + cw - bt_w + 9, cy + 2);
			}

			// draw "drag destination bar" on dragging playlist item
			if (this.ishoverItem && !cPlaylistManager.vscroll_timeout) {
				if (cPlaylistManager.drag_target_id != this.rowTotal && cPlaylistManager.drag_target_id == i) {
					if (cPlaylistManager.drag_target_id > cPlaylistManager.drag_source_id) {
						gr.DrawRect(cx + 1, cy + cPlaylistManager.rowHeight, this.w - this.border - this.scrollbarWidth - 2, 1, 2, txt_color);
					} else if (cPlaylistManager.drag_target_id < cPlaylistManager.drag_source_id) {
						gr.DrawRect(cx + 1, cy, this.w - this.border - this.scrollbarWidth - 2, 1, 2, txt_color);
					}
				}
			} else {
				cPlaylistManager.drag_target_id = -1;
			}

			row_idx++;
		}

		// panel footer
		if (cPlaylistManager.showStatusBar) {
			var fx = this.x - this.woffset + this.border;
			var fy = this.y + this.h - cPlaylistManager.statusBarHeight;
			var fw = this.w - this.border;
			var fh = cPlaylistManager.statusBarHeight;
			gr.FillSolidRect(fx, fy, fw, fh, blendColors(g_color_normal_bg, g_color_normal_txt, 0.01) & 0xf0ffffff);
			gr.FillGradRect(fx, fy, fw, fh, 90, g_color_normal_txt & 0x15ffffff, 0, 0.96);
			gr.FillSolidRect(fx, fy, fw, 1, g_color_normal_txt & 0x50ffffff);
			var status_txt = this.playlists.length + (this.playlists.length > 1 ? " PLAYLISTS" : " PLAYLIST");
			gr.GdiDrawText(status_txt, g_font_12, blendColors(g_color_normal_txt, g_color_normal_bg, 0.25), fx + 5, fy, fw - 10, fh, DT_RIGHT | DT_CALCRECT | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
			// draw sort buttons
			this.sortAz_button.draw(gr, fx, fy);
			this.sortZa_button.draw(gr, fx + this.sortAz_button.img[0].Width, fy);
		}

		// draw scrollbar
		if (this.scrollbarWidth > 0) {
			this.scrollbar.drawXY(gr, this.x - this.woffset + this.w - this.scrollbarWidth, this.y + cPlaylistManager.rowHeight);
		}
	}

	this.isHoverObject = function (x, y) {
		return (x > this.x - this.woffset && x < this.x - this.woffset + this.w - this.scrollbarWidth && y > this.y && y < this.y + this.h);
	}

	this.check = function (event, x, y, delta) {
		this.ishover = this.isHoverObject(x, y);
		this.ishoverHeader = x > this.x - this.woffset && x < this.x - this.woffset + this.w + this.scrollbarWidth && y > this.y && y <= this.y + cPlaylistManager.rowHeight;

		this.ishoverItem = x > this.x - this.woffset && x < this.x - this.woffset + this.w - this.scrollbarWidth && y > this.y + cPlaylistManager.rowHeight && y < this.y + this.h;
		if (this.ishoverItem) {
			this.hoverId = Math.floor((y - ((this.y + cPlaylistManager.rowHeight) - this.offset * cPlaylistManager.rowHeight)) / cPlaylistManager.rowHeight);
			if (this.hoverId >= this.playlists.length)
				this.hoverId = -1;
		} else {
			this.hoverId = -1;
		}

		if (this.hoverId > -1) {
			if (this.hoverId == this.inputboxID) {
				var inputbox_ishover = x > this.inputbox.x - 3 && x < this.inputbox.x + this.inputbox.w + 6 && y > this.inputbox.y && y < this.inputbox.y + this.inputbox.h;
				this.ishover = this.ishover && !inputbox_ishover;
			}
		}

		switch (event) {
		case "down":
			this.sortAz_button.checkstate(event, x, y);
			this.sortZa_button.checkstate(event, x, y);

			if (this.inputboxID >= 0) {
				this.inputbox.check("down", x, y);
			}

			if (this.hoverId > -1 && this.inputboxID == -1) {
				if (this.hoverId != g_active_playlist) {
					plman.ActivePlaylist = this.hoverId;
				}
				if (cPlaylistManager.visible) {
					// prepare drag item to reorder list
					cPlaylistManager.drag_clicked = true;
					cPlaylistManager.drag_x = x;
					cPlaylistManager.drag_y = y;
					cPlaylistManager.drag_source_id = this.hoverId;
				}
			} else if (this.scrollbar.visible) {
				this.scrollbar.check(event, x, y, delta);
			}
			break;
		case "dblclk":
			this.check("down", x, y);
			break;
		case "right":
			if (this.inputboxID >= 0) {
				this.inputbox.check("right", x, y);
			} else {
				if (this.hoverId > -1 && this.inputboxID == -1) {
					cPlaylistManager.rightClickedId = this.hoverId;
					full_repaint();
					this.contextMenu(x, y, this.playlists[this.hoverId].idx);
				} else if (this.ishover && this.inputboxID == -1) {
					this.contextMenu(x, y, null);
				}
			}
			break;
		case "up":
			if (this.inputboxID >= 0) {
				this.inputbox.check("up", x, y);
			} else {
				if (this.scrollbar.visible)
					this.scrollbar.check(event, x, y, delta);

				if (this.sortAz_button.checkstate(event, x, y) == ButtonStates.hover) {
					plman.SortPlaylistsByName(1);
					this.refresh();
					full_repaint();
				}
				if (this.sortZa_button.checkstate(event, x, y) == ButtonStates.hover) {
					plman.SortPlaylistsByName(-1);
					this.refresh();
					full_repaint();
				} else {
					// check remove button
					for (var pl = 0; pl < this.playlists.length; pl++) {
						if (this.playlists[pl].bt_remove.checkstate(event, x, y) == ButtonStates.hover) {
							plman.RemovePlaylistSwitch(pl);
							if (this.offset > 0 && this.offset >= this.playlists.length - Math.floor((this.h - (cPlaylistManager.showStatusBar ? cPlaylistManager.statusBarHeight : 0)) / cPlaylistManager.rowHeight)) {
								this.offset--;
								this.refresh();
							}
						}
					}
				}

				// hide playlist manager panel if not visible by default
				if (!cPlaylistManager.visible) {
					if (cPlaylistManager.hscroll_interval) {
						window.ClearTimeout(cPlaylistManager.hscroll_interval);
						cPlaylistManager.hscroll_interval = false;
					}
					if (p.playlistManager.woffset > 0) { // if panel opened
						cPlaylistManager.hscroll_interval = window.SetInterval(function () {
							full_repaint();
							if (!cPlaylistManager.blink_interval) {
								p.playlistManager.woffset -= cPlaylistManager.step;
							}
							if (p.playlistManager.woffset <= 0) {
								p.playlistManager.woffset = 0;
								window.ClearInterval(cPlaylistManager.hscroll_interval);
								cPlaylistManager.hscroll_interval = false;
								full_repaint();
							}
						}, 16);
					}
				}

				// drop item playlist
				if (cPlaylistManager.drag_target_id > -1) {
					if (cPlaylistManager.drag_target_id == this.rowTotal) {
						plman.MovePlaylist(this.playlists[cPlaylistManager.drag_source_id].idx, this.playlists[this.rowTotal - 1].idx);
					} else {
						cPlaylistManager.drag_dropped = (cPlaylistManager.drag_source_id != cPlaylistManager.drag_target_id);
						plman.MovePlaylist(this.playlists[cPlaylistManager.drag_source_id].idx, this.playlists[cPlaylistManager.drag_target_id].idx);
					}
				}

			}

			if (cPlaylistManager.drag_moved)
				window.SetCursor(IDC_ARROW);

			cPlaylistManager.drag_clicked = false;
			cPlaylistManager.drag_moved = false;
			cPlaylistManager.drag_source_id = -1;
			cPlaylistManager.drag_target_id = -1;
			cPlaylistManager.drag_x = -1;
			cPlaylistManager.drag_y = -1;
			break;
		case "drag_over":
			g_drag_drop_playlist_id = this.hoverId;
			break;
		case "move":
			if (!cPlaylistManager.drag_clicked) {
				this.sortAz_button.checkstate(event, x, y);
				this.sortZa_button.checkstate(event, x, y);
			}

			if (this.inputboxID >= 0) {
				this.inputbox.check("move", x, y);
			} else {
				if (this.scrollbar.visible)
					this.scrollbar.check(event, x, y, delta);
				if (cPlaylistManager.drag_moved) {
					if (!this.ishoverHeader) {
						if (this.hoverId > -1 && this.hoverId != cPlaylistManager.drag_source_id) {
							cPlaylistManager.drag_target_id = this.hoverId;
						} else if (y > this.playlists[this.rowTotal - 1].y + cPlaylistManager.rowHeight && y < this.playlists[this.rowTotal - 1].y + cPlaylistManager.rowHeight * 2) {
							cPlaylistManager.drag_target_id = this.rowTotal;
						} else {
							cPlaylistManager.drag_target_id = -1;
						}
					}
				} else {
					for (var pl = 0; pl < this.playlists.length; pl++) {
						this.playlists[pl].bt_remove.checkstate(event, x, y);
					}

					if (cPlaylistManager.drag_clicked) {
						cPlaylistManager.drag_moved = true;
					} else {
						if (cPlaylistManager.drag_move_timeout) {
							cPlaylistManager.drag_move_timeout = false;
							window.ClearTimeout(cPlaylistManager.drag_move_timeout);
						}
					}
				}
			}
			break;
		case "wheel":
			if (this.scrollbar.visible)
				this.scrollbar.check(event, x, y, delta);
			break;
		}
	}

	this.contextMenu = function (x, y, id) {
		var _menu = window.CreatePopupMenu();
		var _newplaylist = window.CreatePopupMenu();
		var _restore = window.CreatePopupMenu();

		_menu.AppendMenuItem(MF_STRING, 1, "Load Playlist...");
		_newplaylist.AppendMenuItem(MF_STRING, 2, "New Playlist");
		_newplaylist.AppendMenuItem(MF_STRING, 3, "New Autoplaylist");
		_newplaylist.AppendTo(_menu, MF_STRING, "Add");

		var count = plman.RecyclerCount;
		if (count > 0) {
			var history = [];
			for (var i = 0; i < count; i++) {
				history.push(i);
				_restore.AppendMenuItem(MF_STRING, 20 + i, plman.GetRecyclerName(i));
			}
			_restore.AppendMenuSeparator();
			_restore.AppendMenuItem(MF_STRING, 4, "Clear history");
			_restore.AppendTo(_menu, MF_STRING, "Restore...");
		}

		if (id != null) {
			_menu.AppendMenuSeparator();
			var lock_name = plman.GetPlaylistLockName(id);

			_menu.AppendMenuItem(MF_STRING, 5, "Duplicate this playlist");
			_menu.AppendMenuItem(playlist_can_rename(id) ? MF_STRING : MF_GRAYED, 6, "Rename this playlist\tF2");
			_menu.AppendMenuItem(playlist_can_remove(id) ? MF_STRING : MF_GRAYED, 7, "Remove this playlist\tDel");
			_menu.AppendMenuSeparator();
			if (plman.IsAutoPlaylist(id)) {
				_menu.AppendMenuItem(MF_STRING, 8, lock_name + " properties");
				_menu.AppendMenuItem(MF_STRING, 9, "Convert to a normal playlist");
			} else {
				var is_locked = plman.IsPlaylistLocked(id);
				var is_mine = lock_name == "JScript Panel";

				_menu.AppendMenuItem(is_mine || !is_locked ? MF_STRING : MF_GRAYED, 10, "Edit playlist lock...");
				_menu.AppendMenuItem(is_mine ? MF_STRING : MF_GRAYED, 11, "Remove playlist lock");
			}
		}

		var idx = _menu.TrackPopupMenu(x, y);

		switch (true) {
		case idx == 0:
			break;
		case idx == 1:
			fb.LoadPlaylist();
			break;
		case idx == 2:
			var pl = plman.CreatePlaylist();
			plman.ActivePlaylist = pl;
			this.inputbox = new oInputbox(this.w - this.border - this.scrollbarWidth - scale(40), cPlaylistManager.rowHeight - 10, plman.GetPlaylistName(pl), "", "renamePlaylist()", 0, 12, 225);
			this.inputboxID = pl;
			if (cPlaylistManager.inputbox_timeout) window.ClearTimeout(cPlaylistManager.inputbox_timeout);
			cPlaylistManager.inputbox_timeout = window.SetTimeout(inputboxPlaylistManager_activate, 20);
			break;
		case idx == 3:
			var pl = plman.CreateAutoPlaylist(plman.PlaylistCount, "", "");
			plman.ActivePlaylist = pl;
			plman.ShowAutoPlaylistUI(pl);
			this.inputbox = new oInputbox(this.w - this.border - this.scrollbarWidth - scale(40), cPlaylistManager.rowHeight - 10, plman.GetPlaylistName(pl), "", "renamePlaylist()", 0, 12, 225);
			this.inputboxID = pl;
			if (cPlaylistManager.inputbox_timeout) window.ClearTimeout(cPlaylistManager.inputbox_timeout);
			cPlaylistManager.inputbox_timeout = window.SetTimeout(inputboxPlaylistManager_activate, 20);
			break;
		case idx == 4:
			plman.RecyclerPurge(history);
			break;
		case idx == 5:
			plman.DuplicatePlaylist(id, "Copy of " + plman.GetPlaylistName(id));
			plman.ActivePlaylist = id + 1;
			break;
		case idx == 6:
			this.inputbox = new oInputbox(this.w - this.border - this.scrollbarWidth - scale(40), cPlaylistManager.rowHeight - 10, plman.GetPlaylistName(id), "", "renamePlaylist()", 0, 12, 225);
			this.inputboxID = id;
			if (cPlaylistManager.inputbox_timeout) window.ClearTimeout(cPlaylistManager.inputbox_timeout);
			cPlaylistManager.inputbox_timeout = window.SetTimeout(inputboxPlaylistManager_activate, 20);
			break;
		case idx == 7:
			plman.RemovePlaylistSwitch(id);
			if (this.offset > 0 && this.offset >= this.playlists.length - Math.floor((this.h - (cPlaylistManager.showStatusBar ? cPlaylistManager.statusBarHeight : 0)) / cPlaylistManager.rowHeight)) {
				this.offset--;
				this.refresh();
			}
			break;
		case idx == 8:
			plman.ShowAutoPlaylistUI(id);
			break;
		case idx == 9:
			plman.DuplicatePlaylist(id, plman.GetPlaylistName(id));
			plman.RemovePlaylist(id);
			plman.ActivePlaylist = id;
			break;
		case idx == 10:
			plman.ShowPlaylistLockUI(id);
			break;
		case idx == 11:
			plman.RemovePlaylistLock(id);
			break;
		case idx < 99:
			plman.RecyclerRestore(idx - 20);
			plman.ActivePlaylist = plman.PlaylistCount - 1;
			break;
		}
		_newplaylist.Dispose();
		_restore.Dispose();
		_menu.Dispose();
		cPlaylistManager.rightClickedId = null;
		full_repaint();
		return true;
	}
}
