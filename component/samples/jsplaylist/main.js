var _bmp = gdi.CreateImage(1, 1);
var _gr = _bmp.GetGraphics();

String.prototype.calc_width = function (font) {
	return _gr.CalcTextWidth(this, font);
}

Number.prototype.calc_width = function (font) {
	return _gr.CalcTextWidth(this.toString(), font);
}

String.prototype.repeat = function (num) {
	if (num >= 0 && num <= 5) {
		var g = Math.round(num);
	} else {
		return "";
	}
	return new Array(g + 1).join(this);
}

window.MinWidth = 360;
window.MinHeight = 200;

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

var g_script_version = "v2022";
var g_LDT = DT_LEFT | DT_VCENTER | DT_CALCRECT | DT_NOPREFIX | DT_END_ELLIPSIS;
var g_middle_clicked = false;
var g_middle_click_timeout = false;
var g_textbox_tabbed = false;
var g_init_on_size = false;
var g_seconds = 0;
var g_mouse_wheel_timeout = false;
var g_active_playlist = plman.ActivePlaylist;
var g_image_cache = new image_cache();
var g_selHolder = fb.AcquireUiSelectionHolder();
g_selHolder.SetPlaylistSelectionTracking();

var g_drag_drop_status = false;
var g_drag_drop_bottom = false;
var g_drag_drop_track_id = -1;
var g_drag_drop_row_id = -1;
var g_drag_drop_playlist_id = -1;
var g_drag_drop_playlist_manager_hover = false;
var g_drag_drop_internal = false;

var g_color_normal_bg = 0;
var g_color_selected_bg = 0;
var g_color_normal_txt = 0;
var g_font_size = 0;

var ww = 0, wh = 0;
var mouse_x = 0, mouse_y = 0;
var need_repaint = false;
var foo_playcount = utils.CheckComponent("foo_playcount");
var foo_lastfm_playcount_sync = utils.CheckComponent("foo_lastfm_playcount_sync");
var foo_lastfm = utils.CheckComponent("foo_lastfm");
var tfo = {};
var tf_group_key = null;

