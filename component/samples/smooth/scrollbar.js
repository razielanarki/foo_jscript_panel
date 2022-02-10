var cScrollBar = {
	visible: true,
	defaultWidth: 17,
	width: 17,
	ButtonType: {
		cursor: 0,
		up: 1,
		down: 2
	},
	defaultMinCursorHeight: 20,
	minCursorHeight: 20,
	timerID: false,
	timerCounter: -1
};

function oScrollbar() {
	this.buttons = new Array(3);
	this.buttonClick = false;

	this.setNewColors = function () {
		this.setButtons();
		this.setCursorButton();
	}

	this.setButtons = function () {
		// normal scroll_up Image
		this.upImage_normal = gdi.CreateImage(this.w, this.w);
		var gb = this.upImage_normal.GetGraphics();
		gb.SetTextRenderingHint(4);
		gb.DrawString(chars.up, g_font_awesome, g_color_normal_txt & 0x55ffffff, 0, 0, this.w, this.w, cc_stringformat);
		this.upImage_normal.ReleaseGraphics(gb);

		// hover scroll_up Image
		this.upImage_hover = gdi.CreateImage(this.w, this.w);
		gb = this.upImage_hover.GetGraphics();
		gb.SetTextRenderingHint(4);
		gb.DrawString(chars.up, g_font_awesome, g_color_normal_txt & 0x99ffffff, 0, 0, this.w, this.w, cc_stringformat);
		this.upImage_hover.ReleaseGraphics(gb);

		// down scroll_up Image
		this.upImage_down = gdi.CreateImage(this.w, this.w);
		gb = this.upImage_down.GetGraphics();
		gb.SetTextRenderingHint(4);
		gb.DrawString(chars.up, g_font_awesome, g_color_normal_txt, 0, 0, this.w, this.w, cc_stringformat);
		this.upImage_down.ReleaseGraphics(gb);

		// normal scroll_down Image
		this.downImage_normal = gdi.CreateImage(this.w, this.w);
		gb = this.downImage_normal.GetGraphics();
		gb.SetTextRenderingHint(4);
		gb.DrawString(chars.down, g_font_awesome, g_color_normal_txt & 0x55ffffff, 0, 0, this.w, this.w, cc_stringformat);
		this.downImage_normal.ReleaseGraphics(gb);

		// hover scroll_down Image
		this.downImage_hover = gdi.CreateImage(this.w, this.w);
		gb = this.downImage_hover.GetGraphics();
		gb.SetTextRenderingHint(4);
		gb.DrawString(chars.down, g_font_awesome, g_color_normal_txt & 0x99ffffff, 0, 0, this.w, this.w, cc_stringformat);
		this.downImage_hover.ReleaseGraphics(gb);

		// down scroll_down Image
		this.downImage_down = gdi.CreateImage(this.w, this.w);
		gb = this.downImage_down.GetGraphics();
		gb.SetTextRenderingHint(4);
		gb.DrawString(chars.down, g_font_awesome, g_color_normal_txt, 0, 0, this.w, this.w, cc_stringformat);
		this.downImage_down.ReleaseGraphics(gb);

		this.buttons[cScrollBar.ButtonType.up] = new button(this.upImage_normal, this.upImage_hover, this.upImage_down);
		this.buttons[cScrollBar.ButtonType.down] = new button(this.downImage_normal, this.downImage_hover, this.downImage_down);
	}

	this.setCursorButton = function () {
		// normal cursor Image
		this.cursorImage_normal = gdi.CreateImage(this.cursorw, this.cursorh);
		var gb = this.cursorImage_normal.GetGraphics();
		gb.FillSolidRect(1, 0, this.cursorw - 2, this.cursorh, blendColors(g_color_normal_txt, g_color_normal_bg, 0.5) & 0x88ffffff);
		gb.DrawRect(1, 0, this.cursorw - 2 - 1, this.cursorh - 1, 1.0, g_color_normal_txt & 0x44ffffff);
		this.cursorImage_normal.ReleaseGraphics(gb);

		// hover cursor Image
		this.cursorImage_hover = gdi.CreateImage(this.cursorw, this.cursorh);
		gb = this.cursorImage_hover.GetGraphics();
		gb.FillSolidRect(1, 0, this.cursorw - 2, this.cursorh, blendColors(g_color_normal_txt, g_color_normal_bg, 0.3) & 0x88ffffff);
		gb.DrawRect(1, 0, this.cursorw - 2 - 1, this.cursorh - 1, 1.0, g_color_normal_txt & 0x44ffffff);
		this.cursorImage_hover.ReleaseGraphics(gb);

		// down cursor Image
		this.cursorImage_down = gdi.CreateImage(this.cursorw, this.cursorh);
		gb = this.cursorImage_down.GetGraphics();
		gb.FillSolidRect(1, 0, this.cursorw - 2, this.cursorh, blendColors(g_color_normal_txt, g_color_normal_bg, 0.05) & 0x88ffffff);
		gb.DrawRect(1, 0, this.cursorw - 2 - 1, this.cursorh - 1, 1.0, g_color_normal_txt & 0x44ffffff);
		this.cursorImage_down.ReleaseGraphics(gb);

		// create/refresh cursor Button in buttons array
		this.buttons[cScrollBar.ButtonType.cursor] = new button(this.cursorImage_normal, this.cursorImage_hover, this.cursorImage_down);
		this.buttons[cScrollBar.ButtonType.cursor].x = this.x;
		this.buttons[cScrollBar.ButtonType.cursor].y = this.cursory;
	}

	this.draw = function (gr) {
		// scrollbar background
		gr.FillSolidRect(this.x, this.y, this.w, this.h, g_color_normal_bg & 0x25ffffff);
		gr.FillSolidRect(this.x, this.y, 1, this.h, g_color_normal_txt & 0x05ffffff);

		// scrollbar buttons
		if (cScrollBar.visible) {
			this.buttons[cScrollBar.ButtonType.cursor].draw(gr, this.x, this.cursory, 200);
		}
		this.buttons[cScrollBar.ButtonType.up].draw(gr, this.x, this.y, 200);
		this.buttons[cScrollBar.ButtonType.down].draw(gr, this.x, this.areay + this.areah, 200);
	}

	this.updateScrollbar = function () {
		var prev_cursorh = this.cursorh;
		this.total = typeof brw.rowsCount == "number" ? brw.rowsCount : brw.rows.length;
		this.rowh = typeof brw.rowHeight == "number" ? brw.rowHeight : ppt.rowHeight;
		this.totalh = this.total * this.rowh;
		// set scrollbar visibility
		cScrollBar.visible = this.totalh > brw.h;
		// set cursor width/height
		this.cursorw = cScrollBar.width;
		if (this.total > 0) {
			this.cursorh = Math.round((brw.h / this.totalh) * this.areah);
			if (this.cursorh < cScrollBar.minCursorHeight)
				this.cursorh = cScrollBar.minCursorHeight;
		} else {
			this.cursorh = cScrollBar.minCursorHeight;
		}
		// set cursor y pos
		this.setCursorY();
		if (this.cursorw && this.cursorh && this.cursorh != prev_cursorh)
			this.setCursorButton();
	}

	this.setCursorY = function () {
		// set cursor y pos
		var ratio = scroll / (this.totalh - brw.h);
		this.cursory = this.areay + Math.round((this.areah - this.cursorh) * ratio);
	}

	this.setSize = function () {
		this.buttonh = cScrollBar.width;
		this.x = brw.x + brw.w;
		this.y = brw.y - ppt.headerBarHeight * 0;
		this.w = cScrollBar.width;
		this.h = brw.h + ppt.headerBarHeight * 0;
		this.areay = this.y + this.buttonh;
		this.areah = this.h - (this.buttonh * 2);
		this.setButtons();
	}

	this.setScrollFromCursorPos = function () {
		// calc ratio of the scroll cursor to calc the equivalent item for the full list (with gh)
		var ratio = (this.cursory - this.areay) / (this.areah - this.cursorh);
		// calc idx of the item (of the full list with gh) to display at top of the panel list (visible)
		scroll = Math.round((this.totalh - brw.h) * ratio);
	}

	this.cursorCheck = function (event, x, y) {
		if (!this.buttons[cScrollBar.ButtonType.cursor])
			return;
		switch (event) {
		case "down":
			var tmp = this.buttons[cScrollBar.ButtonType.cursor].checkstate(event, x, y);
			if (tmp == ButtonStates.down) {
				this.cursorClickX = x;
				this.cursorClickY = y;
				this.cursorDrag = true;
				this.cursorDragDelta = y - this.cursory;
			}
			break;
		case "up":
			this.buttons[cScrollBar.ButtonType.cursor].checkstate(event, x, y);
			if (this.cursorDrag) {
				this.setScrollFromCursorPos();
				brw.repaint();
			}
			this.cursorClickX = 0;
			this.cursorClickY = 0;
			this.cursorDrag = false;
			break;
		case "move":
			this.buttons[cScrollBar.ButtonType.cursor].checkstate(event, x, y);
			if (this.cursorDrag) {
				this.cursory = y - this.cursorDragDelta;
				if (this.cursory + this.cursorh > this.areay + this.areah) {
					this.cursory = (this.areay + this.areah) - this.cursorh;
				}
				if (this.cursory < this.areay) {
					this.cursory = this.areay;
				}
				this.setScrollFromCursorPos();
				brw.repaint();
			}
			break;
		}
	}

	this._isHover = function (x, y) {
		return (x >= this.x && x <= this.x + this.w && y >= this.y && y <= this.y + this.h);
	}

	this._isHoverArea = function (x, y) {
		return (x >= this.x && x <= this.x + this.w && y >= this.areay && y <= this.areay + this.areah);
	}

	this._isHoverCursor = function (x, y) {
		return (x >= this.x && x <= this.x + this.w && y >= this.cursory && y <= this.cursory + this.cursorh);
	}

	this.on_mouse = function (event, x, y, delta) {
		this.isHover = this._isHover(x, y);
		this.isHoverArea = this._isHoverArea(x, y);
		this.isHoverCursor = this._isHoverCursor(x, y);
		this.isHoverButtons = this.isHover && !this.isHoverCursor && !this.isHoverArea;
		this.isHoverEmptyArea = this.isHoverArea && !this.isHoverCursor;

		var scroll_step = this.rowh;
		var scroll_step_page = brw.h;

		switch (event) {
		case "down":
		case "dblclk":
			if ((this.isHoverCursor || this.cursorDrag) && !this.buttonClick && !this.isHoverEmptyArea) {
				this.cursorCheck(event, x, y);
			} else {
				// buttons events
				var bt_state = ButtonStates.normal;
				for (var i = 1; i < 3; i++) {
					switch (i) {
					case 1: // up button
						bt_state = this.buttons[i].checkstate(event, x, y);
						if ((event == "down" && bt_state == ButtonStates.down) || (event == "dblclk" && bt_state == ButtonStates.hover)) {
							this.buttonClick = true;
							scroll = scroll - scroll_step;
							scroll = check_scroll(scroll);
							if (!cScrollBar.timerID) {
								cScrollBar.timerID = window.SetInterval(function () {
									if (cScrollBar.timerCounter > 6) {
										scroll = scroll - scroll_step;
										scroll = check_scroll(scroll);
									} else {
										cScrollBar.timerCounter++;
									}
								}, 80);
							}
						}
						break;
					case 2: // down button
						bt_state = this.buttons[i].checkstate(event, x, y);
						if ((event == "down" && bt_state == ButtonStates.down) || (event == "dblclk" && bt_state == ButtonStates.hover)) {
							this.buttonClick = true;
							scroll = scroll + scroll_step;
							scroll = check_scroll(scroll);
							if (!cScrollBar.timerID) {
								cScrollBar.timerID = window.SetInterval(function () {
									if (cScrollBar.timerCounter > 6) {
										scroll = scroll + scroll_step;
										scroll = check_scroll(scroll);
									} else {
										cScrollBar.timerCounter++;
									}
								}, 80);
							}
						}
						break;
					}
				}
				if (!this.buttonClick && this.isHoverEmptyArea) {
					// check click on empty area scrollbar
					if (y < this.cursory) {
						// up
						this.buttonClick = true;
						scroll = scroll - scroll_step_page;
						scroll = check_scroll(scroll);
						if (!cScrollBar.timerID) {
							cScrollBar.timerID = window.SetInterval(function () {
								if (cScrollBar.timerCounter > 6 && m_y < brw.scrollbar.cursory) {
									scroll = scroll - scroll_step_page;
									scroll = check_scroll(scroll);
								} else {
									cScrollBar.timerCounter++;
								}
							}, 80);
						}
					} else {
						// down
						this.buttonClick = true;
						scroll = scroll + scroll_step_page;
						scroll = check_scroll(scroll);
						if (!cScrollBar.timerID) {
							cScrollBar.timerID = window.SetInterval(function () {
								if (cScrollBar.timerCounter > 6 && m_y > brw.scrollbar.cursory + brw.scrollbar.cursorh) {
									scroll = scroll + scroll_step_page;
									scroll = check_scroll(scroll);
								} else {
									cScrollBar.timerCounter++;
								}
							}, 80);
						}
					}
				}
			}
			break;
		case "right":
		case "up":
			if (cScrollBar.timerID) {
				window.ClearInterval(cScrollBar.timerID);
				cScrollBar.timerID = false;
			}
			cScrollBar.timerCounter = -1;

			this.cursorCheck(event, x, y);
			for (var i = 1; i < 3; i++) {
				this.buttons[i].checkstate(event, x, y);
			}
			this.buttonClick = false;
			break;
		case "move":
			this.cursorCheck(event, x, y);
			for (var i = 1; i < 3; i++) {
				this.buttons[i].checkstate(event, x, y);
			}
			break;
		case "wheel":
			if (!this.buttonClick) {
				this.updateScrollbar();
			}
			break;
		}
	}
}
