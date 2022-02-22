function _albumart(x, y, w, h) {
	this.paint = function (gr) {
		if (this.properties.cd.enabled) {
			if (this.properties.shadow.enabled) {
				_drawImage(gr, this.images.shadow, this.x, this.y, this.w, this.h);
			}
			_drawImage(gr, this.images.case_, this.x, this.y, this.w, this.h);
			if (this.img) {
				var ratio = Math.min(this.w / this.images.case_.Width, this.h / this.images.case_.Height);
				var nw = 488 * ratio;
				var nh = 476 * ratio;
				var nx = this.x + Math.floor((this.w - (452 * ratio)) / 2);
				var ny = this.y + Math.floor((this.h - nh) / 2);
				_drawImage(gr, this.img, nx, ny, nw, nh, this.properties.aspect.value);
			}
			_drawImage(gr, this.images.semi, this.x, this.y, this.w, this.h);
			if (this.properties.gloss.enabled) {
				_drawImage(gr, this.images.gloss, this.x, this.y, this.w, this.h);
			}
		} else if (this.img) {
			_drawImage(gr, this.img, this.x, this.y, this.w, this.h, this.properties.aspect.value);
		}
	}
	
	this.metadb_changed = function () {
		_dispose(this.img);
		this.img = null;
		this.tooltip = this.path = '';
		if (panel.metadb) {
			var arr = panel.metadb.GetAlbumArt(this.properties.id.value).toArray();
			this.img = arr[0];
			if (this.img) {
				this.tooltip = 'Original dimensions: ' + this.img.Width + 'x' + this.img.Height + 'px';
				this.path = arr[1];
				if (this.path.length) {
					this.tooltip += '\nPath: ' + this.path;
				}
				if (this.img.Size > 0) {
					this.tooltip += '\nSize: ' + utils.FormatFileSize(this.img.Size);
				}
			}
		}
		window.Repaint();
	}
	
	this.containsXY = function (x, y) {
		return x > this.x && x < this.x + this.w && y > this.y && y < this.y + this.h;
	}
	
	this.wheel = function (s) {
		if (this.containsXY(this.mx, this.my)) {
			var id = this.properties.id.value - s;
			if (id < 0) {
				id = 4;
			}
			if (id > 4) {
				id = 0;
			}
			this.properties.id.value = id;
			_tt('');
			panel.item_focus_change();
			return true;
		}
		return false;
	}
	
	this.move = function (x, y) {
		this.mx = x;
		this.my = y;
		
		if (this.containsXY(x, y)) {
			if (this.img) {
				_tt(this.tooltip);
			}
			this.hover = true;
			return true;
		}
		
		if (this.hover) {
			_tt('');
		}
		this.hover = false;
		return false;
	}
	
	this.lbtn_dblclk = function (x, y) {
		if (this.containsXY(x, y)) {
			if (panel.metadb) {
				switch (this.properties.double_click_mode.value) {
				case 0:
					if (panel.metadb.Path == this.path) {
						_explorer(this.path);
					} else if (utils.IsFile(this.path) || this.path.indexOf("http") == 0) {
						_run(this.path);
					}
					break;
				case 1:
					panel.metadb.ShowAlbumArtViewer(this.properties.id.value);
					break;
				case 2:
					if (utils.IsFile(this.path)) _explorer(this.path);
					break;
				}
			}
			return true;
		}
		return false;
	}
	
	this.rbtn_up = function (x, y) {
		panel.m.AppendMenuItem(MF_STRING, 1000, 'Refresh');
		panel.m.AppendMenuSeparator();
		panel.m.AppendMenuItem(MF_STRING, 1001, 'CD Jewel Case');
		panel.m.CheckMenuItem(1001, this.properties.cd.enabled);
		panel.m.AppendMenuItem(this.properties.cd.enabled ? MF_STRING : MF_GRAYED, 1002, 'Gloss effect');
		panel.m.CheckMenuItem(1002, this.properties.gloss.enabled);
		panel.m.AppendMenuItem(this.properties.cd.enabled ? MF_STRING : MF_GRAYED, 1003, 'Shadow effect');
		panel.m.CheckMenuItem(1003, this.properties.shadow.enabled);
		panel.m.AppendMenuSeparator();
		_.forEach(this.ids, function (item, i) {
			panel.m.AppendMenuItem(MF_STRING, i + 1010, item);
		});
		panel.m.CheckMenuRadioItem(1010, 1014, this.properties.id.value + 1010);
		panel.m.AppendMenuSeparator();
		panel.m.AppendMenuItem(MF_STRING, 1020, 'Crop (focus on centre)');
		panel.m.AppendMenuItem(MF_STRING, 1021, 'Crop (focus on top)');
		panel.m.AppendMenuItem(MF_STRING, 1022, 'Stretch');
		panel.m.AppendMenuItem(MF_STRING, 1023, 'Centre');
		panel.m.CheckMenuRadioItem(1020, 1023, this.properties.aspect.value + 1020);
		panel.m.AppendMenuSeparator();
		panel.m.AppendMenuItem(utils.IsFile(this.path) ? MF_STRING : MF_GRAYED, 1030, 'Open containing folder');
		panel.m.AppendMenuSeparator();
		panel.m.AppendMenuItem(panel.metadb ? MF_STRING : MF_GRAYED, 1040, 'Google image search');
		panel.m.AppendMenuSeparator();
		panel.s10.AppendMenuItem(MF_STRING, 1050, "Opens image in external viewer");
		panel.s10.AppendMenuItem(MF_STRING, 1051, "Opens image using fb2k viewer");
		panel.s10.AppendMenuItem(MF_STRING, 1052, "Opens containing folder");
		panel.s10.CheckMenuRadioItem(1050, 1052, this.properties.double_click_mode.value + 1050);
		panel.s10.AppendTo(panel.m, MF_STRING, 'Double click');
		panel.m.AppendMenuSeparator();
	}
	
	this.rbtn_up_done = function (idx) {
		switch (idx) {
		case 1000:
			panel.item_focus_change();
			break;
		case 1001:
			this.properties.cd.toggle();
			window.Repaint();
			break;
		case 1002:
			this.properties.gloss.toggle();
			window.RepaintRect(this.x, this.y, this.w, this.h);
			break;
		case 1003:
			this.properties.shadow.toggle();
			window.RepaintRect(this.x, this.y, this.w, this.h);
			break;
		case 1010:
		case 1011:
		case 1012:
		case 1013:
		case 1014:
			this.properties.id.value = idx - 1010;
			panel.item_focus_change();
			break;
		case 1020:
		case 1021:
		case 1022:
		case 1023:
			this.properties.aspect.value = idx - 1020;
			window.RepaintRect(this.x, this.y, this.w, this.h);
			break;
		case 1030:
			_explorer(this.path);
			break;
		case 1040:
			_run('https://www.google.com/search?tbm=isch&q=' + encodeURIComponent(panel.tf('%album artist%[ %album%]')));
			break;
		case 1050:
		case 1051:
		case 1052:
			this.properties.double_click_mode.value = idx - 1050;
			break;
		}
	}
	
	this.key_down = function (k) {
		switch (k) {
		case VK_LEFT:
		case VK_UP:
			this.wheel(1);
			return true;
		case VK_RIGHT:
		case VK_DOWN:
			this.wheel(-1);
			return true;
		default:
			return false;
		}
	}
	
	this.x = x;
	this.y = y;
	this.w = w;
	this.h = h;
	this.mx = 0;
	this.my = 0;
	this.tooltip = '';
	this.img = null;
	this.path = null;
	this.hover = false;
	this.ids = ['Front', 'Back', 'Disc', 'Icon', 'Artist'];
	this.images = {
		shadow : _img('cd\\shadow.png'),
		case_ : _img('cd\\case.png'),
		semi : _img('cd\\semi.png'),
		gloss : _img('cd\\gloss.png')
	};
	this.properties = {
		aspect : new _p('2K3.ARTREADER.ASPECT', image.crop),
		gloss : new _p('2K3.ARTREADER.GLOSS', false),
		cd : new _p('2K3.ARTREADER.CD', false),
		id : new _p('2K3.ARTREADER.ID', 0),
		shadow : new _p('2K3.ARTREADER.SHADOW', false),
		double_click_mode : new _p('2K3.ARTREADER.DOUBLE.CLICK.MODE', 0) // 0 external viewer 1 fb2k viewer 2 explorer
	};
}
