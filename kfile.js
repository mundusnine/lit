
const system = platform === Platform.Windows ? "win32" :
platform === Platform.Linux   ? "linux" :
platform === Platform.FreeBSD   ? "freebsd" :
platform === Platform.OSX     ? "macos" :
platform === Platform.Wasm    ? "wasm" :
platform === Platform.Android ? "android" :
platform === Platform.iOS     ? "ios" :
								   "unknown";

const renderer_type = "GPU";
let project = new Project('lit');

await project.addProject('Kinc');

project.icon = "./icon.png"

project.addIncludeDir('src');
project.addFiles('src/**','Shaders/**');

if(renderer_type === "GPU"){
	project.addIncludeDir('src/gpu');
	project.addExclude('src/cpu/renderer.c');
	project.addExclude('src/cpu/renderer.h');
	project.addExclude('src/cpu/rencache.c');
	project.addExclude('src/cpu/rencache.h');
}
else {
	project.addIncludeDir('src/cpu');
	project.addExclude('src/gpu/renderer.c');
	project.addExclude('src/gpu/renderer.h');
}
//nfd
    project.addIncludeDir("Libraries/nfd/include");
	project.addFile('Libraries/nfd/nfd_common.c');

	if (system === "win32") {
		project.addExclude('src/api/dirmonitor/kqueue.c');
		project.addExclude('src/api/dirmonitor/fsevents.c');
		project.addExclude('src/api/dirmonitor/inotify.c');
		
		project.addFile('Libraries/nfd/nfd_win.cpp');
	}
	else if (system === "linux") {
		project.addExclude('src/api/dirmonitor/kqueue.c');
		project.addExclude('src/api/dirmonitor/fsevents.c');
		project.addExclude('src/api/dirmonitor/win32.c');
		let gtk = true;
		if (gtk) {
			project.addFile('Libraries/nfd/nfd_gtk.c');

			project.addIncludeDir("/usr/include/gtk-3.0");
			project.addIncludeDir("/usr/include/glib-2.0");
			project.addIncludeDir("/usr/lib/x86_64-linux-gnu/glib-2.0/include");
			project.addIncludeDir("/usr/include/pango-1.0");
			project.addIncludeDir("/usr/include/cairo");
			project.addIncludeDir("/usr/include/gdk-pixbuf-2.0");
			project.addIncludeDir("/usr/include/atk-1.0");
			project.addIncludeDir("/usr/lib64/glib-2.0/include");
			project.addIncludeDir("/usr/lib/glib-2.0/include");
			project.addIncludeDir("/usr/include/harfbuzz");
			project.addLib('gtk-3');
			project.addLib('gobject-2.0');
			project.addLib('glib-2.0');
		}
		else {
			project.addFile('Libraries/nfd/nfd_zenity.c');
		}
	}
	else if(system === "macos" || system === "ios") {
		project.addExclude('src/api/dirmonitor/kqueue.c');
		project.addExclude('src/api/dirmonitor/win32.c');
		project.addExclude('src/api/dirmonitor/inotify.c');
		project.addFile('Libraries/nfd/nfd_cocoa.m');
	}
	else if( system === "freebsd"){
		project.addExclude('src/api/dirmonitor/win32.c');
		project.addExclude('src/api/dirmonitor/fsevents.c');
		project.addExclude('src/api/dirmonitor/inotify.c');
	}  
project.setDebugDir('Deployment');

project.flatten();

resolve(project);
