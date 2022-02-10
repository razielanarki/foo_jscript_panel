var _bmp = gdi.CreateImage(1, 1);
var _gr = _bmp.GetGraphics();

String.prototype.calc_width = function (font) {
	return _gr.CalcTextWidth(this, font);
}

String.prototype.repeat = function (num) {
	if (num >= 0 && num <= 5) {
		var g = Math.round(num);
	} else {
		return "";
	}
	return new Array(g + 1).join(this);
}

function get_images() {
	var gb;
	var cover_size = 200;

	images.noart = gdi.CreateImage(cover_size, cover_size);
	gb = images.noart.GetGraphics();
	gb.FillSolidRect(0, 0, cover_size, cover_size, g_color_normal_txt & 0x10ffffff);
	gb.SetTextRenderingHint(4);
	gb.DrawString("NO\nCOVER", g_font_cover, blendColors(g_color_normal_txt, g_color_normal_bg, 0.30), 1, 1, cover_size, cover_size, cc_stringformat);
	images.noart.ReleaseGraphics(gb);

	images.stream = gdi.CreateImage(cover_size, cover_size);
	gb = images.stream.GetGraphics();
	gb.FillSolidRect(0, 0, cover_size, cover_size, g_color_normal_txt & 0x10ffffff);
	gb.SetTextRenderingHint(4);
	gb.DrawString("STREAM", g_font_cover, blendColors(g_color_normal_txt, g_color_normal_bg, 0.30), 1, 1, cover_size, cover_size, cc_stringformat);
	images.stream.ReleaseGraphics(gb);

	images.all = gdi.CreateImage(cover_size, cover_size);
	gb = images.all.GetGraphics();
	gb.FillSolidRect(0, 0, cover_size, cover_size, g_color_normal_txt & 0x10ffffff);
	gb.SetTextRenderingHint(4);
	gb.DrawString("ALL\nITEMS", g_font_cover, blendColors(g_color_normal_txt, g_color_normal_bg, 0.30), 1, 1, cover_size, cover_size, cc_stringformat);
	images.all.ReleaseGraphics(gb);
}

function validate_indexes(playlist, item) {
	return playlist >=0 && playlist < plman.PlaylistCount && item >= 0 && item < plman.PlaylistItemCount(playlist);
}

function play(playlist, item) {
	if (validate_indexes(playlist, item)) {
		plman.ExecutePlaylistDefaultAction(playlist, item);
	}
}

function reload() {
	window.SetTimeout(function () {
		window.Reload();
	}, 50);
}

function reset_cover_timers() {
	if (timers.coverDone) {
		window.ClearTimeout(timers.coverDone);
		timers.coverDone = false;
	}
}

function generate_filename(cachekey, art_id) {
	var prefix = art_id == 4 ? "artist" : "front";
	return CACHE_FOLDER + prefix + cachekey + ".jpg";
}
0
function get_art(metadb, filename, art_id) {
	var img = images.cache[filename];
	if (img) return img;

	img = gdi.Image(filename);
	if (img) {
		images.cache[filename] = img;
		return img;
	}

	window.SetTimeout(function () {
		utils.GetAlbumArtAsync(window.ID, metadb, art_id, false, false);
	}, 10);
	return img;
}

function on_get_album_art_done(metadb, art_id, image, image_path) {
	if (!image) return;
	for (var i = 0; i < brw.groups.length; i++) {
		if (brw.groups[i].metadb && brw.groups[i].metadb.Compare(metadb)) {
			if (image.Width > 200 || image.Height > 200) {
				var s = Math.min(200 / image.Width, 200 / image.Height);
				var w = Math.floor(image.Width * s);
				var h = Math.floor(image.Height * s);
				image = image.Resize(w, h, 2);
			}
			var cached_filename = generate_filename(brw.groups[i].cachekey, art_id);
			image.SaveAs(cached_filename, "image/jpeg");
			images.cache[cached_filename] = image;
			brw.groups[i].cover_image = image;
			brw.repaint();
			break;
		}
	}
}

