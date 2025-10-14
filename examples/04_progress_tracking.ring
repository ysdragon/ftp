# Example 04: Progress Tracking with Callbacks
# This example demonstrates how to track upload and download progress
# using progress callbacks

load "ftp.ring"

# Create FTP client instance
ftp = new FTP()

try
	# Connect and perform operations with progress tracking
	? "Connecting to FTP server..."
	ftp {
		# Configure and connect
		setHost("localhost", 21) # Change to your FTP server
		setCredentials("username", "password") # Change to your credentials
		setProgressCallback(:myProgressCallback)
		connect()
		
		? "✓ Connected successfully!" + nl
		
		# Create a test file with some content
		? "Creating test file..."
		testContent = ""
		for i = 1 to 1000
			testContent += "Line " + i + ": This is sample text for testing progress tracking." + nl
		next
		write("large_test_file.txt", testContent)
		? "✓ Test file created" + nl

		# Upload with progress tracking
		? "Uploading file with progress tracking..."
		upload("large_test_file.txt", "uploaded_large_file.txt")
		? nl + "✓ Upload complete!" + nl
		
		# Download with progress tracking
		? "Downloading file with progress tracking..."
		download("uploaded_large_file.txt", "downloaded_large_file.txt")
		? nl + "✓ Download complete!" + nl
		
		# Clean up remote file
		? "Cleaning up..."
		delete("uploaded_large_file.txt")
		? "✓ Remote file deleted" + nl
		
		# Close the connection
		close()
	}
	
	? "✓ Connection closed successfully!" + nl
	? copy("=", 50)
	? "Progress tracking example completed successfully!"
	? copy("=", 50)
	
catch
	? "Error occurred: " + cCatchError
	? "FTP Error: " + ftp.getError()
end

# Progress callback function
func myProgressCallback(download_total, download_now, upload_total, upload_now)
	if upload_total > 0
		percent = (upload_now / upload_total) * 100
		? "  Upload Progress: " + floor(percent) + "% (" + upload_now + "/" + upload_total + " bytes)"
	ok
	if download_total > 0
		percent = (download_now / download_total) * 100
		? "  Download Progress: " + floor(percent) + "% (" + download_now + "/" + download_total + " bytes)"
	ok
