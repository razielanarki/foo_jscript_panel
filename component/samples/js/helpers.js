var _bmp = gdi.CreateImage(1, 1);
var _gr = _bmp.GetGraphics();

String.prototype.calc_width = function (font) {
	return _gr.CalcTextWidth(this, font);
}

Number.prototype.calc_width = function (font) {
	return _gr.CalcTextWidth(this.toString(), font);
}

Array.prototype.srt=function(){for(var z=0,t;t=this[z];z++){this[z]=[];var x=0,y=-1,n=true,i,j;while(i=(j=t.charAt(x++)).charCodeAt(0)){var m=(i==46||(i>=48&&i<=57));if(m!==n){this[z][++y]='';n=m;}
this[z][y]+=j;}}
this.sort(function(a,b){for(var x=0,aa,bb;(aa=a[x])&&(bb=b[x]);x++){aa=aa.toLowerCase();bb=bb.toLowerCase();if(aa!==bb){var c=Number(aa),d=Number(bb);if(c==aa&&d==bb){return c-d;}else return(aa>bb)?1:-1;}}
return a.length-b.length;});for(var z=0;z<this.length;z++)
this[z]=this[z].join('');}

function _artistFolder(artist) {
	var folder = folders.artists + utils.ReplaceIllegalChars(artist);
	_createFolder(folder);
	return fso.GetFolder(folder) + '\\';
}

function _blendColours(c1, c2, f) {
	c1 = _toRGB(c1);
	c2 = _toRGB(c2);
	var r = Math.round(c1[0] + f * (c2[0] - c1[0]));
	var g = Math.round(c1[1] + f * (c2[1] - c1[1]));
	var b = Math.round(c1[2] + f * (c2[2] - c1[2]));
	return _RGB(r, g, b);
}

function _button(x, y, w, h, img_src, fn, tiptext) {
	this.paint = function (gr) {
		if (this.img) {
			_drawImage(gr, this.img, this.x, this.y, this.w, this.h);
		}
	}
	
	this.containsXY = function (x, y) {
		return x > this.x && x < this.x + this.w && y > this.y && y < this.y + this.h;
	}
	
	this.lbtn_up = function (x, y, mask) {
		if (this.fn) {
			this.fn(x, y, mask);
		}
	}
	
	this.cs = function (s) {
		if (s == 'hover') {
			this.img = this.img_hover;
			_tt(this.tiptext);
		} else {
			this.img = this.img_normal;
		}
		window.RepaintRect(this.x, this.y, this.w, this.h);
	}
	
	this.x = x;
	this.y = y;
	this.w = w;
	this.h = h;
	this.fn = fn;
	this.tiptext = tiptext;
	this.img_normal = typeof img_src.normal == 'string' ? _img(img_src.normal) : img_src.normal;
	this.img_hover = img_src.hover ? (typeof img_src.hover == 'string' ? _img(img_src.hover) : img_src.hover) : this.img_normal;
	this.img = this.img_normal;
}

function _buttons() {
	this.paint = function (gr) {
		_.invoke(this.buttons, 'paint', gr);
	}
	
	this.move = function (x, y) {
		var temp_btn = null;
		_.forEach(this.buttons, function (item, i) {
			if (item.containsXY(x, y)) {
				temp_btn = i;
			}
		});
		if (this.btn == temp_btn) {
			return this.btn;
		}
		if (this.btn) {
			this.buttons[this.btn].cs('normal');
		}
		if (temp_btn) {
			this.buttons[temp_btn].cs('hover');
		} else {
			_tt('');
		}
		this.btn = temp_btn;
		return this.btn;
	}
	
	this.leave = function () {
		if (this.btn) {
			_tt('');
			this.buttons[this.btn].cs('normal');
		}
		this.btn = null;
	}
	
	this.lbtn_up = function (x, y, mask) {
		if (this.btn) {
			this.buttons[this.btn].lbtn_up(x, y, mask);
			return true;
		}
		return false;
	}
	
	this.buttons = {};
	this.btn = null;
}

