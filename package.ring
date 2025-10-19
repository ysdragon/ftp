aPackageInfo = [
	:name = "Ring FTP",
	:description = "A comprehensive FTP client library for Ring with support for FTP/FTPS file transfers, directory management, and progress tracking",
	:folder = "ftp",
	:developer = "ysdragon",
	:email = "",
	:license = "MIT License",
	:version = "1.0.0",
	:ringversion = "1.24",
	:versions = 	[
		[
			:version = "1.0.0",
			:branch = "master"
		]
	],
	:libs = 	[
		[
			:name = "",
			:version = "",
			:providerusername = ""
		]
	],
	:files = 	[
		"lib.ring",
		"main.ring",
		".clang-format",
		"CMakeLists.txt",
		"examples/01_basic_upload_download.ring",
		"examples/02_directory_operations.ring",
		"examples/03_file_management.ring",
		"examples/04_progress_tracking.ring",
		"examples/05_method_chaining.ring",
		"examples/06_secure_ftps.ring",
		"examples/07_custom_commands.ring",
		"examples/08_batch_upload.ring",
		"LICENSE",
		"README.md",
		"src/c_src/ring_ftp.c",
		"src/ftp.rh",
		"src/ftp.ring",
		"src/utils/color.ring",
		"src/utils/install.ring",
		"src/utils/uninstall.ring"
	],
	:ringfolderfiles = 	[

	],
	:windowsfiles = 	[
		"lib/windows/i386/ring_ftp.dll",
		"lib/windows/amd64/ring_ftp.dll",
		"lib/windows/arm64/ring_ftp.dll"
	],
	:linuxfiles = 	[
		"lib/linux/amd64/libring_ftp.so",
		"lib/linux/arm64/libring_ftp.so"
	],
	:ubuntufiles = 	[

	],
	:fedorafiles = 	[

	],
	:macosfiles = 	[
		"lib/macos/amd64/libring_ftp.dylib",
		"lib/macos/arm64/libring_ftp.dylib"
	],
	:freebsdfiles = 	[
		"lib/freebsd/amd64/libring_ftp.so",
		"lib/freebsd/arm64/libring_ftp.so"
	],
	:windowsringfolderfiles = 	[

	],
	:linuxringfolderfiles = 	[

	],
	:ubunturingfolderfiles = 	[

	],
	:fedoraringfolderfiles = 	[

	],
	:freebsdringfolderfiles = 	[

	],
	:macosringfolderfiles = 	[

	],
	:run = "ring main.ring",
	:windowsrun = "",
	:linuxrun = "",
	:macosrun = "",
	:ubunturun = "",
	:fedorarun = "",
	:setup = "ring src/utils/install.ring",
	:windowssetup = "",
	:linuxsetup = "",
	:macossetup = "",
	:ubuntusetup = "",
	:fedorasetup = "",
	:remove = "ring src/utils/uninstall.ring",
	:windowsremove = "",
	:linuxremove = "",
	:macosremove = "",
	:ubunturemove = "",
	:fedoraremove = ""
]