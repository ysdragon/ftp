# Example 01: Basic Upload and Download with Ring FTP
# This example demonstrates how to connect to an FTP server,
# upload a file, and download it back.

load "ftp.ring"

# Create FTP client instance
ftp = new FTP()

try
	? "Connecting and performing FTP operations..."
	ftp {
		# Configure and connect
		setHost("localhost", 21) # Change to your FTP server
		setCredentials("username", "password") # Change to your credentials
		setMode(FTP_MODE_PASSIVE)
		setTimeout(60, 30)
		connect()
		
		? "✓ Connected successfully!" + nl
		
		# Create a test file to upload
		? "Creating test file..."
		testContent = "Hello from Ring FTP!" + nl + 
					"This is a test file." + nl + 
					"Timestamp: " + date() + " " + time()
		write("test_file.txt", testContent)
		? "✓ Test file created" + nl

		# Upload the file
		? "Uploading file to FTP server..."
		upload("test_file.txt", "/uploaded_file.txt")
		? "✓ Upload complete!" + nl
		
		# Get the uploaded file size
		? "Checking uploaded file size..."
		fileSize = getFileSize("/uploaded_file.txt")
		? "File size: " + fileSize + " bytes" + nl
		
		# Download the file back
		? "Downloading file from FTP server..."
		download("/uploaded_file.txt", "downloaded_file.txt")
		? "✓ Download complete!" + nl
		
		# Close the connection
		close()
	}
	
	? "✓ Connection closed successfully!" + nl
	
	# Verify the downloaded file
	? "Verifying downloaded file..."
	downloadedContent = read("downloaded_file.txt")
	? "Downloaded content:" + nl + downloadedContent + nl

	# Cleanup local files
	remove("test_file.txt")
	remove("downloaded_file.txt")

	? copy("=", 50)
	? "Done!"
	? copy("=", 50)

catch
	? "Error occurred: " + cCatchError
	? "FTP Error: " + ftp.getError()
	ftp.close()
end
