function oFilterBox() {
	this.magnify_image = null;

	this.getImages = function () {
		var size = 48;

		this.magnify_image = gdi.CreateImage(size, size);
		var gb = this.magnify_image.GetGraphics();
		gb.SetTextRenderingHint(4);
		gb.DrawString(chars.search, g_font_awesome_40, g_color_normal_txt, 0, 0, size, size, cc_stringformat);
		this.magnify_image.ReleaseGraphics(gb);

		var resetIcon = gdi.CreateImage(size, size);
		var gb = resetIcon.GetGraphics();
		gb.SetTextRenderingHint(4);
		gb.DrawString(chars.close, g_font_awesome_40, blendColors(g_color_normal_txt, g_color_normal_bg, 0.35), 0, 0, size, size, cc_stringformat);
		resetIcon.ReleaseGraphics(gb);
		resetIcon = resetIcon.Resize(cFilterBox.h, cFilterBox.h, 2);

		var resetIcon_hover = gdi.CreateImage(size, size);
		var gb = resetIcon_hover.GetGraphics();
		gb.SetTextRenderingHint(4);
		gb.DrawString(chars.close, g_font_awesome_40, RGB(255, 50, 50), 0, 0, size, size, cc_stringformat);
		resetIcon_hover.ReleaseGraphics(gb);
		resetIcon_hover = resetIcon_hover.Resize(cFilterBox.h, cFilterBox.h, 2);

		this.reset_bt = new button(resetIcon, resetIcon, resetIcon_hover);
	}
	this.getImages();

	this.init = function () {
		this.inputbox = new oInputbox(cFilterBox.w, cFilterBox.h, "", "Filter", g_color_normal_txt, 0, 0, g_color_selected_bg, g_sendResponse);
		this.inputbox.autovalidation = true;
	}
	this.init();

	this.reset_colors = function () {
		this.inputbox.textcolor = g_color_normal_txt;
		this.inputbox.backselectioncolor = g_color_selected_bg;
	}

	this.setSize = function (w, h, font_size) {
		this.inputbox.setSize(w, h, font_size);
		this.getImages();
	}

	this.clearInputbox = function () {
		if (this.inputbox.text.length > 0) {
			this.inputbox.text = "";
			this.inputbox.offset = 0;
			g_filter_text = "";
		}
	}

	this.draw = function (gr, x, y) {
		if (this.inputbox.text.length > 0) {
			this.reset_bt.draw(gr, x, y);
		} else {
			gr.DrawImage(this.magnify_image, x, y, cFilterBox.h - 1, cFilterBox.h - 1, 0, 0, this.magnify_image.Width, this.magnify_image.Height);
		}
		for (var i = 0; i < cFilterBox.h - 2; i += 2) {
			gr.FillSolidRect(x + scale(22) + cFilterBox.w, y + 2 + i, 1, 1, RGB(100, 100, 100));
		}
		this.inputbox.draw(gr, x + scale(22), y, 0, 0);
	}

	this.on_mouse = function (event, x, y, delta) {
		switch (event) {
		case "lbtn_down":
			this.inputbox.check("down", x, y);
			if (this.inputbox.text.length > 0)
				this.reset_bt.checkstate("down", x, y);
			break;
		case "lbtn_up":
			this.inputbox.check("up", x, y);
			if (this.inputbox.text.length > 0) {
				if (this.reset_bt.checkstate("up", x, y) == ButtonStates.hover) {
					this.inputbox.text = "";
					this.inputbox.offset = 0;
					g_sendResponse();
				}
			}
			break;
		case "lbtn_dblclk":
			this.inputbox.check("dblclk", x, y);
			break;
		case "rbtn_up":
			this.inputbox.check("right", x, y);
			break;
		case "move":
			this.inputbox.check("move", x, y);
			if (this.inputbox.text.length > 0)
				this.reset_bt.checkstate("move", x, y);
			break;
		}
	}
}
