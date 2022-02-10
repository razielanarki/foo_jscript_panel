function _lastfm() {
	this.notify_data = function (name, data) {
		if (name == '2K3.NOTIFY.LASTFM') {
			this.username = this.read_ini('username');
			_.forEach(panel.list_objects, function (item) {
				if (item.mode == 'lastfm_info' && item.properties.mode.value > 0) {
					item.update();
				}
			});
		}
	}
	
	this.update_username = function () {
		var username = utils.InputBox(window.ID, 'Enter your Last.fm username', window.Name, this.username);
		if (username != this.username) {
			this.write_ini('username', username);
			window.NotifyOthers('2K3.NOTIFY.LASTFM', 'update');
			this.notify_data('2K3.NOTIFY.LASTFM', 'update');
		}
	}
	
	this.get_base_url = function () {
		return 'http://ws.audioscrobbler.com/2.0/?format=json&api_key=' + this.api_key;
	}
	
	this.read_ini = function (k) {
		return utils.ReadINI(this.ini_file, 'Last.fm', k);
	}
	
	this.write_ini = function (k, v) {
		utils.WriteINI(this.ini_file, 'Last.fm', k, v);
	}
	
	_createFolder(folders.data);
	this.ini_file = folders.data + 'lastfm.ini';
	this.api_key = '1f078d9e59cb34909f7ed56d7fc64aba';
	this.username = this.read_ini('username');
	this.ua = 'foo_jscript_panel_lastfm2';
}
