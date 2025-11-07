# Example 08: Batch File Upload
# This example demonstrates how to upload multiple files
# to an FTP server in a batch operation

load "ftp.ring"

# Create FTP client instance
ftp = new FTP()

try
	# Connect to FTP server
	? "Connecting to FTP server..."
	ftp {
		setHost("ftp.example.com", 21) # Change to your FTP server
		setCredentials("username", "password") # Change to your credentials
		setProgressCallback(:uploadProgressCallback)
		connect()
		
		? "✓ Connected successfully!" + nl
		
		# Create a directory for batch upload
		? "Creating 'batch_upload' directory..."
		mkdir("/batch_upload")
		? "✓ Directory created!" + nl

		# Create multiple test files
		? "Creating test files for batch upload..."
		files = ["batch1.txt", "batch2.txt", "batch3.txt", "batch4.txt", "batch5.txt"]

		for i = 1 to len(files)
			content = "This is test file " + i + nl + "Content for batch upload demo."
			write(files[i], content)
			? "✓ Created " + files[i]
		next

		# Upload all files
		? "Uploading files..." + nl
		for i = 1 to len(files)
			? "Uploading " + files[i] + "..."
			upload(files[i], "/batch_upload/" + files[i])
			? "  ✓ " + files[i] + " uploaded successfully!"
		next
		? nl
		
		# Verify uploads by listing directory
		? "Verifying uploads..."
		listing = listDir("/batch_upload")
		? "Files in 'batch_upload':" + nl + listing + nl
		
		# Get total size of uploaded files
		? "Calculating total uploaded size..."
		totalSize = 0
		for i = 1 to len(files)
			size = getFileSize("/batch_upload/" + files[i])
			totalSize += size
			? "  " + files[i] + ": " + size + " bytes"
		next
		? "Total size: " + totalSize + " bytes" + nl
		
		# Clean up - delete all uploaded files
		? "Cleaning up remote files..."
		for i = 1 to len(files)
			delete("/batch_upload/" + files[i])
		next
		rmdir("/batch_upload")
		? "✓ Cleanup complete!" + nl
		
		# Close the connection
		close()
	}
	
	? "✓ Connection closed successfully!" + nl
	? copy("=", 50)
	? "Batch upload example completed successfully!"
	? copy("=", 50)
catch
	? "Error occurred: " + cCatchError
	? "FTP Error: " + ftp.getError()
end

# Progress callback for uploads
func uploadProgressCallback(download_total, download_now, upload_total, upload_now)
	if upload_total > 0
		percent = (upload_now / upload_total) * 100
		if percent % 25 = 0
			? "    Progress: " + floor(percent) + "%"
		ok
	ok
