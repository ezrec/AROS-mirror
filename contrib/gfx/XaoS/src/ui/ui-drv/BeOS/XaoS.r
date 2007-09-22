resource('MIMS', 1, "BEOS:APP_SIG") {
	import("XaoSResources", 'MIMS', 1)
}

resource('MSGG', 1, "BEOS:FILE_TYPES") {
	import("XaoSResources", 'MSGG', 1)
}

resource('APPV', 1, "BEOS:APP_VERSION") {
	import("XaoSResources", 'APPV', 1)
}

resource('APPF', 1, "BEOS:APP_FLAGS") {
	import("XaoSResources", 'APPF', 1)
}

resource('ICON', 101, "BEOS:L:STD_ICON") {
	import("XaoSResources", 'ICON', 101)
}

resource('MICN', 101, "BEOS:M:STD_ICON") {
	import("XaoSResources", 'MICN', 101)
}

resource('XaSp', 1, "Splash screen") {
	read("SplashScreen")
}
