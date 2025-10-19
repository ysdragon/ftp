# Example 02: Directory Operations with Ring FTP
# This example demonstrates directory management operations:
# creating, listing, and removing directories

load "ftp.ring"

# Create FTP client instance
ftp = new FTP()

try
	# Connect and perform directory operations
	? "Connecting to FTP server..."
	ftp {
		# Configure and connect
		setHost("localhost", 21) # Change to your FTP server
		setCredentials("username", "password") # Change to your credentials
		setMode(FTP_MODE_PASSIVE)
		connect()
		
		? "✓ Connected successfully!" + nl
		
		# List root directory contents
		? "Listing root directory contents:"
		listing = listDir("/")
		? listing + nl
		
		# Create a new directory
		? "Creating directory 'test_folder'..."
		mkdir("test_folder")
		? "✓ Directory created!" + nl
		
		# Create a nested directory structure
		? "Creating nested directory 'test_folder/subfolder'..."
		mkdir("test_folder/subfolder")
		? "✓ Nested directory created!" + nl
		
		# List the test_folder to verify
		? "Listing 'test_folder' contents:"
		listing = listDir("test_folder")
		? listing + nl
		
		# Remove the nested directory
		? "Removing 'test_folder/subfolder'..."
		rmdir("test_folder/subfolder")
		? "✓ Subdirectory removed!" + nl
		
		# Remove the main directory
		? "Removing 'test_folder'..."
		rmdir("test_folder")
		? "✓ Directory removed!" + nl
		
		# Close the connection
		close()
	}
	
	? "✓ Connection closed successfully!" + nl
	? copy("=", 50)
	? "Directory operations completed successfully!"
	? copy("=", 50)

catch
	? "Error occurred: " + cCatchError
	? "FTP Error: " + ftp.getError()
end