function _cc(name) {
	return utils.CheckComponent(name);
}

function _chrToImg(chr, colour) {
	var size = 96;
	var temp_bmp = gdi.CreateImage(size, size);
	var temp_gr = temp_bmp.GetGraphics();
	temp_gr.SetTextRenderingHint(4);
	temp_gr.DrawString(chr, fontawesome, colour, 0, 0, size, size, SF_CENTRE);
	temp_bmp.ReleaseGraphics(temp_gr);
	temp_gr = null;
	return temp_bmp;
}

function _createFolder(folder) {
	if (!utils.IsFolder(folder)) {
		fso.CreateFolder(folder);
	}
}

function _deleteFile(file) {
	if (utils.IsFile(file)) {
		try {
			fso.DeleteFile(file);
		} catch (e) {
		}
	}
}

function _dispose() {
	_.forEach(arguments, function (item) {
		if (item) {
			item.Dispose();
		}
	});
}

function _drawImage(gr, img, dst_x, dst_y, dst_w, dst_h, mode, border, alpha) {
	if (!img) {
		return [];
	}
	gr.SetInterpolationMode(7);
	switch (true) {
	case (dst_w == dst_h && img.Width == img.Height) || (dst_w == img.Width && dst_h == img.Height):
	case mode == image.stretch:
		gr.DrawImage(img, dst_x, dst_y, dst_w, dst_h, 0, 0, img.Width, img.Height, 0, alpha || 255);
		break;
	case mode == image.crop:
	case mode == image.crop_top:
		if (img.Width / img.Height < dst_w / dst_h) {
			var src_x = 0;
			var src_w = img.Width;
			var src_h = Math.round(dst_h * img.Width / dst_w);
			var src_y = Math.round((img.Height - src_h) / (mode == image.crop_top ? 4 : 2));
		} else {
			var src_y = 0;
			var src_w = Math.round(dst_w * img.Height / dst_h);
			var src_h = img.Height;
			var src_x = Math.round((img.Width - src_w) / 2);
		}
		gr.DrawImage(img, dst_x, dst_y, dst_w, dst_h, src_x + 3, src_y + 3, src_w - 6, src_h - 6, 0, alpha || 255);
		break;
	case mode == image.centre:
	default:
		var s = Math.min(dst_w / img.Width, dst_h / img.Height);
		var w = Math.floor(img.Width * s);
		var h = Math.floor(img.Height * s);
		dst_x += Math.round((dst_w - w) / 2);
		dst_y += Math.round((dst_h - h) / 2);
		dst_w = w;
		dst_h = h;
		gr.DrawImage(img, dst_x, dst_y, dst_w, dst_h, 0, 0, img.Width, img.Height, 0, alpha || 255);
		break;
	}
	if (border) {
		gr.DrawRect(dst_x, dst_y, dst_w - 1, dst_h - 1, 1, border);
	}
	return [dst_x, dst_y, dst_w, dst_h];
}

function _drawOverlay(gr, x, y, w, h) {
	gr.FillGradRect(x, y, w, h, 90, _RGBA(0, 0, 0, 230), _RGBA(0, 0, 0, 200));
}

function _explorer(file) {
	if (utils.IsFile(file)) {
		WshShell.Run('explorer /select,' + _q(file));
	}
}