var KMask = {
	none: 0,
	ctrl: 1,
	shift: 2
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

var StringAlignment = {
	Near: 0,
	Centre: 1,
	Far: 2
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

var AlbumArtId = {
	front: 0,
	back: 1,
	disc: 2,
	icon: 3,
	artist: 4
};

var ButtonStates = {
	normal: 0,
	hover: 1,
	down: 2
};

var PlaylistLockFilterMask = {
	filter_add: 1,
	filter_remove: 2,
	filter_reorder: 4,
	filter_replace: 8,
	filter_rename: 16,
	filter_remove_playlist: 32,
	filter_default_action: 64
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
};

var properties = {
	enableCustomColors: window.GetProperty("SYSTEM.Enable Custom Colors", false),
	collapseGroupsByDefault : window.GetProperty("SYSTEM.Collapse Groups by default", false),
	autocollapse : window.GetProperty("SYSTEM.Auto-Collapse", false),
	showgroupheaders : window.GetProperty("GROUP.Show Group Headers", true),
	showscrollbar : window.GetProperty("CUSTOM Show Scrollbar", true),
	showwallpaper : window.GetProperty("CUSTOM Show Wallpaper", false),
	wallpaperalpha : window.GetProperty("CUSTOM Wallpaper Alpha", 192),
	wallpaperblurred : window.GetProperty("CUSTOM Wallpaper Blurred", true),
	wallpaperblurvalue : window.GetProperty("CUSTOM Wallpaper StackBlur value", 60),
	wallpapertype : window.GetProperty("CUSTOM Wallpaper Type", 0),
	wallpaperpath : window.GetProperty("CUSTOM Default Wallpaper Path", "user-components\\foo_jscript_panel\\samples\\images\\jsplaylist\\default.jpg"),
	settingspanel : false,
	max_columns : 24,
	max_patterns : 25,
	use_foo_lastfm_playcount_sync : window.GetProperty("Love tracks with foo_lastfm_playcount_sync", false),
};

var images = {
	beam : null,
	sortdirection : null,
	wallpaper : null,
};

var cRow = {
	default_playlist_h : window.GetProperty("SYSTEM.Playlist Row Height in Pixel", 28),
	playlist_h : 29,
};

var p = {
	topbar : null,
	headerBar : null,
	list : null,
	playlistManager : null,
	settings : null,
	on_key_timeout : false
};

var cSettings = {
	visible : false,
	topBarHeight : 50,
	tabPaddingWidth : 16,
	rowHeight : 30,
	wheel_timeout : false
};

var cPlaylistManager = {
	width : 220,
	rowHeight : 28,
	showStatusBar : true,
	statusBarHeight : 18,
	step : 50,
	visible : window.GetProperty("SYSTEM.PlaylistManager.Visible", false),
	visible_on_launch : false,
	drag_move_timeout : false,
	hscroll_interval : false,
	vscroll_interval : false,
	vscroll_timeout : false,
	blink_interval : false,
	blink_counter : -1,
	blink_id : null,
	drag_clicked : false,
	drag_moved : false,
	drag_target_id : -1,
	drag_source_id : -1,
	drag_x : -1,
	drag_y : -1,
	drag_dropped : false,
	rightClickedId : null,
	init_timeout : false,
	inputbox_timeout : false,
	sortPlaylists_timer : false
};

var cTopBar = {
	height : 54,
	visible : window.GetProperty("SYSTEM.TopBar.Visible", true)
};

var cHeaderBar = {
	height : 26,
	borderWidth : 2,
	locked : window.GetProperty("SYSTEM.HeaderBar.Locked", true),
	timerAutoHide : false,
	sortRequested : false
};

var cScrollBar = {
	defaultWidth : 17,
	width : 17,
	buttonType : {
		cursor : 0,
		up : 1,
		down : 2
	},
	interval : false,
	timeout : false,
	timer_counter : 0,
	repaint_timeout : false,
};

var cGroup = {
	default_collapsed_height : 3,
	default_expanded_height : 3,
	collapsed_height : 3,
	expanded_height : 3,
	default_count_minimum : window.GetProperty("GROUP.Minimum number of rows in a group", 0),
	count_minimum : window.GetProperty("GROUP.Minimum number of rows in a group", 0),
	extra_rows : 0,
	pattern_idx : window.GetProperty("SYSTEM.Groups.Pattern Index", 0)
};

var cover = {
	show : true,
	column : false,
	timeout : false,
	repaint_timeout : false,
	w : 250,
	h : 250,
};

var cList = {
	scrollstep : window.GetProperty("SYSTEM.Playlist Scroll Step", 3),
	scroll_timer : false,
	scroll_delta : cRow.playlist_h,
	scroll_direction : 1,
	scroll_step : Math.floor(cRow.playlist_h / 3),
	scroll_div : 2,
	borderWidth : 2,
	beam_interval : false,
	enableExtraLine : window.GetProperty("SYSTEM.Enable Extra Line", true)
};

var columns = {
	mood_x : 0,
	mood_w : 0,
	rating_x : 0,
	rating_w : 0,
};

init();

function DrawCover(gr, img, dst_x, dst_y, dst_w, dst_h) {
	if (img) {
		var s = Math.min(dst_w / img.Width, dst_h / img.Height);
		var w = Math.floor(img.Width * s);
		var h = Math.floor(img.Height * s);
		dst_x += Math.round((dst_w - w) / 2);
		dst_w = w;
		dst_h = h;
		gr.SetInterpolationMode(7);
		gr.DrawImage(img, dst_x, dst_y, dst_w, dst_h, 0, 0, img.Width, img.Height);
	}
	gr.DrawRect(dst_x, dst_y, dst_w - 1, dst_h - 1, 1, g_color_normal_txt);
}

function DrawWallpaper(gr) {
	if (images.wallpaper.Width / images.wallpaper.Height < ww / wh) {
		var src_x = 0;
		var src_w = images.wallpaper.Width;
		var src_h = Math.round(wh * images.wallpaper.Width / ww);
		var src_y = Math.round((images.wallpaper.Height - src_h) / 2);
	} else {
		var src_y = 0;
		var src_w = Math.round(ww * images.wallpaper.Height / wh);
		var src_h = images.wallpaper.Height;
		var src_x = Math.round((images.wallpaper.Width - src_w) / 2);
	}
	gr.SetInterpolationMode(7);
	gr.DrawImage(images.wallpaper, 0, 0, ww, wh, src_x, src_y, src_w, src_h);
	gr.FillSolidRect(0, 0, ww, wh, g_color_normal_bg & RGBA(255, 255, 255, properties.wallpaperalpha));
}

function DrawColoredText(gr, text, font, default_color, x, y, w, h, alignment) {
	var color = default_color;
	var txt = "", tmp = "";
	var lg = 0, i = 1, z = 0;
	var pos = text.indexOf(String.fromCharCode(3));
	if (pos < 0) { // no specific color
		gr.GdiDrawText(text, font, default_color, x, y, w, h, alignment | DT_CALCRECT | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
	} else {
		var tab = text.split(String.fromCharCode(3));
		var fin = tab.length;

		switch (alignment) {
		case DT_CENTER:
			var full_lg = gr.CalcTextWidth(tab[0], font);
			for (var m = i; m < fin; m += 2) {
				full_lg += gr.CalcTextWidth(tab[m + 1], font);
			};
			if (full_lg > w)
				full_lg = w;
			var delta_align = ((w - full_lg) / 2);
			break;
		case DT_RIGHT:
			var full_lg = gr.CalcTextWidth(tab[0], font);
			for (var m = i; m < fin; m += 2) {
				full_lg += gr.CalcTextWidth(tab[m + 1], font);
			};
			if (full_lg > w)
				full_lg = w;
			var delta_align = (w - full_lg);
			break;
		default:
			var delta_align = 0;
		}

		if (pos > 0) {
			txt = tab[0];
			lg = gr.CalcTextWidth(txt, font);
			gr.GdiDrawText(txt, font, color, x + delta_align + z, y, w - z, h, DT_LEFT | DT_CALCRECT | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
			z += lg;
		}

		while (i < fin && z < w) {
			tmp = tab[i];
			color = eval("0xFF" + tmp.substr(4, 2) + tmp.substr(2, 2) + tmp.substr(0, 2));
			txt = tab[i + 1];
			lg = gr.CalcTextWidth(txt, font);
			gr.GdiDrawText(txt, font, color, x + delta_align + z, y, w - z, h, DT_LEFT | DT_CALCRECT | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
			z += lg;
			i += 2;
		}
	}
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

function GetKeyboardMask() {
	if (utils.IsKeyPressed(VK_CONTROL))
		return KMask.ctrl;
	if (utils.IsKeyPressed(VK_SHIFT))
		return KMask.shift;
	return KMask.none;
}

function SetAlpha(colour, a) {
	return colour & 0x00ffffff | a << 24;
}

function RGB(r, g, b) {
	return (0xff000000 | (r << 16) | (g << 8) | (b));
}

function RGBA(r, g, b, a) {
	return ((a << 24) | (r << 16) | (g << 8) | (b));
}

function toRGB(d) {
	var d = d - 0xff000000;
	var r = d >> 16;
	var g = d >> 8 & 0xFF;
	var b = d & 0xFF;
	return [r, g, b];
}

function blendColors(c1, c2, factor) {
	var c1 = toRGB(c1);
	var c2 = toRGB(c2);
	var r = Math.round(c1[0] + factor * (c2[0] - c1[0]));
	var g = Math.round(c1[1] + factor * (c2[1] - c1[1]));
	var b = Math.round(c1[2] + factor * (c2[2] - c1[2]));
	return (0xff000000 | (r << 16) | (g << 8) | (b));
}

function num(strg, nb) {
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

function button(normal, hover, down) {
	this.img = [normal, hover, down];
	this.w = this.img[0].Width;
	this.h = this.img[0].Height;
	this.state = ButtonStates.normal;

	this.update = function (normal, hover, down) {
		this.img = [normal, hover, down];
		this.w = this.img[0].Width;
		this.h = this.img[0].Height;
	}

	this.draw = function (gr, x, y, alpha) {
		this.x = x;
		this.y = y;
		this.img[this.state] && gr.DrawImage(this.img[this.state], this.x, this.y, this.w, this.h, 0, 0, this.w, this.h, 0, alpha || 255);
	}

	this.checkstate = function (event, x, y) {
		this.ishover = (x > this.x && x < this.x + this.w - 1 && y > this.y && y < this.y + this.h - 1);
		var old = this.state;
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
		case "move":
			switch (this.state) {
			case ButtonStates.normal:
			case ButtonStates.hover:
				this.state = this.ishover ? ButtonStates.hover : ButtonStates.normal;
				break;
			}
			break;
		}

		if (this.state != old) {
			window.RepaintRect(this.x, this.y, this.w, this.h);
		}
		return this.state;
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

function playlist_can_reorder(playlistIndex) {
	return !(plman.GetPlaylistLockFilterMask(playlistIndex) & PlaylistLockFilterMask.filter_reorder);
}

function get_tfo(pattern) {
	if (!tfo[pattern]) {
		tfo[pattern] = fb.TitleFormat(pattern);
	}
	return tfo[pattern];
}

function scale(size) {
	return Math.round(size * g_font_size / 12);
}

function renamePlaylist() {
	if (!p.playlistManager.inputbox.text || p.playlistManager.inputbox.text == "" || p.playlistManager.inputboxID == -1)
		p.playlistManager.inputbox.text = p.playlistManager.playlists[p.playlistManager.inputboxID].name;
	if (p.playlistManager.inputbox.text.length > 1 || (p.playlistManager.inputbox.text.length == 1 && (p.playlistManager.inputbox.text >= "a" && p.playlistManager.inputbox.text <= "z") || (p.playlistManager.inputbox.text >= "A" && p.playlistManager.inputbox.text <= "Z") || (p.playlistManager.inputbox.text >= "0" && p.playlistManager.inputbox.text <= "9"))) {
		p.playlistManager.playlists[p.playlistManager.inputboxID].name = p.playlistManager.inputbox.text;
		plman.RenamePlaylist(p.playlistManager.playlists[p.playlistManager.inputboxID].idx, p.playlistManager.inputbox.text);
		full_repaint();
	}
	p.playlistManager.inputboxID = -1;
}

function inputboxPlaylistManager_activate() {
	if (cPlaylistManager.inputbox_timeout) {
		window.ClearTimeout(cPlaylistManager.inputbox_timeout);
		cPlaylistManager.inputbox_timeout = false;
	}

	p.playlistManager.inputbox.on_focus(true);
	p.playlistManager.inputbox.edit = true;
	p.playlistManager.inputbox.Cpos = p.playlistManager.inputbox.text.length;
	p.playlistManager.inputbox.anchor = p.playlistManager.inputbox.Cpos;
	p.playlistManager.inputbox.SelBegin = p.playlistManager.inputbox.Cpos;
	p.playlistManager.inputbox.SelEnd = p.playlistManager.inputbox.Cpos;
	if (!cInputbox.cursor_interval) {
		p.playlistManager.inputbox.resetCursorTimer();
	}
	p.playlistManager.inputbox.dblclk = true;
	p.playlistManager.inputbox.SelBegin = 0;
	p.playlistManager.inputbox.SelEnd = p.playlistManager.inputbox.text.length;
	p.playlistManager.inputbox.text_selected = p.playlistManager.inputbox.text;
	p.playlistManager.inputbox.select = true;
	full_repaint();
}

function togglePlaylistManager() {
	if (!cPlaylistManager.hscroll_interval) {
		if (cPlaylistManager.visible) {
			cPlaylistManager.hscroll_interval = window.SetInterval(function () {
				p.playlistManager.repaint();
				p.playlistManager.woffset -= cPlaylistManager.step;
				if (p.playlistManager.woffset <= 0) {
					p.playlistManager.woffset = 0;
					cPlaylistManager.visible = false;
					window.SetProperty("SYSTEM.PlaylistManager.Visible", cPlaylistManager.visible);
					p.headerBar.button.update(p.headerBar.slide_open, p.headerBar.slide_open, p.headerBar.slide_open);
					full_repaint();
					window.ClearInterval(cPlaylistManager.hscroll_interval);
					cPlaylistManager.hscroll_interval = false;
				}
			}, 16);
		} else {
			p.playlistManager.refresh();
			cPlaylistManager.hscroll_interval = window.SetInterval(function () {
				p.playlistManager.woffset += cPlaylistManager.step;
				if (p.playlistManager.woffset >= cPlaylistManager.width) {
					p.playlistManager.woffset = cPlaylistManager.width;
					cPlaylistManager.visible = true;
					window.SetProperty("SYSTEM.PlaylistManager.Visible", cPlaylistManager.visible);
					p.headerBar.button.update(p.headerBar.slide_close, p.headerBar.slide_close, p.headerBar.slide_close);
					full_repaint();
					window.ClearInterval(cPlaylistManager.hscroll_interval);
					cPlaylistManager.hscroll_interval = false;
				} else {
					p.playlistManager.repaint();
				}
			}, 16);
		}
	}
}

function on_get_album_art_done(metadb, art_id, image, image_path) {
	var cover_metadb = null;
	for (var i = 0; i < p.list.items.length; i++) {
		if (p.list.items[i].metadb && p.list.items[i].metadb.Compare(metadb)) {
			p.list.items[i].cover_img = g_image_cache.set(metadb, image);
			if (!g_mouse_wheel_timeout && !cScrollBar.interval && !cover.repaint_timeout) {
				cover.repaint_timeout = window.SetTimeout(function () {
					cover.repaint_timeout = false;
					if (!g_mouse_wheel_timeout && !cScrollBar.interval)
						full_repaint();
				}, 75);
			}
			break;
		}
	}
}

function image_cache() {
	this.get = function (metadb) {
		var d = properties.showgroupheaders ? tf_group_key.EvalWithMetadb(metadb) : metadb.Path;
		var img = this.cachelist[d];
		if (!img && !cover.timeout) {
			cover.timeout = window.SetTimeout(function () {
				cover.timeout = false;
				utils.GetAlbumArtAsync(window.ID, metadb);
			}, (g_mouse_wheel_timeout || cScrollBar.interval ? 20 : 10));
		}
		return img;
	}

	this.set = function (metadb, image) {
		var max = 250;
		var img;
		if (image) {
			if (image.Width > max || image.Height > max) {
				var s = Math.min(max / image.Width, max / image.Height);
				var w = Math.floor(image.Width * s);
				var h = Math.floor(image.Height * s);
				img = image.Resize(w, h, 2);
			} else {
				img = image;
			}
		} else {
			img = images.nocover;
		}
		var d = properties.showgroupheaders ? tf_group_key.EvalWithMetadb(metadb) : metadb.Path;
		this.cachelist[d] = img;
		return img;
	}

	this.reset = function () {
		for (var key in this.cachelist) {
			this.cachelist[key].Dispose();
		}
		this.cachelist = {};
	}

	this.cachelist = {};
}

function reset_cover_timers() {
	if (cover.timeout) {
		window.ClearTimeout(cover.timeout);
		cover.timeout = false;
	}
}

function full_repaint() {
	need_repaint = true;
}

function resize_panels() {
	cRow.playlist_h = scale(cRow.default_playlist_h);
	if (cList.enableExtraLine) {
		cRow.playlist_h += scale(6);
	}

	p.topBar.setSize(0, 0, ww, cTopBar.visible ? cTopBar.height + cHeaderBar.borderWidth : 0);

	p.headerBar.visible = cHeaderBar.locked;
	p.headerBar.setSize(0, p.topBar.h, ww, cHeaderBar.height);
	p.headerBar.calculateColumns();

	// set Size of List
	var list_h = wh - p.topBar.h - (p.headerBar.visible ? p.headerBar.h + cHeaderBar.borderWidth : 0);
	p.list.setSize(0, wh - list_h, ww, list_h);
	if (g_init_on_size) {
		p.list.setItems(true);
	}

	// set Size of scrollbar
	p.scrollbar.setSize(p.list.x + p.list.w - cScrollBar.width, p.list.y, cScrollBar.width, p.list.h);
	p.scrollbar.setCursor(p.list.totalRowVisible, p.list.totalRows, p.list.offset);

	// set Size of Settings
	p.settings.setSize(0, 0, ww, wh);

	// set Size of PlaylistManager
	cPlaylistManager.visible_on_launch = cPlaylistManager.visible;
	if (cPlaylistManager.visible) {
		cPlaylistManager.visible = g_init_on_size;
		p.playlistManager.woffset = g_init_on_size ? 0 : cPlaylistManager.width;
	}
	p.playlistManager.setSize(ww, p.list.y, cPlaylistManager.width, p.list.h);
	p.playlistManager.refresh();
}

function init() {
	get_font();
	get_colors();
	plman.SetActivePlaylistContext();
	images.wallpaper = get_wallpaper();

	if (!properties.showgroupheaders) {
		cGroup.collapsed_height = 0;
		cGroup.expanded_height = 0;
	}

	p.list = new oList("p.list");
	p.topBar = new oTopBar();
	p.headerBar = new oHeaderBar();
	p.headerBar.initColumns();
	p.scrollbar = new PlaylistScrollBar();

	p.playlistManager = new oPlaylistManager();
	p.settings = new oSettings();

	window.SetInterval(function () {
		if (!window.IsVisible) {
			need_repaint = true;
			return;
		}

		if (need_repaint) {
			need_repaint = false;
			window.Repaint();
		}
	}, 40);
}

function on_size() {
	ww = window.Width;
	wh = window.Height;

	resize_panels();

	if (p.headerBar.columns[0].percent > 0) {
		cover.column = true;
		cGroup.count_minimum = Math.ceil((p.headerBar.columns[0].w) / cRow.playlist_h);
		if (cGroup.count_minimum < cGroup.default_count_minimum)
			cGroup.count_minimum = cGroup.default_count_minimum;
	} else {
		cover.column = false;
		cGroup.count_minimum = cGroup.default_count_minimum;
	}

	if (!g_init_on_size) {
		properties.collapseGroupsByDefault = p.list.groupby[cGroup.pattern_idx].collapseGroupsByDefault != 0;
		update_playlist(properties.collapseGroupsByDefault);
		if (cPlaylistManager.visible_on_launch) {
			if (!cPlaylistManager.init_timeout) {
				cPlaylistManager.init_timeout = window.SetTimeout(function () {
					cPlaylistManager.init_timeout = false;
					togglePlaylistManager();
				}, 150);
			}
		}
		g_init_on_size = true;
	}
}

function on_paint(gr) {
	if (cSettings.visible) {
		p.settings && p.settings.draw(gr);
	} else {
		if (fb.IsPlaying && properties.showwallpaper && images.wallpaper) {
			DrawWallpaper(gr);
		} else {
			gr.FillSolidRect(0, 0, ww, wh, g_color_normal_bg);
		}

		// List
		if (p.list) {
			if (p.list.count > 0) {
				// calculate columns metrics before drawing row contents!
				p.headerBar.calculateColumns();

				// scrollbar
				if (properties.showscrollbar && p.scrollbar && p.list.totalRows > 0 && (p.list.totalRows > p.list.totalRowVisible)) {
					p.scrollbar.visible = true;
					p.scrollbar.draw(gr);
				} else {
					p.scrollbar.visible = false;
				}

				// draw rows of the playlist
				p.list.draw(gr);

				// draw flashing beam if scroll max reached on mouse wheel! (android like effect)
				if (p.list.beam > 0) {
					var beam_h = Math.floor(cRow.playlist_h * 7 / 4);
					var alpha = (p.list.beam_alpha <= 255 ? p.list.beam_alpha : 255);
					switch (p.list.beam) {
					case 1: // top beam
						gr.DrawImage(images.beam, p.list.x, p.list.y - cHeaderBar.borderWidth * 10, p.list.w, beam_h - cHeaderBar.borderWidth, 0, 0, images.beam.Width, images.beam.Height, 180, alpha);
						break;
					case 2: // bot beam
						gr.DrawImage(images.beam, p.list.x, p.list.y + p.list.h - beam_h + cHeaderBar.borderWidth * 10, p.list.w, beam_h, 0, 0, images.beam.Width, images.beam.Height, 0, alpha);
						break;
					}
				}
			} else {
				if (plman.PlaylistCount > 0) {
					var text_top = plman.GetPlaylistName(g_active_playlist);
					var text_bot = "This playlist is empty";
				} else {
					var text_top = "JSPlaylist " + g_script_version + " coded by Br3tt";
					var text_bot = "Create a playlist to start!";
				}
				var y = Math.floor(wh / 2);
				gr.GdiDrawText(text_top, g_font_19_1, g_color_normal_txt, 0, y - g_z5 - g_font_19_1.Height, ww, g_font_19_1.Height, DT_CENTER | DT_BOTTOM | DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE | DT_END_ELLIPSIS);
				gr.FillGradRect(40, Math.floor(wh / 2), ww - 80, 1, 0, 0, g_color_normal_txt & 0x40ffffff, 0.5);
				gr.GdiDrawText(text_bot, g_font_12_1, blendColors(g_color_normal_txt, g_color_normal_bg, 0.35), 0, y + g_z5, ww, g_font_12_1.Height, DT_CENTER | DT_TOP | DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE | DT_END_ELLIPSIS);
			}
		}

		// draw background part above playlist (topbar + headerbar)
		if (cTopBar.visible || p.headerBar.visible) {
			gr.FillSolidRect(0, 0, ww, p.list.y, g_color_normal_bg);
		}

		// TopBar
		if (cTopBar.visible) {
			p.topBar && p.topBar.draw(gr);
		}

		// HeaderBar
		if (p.headerBar.visible) {
			p.headerBar && p.headerBar.drawColumns(gr);
			if (p.headerBar.borderDragged && p.headerBar.borderDraggedId >= 0) {
				// all borders
				for (var b = 0; b < p.headerBar.borders.length; b++) {
					var lg_x = p.headerBar.borders[b].x - 2;
					var lg_w = p.headerBar.borders[b].w;
					var segment_h = 5;
					var gap_h = 5;
					if (b == p.headerBar.borderDraggedId) {
						var d = ((mouse_x / 10) - Math.floor(mouse_x / 10)) * 10; // give a value between [0;9]
					} else {
						d = 5;
					}
					var ty = 0;
					for (var lg_y = p.list.y; lg_y < p.list.y + p.list.h + segment_h; lg_y += segment_h + gap_h) {
						ty = lg_y - segment_h + d;
						th = segment_h;
						if (ty < p.list.y) {
							th = th - Math.abs(p.list.y - ty);
							ty = p.list.y;
						}
						if (b == p.headerBar.borderDraggedId) {
							gr.FillSolidRect(lg_x, ty, lg_w, th, g_color_normal_txt & 0x32ffffff);
						} else {
							gr.FillSolidRect(lg_x, ty, lg_w, th, g_color_normal_txt & 0x16ffffff);
						}
					}
				}
			}
		} else {
			p.headerBar && p.headerBar.drawHiddenPanel(gr);
		}

		// PlaylistManager
		p.playlistManager && p.playlistManager.draw(gr);
	}
}

function on_mouse_lbtn_down(x, y) {
	// check settings
	if (cSettings.visible) {
		p.settings.on_mouse("down", x, y);
	} else {
		// check list
		p.list.check("down", x, y);

		// check scrollbar
		if (!cPlaylistManager.visible) {
			if (p.playlistManager.woffset == 0 && properties.showscrollbar && p.scrollbar && p.list.totalRows > 0 && (p.list.totalRows > p.list.totalRowVisible)) {
				p.scrollbar.check("down", x, y);
			}

			// check scrollbar scroll on click above or below the cursor
			if (p.scrollbar.hover && !p.scrollbar.cursorDrag) {
				var scrollstep = p.list.totalRowVisible;
				if (y < p.scrollbar.cursorPos) {
					if (!p.list.buttonclicked && !cScrollBar.timeout) {
						p.list.buttonclicked = true;
						p.list.scrollItems(1, scrollstep);
						cScrollBar.timeout = window.SetTimeout(function () {
							cScrollBar.timeout = false;
							p.list.scrollItems(1, scrollstep);
							if (cScrollBar.interval) window.ClearInterval(cScrollBar.interval);
							cScrollBar.interval = window.SetInterval(function () {
								if (p.scrollbar.hover) {
									if (mouse_x > p.scrollbar.x && p.scrollbar.cursorPos > mouse_y) {
										p.list.scrollItems(1, scrollstep);
									}
								}
							}, 60);
						}, 400);
					}
				} else {
					if (!p.list.buttonclicked && !cScrollBar.timeout) {
						p.list.buttonclicked = true;
						p.list.scrollItems(-1, scrollstep);
						cScrollBar.timeout = window.SetTimeout(function () {
							cScrollBar.timeout = false;
							p.list.scrollItems(-1, scrollstep);
							if (cScrollBar.interval) window.ClearInterval(cScrollBar.interval);
							cScrollBar.interval = window.SetInterval(function () {
								if (p.scrollbar.hover) {
									if (mouse_x > p.scrollbar.x && p.scrollbar.cursorPos + p.scrollbar.cursorHeight < mouse_y) {
										p.list.scrollItems(-1, scrollstep);
									}
								}
							}, 60);
						}, 400)
					}
				}
			}
		} else {
			p.playlistManager.check("down", x, y);
		}

		// check topbar
		if (cTopBar.visible)
			p.topBar.buttonCheck("down", x, y);
		// check headerbar
		if (p.headerBar.visible)
			p.headerBar.on_mouse("down", x, y);
	}
}

function on_mouse_lbtn_dblclk(x, y, mask) {
	// check settings
	if (cSettings.visible) {
		p.settings.on_mouse("dblclk", x, y);
	} else {
		// check list
		p.list.check("dblclk", x, y);
		// check headerbar
		if (p.headerBar.visible)
			p.headerBar.on_mouse("dblclk", x, y);

		// check scrollbar
		if (!cPlaylistManager.visible) {
			if (properties.showscrollbar && p.scrollbar && p.list.totalRows > 0 && (p.list.totalRows > p.list.totalRowVisible)) {
				p.scrollbar.check("dblclk", x, y);
				if (p.scrollbar.hover) {
					on_mouse_lbtn_down(x, y);
				}
			}
		} else {
			p.playlistManager.check("dblclk", x, y);
		}
	}
}

function on_mouse_lbtn_up(x, y) {
	if (cSettings.visible) {
		p.settings.on_mouse("up", x, y);
	} else {
		// scrollbar scrolls up and down RESET
		p.list.buttonclicked = false;
		if (cScrollBar.timeout) {
			window.ClearTimeout(cScrollBar.timeout);
			cScrollBar.timeout = false;
		}
		if (cScrollBar.interval) {
			window.ClearInterval(cScrollBar.interval);
			cScrollBar.interval = false;
		}

		// check list
		p.list.check("up", x, y);

		// playlist manager (if visible)
		if (p.playlistManager.woffset > 0 || cPlaylistManager.visible) {
			p.playlistManager.check("up", x, y);
		}

		// check scrollbar
		if (properties.showscrollbar && p.scrollbar && p.list.totalRows > 0 && (p.list.totalRows > p.list.totalRowVisible)) {
			p.scrollbar.check("up", x, y);
		}

		// Drop items after a drag'n drop INSIDE the playlist
		// FIXME

		// check topbar
		if (cTopBar.visible)
			p.topBar.buttonCheck("up", x, y);

		// check headerbar
		if (p.headerBar.visible)
			p.headerBar.on_mouse("up", x, y);

		// repaint on mouse up to refresh covers just loaded
		full_repaint();
	}
}

function on_mouse_rbtn_up(x, y) {
	if (cSettings.visible) {
		p.settings.on_mouse("right", x, y);
	} else {
		// check list
		p.list.check("right", x, y);
		// check headerbar
		if (p.headerBar.visible)
			p.headerBar.on_mouse("right", x, y);
		// playlist manager
		if (p.playlistManager.ishoverItem || p.playlistManager.ishoverHeader) {
			p.playlistManager.check("right", x, y);
		}
	}
	return true;
}

function on_mouse_move(x, y) {
	if (x == mouse_x && y == mouse_y)
		return;

	// check settings
	if (cSettings.visible) {
		p.settings.on_mouse("move", x, y);
	} else {
		// playlist manager (if visible)
		if (p.playlistManager.woffset > 0) {
			if (!cPlaylistManager.blink_interval) {
				p.playlistManager.check("move", x, y);
			}
		}

		// check list
		p.list.check("move", x, y);

		// check scrollbar
		if (!cPlaylistManager.visible) {
			if (properties.showscrollbar && p.scrollbar && p.list.totalRows > 0 && (p.list.totalRows > p.list.totalRowVisible)) {
				p.scrollbar.check("move", x, y);
			}
		}

		// check headerbar
		if (p.headerBar.visible)
			p.headerBar.on_mouse("move", x, y);

		// check toolbar for mouse icon dragging mode ***
		if (cPlaylistManager.drag_moved) {
			if (p.playlistManager.ishoverItem) {
				window.SetCursor(IDC_HELP);
			} else {
				window.SetCursor(IDC_NO);
			}
		}
	}

	// save coords
	mouse_x = x;
	mouse_y = y;
}

function on_mouse_wheel(delta) {
	if (g_middle_clicked)
		return;

	// check settings
	if (cSettings.visible) {
		p.settings.on_mouse("wheel", mouse_x, mouse_y, delta);
		if (cSettings.wheel_timeout) window.ClearTimeout(cSettings.wheel_timeout);
		cSettings.wheel_timeout = window.SetTimeout(function () {
			cSettings.wheel_timeout = false;
			on_mouse_move(mouse_x + 1, mouse_y + 1);
		}, 50);
	} else {
		// handle p.list Beam
		var limit_reached = false;
		var maxOffset = (p.list.totalRows > p.list.totalRowVisible ? p.list.totalRows - p.list.totalRowVisible : 0);
		if (maxOffset > 0) {
			if (delta > 0) { // scroll up requested
				if (p.list.offset == 0) {
					// top beam to draw
					p.list.beam = 1;
					cList.beam_sens = 1;
					limit_reached = true;
				}
			} else { // scroll down requested
				if (p.list.offset >= maxOffset) {
					// bottom beam to draw
					p.list.beam = 2;
					cList.beam_sens = 1;
					limit_reached = true;
				}
			}
			if (limit_reached) {
				if (!cList.beam_interval) {
					p.list.beam_alpha = 0;
					cList.beam_interval = window.SetInterval(function () {
						if (cList.beam_sens == 1) {
							p.list.beam_alpha = (p.list.beam_alpha <= 275 ? p.list.beam_alpha + 25 : 300);
							if (p.list.beam_alpha >= 300) {
								cList.beam_sens = 2;
							}
						} else {
							p.list.beam_alpha = (p.list.beam_alpha >= 25 ? p.list.beam_alpha - 25 : 0);
							if (p.list.beam_alpha <= 0) {
								p.list.beam = 0;
								window.ClearInterval(cList.beam_interval);
								cList.beam_interval = false;
							}
						}
						full_repaint();
					}, 32);
				}
			}
		}

		reset_cover_timers();

		if (p.list.ishover || cScrollBar.timeout) {
			if (!g_mouse_wheel_timeout) {
				g_mouse_wheel_timeout = window.SetTimeout(function () {
					g_mouse_wheel_timeout = false;
					p.list.scrollItems(delta, cList.scrollstep);
				}, 20);
			}
		} else {
			p.playlistManager.check("wheel", mouse_x, mouse_y, delta);
		}
	}
}

function on_mouse_mbtn_down(x, y, mask) {
	g_middle_clicked = true;
	togglePlaylistManager();
}

function on_mouse_mbtn_up(x, y, mask) {
	if (g_middle_click_timeout) window.ClearTimeout(g_middle_click_timeout);
	g_middle_click_timeout = window.SetTimeout(function () {
		g_middle_click_timeout = false;
		g_middle_clicked = false;
	}, 250);
}

function update_playlist(iscollapsed) {
	g_group_id_focused = 0;
	p.list.updateHandleList(iscollapsed);

	p.list.setItems(false);
	p.scrollbar.setCursor(p.list.totalRowVisible, p.list.totalRows, p.list.offset);
	if (cHeaderBar.sortRequested) {
		window.SetCursor(IDC_ARROW);
		cHeaderBar.sortRequested = false;
	}
}

function on_playlist_switch() {
	g_active_playlist = plman.ActivePlaylist
	update_playlist(properties.collapseGroupsByDefault);
	p.topBar.setDatas();
	p.headerBar.resetSortIndicators();
	full_repaint();
}

function on_playlists_changed() {
	g_active_playlist = plman.ActivePlaylist;

	p.topBar.setDatas();
	if (cPlaylistManager.visible && cPlaylistManager.drag_dropped) {
		window.SetCursor(IDC_ARROW);
	}
	p.playlistManager.refresh();
	full_repaint();
}

function on_playlist_items_added(playlist_idx) {
	if (playlist_idx == g_active_playlist) {
		update_playlist(properties.collapseGroupsByDefault);
		p.topBar.setDatas();
		p.headerBar.resetSortIndicators();
		full_repaint();
	}
}

function on_playlist_items_removed(playlist_idx, new_count) {
	if (playlist_idx == g_active_playlist) {
		update_playlist(properties.collapseGroupsByDefault);
		p.topBar.setDatas();
		p.headerBar.resetSortIndicators();
		full_repaint();
	}
}

function on_playlist_items_reordered(playlist_idx) {
	if (playlist_idx == g_active_playlist && p.headerBar.columnDragged == 0) {
		update_playlist(properties.collapseGroupsByDefault);
		p.headerBar.resetSortIndicators();
		full_repaint();
	} else {
		p.headerBar.columnDragged = 0;
	}
}

function on_playback_queue_changed() {
	full_repaint();
}

function on_item_focus_change(playlist, from, to) {
	if (playlist == g_active_playlist) {
		p.list.focusedTrackId = to;
		var center_focus_item = p.list.isFocusedItemVisible();

		if (properties.autocollapse) {
			var grpId = p.list.getGroupIdfromTrackId(to);
			if (grpId >= 0) {
				if (p.list.groups[grpId].collapsed) {
					p.list.updateGroupStatus(grpId);
					p.list.setItems(true);
					center_focus_item = p.list.isFocusedItemVisible();
				} else {
					if ((!center_focus_item && !p.list.drawRectSel) || (center_focus_item && to == 0)) {
						p.list.setItems(true);
					}
				}
			}
		} else {
			if ((!center_focus_item && !p.list.drawRectSel) || (center_focus_item && to == 0)) {
				p.list.setItems(true);
			}
		}
		p.scrollbar.setCursor(p.list.totalRowVisible, p.list.totalRows, p.list.offset);
	}
	full_repaint();
}

function on_metadb_changed() {
	p.list.setItems(false);
	full_repaint();
}

function on_playlist_items_selection_change() {
	full_repaint();
}

function on_key_up(vkey) {
	if (!cSettings.visible) {
		p.list.keypressed = false;
		if (cScrollBar.timeout) {
			window.ClearTimeout(cScrollBar.timeout);
			cScrollBar.timeout = false;
		}
		if (cScrollBar.interval) {
			window.ClearInterval(cScrollBar.interval);
			cScrollBar.interval = false;
		}
		if (vkey == VK_SHIFT) {
			p.list.SHIFT_start_id = null;
			p.list.SHIFT_count = 0;
		}
	}
}

function on_key_down(vkey) {
	if (cSettings.visible) {
		g_textbox_tabbed = false;
		var elements = p.settings.pages[p.settings.currentPageId].elements;
		for (var j = 0; j < elements.length; j++) {
			if (typeof elements[j].on_key_down == "function") elements[j].on_key_down(vkey);
		}
	} else {
		if (p.playlistManager.inputboxID >= 0) {
			switch (vkey) {
			case VK_ESCAPE:
			case 222:
				p.playlistManager.inputboxID = -1;
				full_repaint();
				break;
			default:
				p.playlistManager.inputbox.on_key_down(vkey);
			}
		} else {
			var mask = GetKeyboardMask();
			if (mask == KMask.none) {
				switch (vkey) {
				case VK_F2:
					// rename playlist (playlist manager panel visible)
					if (cPlaylistManager.visible && playlist_can_rename(g_active_playlist)) {
						p.playlistManager.inputbox = new oInputbox(p.playlistManager.w - p.playlistManager.border - p.playlistManager.scrollbarWidth - scale(40), cPlaylistManager.rowHeight - 10, plman.GetPlaylistName(g_active_playlist), "", "renamePlaylist()", 0, 12, 225);
						p.playlistManager.inputboxID = g_active_playlist;
						// activate box content + selection activated
						if (cPlaylistManager.inputbox_timeout) window.ClearTimeout(cPlaylistManager.inputbox_timeout);
						cPlaylistManager.inputbox_timeout = window.SetTimeout(inputboxPlaylistManager_activate, 20);
					}
					break;
				case VK_F5:
					// refresh covers
					g_image_cache.reset();
					full_repaint();
					break;
				case VK_TAB:
					togglePlaylistManager();
					break;
				case VK_UP:
					var scrollstep = 1;
					var new_focus_id = 0;
					if (p.list.count > 0 && !p.list.keypressed && !cScrollBar.timeout) {
						p.list.keypressed = true;
						reset_cover_timers();

						if (p.list.focusedTrackId < 0) {
							var old_grpId = 0;
						} else {
							var old_grpId = p.list.getGroupIdfromTrackId(p.list.focusedTrackId);
						}
						new_focus_id = (p.list.focusedTrackId > 0) ? p.list.focusedTrackId - scrollstep : 0;
						var grpId = p.list.getGroupIdfromTrackId(new_focus_id);
						if (!properties.autocollapse) {
							if (p.list.groups[old_grpId].collapsed) {
								if (old_grpId > 0 && old_grpId == grpId) {
									new_focus_id = (p.list.groups[grpId].start > 0) ? p.list.groups[grpId].start - scrollstep : 0;
									var grpId = p.list.getGroupIdfromTrackId(new_focus_id);
								}
							}
						}

						if (p.list.groups[grpId].collapsed) {
							if (properties.autocollapse) {
								new_focus_id = p.list.groups[grpId].start + p.list.groups[grpId].count - 1;
							} else {
								new_focus_id = p.list.groups[grpId].start;
							}
						}

						if (p.list.focusedTrackId == 0 && p.list.offset > 0) {
							p.list.scrollItems(1, scrollstep);
							cScrollBar.timeout = window.SetTimeout(function () {
								cScrollBar.timeout = false;
								p.list.scrollItems(1, scrollstep);
								if (cScrollBar.interval) window.ClearInterval(cScrollBar.interval);
								cScrollBar.interval = window.SetInterval(function () {
									p.list.scrollItems(1, scrollstep);
								}, 50);
							}, 400);
						} else {
							plman.SetPlaylistFocusItem(g_active_playlist, new_focus_id);
							plman.ClearPlaylistSelection(g_active_playlist);
							plman.SetPlaylistSelectionSingle(g_active_playlist, new_focus_id, true);
							cScrollBar.timeout = window.SetTimeout(function () {
								cScrollBar.timeout = false;
								if (cScrollBar.interval) window.ClearInterval(cScrollBar.interval);
								cScrollBar.interval = window.SetInterval(function () {
									new_focus_id = (p.list.focusedTrackId > 0) ? p.list.focusedTrackId - scrollstep : 0;
									// if new track focused id is in a collapsed group, set the 1st track of the group as the focused track (= group focused)
									var grpId = p.list.getGroupIdfromTrackId(new_focus_id);
									if (p.list.groups[grpId].collapsed) {
										if (properties.autocollapse) {
											new_focus_id = p.list.groups[grpId].start + p.list.groups[grpId].count - 1;
										} else {
											new_focus_id = p.list.groups[grpId].start;
										}
									}
									plman.SetPlaylistFocusItem(g_active_playlist, new_focus_id);
									plman.ClearPlaylistSelection(g_active_playlist);
									plman.SetPlaylistSelectionSingle(g_active_playlist, new_focus_id, true);
								}, 50);
							}, 400);
						}
					}
					break;
				case VK_DOWN:
					var new_focus_id = 0;
					if (p.list.count > 0 && !p.list.keypressed && !cScrollBar.timeout) {
						p.list.keypressed = true;
						reset_cover_timers();

						if (p.list.focusedTrackId < 0) {
							var old_grpId = 0;
						} else {
							var old_grpId = p.list.getGroupIdfromTrackId(p.list.focusedTrackId);
						}
						new_focus_id = (p.list.focusedTrackId < p.list.count - 1) ? p.list.focusedTrackId + 1 : p.list.count - 1;
						var grpId = p.list.getGroupIdfromTrackId(new_focus_id);
						if (!properties.autocollapse) {
							if (p.list.groups[old_grpId].collapsed) {
								if (old_grpId < (p.list.groups.length - 1) && old_grpId == grpId) {
									new_focus_id = ((p.list.groups[grpId].start + p.list.groups[grpId].count - 1) < (p.list.count - 1)) ? (p.list.groups[grpId].start + p.list.groups[grpId].count - 1) + 1 : p.list.count - 1;
									var grpId = p.list.getGroupIdfromTrackId(new_focus_id);
								}
							}
						}

						if (p.list.groups[grpId].collapsed) {
							if (properties.autocollapse) {
								new_focus_id = p.list.groups[grpId].start;
							} else {
								new_focus_id = p.list.groups[grpId].start + p.list.groups[grpId].count - 1;
							}
						}
						plman.SetPlaylistFocusItem(g_active_playlist, new_focus_id);
						plman.ClearPlaylistSelection(g_active_playlist);
						plman.SetPlaylistSelectionSingle(g_active_playlist, new_focus_id, true);
						cScrollBar.timeout = window.SetTimeout(function () {
							cScrollBar.timeout = false;
							if (cScrollBar.interval) window.ClearInterval(cScrollBar.interval);
							cScrollBar.interval = window.SetInterval(function () {
								new_focus_id = (p.list.focusedTrackId < p.list.count - 1) ? p.list.focusedTrackId + 1 : p.list.count - 1;
								var grpId = p.list.getGroupIdfromTrackId(new_focus_id);
								if (p.list.groups[grpId].collapsed) {
									if (properties.autocollapse) {
										new_focus_id = p.list.groups[grpId].start;
									} else {
										new_focus_id = p.list.groups[grpId].start + p.list.groups[grpId].count - 1;
									}
								}
								plman.SetPlaylistFocusItem(g_active_playlist, new_focus_id);
								plman.ClearPlaylistSelection(g_active_playlist);
								plman.SetPlaylistSelectionSingle(g_active_playlist, new_focus_id, true);
							}, 50);
						}, 400);
					}
					break;
				case VK_PGUP:
					var scrollstep = p.list.totalRowVisible;
					var new_focus_id = 0;
					if (p.list.count > 0 && !p.list.keypressed && !cScrollBar.timeout) {
						p.list.keypressed = true;
						reset_cover_timers();
						new_focus_id = (p.list.focusedTrackId > scrollstep) ? p.list.focusedTrackId - scrollstep : 0;
						if (p.list.focusedTrackId == 0 && p.list.offset > 0) {
							p.list.scrollItems(1, scrollstep);
							cScrollBar.timeout = window.SetTimeout(function () {
								cScrollBar.timeout = false;
								p.list.scrollItems(1, scrollstep);
								if (cScrollBar.interval) window.ClearInterval(cScrollBar.interval);
								cScrollBar.interval = window.SetInterval(function () {
									p.list.scrollItems(1, scrollstep);
								}, 60);
							}, 400);
						} else {
							plman.SetPlaylistFocusItem(g_active_playlist, new_focus_id);
							plman.ClearPlaylistSelection(g_active_playlist);
							plman.SetPlaylistSelectionSingle(g_active_playlist, new_focus_id, true);
							cScrollBar.timeout = window.SetTimeout(function () {
								cScrollBar.timeout = false;
								if (cScrollBar.interval) window.ClearInterval(cScrollBar.interval);
								cScrollBar.interval = window.SetInterval(function () {
									new_focus_id = (p.list.focusedTrackId > scrollstep) ? p.list.focusedTrackId - scrollstep : 0;
									plman.SetPlaylistFocusItem(g_active_playlist, new_focus_id);
									plman.ClearPlaylistSelection(g_active_playlist);
									plman.SetPlaylistSelectionSingle(g_active_playlist, new_focus_id, true);
								}, 60);
							}, 400);
						}
					}
					break;
				case VK_PGDN:
					var scrollstep = p.list.totalRowVisible;
					var new_focus_id = 0;
					if (p.list.count > 0 && !p.list.keypressed && !cScrollBar.timeout) {
						p.list.keypressed = true;
						reset_cover_timers();
						new_focus_id = (p.list.focusedTrackId < p.list.count - scrollstep) ? p.list.focusedTrackId + scrollstep : p.list.count - 1;
						plman.SetPlaylistFocusItem(g_active_playlist, new_focus_id);
						plman.ClearPlaylistSelection(g_active_playlist);
						plman.SetPlaylistSelectionSingle(g_active_playlist, new_focus_id, true);
						cScrollBar.timeout = window.SetTimeout(function () {
							cScrollBar.timeout = false;
							if (cScrollBar.interval) window.ClearInterval(cScrollBar.interval);
							cScrollBar.interval = window.SetInterval(function () {
								new_focus_id = (p.list.focusedTrackId < p.list.count - scrollstep) ? p.list.focusedTrackId + scrollstep : p.list.count - 1;
								plman.SetPlaylistFocusItem(g_active_playlist, new_focus_id);
								plman.ClearPlaylistSelection(g_active_playlist);
								plman.SetPlaylistSelectionSingle(g_active_playlist, new_focus_id, true);
							}, 60);
						}, 400);
					}
					break;
				case VK_RETURN:
					plman.ExecutePlaylistDefaultAction(g_active_playlist, p.list.focusedTrackId);
					break;
				case VK_END:
					if (p.list.count > 0) {
						plman.SetPlaylistFocusItem(g_active_playlist, p.list.count - 1);
						plman.ClearPlaylistSelection(g_active_playlist);
						plman.SetPlaylistSelectionSingle(g_active_playlist, p.list.count - 1, true);
					}
					break;
				case VK_HOME:
					if (p.list.count > 0) {
						plman.SetPlaylistFocusItem(g_active_playlist, 0);
						plman.ClearPlaylistSelection(g_active_playlist);
						plman.SetPlaylistSelectionSingle(g_active_playlist, 0, true);
					}
					break;
				case VK_DELETE:
					if (playlist_can_remove_items(g_active_playlist)) {
						plman.UndoBackup(g_active_playlist);
						plman.RemovePlaylistSelection(g_active_playlist, false);
					}
					break;
				}
			} else {
				switch (mask) {
				case KMask.shift:
					switch (vkey) {
					case VK_SHIFT: // SHIFT key alone
						p.list.SHIFT_count = 0;
						break;
					case VK_UP: // SHIFT + KEY UP
						if (p.list.SHIFT_count == 0) {
							if (p.list.SHIFT_start_id == null) {
								p.list.SHIFT_start_id = p.list.focusedTrackId;
							}
							plman.ClearPlaylistSelection(g_active_playlist);
							plman.SetPlaylistSelectionSingle(g_active_playlist, p.list.focusedTrackId, true);
							if (p.list.focusedTrackId > 0) {
								p.list.SHIFT_count--;
								p.list.focusedTrackId--;
								plman.SetPlaylistSelectionSingle(g_active_playlist, p.list.focusedTrackId, true);
								plman.SetPlaylistFocusItem(g_active_playlist, p.list.focusedTrackId);
							}
						} else if (p.list.SHIFT_count < 0) {
							if (p.list.focusedTrackId > 0) {
								p.list.SHIFT_count--;
								p.list.focusedTrackId--;
								plman.SetPlaylistSelectionSingle(g_active_playlist, p.list.focusedTrackId, true);
								plman.SetPlaylistFocusItem(g_active_playlist, p.list.focusedTrackId);
							}
						} else {
							plman.SetPlaylistSelectionSingle(g_active_playlist, p.list.focusedTrackId, false);
							p.list.SHIFT_count--;
							p.list.focusedTrackId--;
							plman.SetPlaylistFocusItem(g_active_playlist, p.list.focusedTrackId);
						}
						break;
					case VK_DOWN: // SHIFT + KEY DOWN
						if (p.list.SHIFT_count == 0) {
							if (p.list.SHIFT_start_id == null) {
								p.list.SHIFT_start_id = p.list.focusedTrackId;
							}
							plman.ClearPlaylistSelection(g_active_playlist);
							plman.SetPlaylistSelectionSingle(g_active_playlist, p.list.focusedTrackId, true);
							if (p.list.focusedTrackId < p.list.count - 1) {
								p.list.SHIFT_count++;
								p.list.focusedTrackId++;
								plman.SetPlaylistSelectionSingle(g_active_playlist, p.list.focusedTrackId, true);
								plman.SetPlaylistFocusItem(g_active_playlist, p.list.focusedTrackId);
							}
						} else if (p.list.SHIFT_count > 0) {
							if (p.list.focusedTrackId < p.list.count - 1) {
								p.list.SHIFT_count++;
								p.list.focusedTrackId++;
								plman.SetPlaylistSelectionSingle(g_active_playlist, p.list.focusedTrackId, true);
								plman.SetPlaylistFocusItem(g_active_playlist, p.list.focusedTrackId);
							}
						} else {
							plman.SetPlaylistSelectionSingle(g_active_playlist, p.list.focusedTrackId, false);
							p.list.SHIFT_count++;
							p.list.focusedTrackId++;
							plman.SetPlaylistFocusItem(g_active_playlist, p.list.focusedTrackId);
						}
						break;
					}
					break;
				case KMask.ctrl:
					if (vkey == 65) { // CTRL+A
						fb.RunMainMenuCommand("Edit/Select all");
						full_repaint();
					}
					if (vkey == 88) { // CTRL+X
						if (playlist_can_remove_items(g_active_playlist)) {
							var items = plman.GetPlaylistSelectedItems(g_active_playlist);
							if (fb.CopyHandleListToClipboard(items)) {
								plman.UndoBackup(g_active_playlist);
								plman.RemovePlaylistSelection(g_active_playlist);
							}
							items.Dispose();
						}
					}
					if (vkey == 67) { // CTRL+C
						var items = plman.GetPlaylistSelectedItems(g_active_playlist);
						fb.CopyHandleListToClipboard(items);
						items.Dispose();
					}
					if (vkey == 86) { // CTRL+V
						if (playlist_can_add_items(g_active_playlist) && fb.CheckClipboardContents()) {
							var items = fb.GetClipboardContents();
							plman.UndoBackup(g_active_playlist);
							plman.InsertPlaylistItems(g_active_playlist, p.list.focusedTrackId + 1, items, false);
							items.Dispose();
						}
					}
					if (vkey == 73) { // CTRL+I
						cTopBar.visible = !cTopBar.visible;
						window.SetProperty("SYSTEM.TopBar.Visible", cTopBar.visible);
						resize_panels();
						full_repaint();
					}
					if (vkey == 84) { // CTRL+T
						// Toggle Toolbar
						if (!p.on_key_timeout) {
							cHeaderBar.locked = !cHeaderBar.locked;
							window.SetProperty("SYSTEM.HeaderBar.Locked", cHeaderBar.locked);
							if (!cHeaderBar.locked) {
								p.headerBar.visible = false;
							}
							resize_panels();
							full_repaint();
							p.on_key_timeout = window.SetTimeout(function () {
								p.on_key_timeout = false;
							}, 300);
						}
					}
					if (vkey == 89) { // CTRL+Y
						fb.RunMainMenuCommand("Edit/Redo");
					}
					if (vkey == 90) { // CTRL+Z
						fb.RunMainMenuCommand("Edit/Undo");
					}
					break;
				}
			}
		}
	}
}

function on_char(code) {
	if (cSettings.visible) {
		for (var i = 0; i < p.settings.pages.length; i++) {
			for (var j = 0; j < p.settings.pages[i].elements.length; j++) {
				if (p.settings.pages[i].elements[j].objType == "TB") p.settings.pages[i].elements[j].on_char(code);
			}
		}
	} else if (p.playlistManager.inputboxID >= 0) {
		p.playlistManager.inputbox.on_char(code);
	}
}

function on_playback_new_track() {
	images.wallpaper = get_wallpaper();
	full_repaint();
}

function on_playback_dynamic_info_track(type) {
	if (type == 1) {
		images.wallpaper = get_wallpaper();
		full_repaint();
	}
}

function on_playback_stop(reason) {
	if (reason != 2) {
		images.wallpaper = null
		full_repaint();
	}
}

function on_playback_pause(state) {
	if (p.list.nowplaying_y + cRow.playlist_h > p.list.y && p.list.nowplaying_y < p.list.y + p.list.h) {
		window.RepaintRect(p.list.x, p.list.nowplaying_y, p.list.w, cRow.playlist_h);
	}
}

function on_playback_time(time) {
	g_seconds = time;
	if (!cSettings.visible && p.list.nowplaying_y + cRow.playlist_h > p.list.y && p.list.nowplaying_y < p.list.y + p.list.h)
		window.RepaintRect(p.list.x, p.list.nowplaying_y, p.list.w, cRow.playlist_h);
}

function on_focus(is_focused) {
	if (p.playlistManager.inputboxID >= 0) {
		p.playlistManager.inputbox.on_focus(is_focused);
	}
	if (is_focused) {
		plman.SetActivePlaylistContext();
		g_selHolder.SetPlaylistSelectionTracking();
	} else {
		p.playlistManager.inputboxID = -1;
		full_repaint();
	}
}

function on_font_changed() {
	get_font();
	p.topBar.setButtons();
	p.headerBar.setButtons();
	p.scrollbar.setButtons();
	p.scrollbar.setCursorButton();
	p.playlistManager.setButtons();
	p.settings.setButtons();
	resize_panels();
	full_repaint();
}

function on_colours_changed() {
	get_colors();
	p.topBar.setButtons();
	p.headerBar.setButtons();
	p.scrollbar.setButtons();
	p.scrollbar.setCursorButton();
	p.playlistManager.setButtons();
	p.settings.setButtons();
	full_repaint();
}

function get_font() {
	var font = window.InstanceType == 1 ? window.GetFontDUI(0) : window.GetFontCUI(0);
	var name = font.Name;
	g_font_size = font.Size;

	cTopBar.height = scale(54);
	cHeaderBar.height = scale(26);
	cHeaderBar.borderWidth = scale(2);
	cSettings.topBarHeight = scale(50);
	cSettings.tabPaddingWidth = scale(16);
	cSettings.rowHeight = scale(30);
	cPlaylistManager.width = scale(220);
	cPlaylistManager.rowHeight = scale(28);
	cPlaylistManager.statusBarHeight = scale(18);
	cScrollBar.width = scale(cScrollBar.defaultWidth);

	g_z2 = scale(2);
	g_z4 = scale(4);
	g_z5 = scale(5);
	g_z10 = scale(10);
	g_z16 = scale(16);

	g_font_12 = gdi.Font(name, scale(12));
	g_font_12_1 = gdi.Font(name, scale(12), 1);
	g_font_12_3 = gdi.Font(name, scale(12), 3);
	g_font_12_5 = gdi.Font(name, scale(12), 5);

	g_font_15_1 = gdi.Font(name, scale(15), 1);
	g_font_19_1 = gdi.Font(name, scale(19), 1);
	g_font_21_3 = gdi.Font(name, scale(21), 3);

	g_font_awesome_12 = gdi.Font("FontAwesome", scale(12));
	g_font_awesome_20 = gdi.Font("FontAwesome", scale(20));

	g_font_group1 = gdi.Font(name, scale(16), 0);
	g_font_group2 = gdi.Font(name, scale(14), 0);

	columns.rating_w = (chars.rating_off.calc_width(g_font_awesome_20) * 5) + 2;
}

function get_colors() {
	if (properties.enableCustomColors) {
		g_color_normal_txt = window.GetProperty("SYSTEM.COLOUR TEXT NORMAL", RGB(180, 180, 180));
		g_color_normal_bg = window.GetProperty("SYSTEM.COLOUR BACKGROUND NORMAL", RGB(25, 25, 35));
		g_color_selected_bg = window.GetProperty("SYSTEM.COLOUR BACKGROUND SELECTED", RGB(130,150,255));
	} else {
		if (window.InstanceType == 1) {
			g_color_normal_txt = window.GetColourDUI(ColorTypeDUI.text);
			g_color_normal_bg = window.GetColourDUI(ColorTypeDUI.background);
			g_color_selected_bg = window.GetColourDUI(ColorTypeDUI.selection);
		} else {
			g_color_normal_txt = window.GetColourCUI(ColorTypeCUI.text);
			g_color_normal_bg = window.GetColourCUI(ColorTypeCUI.background);
			g_color_selected_bg = window.GetColourCUI(ColorTypeCUI.selection_background);
		}
	}

	g_line_colour = blendColors(g_color_normal_txt, g_color_normal_bg, 0.45);

	if (images.sortdirection) images.sortdirection.Dispose();
	images.sortdirection = gdi.CreateImage(7, 5);
	var gb = images.sortdirection.GetGraphics();
	gb.FillSolidRect(1, 1, 5, 1, g_color_normal_txt);
	gb.FillSolidRect(2, 2, 3, 1, g_color_normal_txt);
	gb.FillSolidRect(3, 3, 1, 1, g_color_normal_txt);
	images.sortdirection.ReleaseGraphics(gb);

	if (images.beam) images.beam.Dispose();
	images.beam = gdi.CreateImage(500, 128);
	var gb = images.beam.GetGraphics();
	gb.FillEllipse(-250, 50, 1000, 640, g_line_colour & 0x60ffffff);
	images.beam.ReleaseGraphics(gb);
	images.beam = images.beam.Resize(10, 3, 2);
}

function get_wallpaper() {
	if (!properties.showwallpaper) return null;

	var metadb = fb.GetNowPlaying();
	if (!metadb) return null;

	if (properties.wallpapertype == -1) {
		var img = gdi.Image(fb.ProfilePath + properties.wallpaperpath);
	} else {
		var img = metadb.GetAlbumArt(properties.wallpapertype).toArray()[0];
	}

	if (img && properties.wallpaperblurred) {
		img.StackBlur(properties.wallpaperblurvalue);
	}
	return img;
}

function on_drag_enter() {
	g_drag_drop_status = true;
}

function on_drag_leave() {
	g_drag_drop_status = false;
	g_drag_drop_playlist_manager_hover = false;
	g_drag_drop_track_id = -1;
	g_drag_drop_row_id = -1;
	g_drag_drop_playlist_id = -1;
	p.list.buttonclicked = false;
	if (cScrollBar.interval) {
		window.ClearInterval(cScrollBar.interval);
		cScrollBar.interval = false;
	}
	window.Repaint();
}

function on_drag_over(action, x, y, mask) {
	g_drag_drop_playlist_manager_hover = false;
	g_drag_drop_track_id = -1;
	g_drag_drop_row_id = -1;
	g_drag_drop_bottom = false;

	if (y < p.list.y) {
		action.Effect = 0;
	} else if (cPlaylistManager.visible && p.playlistManager.isHoverObject(x, y)) {
		g_drag_drop_playlist_manager_hover = true;
		p.playlistManager.check("drag_over", x, y);
		if (g_drag_drop_playlist_id == -1) {
			action.Effect = p.playlistManager.ishoverHeader ? 1 : 0;
		} else if (g_drag_drop_internal) {
			action.Effect = g_drag_drop_playlist_id == g_active_playlist ? 0 : 1;
		} else if (playlist_can_add_items(g_drag_drop_playlist_id)) {
			action.Effect = 1;
		} else {
			action.Effect = 0;
		}
	} else if (g_drag_drop_internal || playlist_can_add_items(g_active_playlist)) {
		p.list.check("drag_over", x, y);
		if (y > p.list.y && y < p.list.y + 40) {
			on_mouse_wheel(1);
		} else if (y > p.list.y + p.list.h - 40 && y < p.list.y + p.list.h) {
			on_mouse_wheel(-1);
		}
		action.Effect = 1;
	} else {
		action.Effect = 0;
	}
	full_repaint();
}

function on_drag_drop(action, x, y, mask) {
	if (y < p.list.y) {
		action.Effect = 0;
	} else if (cPlaylistManager.visible && p.playlistManager.isHoverObject(x, y)) {
		if (g_drag_drop_playlist_id == -1) {
			if (p.playlistManager.ishoverHeader) {
				if (g_drag_drop_internal) {
					var pl = plman.CreatePlaylist(plman.PlaylistCount, "Dropped Items")
					plman.InsertPlaylistItems(pl, 0, plman.GetPlaylistSelectedItems(g_active_playlist));
					action.Effect = 0;
				} else {
					action.Playlist = plman.CreatePlaylist(plman.PlaylistCount, "Dropped Items");
					action.Base = 0;
					action.ToSelect = false;
					action.Effect = 1;
				}
			} else {
				action.Effect = 0;
			}
		} else if (playlist_can_add_items(g_drag_drop_playlist_id)) {
			var base = plman.PlaylistItemCount(g_drag_drop_playlist_id);

			if (g_drag_drop_internal) {
				if (g_drag_drop_playlist_id != g_active_playlist) {
					plman.UndoBackup(g_drag_drop_playlist_id);
					plman.InsertPlaylistItems(g_drag_drop_playlist_id, base, plman.GetPlaylistSelectedItems(g_active_playlist));
				}
				action.Effect = 0;
			} else {
				plman.UndoBackup(g_drag_drop_playlist_id);
				action.Playlist = g_drag_drop_playlist_id;
				action.Base = base;
				action.ToSelect = false;
				action.Effect = 1;
			}
		} else {
			action.Effect = 0;
		}
	} else {
		var new_pos = g_drag_drop_bottom ? plman.PlaylistItemCount(g_active_playlist) : g_drag_drop_track_id;
		if (g_drag_drop_internal) {
			plman.UndoBackup(g_active_playlist);
			plman.MovePlaylistSelectionV2(g_active_playlist, new_pos);
			action.Effect = 0;
		} else if (playlist_can_add_items(g_active_playlist)) {
			plman.ClearPlaylistSelection(g_active_playlist);
			plman.UndoBackup(g_active_playlist);
			action.Playlist = g_active_playlist;
			action.Base = new_pos;
			action.ToSelect = true;
			action.Effect = 1;
		} else {
			action.Effect = 0;
		}
	}
	g_drag_drop_playlist_manager_hover = false;
	g_drag_drop_playlist_id = -1;
	g_drag_drop_track_id = -1;
	g_drag_drop_row_id = -1;
	g_drag_drop_bottom = false;
	g_drag_drop_internal = false;
	full_repaint();
}
