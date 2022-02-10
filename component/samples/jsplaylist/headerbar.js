function oBorder() {
	this.leftId = arguments[0];
	this.rightId = arguments[1];
	this.percent = arguments[2];

	this.isHover = function (x, y) {
		return (x >= this.x - 3 && x <= this.x + this.w + 2 && y >= this.y && y <= this.y + this.h);
	}

	this.on_mouse = function (event, x, y) {
		this.ishover = (x >= this.x - 3 && x <= this.x + this.w + 2 && y >= this.y && y <= this.y + this.h);
		switch (event) {
		case "down":
			if (this.ishover) {
				this.sourceX = x;
				this.drag = true;
			}
			break;
		case "up":
			this.drag = false;
			break;
		case "move":
			if (this.drag) {
				this.delta = x - this.sourceX;
			}
			break;
		}
	}
}

function oColumn() {
	this.label = arguments[0];
	this.tf = arguments[1];
	this.tf2 = arguments[2];
	this.percent = arguments[3];
	this.ref = arguments[4];
	this.align = Math.round(arguments[5]);
	this.sortOrder = arguments[6];
	this.DT_align = (this.align == 0 ? DT_LEFT : (this.align == 2 ? DT_RIGHT : DT_CENTER));
	this.minWidth = 32;
	this.drag = false;

	this.isHover = function (x, y) {
		return (x > this.x && x < this.x + this.w && y >= this.y && y <= this.y + this.h);
	}

	this.on_mouse = function (event, x, y) {
		this.ishover = (x > this.x + 2 && x < this.x + this.w - 2 && y >= this.y && y <= this.y + this.h);
		switch (event) {
		case "down":
			if (this.ishover && this.percent > 0) {
				this.drag = true;
			}
			break;
		case "up":
			this.drag = false;
			break;
		}
	}
}