function _fbEscape(value) {
	return value.replace(/'/g, "''").replace(/[\(\)\[\],$]/g, "'$&'");
}

function _fileExpired(file, period) {
	return _.now() - _lastModified(file) > period;
}

function _formatNumber(number, separator) {
	return number.toString().replace(/\B(?=(\d{3})+(?!\d))/g, separator);
}

function _gdiFont(name, size, style) {
	return gdi.Font(name, _scale(size), style);
}

function _getClipboardData() {
	return doc.parentWindow.clipboardData.getData('Text');
}

function _getElementsByTagName(value, tag) {
	doc.open();
	var div = doc.createElement('div');
	div.innerHTML = value;
	var data = div.getElementsByTagName(tag);
	doc.close();
	return data;
}

function _getExt(path) {
	return path.split('.').pop().toLowerCase();
}

function _getFiles(folder, exts) {
	var files = [];
	var folders = folder.split('|').map(function (item) { return item.trim() });
	for (var i = 0; i < folders.length; i++) {
		Array.prototype.push.apply(files, utils.ListFiles(folders[i]).toArray());
	}
	files.srt();
	if (!exts) {
		return files;
	}
	return _.filter(files, function (item) {
		var ext = _getExt(item);
		return _.includes(exts, ext);
	});
}

function _help(x, y, flags) {
	var m = window.CreatePopupMenu();
	_.forEach(ha_links, function (item, i) {
		m.AppendMenuItem(MF_STRING, i + 100, item[0]);
		if (i == 1) {
			m.AppendMenuSeparator();
		}
	});
	m.AppendMenuSeparator();
	m.AppendMenuItem(MF_STRING, 1, 'Configure...');
	var idx = m.TrackPopupMenu(x, y, flags);
	switch (true) {
	case idx == 0:
		break;
	case idx == 1:
		window.ShowConfigure();
		break;
	default:
		_run(ha_links[idx - 100][1]);
		break;
	}
	_dispose(m);
}

function _img(value) {
	if (utils.IsFile(value)) {
		return gdi.Image(value);
	}
	return gdi.Image(folders.images + value);
}

function _isUUID(value) {
	var re = /^[0-9a-f]{8}-[0-9a-f]{4}-[345][0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/
	return re.test(value);
}

function _jsonParse(value) {
	try {
		var data = JSON.parse(value);
		return data;
	} catch (e) {
		return [];
	}
}

function _jsonParseFile(file) {
	return _jsonParse(_open(file));
}

function _lastModified(file) {
	return Date.parse(fso.GetFile(file).DateLastModified);
}

function _lineWrap(str, font, width) {
	return _(_gr.EstimateLineWrap(str, font, width).toArray())
		.filter(function (item, i) { return i % 2 == 0; })
		.map(function (line) {
			// only trim if line begins with single space.
			if (_.startsWith(line, ' ') && !_.startsWith(line, '  ')) return _.trim(line);
			else return line;
		})
		.value();
}

function _lockSize(w, h) {
	window.MinWidth = window.MaxWidth = w;
	window.MinHeight = window.MaxHeight = h;
}

function _menu(x, y, flags) {
	var menu = window.CreatePopupMenu();
	var file = new _main_menu_helper('File', 1000, menu);
	var edit = new _main_menu_helper('Edit', 2000, menu);
	var view = new _main_menu_helper('View', 3000, menu);
	var playback = new _main_menu_helper('Playback', 4000, menu);
	var library = new _main_menu_helper('Library', 5000, menu);
	var help = new _main_menu_helper('Help', 6000, menu);

	var idx = menu.TrackPopupMenu(x, y, flags);
	switch (true) {
	case idx == 0:
		break;
	case idx < 2000:
		file.mm.ExecuteByID(idx - 1000);
		break;
	case idx < 3000:
		edit.mm.ExecuteByID(idx - 2000);
		break;
	case idx < 4000:
		view.mm.ExecuteByID(idx - 3000);
		break;
	case idx < 5000:
		playback.mm.ExecuteByID(idx - 4000);
		break;
	case idx < 6000:
		library.mm.ExecuteByID(idx - 5000);
		break;
	case idx < 7000:
		help.mm.ExecuteByID(idx - 6000);
		break;
	}
	_dispose(menu, file, edit, view, playback, library, help);
}

function _main_menu_helper(name, base_id, main_menu) {
	this.Dispose = function () {
		this.mm.Dispose();
		this.popup.Dispose();
	}
	
	this.popup = window.CreatePopupMenu();
	this.mm = fb.CreateMainMenuManager();
	this.mm.Init(name);
	this.mm.BuildMenu(this.popup, base_id);
	this.popup.AppendTo(main_menu, MF_STRING, name);
}

function _open(file) {
	return utils.ReadUTF8(file);
}

function _p(name, default_) {
	Object.defineProperty(this, _.isBoolean(default_) ? 'enabled' : 'value', {
		get : function () {
			return this.val;
		},
		set : function (value) {
			this.val = value;
			window.SetProperty(this.name, this.val);
		}
	});
	
	this.toggle = function () {
		this.val = !this.val;
		window.SetProperty(this.name, this.val);
	}
	
	this.name = name;
	this.default_ = default_;
	this.val = window.GetProperty(name, default_);
}

function _q(value) {
	return '"' + value + '"';
}

function _recycleFile(file) {
	if (utils.IsFile(file)) {
		app.NameSpace(10).MoveHere(file);
	}
}

function _RGB(r, g, b) {
	return 0xFF000000 | r << 16 | g << 8 | b;
}

function _RGBA(r, g, b, a) {
	return a << 24 | r << 16 | g << 8 | b;
}

function _run() {
	try {
		WshShell.Run(_.map(arguments, _q).join(' '));
		return true;
	} catch (e) {
		return false;
	}
}

function _runCmd(command, wait) {
	try {
		WshShell.Run(command, 0, wait);
	} catch (e) {
	}
}

function _save(file, value) {
	if (utils.WriteTextFile(file, value)) {
		return true;
	}
	console.log('Error saving to ' + file);
	return false;
}

function _sb(t, x, y, w, h, v, fn) {
	this.paint = function (gr, colour) {
		gr.SetTextRenderingHint(4);
		if (this.v()) {
			gr.DrawString(this.t, this.font, colour, this.x, this.y, this.w, this.h, SF_CENTRE);
		}
	}
	
	this.containsXY = function (x, y) {
		return x > this.x && x < this.x + this.w && y > this.y && y < this.y + this.h && this.v();
	}
	
	this.move = function (x, y) {
		if (this.containsXY(x, y)) {
			window.SetCursor(IDC_HAND);
			return true;
		}
		
		//window.SetCursor(IDC_ARROW);
		return false;
	}
	
	this.lbtn_up = function (x, y) {
		if (this.containsXY(x, y)) {
			if (this.fn) {
				this.fn(x, y);
			}
			return true;
		}
		return false;
	}
	
	this.t = t;
	this.x = x;
	this.y = y;
	this.w = w;
	this.h = h;
	this.v = v;
	this.fn = fn;
	this.font = gdi.Font('FontAwesome', this.h);
}

function _setAlpha(colour, a) {
	return colour & 0x00ffffff | a << 24;
}

function _setClipboardData(value) {
	doc.parentWindow.clipboardData.setData('Text', value.toString());
}

function _scale(size) {
	return Math.round(size * DPI / 72);
}

function _stripTags(value) {
	doc.open();
	var div = doc.createElement('div');
	div.innerHTML = value.toString().replace(/<[Pp][^>]*>/g, '').replace(/<\/[Pp]>/g, '<br>').replace(/\n/g, '<br>');
	var tmp = _.trim(div.innerText);
	doc.close();
	return tmp;
}

function _tagged(value) {
	return value != '' && value != '?';
}

function _toRGB(a) {
	var b = a - 0xFF000000;
	return [b >> 16, b >> 8 & 0xFF, b & 0xFF];
}

function _ts() {
	return Math.floor(_.now() / 1000);
}

function _tt(value) {
	if (tooltip.Text != value) {
		tooltip.Text = value;
		tooltip.Activate();
	}
}

var doc = new ActiveXObject('htmlfile');
var app = new ActiveXObject('Shell.Application');
var WshShell = new ActiveXObject('WScript.Shell');
var fso = new ActiveXObject('Scripting.FileSystemObject');

var DT_LEFT = 0x00000000;
var DT_CENTER = 0x00000001;
var DT_RIGHT = 0x00000002;
var DT_VCENTER = 0x00000004;
var DT_WORDBREAK = 0x00000010;
var DT_CALCRECT = 0x00000400;
var DT_NOPREFIX = 0x00000800;
var DT_END_ELLIPSIS = 0x00008000;

var LEFT = DT_VCENTER | DT_END_ELLIPSIS | DT_CALCRECT | DT_NOPREFIX;
var RIGHT = DT_VCENTER | DT_RIGHT | DT_END_ELLIPSIS | DT_CALCRECT | DT_NOPREFIX;
var CENTRE = DT_VCENTER | DT_CENTER | DT_END_ELLIPSIS | DT_CALCRECT | DT_NOPREFIX;
var SF_CENTRE = 285212672;

var VK_ESCAPE = 0x1B;
var VK_SHIFT = 0x10;
var VK_LEFT = 0x25;
var VK_UP = 0x26;
var VK_RIGHT = 0x27;
var VK_DOWN = 0x28;

var MF_STRING = 0x00000000;
var MF_GRAYED = 0x00000001;

var IDC_ARROW = 32512;
var IDC_HAND = 32649;

var TPM_RIGHTALIGN = 0x0008;
var TPM_BOTTOMALIGN = 0x0020;

var ONE_DAY = 86400000;

var DEFAULT_ARTIST = '$meta(artist,0)';
var N = window.Name + ':';

var DPI = WshShell.RegRead('HKCU\\Control Panel\\Desktop\\WindowMetrics\\AppliedDPI');

var LM = _scale(5);
var TM = _scale(20);

var tooltip = window.CreateTooltip('Segoe UI', _scale(12));
tooltip.SetMaxWidth(1200);

var folders = {};
folders.home = fb.ComponentPath + 'samples\\';
folders.images = folders.home + 'images\\';
folders.data = fb.ProfilePath + 'js_data\\';
folders.artists = folders.data + 'artists\\';
folders.lastfm = folders.data + 'lastfm\\';

var fontawesome = gdi.Font('FontAwesome', 48);
var chars = {
	up : '\uF077',
	down : '\uF078',
	close : '\uF00D',
	rating_on : '\uF005',
	rating_off : '\uF006',
	heart_on : '\uF004',
	heart_off : '\uF08A',
	prev : '\uF049',
	next : '\uF050',
	play : '\uF04B',
	pause : '\uF04C',
	stop : '\uF04D',
	preferences : '\uF013',
	search : '\uF002',
	console : '\uF120',
	info : '\uF05A',
	audioscrobbler : '\uF202',
	minus : '\uF068',
	music : '\uF001',
	menu : '\uF0C9',
	hourglass : '\uF254',
};

var popup = {
	ok : 0,
	yes_no : 4,
	yes : 6,
	no : 7,
	stop : 16,
	question : 32,
	info : 64
};

var image = {
	crop : 0,
	crop_top : 1,
	stretch : 2,
	centre : 3
};

var ha_links = [
	['Title Formatting Reference', 'https://wiki.hydrogenaud.io/index.php?title=Foobar2000:Title_Formatting_Reference'],
	['Query Syntax', 'https://wiki.hydrogenaud.io/index.php?title=Foobar2000:Query_syntax'],
	['Homepage', 'https://www.foobar2000.org/'],
	['Components', 'https://www.foobar2000.org/components'],
	['Wiki', 'https://wiki.hydrogenaud.io/index.php?title=Foobar2000:Foobar2000'],
	['Forums', 'https://hydrogenaud.io/index.php/board,28.0.html']
];