function drawImage(gr, img, dst_x, dst_y, dst_w, dst_h, auto_fill, border, alpha) {
	if (!img || !dst_w || !dst_h) {
		return;
	}
	gr.SetInterpolationMode(7);
	if (auto_fill) {
		if (img.Width / img.Height < dst_w / dst_h) {
			var src_x = 0;
			var src_w = img.Width;
			var src_h = Math.round(dst_h * img.Width / dst_w);
			var src_y = Math.round((img.Height - src_h) / 2);
		} else {
			var src_y = 0;
			var src_w = Math.round(dst_w * img.Height / dst_h);
			var src_h = img.Height;
			var src_x = Math.round((img.Width - src_w) / 2);
		}
		gr.DrawImage(img, dst_x, dst_y, dst_w, dst_h, src_x + 3, src_y + 3, src_w - 6, src_h - 6, 0, alpha || 255);
	} else {
		var s = Math.min(dst_w / img.Width, dst_h / img.Height);
		var w = Math.floor(img.Width * s);
		var h = Math.floor(img.Height * s);
		dst_x += Math.round((dst_w - w) / 2);
		dst_y += Math.round((dst_h - h) / 2);
		dst_w = w;
		dst_h = h;
		gr.DrawImage(img, dst_x, dst_y, dst_w, dst_h, 0, 0, img.Width, img.Height, 0, alpha || 255);
	}
	if (border) {
		gr.DrawRect(dst_x, dst_y, dst_w - 1, dst_h - 1, 1, border);
	}
}

function GetKeyboardMask() {
	if (utils.IsKeyPressed(VK_CONTROL))
		return KMask.ctrl;
	if (utils.IsKeyPressed(VK_SHIFT))
		return KMask.shift;
	return KMask.none;
}

function StringFormat() {
	var h_align = 0,
	v_align = 0,
	trimming = 0,
	flags = 0;
	switch (arguments.length) {
	case 3:
		trimming = arguments[2];
	case 2:
		v_align = arguments[1];
	case 1:
		h_align = arguments[0];
		break;
	default:
		return 0;
	}
	return ((h_align << 28) | (v_align << 24) | (trimming << 20) | flags);
}

function RGB(r, g, b) {
	return (0xff000000 | (r << 16) | (g << 8) | (b));
}

function RGBA(r, g, b, a) {
	return ((a << 24) | (r << 16) | (g << 8) | (b));
}

function toRGB(d) { // convert back to RGB values
	var d = d - 0xff000000;
	var r = d >> 16;
	var g = d >> 8 & 0xFF;
	var b = d & 0xFF;
	return [r, g, b];
}

function SetAlpha(colour, a) {
	return colour & 0x00ffffff | a << 24;
}

function blendColors(c1, c2, factor) {
	// When factor is 0, result is 100% color1, when factor is 1, result is 100% color2.
	var c1 = toRGB(c1);
	var c2 = toRGB(c2);
	var r = Math.round(c1[0] + factor * (c2[0] - c1[0]));
	var g = Math.round(c1[1] + factor * (c2[1] - c1[1]));
	var b = Math.round(c1[2] + factor * (c2[2] - c1[2]));
	return (0xff000000 | (r << 16) | (g << 8) | (b));
}

function num(strg, nb) {
	if (!strg) return "";
	var i;
	var str = strg.toString();
	var k = nb - str.length;
	if (k > 0) {
		for (i = 0; i < k; i++) {
			str = "0" + str;
		}
	}
	return str.toString();
}

function replaceAll(str, search, repl) {
	while (str.indexOf(search) != -1) {
		str = str.replace(search, repl);
	}
	return str;
}

var ButtonStates = {
	normal: 0,
	hover: 1,
	down: 2
};

