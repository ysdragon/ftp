# Example 03: File Management Operations
# This example demonstrates file operations:
# uploading, renaming, checking size, and deleting files

load "ftp.ring"

# Create FTP client instance
ftp = new FTP()

try
	# Connect and perform file operations
	? "Connecting to FTP server..."
	ftp {
		# Configure and connect
		setHost("localhost", 21) # Change to your FTP server
		setCredentials("username", "password") # Change to your credentials
		connect()
		
		? "✓ Connected successfully!" + nl
		
		# Create test files
		? "Creating test files..."
		write("file1.txt", "This is the first test file.")
		write("file2.txt", "This is the second test file.")
		? "✓ Test files created" + nl

		# Upload multiple files
		? "Uploading files..."
		upload("file1.txt", "/file1.txt")
		? "✓ Uploaded file1.txt"
		
		upload("file2.txt", "/file2.txt")
		? "✓ Uploaded file2.txt" + nl
		
		# Check file sizes
		? "Checking file sizes..."
		size1 = getFileSize("/file1.txt")
		? "file1.txt: " + size1 + " bytes"
		
		size2 = getFileSize("/file2.txt")
		? "file2.txt: " + size2 + " bytes" + nl
		
		# Rename a file
		? "Renaming file1.txt to renamed_file.txt..."
		rename("/file1.txt", "/renamed_file.txt")
		? "✓ File renamed!" + nl
		
		# Verify the renamed file exists
		? "Checking renamed file size..."
		renamedSize = getFileSize("/renamed_file.txt")
		? "renamed_file.txt: " + renamedSize + " bytes" + nl
		
		# Delete files
		? "Deleting files..."
		delete("/renamed_file.txt")
		? "✓ Deleted renamed_file.txt"
		
		delete("/file2.txt")
		? "✓ Deleted file2.txt" + nl
		
		# Close the connection
		close()
	}
	
	? "✓ Connection closed successfully!" + nl
	? copy("=", 50)
	? "File management operations completed successfully!"
	? copy("=", 50)
	
catch
	? "Error occurred: " + cCatchError
	? "FTP Error: " + ftp.getError()
end
