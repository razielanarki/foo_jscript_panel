function on_download_file_done(path, success, error_text) {
	_.invoke(panel.list_objects, 'download_file_done', path, success, error_text);
	_.invoke(panel.text_objects, 'download_file_done', path, success, error_text);
}

function on_http_request_done(task_id, success, text) {
	if (typeof thumbs == 'object' && typeof thumbs.http_request_done == 'function') thumbs.http_request_done(task_id, success, text);
	_.invoke(panel.list_objects, 'http_request_done', task_id, success, text);
	_.invoke(panel.text_objects, 'http_request_done', task_id, success, text);
}

function _panel(options) {
	this.item_focus_change = function () {
		if (this.metadb_func) {
			if (this.selection.value == 0) {
				this.metadb = fb.IsPlaying ? fb.GetNowPlaying() : fb.GetFocusItem();
			} else {
				this.metadb = fb.GetFocusItem();
			}
			on_metadb_changed();
			if (!this.metadb) {
				_tt('');
			}
		}
	}
	
	this.colours_changed = function () {
		if (window.InstanceType == 1) {
			this.colours.background = window.GetColourDUI(1);
			this.colours.text = window.GetColourDUI(0);
			this.colours.highlight = window.GetColourDUI(2);
		} else {
			this.colours.background = window.GetColourCUI(3);
			this.colours.text = window.GetColourCUI(0);
			this.colours.highlight = _blendColours(this.colours.text, this.colours.background, 0.4);
		}
		this.colours.header = _setAlpha(this.colours.highlight, 69)
	}
	
	this.font_changed = function () {
		var font = window.InstanceType == 1 ? window.GetFontDUI(0) : window.GetFontCUI(0);
		var name = font.Name;

		_dispose(this.fonts.title, this.fonts.normal, this.fonts.fixed);
		this.fonts.title = _gdiFont(name, this.fonts.size.value, 1);
		this.fonts.normal = _gdiFont(name, this.fonts.size.value);
		this.fonts.fixed = _gdiFont('Consolas', this.fonts.size.value);
		this.row_height = this.fonts.normal.Height;
		_.invoke(this.list_objects, 'size');
		_.invoke(this.list_objects, 'update');
		_.invoke(this.text_objects, 'size');
	}
	
	this.size = function () {
		this.w = window.Width;
		this.h = window.Height;
	}
	
	this.paint = function (gr) {
		switch (true) {
		case window.IsTransparent:
			return;
		case !this.custom_background:
		case this.colours.mode.value == 0:
			var col = this.colours.background;
			break;
		case this.colours.mode.value == 1:
			var col = utils.GetSysColour(15);
			break;
		case this.colours.mode.value == 2:
			var col = this.colours.custom_background.value;
			break;
		}
		gr.FillSolidRect(0, 0, this.w, this.h, col);
	}
	
	this.rbtn_up = function (x, y, object) {
		this.m = window.CreatePopupMenu();
		this.s1 = window.CreatePopupMenu();
		this.s2 = window.CreatePopupMenu();
		this.s3 = window.CreatePopupMenu();
		this.s10 = window.CreatePopupMenu();
		this.s11 = window.CreatePopupMenu();
		this.s12 = window.CreatePopupMenu();
		this.s13 = window.CreatePopupMenu();
		// panel 1-999
		// object 1000+
		if (object) {
			object.rbtn_up(x, y);
		}
		if (this.list_objects.length || this.text_objects.length) {
			_.forEach(this.fonts.sizes, function (item) {
				this.s1.AppendMenuItem(MF_STRING, item, item);
			}, this);
			this.s1.CheckMenuRadioItem(_.first(this.fonts.sizes), _.last(this.fonts.sizes), this.fonts.size.value);
			this.s1.AppendTo(this.m, MF_STRING, 'Font size');
			this.m.AppendMenuSeparator();
		}
		if (this.custom_background) {
			this.s2.AppendMenuItem(MF_STRING, 100, window.InstanceType == 1 ? 'Use default UI setting' : 'Use columns UI setting');
			this.s2.AppendMenuItem(MF_STRING, 101, 'Splitter');
			this.s2.AppendMenuItem(MF_STRING, 102, 'Custom');
			this.s2.CheckMenuRadioItem(100, 102, this.colours.mode.value + 100);
			this.s2.AppendMenuSeparator();
			this.s2.AppendMenuItem(this.colours.mode.value == 2 ? MF_STRING : MF_GRAYED, 103, 'Set custom colour...');
			this.s2.AppendTo(this.m, window.IsTransparent ? MF_GRAYED : MF_STRING, 'Background');
			this.m.AppendMenuSeparator();
		}
		if (this.metadb_func) {
			this.s3.AppendMenuItem(MF_STRING, 110, 'Prefer now playing');
			this.s3.AppendMenuItem(MF_STRING, 111, 'Follow selected track (playlist)');
			this.s3.CheckMenuRadioItem(110, 111, this.selection.value + 110);
			this.s3.AppendTo(this.m, MF_STRING, 'Selection mode');
			this.m.AppendMenuSeparator();
		}
		this.m.AppendMenuItem(MF_STRING, 120, 'Configure...');
		var idx = this.m.TrackPopupMenu(x, y);
		switch (true) {
		case idx == 0:
			break;
		case idx <= 20:
			this.fonts.size.value = idx;
			this.font_changed();
			window.Repaint();
			break;
		case idx == 100:
		case idx == 101:
		case idx == 102:
			this.colours.mode.value = idx - 100;
			window.Repaint();
			break;
		case idx == 103:
			this.colours.custom_background.value = utils.ColourPicker(window.ID, this.colours.custom_background.value);
			window.Repaint();
			break;
		case idx == 110:
		case idx == 111:
			this.selection.value = idx - 110;
			this.item_focus_change();
			break;
		case idx == 120:
			window.ShowConfigure();
			break;
		case idx > 999:
			if (object) {
				object.rbtn_up_done(idx);
			}
			break;
		}
		_dispose(this.m, this.s1, this.s2, this.s3, this.s10, this.s11, this.s12, this.s13);
		return true;
	}
	
	this.tf = function (t) {
		if (!this.metadb) {
			return '';
		}
		if (!this.tfo[t]) {
			this.tfo[t] = fb.TitleFormat(t);
		}
		if (fb.IsPlaying && (_.startsWith(this.metadb.Path, 'http') || _.startsWith(this.metadb.Path, 'mms'))) {
			return this.tfo[t].Eval();
		}
		return this.tfo[t].EvalWithMetadb(this.metadb);
	}
	
	this.fonts = {};
	this.colours = {};
	this.w = 0;
	this.h = 0;
	this.metadb = fb.GetFocusItem();
	this.metadb_func = typeof on_metadb_changed == 'function';
	this.fonts.sizes = [10, 12, 14, 16];
	this.fonts.size = new _p('2K3.PANEL.FONTS.SIZE', 12);
	if (this.metadb_func) {
		this.selection = new _p('2K3.PANEL.SELECTION', 0);
	}
	switch (true) {
	case typeof options == 'string' && options == 'custom_background':
	case typeof options == 'object' && options.custom_background:
		this.custom_background = true;
		this.colours.mode = new _p('2K3.PANEL.COLOURS.MODE', 0);
		this.colours.custom_background = new _p('2K3.PANEL.COLOURS.CUSTOM.BACKGROUND', _RGB(0, 0, 0));
		break;
	default:
		this.custom_background = false;
	}
	this.list_objects = [];
	this.text_objects = [];
	this.tfo = {
		'$if2(%__@%,%path%)' : fb.TitleFormat('$if2(%__@%,%path%)')
	};
	this.font_changed();
	this.colours_changed();
}
