let project = new Project('lit');

await project.addProject('Kinc');

project.icon = "./icon.png"

project.addDefine('KR_FULL_RGBA_FONTS');
project.addIncludeDir('src');
project.addFiles('src/**','Shaders/**');
//nfd
    project.addIncludeDir("Libraries/nfd/include");
	project.addFile('Libraries/nfd/nfd_common.c');

	if (platform === Platform.Windows) {
		project.addFile('Libraries/nfd/nfd_win.cpp');
	}
	else if (platform === Platform.Linux) {
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
	else {
		project.addFile('Libraries/nfd/nfd_cocoa.m');
	}    
project.setDebugDir('Deployment');

project.flatten();

resolve(project);