var button = function (normal, hover, down) {
	this.w = normal.Width;
	this.h = normal.Height;
	this.img = [normal, hover, down];
	this.state = ButtonStates.normal;
	this.update = function (normal, hover, down) {
		this.w = normal.Width;
		this.h = normal.Height;
		this.img = [normal, hover, down];
	}
	this.draw = function (gr, x, y, alpha) {
		this.x = x;
		this.y = y;
		this.img[this.state] && gr.DrawImage(this.img[this.state], this.x, this.y, this.w, this.h, 0, 0, this.w, this.h, 0, alpha || 255);
	}
	this.repaint = function () {
		window.RepaintRect(this.x, this.y, this.w, this.h);
	}
	this.checkstate = function (event, x, y) {
		this.ishover = (x > this.x && x < this.x + this.w - 1 && y > this.y && y < this.y + this.h - 1);
		this.old = this.state;
		switch (event) {
		case "down":
			switch (this.state) {
			case ButtonStates.normal:
			case ButtonStates.hover:
				this.state = this.ishover ? ButtonStates.down : ButtonStates.normal;
				this.isdown = true;
				break;
			}
			break;
		case "up":
			this.state = this.ishover ? ButtonStates.hover : ButtonStates.normal;
			this.isdown = false;
			break;
		case "right":

			break;
		case "move":
			switch (this.state) {
			case ButtonStates.normal:
			case ButtonStates.hover:
				this.state = this.ishover ? ButtonStates.hover : ButtonStates.normal;
				break;
			}
			break;
		}
		if (this.state != this.old)
			this.repaint();
		return this.state;
	}
}

function setWallpaperImg() {
	if (g_wallpaperImg) {
		g_wallpaperImg.Dispose();
		g_wallpaperImg = null;
	}

	switch (ppt.wallpapermode) {
	case 0:
		return;
	case 1:
		var metadb = fb.GetNowPlaying();
		if (metadb) g_wallpaperImg = utils.GetAlbumArtV2(metadb, 0);
		break;
	case 2:
		g_wallpaperImg = gdi.Image(ppt.wallpaperpath);
		break;
	}

	if (g_wallpaperImg && ppt.wallpaperblurred) {
		g_wallpaperImg.StackBlur(ppt.wallpaperblurvalue);
	}
}

function playlist_can_add_items(playlistIndex) {
	return !(plman.GetPlaylistLockFilterMask(playlistIndex) & PlaylistLockFilterMask.filter_add);
}

function playlist_can_remove_items(playlistIndex) {
	return !(plman.GetPlaylistLockFilterMask(playlistIndex) & PlaylistLockFilterMask.filter_remove);
}

function playlist_can_rename(playlistIndex) {
	return !(plman.GetPlaylistLockFilterMask(playlistIndex) & PlaylistLockFilterMask.filter_rename);
}

function playlist_can_remove(playlistIndex) {
	return !(plman.GetPlaylistLockFilterMask(playlistIndex) & PlaylistLockFilterMask.filter_remove_playlist);
}

function scale(size) {
	return Math.round(size * g_fsize / 12);
}

function get_font() {
	var default_font;

	if (g_is_default_ui) {
		default_font = window.GetFontDUI(FontTypeDUI.playlists);
	} else {
		default_font = window.GetFontCUI(FontTypeCUI.items);
	}

	var name = default_font.Name;
	g_fsize = default_font.Size + ppt.extra_font_size;

	g_font = gdi.Font(name, scale(12), 0);
	g_font_bold = gdi.Font(name, scale(12), 1);
	g_font_box = gdi.Font(name, scale(10), 1);
	g_font_cover = gdi.Font(name, 40, 1);

	g_font_height = g_font.Height;
	g_font_bold_height = g_font_bold.Height;

	g_font_group1 = gdi.Font(name, scale(20), 1);
	g_font_group2 = gdi.Font(name, scale(16));
	g_font_awesome = gdi.Font("FontAwesome", scale(18));
	g_font_awesome_40 = gdi.Font("FontAwesome", 40);

	g_rating_width = chars.rating_off.repeat(5).calc_width(g_font_awesome) + 4;
	g_time_width = "00:00:00".calc_width(g_font) + 20;
}

function get_colors() {
	if (ppt.enableCustomColors) {
		g_color_normal_txt = window.GetProperty("SMOOTH.COLOUR.TEXT", RGB(255, 255, 255));
		g_color_normal_bg = window.GetProperty("SMOOTH.COLOUR.BACKGROUND.NORMAL", RGB(25, 25, 35));
		g_color_selected_bg = window.GetProperty("SMOOTH.COLOUR.BACKGROUND.SELECTED", RGB(15, 177, 255));
	} else {
		if (g_is_default_ui) {
			g_color_normal_txt = window.GetColourDUI(ColorTypeDUI.text);
			g_color_normal_bg = window.GetColourDUI(ColorTypeDUI.background);
			g_color_selected_bg = window.GetColourDUI(ColorTypeDUI.selection);
		} else {
			g_color_normal_txt = window.GetColourCUI(ColorTypeCUI.text);
			g_color_normal_bg = window.GetColourCUI(ColorTypeCUI.background);
			g_color_selected_bg = window.GetColourCUI(ColorTypeCUI.selection_background);
		}
	}
	get_images();
}