function oHeaderBar() {
	this.visible = true;
	this.columns = [];
	this.borders = [];
	this.totalColumns = window.GetProperty("SYSTEM.HeaderBar.TotalColumns", 0);
	this.borderDragged = false;
	this.borderDraggedId = -1;
	this.columnDragged = 0;
	this.columnDraggedId = null;
	this.columnRightClicked = -1;
	this.resetSortIndicators = function () {
		this.sortedColumnId = -1;
		this.sortedColumnDirection = 1;
	}
	this.resetSortIndicators();

	this.borderHover = false;
	this.clickX = 0;

	this.setButtons = function () {
		// normal playlistManager slide Image
		this.slide_close = gdi.CreateImage(cScrollBar.width, this.h);
		var gb = this.slide_close.GetGraphics();
		gb.FillSolidRect(0, 0, cScrollBar.width, this.h, g_color_normal_txt & 0x15ffffff);
		gb.SetTextRenderingHint(4);
		gb.DrawString(chars.right, g_font_awesome_12, g_color_normal_txt, 0, 1, cScrollBar.width, this.h, cc_stringformat);
		this.slide_close.ReleaseGraphics(gb);

		// hover playlistManager slide Image
		this.slide_open = gdi.CreateImage(cScrollBar.width, this.h);
		gb = this.slide_open.GetGraphics();
		gb.FillSolidRect(0, 0, cScrollBar.width, this.h, g_color_normal_txt & 0x15ffffff);
		gb.SetTextRenderingHint(4);
		gb.DrawString(chars.left, g_font_awesome_12, g_color_normal_txt, 0, 1, cScrollBar.width, this.h, cc_stringformat);
		this.slide_open.ReleaseGraphics(gb);

		if (cPlaylistManager.visible) {
			this.button = new button(this.slide_close, this.slide_close, this.slide_close);
		} else {
			this.button = new button(this.slide_open, this.slide_open, this.slide_open);
		}
	}

	this.setSize = function (x, y, w, h) {
		this.x = x;
		this.y = y;
		this.w = w - cScrollBar.width;
		this.h = cHeaderBar.height;
		this.setButtons();
		this.borderWidth = cHeaderBar.borderWidth;
	}

	this.calculateColumns = function () {
		var tmp = this.x;

		// calc column metrics for calculating border metrics as well
		for (var i = 0; i < this.totalColumns; i++) {
			if (this.columns[i].percent > 0) {
				this.columns[i].x = tmp;
				this.columns[i].y = this.y;
				this.columns[i].w = Math.abs(this.w * this.columns[i].percent / 100000);
				this.columns[i].h = this.h;
				if (i != this.columnDraggedId || (this.columnDragged == 1)) {
					// start >> on last column, adjust width of last drawn column to fit headerbar width!
					if (this.columns[i].x + this.columns[i].w >= this.x + this.w) { // if last col width go ahead of the max width (1 pixel more!) downsize col width !
						this.columns[i].w = (this.x + this.w) - this.columns[i].x + 1; // +1 to fit the whole headerbar width due to the -1 in draw width of each column in DrawRect below
					}
					if (this.columns[i].x + this.columns[i].w < this.x + this.w && this.columns[i].x + this.columns[i].w > this.x + this.w - 4) { // if there is a gap of 1 pixel at the end, fill it!
						this.columns[i].w = (this.x + this.w) - this.columns[i].x + 1;
					}
					// end <<
				}
				tmp = this.columns[i].x + this.columns[i].w;
			} else {
				this.columns[i].x = tmp;
				this.columns[i].y = this.y;
				this.columns[i].w = 0;
				this.columns[i].h = this.h;
			}
		}
	}

	this.drawHiddenPanel = function (gr) {
		gr.FillSolidRect(this.x, this.y, this.w + cScrollBar.width, 1, g_color_normal_txt & 0x15ffffff);
	}

	this.drawColumns = function (gr) {
		var cx = 0;
		var cy = 0;
		var cw = 0;
		var sx = 0;
		var bx = 0;

		// tweak to only reset mouse cursor to arrowafter a column sorting
		if (this.columnDragged_saved == 3 && this.columnDragged == 0) {
			this.columnDragged_saved = 0;
		}

		// calc column metrics for calculating border metrics as well
		this.calculateColumns();

		// draw borders and left column from each one!
		var tmp = this.x;
		for (var i = 0; i < this.borders.length; i++) {
			var j = this.borders[i].leftId;
			this.borders[i].x = this.columns[j].x + this.columns[j].w - 1;
			this.borders[i].y = this.columns[j].y;
			this.borders[i].w = this.borderWidth;
			this.borders[i].h = this.columns[j].h;
			bx = Math.floor(this.borders[i].x - 1);

			if (this.columns[j].percent > 0) {
				cx = tmp;
				cy = this.y;
				cw = (bx - cx);
				if (j != this.columnDraggedId || (this.columnDragged == 1)) {
					// draw column header bg
					if (this.columnRightClicked == j) {
						gr.FillSolidRect(cx, cy, cw, this.h, g_color_normal_txt & 0x30ffffff);
					} else { // normal box
						if (this.columnDragged == 1 && j == this.columnDraggedId) {
							gr.FillSolidRect(cx, cy, cw, this.h, g_color_normal_txt & 0x30ffffff);
						} else {
							gr.FillSolidRect(cx, cy, cw, this.h, g_color_normal_txt & 0x15ffffff);
						}
					}
					// draw column header infos
					if (this.columns[j].tf != "null" || this.columns[j].sortOrder != "null") {
						// draw sort indicator (direction)
						if (j == this.sortedColumnId) {
							sx = Math.floor(cx + this.columns[j].w / 2 - 3);
							sh = cy + 1;
							images.sortdirection && gr.DrawImage(images.sortdirection, sx, sh, images.sortdirection.Width, images.sortdirection.Height, 0, 0, images.sortdirection.Width, images.sortdirection.Height, (this.sortedColumnDirection > 0 ? 180 : 0), 130);
						}
					}
					gr.GdiDrawText(this.columns[j].label, g_font_12_1, g_color_normal_txt, cx + (this.borderWidth * 2), cy + 1, cw - (this.borderWidth * 4) - 1, this.h, this.columns[j].DT_align | DT_VCENTER | DT_CALCRECT | DT_NOPREFIX | DT_END_ELLIPSIS);
				} else if (j == this.columnDraggedId && this.columnDragged == 2) {
					gr.FillGradRect(cx, cy, cw, this.h, 90, RGBA(0, 0, 0, 60), 0, 1);
				}
			}

			if (this.borders[i].drag) {
				gr.FillSolidRect(Math.floor(bx - 0), this.y, this.borders[i].w, this.h, g_color_normal_txt);
			}
			tmp = bx + this.borderWidth;
		}

		// draw last colum at the right of the last border Object
		for (var i = this.totalColumns - 1; i >= 0; i--) {
			if (this.columns[i].percent > 0) {
				cx = tmp;
				cy = this.y;
				cw = (this.w - this.borderWidth - cx);
				if (i != this.columnDraggedId || (this.columnDragged == 1)) {
					// draw last column bg
					if (this.columnRightClicked == i) {
						gr.FillSolidRect(cx, cy, cw, this.h, g_color_normal_txt & 0x30ffffff);
					} else { // normal box
						if (this.columnDragged == 1 && i == this.columnDraggedId) {
							gr.FillSolidRect(cx, cy, cw, this.h, g_color_normal_txt & 0x30ffffff);
						} else {
							gr.FillSolidRect(cx, cy, cw, this.h, g_color_normal_txt & 0x15ffffff);
						}
					}
					// draw last column header info
					if (this.columns[i].tf != "null" || this.columns[i].sortOrder != "null") {
						// draw sort indicator (direction)
						if (i == this.sortedColumnId) {
							sx = Math.floor(cx + this.columns[i].w / 2 - 3);
							sh = cy + 1;
							images.sortdirection && gr.DrawImage(images.sortdirection, sx, sh, images.sortdirection.Width, images.sortdirection.Height, 0, 0, images.sortdirection.Width, images.sortdirection.Height, (this.sortedColumnDirection > 0 ? 180 : 0), 130);
						}
					}
					gr.GdiDrawText(this.columns[i].label, g_font_12_1, g_color_normal_txt, cx + (this.borderWidth * 2), cy + 1, cw - (this.borderWidth * 4) - 1, this.h, this.columns[i].DT_align | DT_VCENTER | DT_CALCRECT | DT_NOPREFIX | DT_END_ELLIPSIS);
				} else if (i == this.columnDraggedId && this.columnDragged == 2) {
					gr.FillGradRect(cx, cy, cw, this.h, 90, RGBA(0, 0, 0, 70), 0, 1);
				}
				break;
			}
		}

		// draw dragged column header (last item drawn to be always on the top)
		if (this.columnDragged > 1 && this.columnDraggedId != null) {
			cx = Math.floor(mouse_x - this.clickX) + 2;
			cy = this.y + 3;
			// shadow
			gr.FillSolidRect(cx + 4, cy + 3, Math.floor(this.columns[this.columnDraggedId].w - 2), this.h, RGBA(0, 0, 0, 10));
			gr.FillSolidRect(cx + 3, cy + 2, Math.floor(this.columns[this.columnDraggedId].w - 2), this.h, RGBA(0, 0, 0, 15));
			gr.FillSolidRect(cx + 2, cy + 1, Math.floor(this.columns[this.columnDraggedId].w - 2), this.h, RGBA(0, 0, 0, 30));
			// header bg
			gr.FillSolidRect(cx, cy, Math.floor(this.columns[this.columnDraggedId].w - 2), this.h, g_color_normal_txt & 0x66ffffff);
			gr.DrawRect(cx, cy + 1, Math.floor(this.columns[this.columnDraggedId].w - 2), this.h - 2, 2, g_color_normal_txt);
			gr.DrawRect(cx + 1, cy + 2, Math.floor(this.columns[this.columnDraggedId].w - 5), this.h - 5, 1, blendColors(g_color_normal_txt, g_color_normal_bg, 0.55));
			// header text info
			gr.GdiDrawText(this.columns[this.columnDraggedId].label, g_font_12_1, g_color_normal_bg, cx + (this.borderWidth * 2), cy + 1, this.columns[this.columnDraggedId].w - (this.borderWidth * 4) - 2, this.h, this.columns[this.columnDraggedId].DT_align | DT_VCENTER | DT_CALCRECT | DT_NOPREFIX | DT_END_ELLIPSIS);
		}
		// draw settings button
		this.button.draw(gr, this.x + this.w, this.y);
	}

	this.saveColumns = function () {
		for (var j = 0; j < 7; j++) {
			var tmp = "";
			for (var i = 0; i < this.columns.length; i++) {
				switch (j) {
				case 0:
					tmp = tmp + this.columns[i].label;
					break;
				case 1:
					tmp = tmp + this.columns[i].tf;
					break;
				case 2:
					tmp = tmp + this.columns[i].tf2;
					break;
				case 3:
					tmp = tmp + this.columns[i].percent;
					break;
				case 4:
					tmp = tmp + this.columns[i].ref;
					break;
				case 5:
					tmp = tmp + this.columns[i].align;
					break;
				case 6:
					tmp = tmp + this.columns[i].sortOrder;
					break;
				}
				if (i < this.columns.length - 1) {
					tmp = tmp + "^^";
				}
			}
			switch (j) {
			case 0:
				window.SetProperty("SYSTEM.HeaderBar.label", tmp);
				break;
			case 1:
				window.SetProperty("SYSTEM.HeaderBar.tf", tmp);
				break;
			case 2:
				window.SetProperty("SYSTEM.HeaderBar.tf2", tmp);
				break;
			case 3:
				window.SetProperty("SYSTEM.HeaderBar.percent", tmp);
				break;
			case 4:
				window.SetProperty("SYSTEM.HeaderBar.ref", tmp);
				break;
			case 5:
				window.SetProperty("SYSTEM.HeaderBar.align", tmp);
				break;
			case 6:
				window.SetProperty("SYSTEM.HeaderBar.sort", tmp);
				break;
			}
		}
		this.initColumns();
	}

	this.initColumns = function () {
		var borderPercent = 0;
		var previousColumnToDrawId = -1;
		this.columns.splice(0, this.columns.length);
		this.borders.splice(0, this.borders.length);
		if (this.totalColumns == 0) {
			// INITIALIZE columns and Properties
			var fields = [];
			for (var i = 0; i < 7; i++) {
				switch (i) {
				case 0:
					fields.push(["Cover", "State", "Index", "#", "Title", "Date", "Artist", "Album", "Genre", "Mood", "Rating", "Plays", "Bitrate", "Time"]);
					break;
				case 1:
					fields.push([
						"null",
						"null",
						"$num(%list_index%,$len(%list_total%))",
						"$if2($num(%discnumber%,1).,)$if2($num(%tracknumber%,2),-)",
						"$if2(%title%,%filename_ext%)",
						"[%date%]",
						"[%artist%]",
						"$if2(%album%,$if(%length%,Single,Stream))",
						"$if2(%genre%,Other)",
						"$rgb(196,30,35)$if(%mood%,1,0)",
						"$if2(%rating%,0)",
						"$if2(%play_count%,0)",
						"[%__bitrate% kbps]",
						"$if(%isplaying%,[-%playback_time_remaining%],[%length%])"]);
					break;
				case 2:
					fields.push([
						"null",
						"null",
						"null",
						"null",
						"[%artist%]",
						"null",
						"null",
						"null",
						"null",
						"null",
						"null",
						"null",
						"null",
						"[%__bitrate% kbps]"]);
					break;
				case 3:
					fields.push(["0", "5000", "0", "5000", "60000", "0", "0", "0", "0", "0", "20000", "0", "0", "10000"]);
					break;
				case 4:
					fields.push(["Cover", "State", "Index", "Tracknumber", "Title", "Date", "Artist", "Album", "Genre", "Mood", "Rating", "Playcount", "Bitrate", "Duration"]);
					break;
				case 5:
					fields.push(["0", "1", "1", "2", "0", "2", "0", "0", "0", "1", "1", "2", "1", "2"]);
					break;
				case 6:
					fields.push([
						"%album artist% | $if(%album%,%date%,9999) | %album% | %discnumber% | %tracknumber% | %title%",
						"%album artist% | $if(%album%,%date%,9999) | %album% | %discnumber% | %tracknumber% | %title%",
						"null",
						"%tracknumber% | %album artist% | $if(%album%,%date%,9999) | %album% | %discnumber% | %title%",
						"%title% | %album artist% | $if(%album%,%date%,9999) | %album% | %discnumber% | %tracknumber%",
						"%date% | %album artist% | %album% | %discnumber% | %tracknumber% | %title%",
						"%artist% | $if(%album%,%date%,9999) | %album% | %discnumber% | %tracknumber% | %title%",
						"$if2(%album%,%artist%) | $if(%album%,%date%,9999) | %album artist% | %discnumber% | %tracknumber% | %title%",
						"%genre% | %album artist% | $if(%album%,%date%,9999) | %album% | %discnumber% | %tracknumber% | %title%",
						"%mood% | %album artist% | $if(%album%,%date%,9999) | %album% | %discnumber% | %tracknumber% | %title%",
						"%rating% | %album artist% | $if(%album%,%date%,9999) | %album% | %discnumber% | %tracknumber% | %title%",
						"$if2(%play_count%,0) | %album artist% | $if(%album%,%date%,9999) | %album% | %discnumber% | %tracknumber% | %title%",
						"%__bitrate% | %album artist% | $if(%album%,%date%,9999) | %album% | %discnumber% | %tracknumber% | %title%",
						"$if2(%length%,0:00) | %album artist% | $if(%album%,%date%,9999) | %album% | %discnumber% | %tracknumber% | %title%"]);
					break;
				}
				// convert array to csv string
				var tmp = "";
				for (var j = 0; j < fields[i].length; j++) {
					tmp = tmp + fields[i][j];
					if (j < fields[i].length - 1) {
						tmp = tmp + "^^";
					}
				}
				// save CSV string into window Properties
				switch (i) {
				case 0:
					window.SetProperty("SYSTEM.HeaderBar.label", tmp);
					break;
				case 1:
					window.SetProperty("SYSTEM.HeaderBar.tf", tmp);
					break;
				case 2:
					window.SetProperty("SYSTEM.HeaderBar.tf2", tmp);
					break;
				case 3:
					window.SetProperty("SYSTEM.HeaderBar.percent", tmp);
					break;
				case 4:
					window.SetProperty("SYSTEM.HeaderBar.ref", tmp);
					break;
				case 5:
					window.SetProperty("SYSTEM.HeaderBar.align", tmp);
					break;
				case 6:
					window.SetProperty("SYSTEM.HeaderBar.sort", tmp);
					break;
				}
			}
			// create column Objects
			this.totalColumns = fields[0].length;
			window.SetProperty("SYSTEM.HeaderBar.TotalColumns", this.totalColumns);
			for (var k = 0; k < this.totalColumns; k++) {
				this.columns.push(new oColumn(fields[0][k], fields[1][k], fields[2][k], fields[3][k], fields[4][k], fields[5][k], fields[6][k]));
				if (this.columns[k].percent > 0) {
					if (previousColumnToDrawId >= 0) {
						this.borders.push(new oBorder(previousColumnToDrawId, k, borderPercent));
					}
					borderPercent += Math.round(this.columns[k].percent);
					previousColumnToDrawId = k;
				}
			}
		} else {
			var fields = [];
			var tmp;
			// LOAD columns from Properties
			for (var i = 0; i < 7; i++) {
				switch (i) {
				case 0:
					tmp = window.GetProperty("SYSTEM.HeaderBar.label", "?^^?^^?^^?^^?^^?^^?^^?^^?^^?^^?^^?^^?^^?");
					break;
				case 1:
					tmp = window.GetProperty("SYSTEM.HeaderBar.tf", "?^^?^^?^^?^^?^^?^^?^^?^^?^^?^^?^^?^^?^^?");
					break;
				case 2:
					tmp = window.GetProperty("SYSTEM.HeaderBar.tf2", "?^^?^^?^^?^^?^^?^^?^^?^^?^^?^^?^^?^^?^^?");
					break;
				case 3:
					tmp = window.GetProperty("SYSTEM.HeaderBar.percent", "0^^0^^0^^0^^0^^0^^0^^0^^0^^0^^0^^0^^0^^0");
					break;
				case 4:
					tmp = window.GetProperty("SYSTEM.HeaderBar.ref", "?^^?^^?^^?^^?^^?^^?^^?^^?^^?^^?^^?^^?^^?");
					break;
				case 5:
					tmp = window.GetProperty("SYSTEM.HeaderBar.align", "0^^0^^0^^0^^0^^0^^0^^0^^0^^0^^0^^0^^0^^0");
					break;
				case 6:
					tmp = window.GetProperty("SYSTEM.HeaderBar.sort", "?^^?^^?^^?^^?^^?^^?^^?^^?^^?^^?^^?^^?^^?");
					break;
				}
				fields.push(tmp.split("^^"));
			}
			for (var k = 0; k < this.totalColumns; k++) {
				this.columns.push(new oColumn(fields[0][k], fields[1][k], fields[2][k], fields[3][k], fields[4][k], fields[5][k], fields[6][k]));
				if (this.columns[k].percent > 0) {
					if (previousColumnToDrawId >= 0) {
						this.borders.push(new oBorder(previousColumnToDrawId, k, borderPercent));
					}
					borderPercent += Math.round(this.columns[k].percent);
					previousColumnToDrawId = k;
				}
			}
			this.calculateColumns();
		}
	}

	this.buttonCheck = function (event, x, y) {
		if (!this.columnDragged && !this.borderDragged) {
			var state = this.button.checkstate(event, x, y);
			switch (event) {
			case "down":
				if (state == ButtonStates.down) {
					this.buttonClicked = true;
				}
				break;
			case "up":
				if (this.buttonClicked && state == ButtonStates.hover) {
					togglePlaylistManager();
					this.button.state = ButtonStates.hover;
				}
				this.buttonClicked = false;
				break;
			}
			return state;
		}
	}

	this.on_mouse = function (event, x, y, delta) {
		if (!p.scrollbar.clicked) {
			this.ishover = (x >= this.x && x <= this.x + this.w && y >= this.y && y <= this.y + this.h);

			// check settings button + toolbar if button not hover
			if (this.buttonCheck(event, x, y) != ButtonStates.hover) {
				switch (event) {
				case "down":
					if (this.ishover) {
						// check borders:
						for (var i = 0; i < this.borders.length; i++) {
							this.borders[i].on_mouse(event, x, y);
							if (this.borders[i].drag) {
								this.borderDragged = true;
								this.borderDraggedId = i;
								full_repaint();
							}
						}
						// if no click on a border (no border drag), check columns:
						if (!this.borderDragged) {
							for (var i = 0; i < this.columns.length; i++) {
								this.columns[i].on_mouse(event, x, y);
								if (this.columns[i].drag) {
									this.clickX = x - this.columns[i].x;
									if (this.columns[i].tf != "null" || this.columns[i].sortOrder != "null") {
										this.columnDragged = 1;
										window.SetCursor(IDC_ARROW);
									} else {
										this.columnDragged = 2;
										window.SetCursor(IDC_SIZEALL);
									}
									this.columnDraggedId = i;
									full_repaint();
									break;
								}
							}
						}
					}
					break;
				case "up":
					if (this.borderDragged) {
						for (var i = 0; i < this.borders.length; i++) {
							if (this.borders[i].drag) {
								// save updated left & right columns percent properties
								var tmp = window.GetProperty("SYSTEM.HeaderBar.percent", "0^^0^^0^^0^^0^^0^^0^^0^^0^^0^^0^^0^^0^^0");
								var percents = tmp.split("^^");
								percents[Math.round(this.borders[i].leftId)] = this.columns[this.borders[i].leftId].percent.toString();
								percents[Math.round(this.borders[i].rightId)] = this.columns[this.borders[i].rightId].percent.toString();
								// save new percent CSV string to window Properties
								tmp = "";
								for (var j = 0; j < percents.length; j++) {
									tmp = tmp + percents[j];
									if (j < percents.length - 1) {
										tmp = tmp + "^^";
									}
								}
								window.SetProperty("SYSTEM.HeaderBar.percent", tmp);
								// update border object status
								this.borders[i].on_mouse(event, x, y);
								full_repaint();
							}
						}
						this.borderDragged = false;
						this.borderDraggedId = -1;
						this.on_mouse("move", x, y); // call "Move" to set mouse cursor if border hover
					} else if (this.columnDragged > 0) {
						if (this.columnDragged == 1) {
							if (this.columnDraggedId == 0) {
								this.columns[0].on_mouse(event, x, y);
							}
							if (this.columnDraggedId > 0 || (this.columnDraggedId == 0 && this.columns[this.columnDraggedId].isHover(x, y))) {
								window.SetCursor(IDC_WAIT);
								this.sortedColumnDirection = (this.columnDraggedId == this.sortedColumnId) ? (0 - this.sortedColumnDirection) : 1;
								this.sortedColumnId = this.columnDraggedId;
								this.columns[this.columnDraggedId].drag = false;
								this.columnDragged = 3;
								this.columnDragged_saved = 3;
								cHeaderBar.sortRequested = true;
								plman.UndoBackup(g_active_playlist);
								if (this.columns[this.columnDraggedId].sortOrder != "null") {
									plman.SortByFormatV2(g_active_playlist, this.columns[this.columnDraggedId].sortOrder, this.sortedColumnDirection);
								} else {
									plman.SortByFormatV2(g_active_playlist, this.columns[this.columnDraggedId].tf, this.sortedColumnDirection);
								}
								update_playlist(properties.collapseGroupsByDefault);
							} else {
								this.columns[this.columnDraggedId].drag = false;
								this.columnDragged = 0;
								this.columnDragged_saved = 0;
							}
						} else {
							for (var i = 0; i < this.columns.length; i++) {
								this.columns[i].on_mouse(event, x, y);
							}
							this.columnDragged = 0;
							this.on_mouse("move", x, y); // call "Move" to set mouse cursor if border hover
						}
						this.columnDraggedId = null;
						full_repaint();
					}
					break;
				case "right":
					if (this.ishover) {
						this.columnRightClicked = -1;
						for (var i = 0; i < this.columns.length; i++) {
							if (this.columns[i].percent > 0 && this.columns[i].isHover(x, y)) {
								this.columnRightClicked = i;
								full_repaint();
								break;
							}
						}
						this.contextMenu(x, y, this.columnRightClicked);
					}
					break;
				case "move":
					this.borderHover = false;
					for (var i = 0; i < this.borders.length; i++) {
						if (this.borders[i].isHover(x, y)) {
							this.borderHover = true;
							break;
						}
					}
					if (this.columnDragged < 1) {
						if (this.borderHover || this.borderDragged) {
							window.SetCursor(IDC_SIZEWE);
						} else if (p.playlistManager.inputbox) {
							if (!p.playlistManager.inputbox.ibeam_set)
								window.SetCursor(IDC_ARROW);
						} else {
							window.SetCursor(IDC_ARROW);
						}
					}
					if (this.borderDragged) {
						for (var i = 0; i < this.borders.length; i++) {
							this.borders[i].on_mouse(event, x, y);
							var d = this.borders[i].delta;
							if (this.borders[i].drag) {
								var toDoLeft = (this.columns[this.borders[i].leftId].w + d > this.columns[this.borders[i].leftId].minWidth);
								var toDoRight = (this.columns[this.borders[i].rightId].w - d > this.columns[this.borders[i].rightId].minWidth);
								if (toDoLeft && toDoRight) { // ok, we can resize the left and the right columns
									this.columns[this.borders[i].leftId].w += d;
									this.columns[this.borders[i].rightId].w -= d;
									var addedPercent = Math.abs(this.columns[this.borders[i].leftId].percent) + Math.abs(this.columns[this.borders[i].rightId].percent);
									this.columns[this.borders[i].leftId].percent = Math.abs(this.columns[this.borders[i].leftId].w / this.w * 100000);
									this.columns[this.borders[i].rightId].percent = addedPercent - this.columns[this.borders[i].leftId].percent;
									this.borders[i].sourceX = x;
									full_repaint();
								}
							}
						}
					} else if (this.columnDraggedId != 0 && (this.columnDragged == 1 || this.columnDragged == 2)) {
						this.columnDragged = 2;
						window.SetCursor(IDC_SIZEALL);
						for (var i = 1; i < this.columns.length; i++) {
							if (this.columns[i].percent > 0) {
								if (i != this.columnDraggedId) {
									if ((x > mouse_x && x > this.columns[i].x && i > this.columnDraggedId) || (x < mouse_x && x < this.columns[i].x + this.columns[i].w && i < this.columnDraggedId)) {
										var tmpCol = this.columns[this.columnDraggedId];
										this.columns[this.columnDraggedId] = this.columns[i];
										this.columns[i] = tmpCol;
										// move sortColumnId too !
										if (i == this.sortedColumnId) {
											this.sortedColumnId = this.columnDraggedId;
										} else if (this.columnDraggedId == this.sortedColumnId) {
											this.sortedColumnId = i;
										}
										this.columnDraggedId = i;
										break;
									}
								}
							}
						}
						this.saveColumns();
						full_repaint();
					}
					break;
				}
			}
		}
	}

	this.contextMenu = function (x, y, column_index) {
		var _menu = window.CreatePopupMenu();
		var _groups = window.CreatePopupMenu();
		var _columns = window.CreatePopupMenu();

		_groups.AppendMenuItem(MF_STRING, 2, "Enable Groups");
		_groups.CheckMenuItem(2, properties.showgroupheaders);

		if (properties.showgroupheaders) {
			_groups.AppendMenuSeparator();
			var groupByMenuIdx = 20;
			var totalGroupBy = p.list.groupby.length;
			for (var i = 0; i < totalGroupBy; i++) {
				_groups.AppendMenuItem(MF_STRING, groupByMenuIdx + i, p.list.groupby[i].label);
			}
			_groups.CheckMenuRadioItem(groupByMenuIdx, groupByMenuIdx + totalGroupBy - 1, cGroup.pattern_idx + groupByMenuIdx);
			_groups.AppendMenuSeparator();
			_groups.AppendMenuItem(p.list.totalRows > 0 && !properties.autocollapse && cGroup.expanded_height > 0 && cGroup.collapsed_height > 0 ? MF_STRING : MF_GRAYED, 3, "Collapse All");
			_groups.AppendMenuItem(p.list.totalRows > 0 && !properties.autocollapse && cGroup.expanded_height > 0 && cGroup.collapsed_height > 0 ? MF_STRING : MF_GRAYED, 4, "Expand All");
		}

		_groups.AppendTo(_menu, MF_STRING, "Groups");
		_menu.AppendMenuSeparator();

		// Columns submenu entries
		var columnMenuIdx = 100;
		for (var i = 0; i < this.columns.length; i++) {
			if (i == column_index) {
				_columns.AppendMenuItem(MF_STRING, columnMenuIdx + i, "[" + this.columns[i].label + "]");
			} else {
				_columns.AppendMenuItem(MF_STRING, columnMenuIdx + i, this.columns[i].label);
			}
			_columns.CheckMenuItem(columnMenuIdx + i, this.columns[i].w > 0 ? 1 : 0);
		}
		_columns.AppendTo(_menu, MF_STRING, "Columns");
		_menu.AppendMenuSeparator();

		_menu.AppendMenuItem(MF_STRING, 5, "Double Track Line");
		_menu.CheckMenuItem(5, cList.enableExtraLine);

		var idx = _menu.TrackPopupMenu(x, y);
		switch (true) {
		case idx == 2:
			properties.showgroupheaders = !properties.showgroupheaders;
			window.SetProperty("GROUP.Show Group Headers", properties.showgroupheaders);

			if (cGroup.collapsed_height > 0) {
				cGroup.collapsed_height = 0;
				cGroup.expanded_height = 0;
				// disable autocollapse when there is no group!
				properties.autocollapse = false;
				window.SetProperty("SYSTEM.Auto-Collapse", properties.autocollapse);
			} else {
				cGroup.collapsed_height = cGroup.default_collapsed_height;
				cGroup.expanded_height = cGroup.default_expanded_height;
			}

			p.list.updateHandleList(false);
			p.list.setItems(true);
			p.scrollbar.setCursor(p.list.totalRowVisible, p.list.totalRows, p.list.offset);
			p.playlistManager.refresh();
			break;
		case idx == 3:
		case idx == 4:
			resize_panels();
			p.list.updateHandleList(idx == 3);
			p.list.setItems(true);
			p.scrollbar.setCursor(p.list.totalRowVisible, p.list.totalRows, p.list.offset);
			break;
		case idx == 5:
			cList.enableExtraLine = !cList.enableExtraLine;
			window.SetProperty("SYSTEM.Enable Extra Line", cList.enableExtraLine);
			resize_panels();
			p.list.updateHandleList(false);
			p.list.setItems(true);
			p.scrollbar.setCursor(p.list.totalRowVisible, p.list.totalRows, p.list.offset);
			break;
		case idx >= groupByMenuIdx && idx < 100:
			cGroup.pattern_idx = idx - groupByMenuIdx;
			window.SetProperty("SYSTEM.Groups.Pattern Index", cGroup.pattern_idx);
			p.list.updateHandleList(false);
			p.list.setItems(true);
			p.scrollbar.setCursor(p.list.totalRowVisible, p.list.totalRows, p.list.offset);
			full_repaint();
			break;
		case idx >= 100:
			if (this.columns[idx - 100].percent == 0) {
				var newColumnSize = 8000;
				this.columns[idx - 100].percent = newColumnSize;
				var totalColsToResizeDown = 0;
				var last_idx = 0;
				for (var k = 0; k < this.columns.length; k++) {
					if (k != idx - 100 && this.columns[k].percent > newColumnSize) {
						totalColsToResizeDown++;
						last_idx = k;
					}
				}
				var minus_value = Math.floor(newColumnSize / totalColsToResizeDown);
				var reste = newColumnSize - (minus_value * totalColsToResizeDown);
				for (var k = 0; k < this.columns.length; k++) {
					if (k != idx - 100 && this.columns[k].percent > newColumnSize) {
						this.columns[k].percent = Math.abs(this.columns[k].percent) - minus_value;
						if (reste > 0 && k == last_idx) {
							this.columns[k].percent = Math.abs(this.columns[k].percent) - reste;
						}
					}
					this.columns[k].w = Math.abs(this.w * this.columns[k].percent / 100000);
				}
				this.saveColumns();
			} else {
				// check if it's not the last column visible, otherwise we coundn't hide it!
				var nbvis = 0;
				for (var k = 0; k < this.columns.length; k++) {
					if (this.columns[k].percent > 0) {
						nbvis++;
					}
				}
				if (nbvis > 1) {
					var RemovedColumnSize = Math.abs(this.columns[idx - 100].percent);
					this.columns[idx - 100].percent = 0;
					var totalColsToResizeUp = 0;
					var last_idx = 0;
					for (var k = 0; k < this.columns.length; k++) {
						if (k != idx - 100 && this.columns[k].percent > 0) {
							totalColsToResizeUp++;
							last_idx = k;
						}
					}
					var add_value = Math.floor(RemovedColumnSize / totalColsToResizeUp);
					var reste = RemovedColumnSize - (add_value * totalColsToResizeUp);
					for (var k = 0; k < this.columns.length; k++) {
						if (k != idx - 100 && this.columns[k].percent > 0) {
							this.columns[k].percent = Math.abs(this.columns[k].percent) + add_value;
							if (reste > 0 && k == last_idx) {
								this.columns[k].percent = Math.abs(this.columns[k].percent) + reste;
							}
						}
						this.columns[k].w = Math.abs(this.w * this.columns[k].percent / 100000);
					}
					this.saveColumns();
				}
			}
			this.initColumns();

			if (this.columns[0].w > 0) {
				cover.column = true;
				cGroup.count_minimum = Math.ceil((this.columns[0].w) / cRow.playlist_h);
				if (cGroup.count_minimum < cGroup.default_count_minimum)
					cGroup.count_minimum = cGroup.default_count_minimum;
			} else {
				cover.column = false;
				cGroup.count_minimum = cGroup.default_count_minimum;
			}
			update_playlist(properties.collapseGroupsByDefault);
			break;
		}
		_groups.Dispose();
		_columns.Dispose();
		_menu.Dispose();
		this.columnRightClicked = -1;
		full_repaint();
		return true;
	}
}
