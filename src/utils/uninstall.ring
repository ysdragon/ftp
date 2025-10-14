load "stdlibcore.ring"

cPathSep = "/"

if isWindows()
	cPathSep = "\\"
ok

# Remove the ftp.ring file from the load directory
remove(exefolder() + "load" + cPathSep + "ftp.ring")

# Remove the ftp.ring file from the Ring2EXE libs directory
remove(exefolder() + ".." + cPathSep + "tools" + cPathSep + "ring2exe" + cPathSep + "libs" + cPathSep + "ftp.ring")

# Change current directory to the samples directory
chdir(exefolder() + ".." + cPathSep + "samples")

# Remove the UsingFTP directory if it exists
if direxists("UsingFTP")
	OSDeleteFolder("UsingFTP")
ok