function on_font_changed() {
	get_font();
	get_metrics();
	brw.repaint();
}

function on_colours_changed() {
	get_colors();
	get_images();
	if (brw)
		brw.scrollbar.setNewColors();
	if (g_filterbox) { // null in jssp so must check
		g_filterbox.getImages();
		g_filterbox.reset_colors();
	}
	brw.repaint();
}

var AlbumArtId = {
	front: 0,
	back: 1,
	disc: 2,
	icon: 3,
	artist: 4
};

var ColorTypeCUI = {
	text: 0,
	selection_text: 1,
	inactive_selection_text: 2,
	background: 3,
	selection_background: 4,
	inactive_selection_background: 5,
	active_item_frame: 6
};

var FontTypeCUI = {
	items: 0,
	labels: 1
};

var ColorTypeDUI = {
	text: 0,
	background: 1,
	highlight: 2,
	selection: 3
};

var FontTypeDUI = {
	defaults: 0,
	tabs: 1,
	lists: 2,
	playlists: 3,
	statusbar: 4,
	console: 5
};

var PlaylistLockFilterMask = {
	filter_add : 1,
	filter_remove: 2,
	filter_reorder: 4,
	filter_replace: 8,
	filter_rename: 16,
	filter_remove_playlist: 32,
	filter_default_action: 64
};

var KMask = {
	none: 0,
	ctrl: 1,
	shift: 2,
};

var StringAlignment = {
	Near: 0,
	Centre: 1,
	Far: 2
};

var images = {
	cache : {},
}

var timers = {
	mouseWheel: false,
	mouseDown: false,
	movePlaylist: false,
};

var ppt = {
	tf_crc_path: fb.TitleFormat("$crc32(%path%)"),
	enableCustomColors: window.GetProperty("SMOOTH.CUSTOM.COLOURS.ENABLED", false),
	extra_font_size: window.GetProperty("SMOOTH.EXTRA.FONT.SIZE", 0),
	showHeaderBar: window.GetProperty("SMOOTH.SHOW.TOP.BAR", true),
	autoFill: window.GetProperty("SMOOTH.AUTO.FILL", true),
	wallpapermode: window.GetProperty("SMOOTH.WALLPAPER.MODE", false), // 0 none, 1 front cover 2 custom image
	wallpaperblurred: window.GetProperty("SMOOTH.WALLPAPER.BLURRED", false),
	wallpaperpath: window.GetProperty("SMOOTH.WALLPAPER.PATH", ""),
	wallpaperalpha: 50,
	wallpaperblurvalue: 50,
	enableTouchControl: window.GetProperty("SMOOTH.TOUCH.CONTROL", true),
	refreshRate: 40,
	defaultHeaderBarHeight: 25,
	headerBarHeight: 25,
	scrollSmoothness: 2.5,
	rowScrollStep: 3,
};

var chars = {
	rating_on : '\uF005',
	rating_off : '\uF006',
	lock : "\uF023",
	list : "\uF0C9",
	play : '\uF04B',
	pause : '\uF04C',
	heart_on : '\uF004',
	heart_off : '\uF08A',
	close : '\uF00D',
	left : '\uF060',
	right : '\uF061',
	up : '\uF077',
	down : '\uF078',
	search : '\uF002',
};

var cTouch = {
	down: false,
	y_start: 0,
	y_end: 0,
	y_current: 0,
	y_prev: 0,
	y_move: 0,
	scroll_delta: 0,
	t1: null,
	timer: false,
	multiplier: 0,
	delta: 0
};

var lt_stringformat = StringFormat(StringAlignment.Near, StringAlignment.Near);
var ct_stringformat = StringFormat(StringAlignment.Centre, StringAlignment.Near);
var rt_stringformat = StringFormat(StringAlignment.Far, StringAlignment.Near);
var lc_stringformat = StringFormat(StringAlignment.Near, StringAlignment.Centre);
var cc_stringformat = StringFormat(StringAlignment.Centre, StringAlignment.Centre);
var rc_stringformat = StringFormat(StringAlignment.Far, StringAlignment.Centre);
var lb_stringformat = StringFormat(StringAlignment.Near, StringAlignment.Far);
var cb_stringformat = StringFormat(StringAlignment.Centre, StringAlignment.Far);
var rb_stringformat = StringFormat(StringAlignment.Far, StringAlignment.Far);

var fso = new ActiveXObject("Scripting.FileSystemObject");
var CACHE_FOLDER = fb.ProfilePath + "js_smooth_cache\\";
if (!fso.FolderExists(CACHE_FOLDER)) fso.CreateFolder(CACHE_FOLDER);

var g_is_default_ui = window.InstanceType == 1;

var MF_STRING = 0x00000000;
var MF_GRAYED = 0x00000001;

var COLOR_WINDOW = 5;
var COLOR_HIGHLIGHT = 13;
var COLOR_BTNFACE = 15;
var COLOR_BTNTEXT = 18;

var IDC_ARROW = 32512;
var IDC_IBEAM = 32513;
var IDC_WAIT = 32514;
var IDC_CROSS = 32515;
var IDC_UPARROW = 32516;
var IDC_SIZE = 32640;
var IDC_ICON = 32641;
var IDC_SIZENWSE = 32642;
var IDC_SIZENESW = 32643;
var IDC_SIZEWE = 32644;
var IDC_SIZENS = 32645;
var IDC_SIZEALL = 32646;
var IDC_NO = 32648;
var IDC_APPSTARTING = 32650;
var IDC_HAND = 32649;
var IDC_HELP = 32651;

var DT_LEFT = 0x00000000;
var DT_RIGHT = 0x00000002;
var DT_TOP = 0x00000000;
var DT_BOTTOM = 0x00000008;
var DT_CENTER = 0x00000001;
var DT_VCENTER = 0x00000004;
var DT_WORDBREAK = 0x00000010;
var DT_SINGLELINE = 0x00000020;
var DT_CALCRECT = 0x00000400;
var DT_NOPREFIX = 0x00000800;
var DT_EDITCONTROL = 0x00002000;
var DT_END_ELLIPSIS = 0x00008000;

var VK_F1 = 0x70;
var VK_F2 = 0x71;
var VK_F3 = 0x72;
var VK_F4 = 0x73;
var VK_F5 = 0x74;
var VK_F6 = 0x75;
var VK_BACK = 0x08;
var VK_TAB = 0x09;
var VK_RETURN = 0x0D;
var VK_SHIFT = 0x10;
var VK_CONTROL = 0x11;
var VK_ALT = 0x12;
var VK_ESCAPE = 0x1B;
var VK_PGUP = 0x21;
var VK_PGDN = 0x22;
var VK_END = 0x23;
var VK_HOME = 0x24;
var VK_LEFT = 0x25;
var VK_UP = 0x26;
var VK_RIGHT = 0x27;
var VK_DOWN = 0x28;
var VK_INSERT = 0x2D;
var VK_DELETE = 0x2E;
var VK_SPACEBAR = 0x20;

var g_font = null;
var g_font_bold = null;
var g_font_box = null;
var g_font_cover = null;
var g_font_group1 = null;
var g_font_group2 = null;
var g_font_awesome = null;
var g_font_awesome_40 = null;
var g_font_height =0;
var g_font_bold_height = 0;
var g_fsize = 0;

var g_color_normal_txt = 0;
var g_color_normal_bg = 0;
var g_color_selected_bg = 0;

var g_time_width = 0;
var g_rating_width = 0;

var g_filter_text = "";
var g_wallpaperImg = null;
var isScrolling = false;
var need_repaint = false;
var g_start_ = 0, g_end_ = 0;
var m_x = 0, m_y = 0;
var scroll_ = 0, scroll = 0, scroll_prev = 0;
var ww = 0, wh = 0;
