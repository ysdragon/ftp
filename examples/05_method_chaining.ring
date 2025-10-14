# Example 05: Method Chaining with Ring FTP
# This example demonstrates the fluent interface pattern
# by chaining multiple FTP operations together

load "ftp.ring"

# Create FTP client instance
ftp = new FTP()

try
	# Chain configuration and connection
	? "Configuring and connecting using method chaining..."
	ftp.setHost("localhost", 21) # Change to your FTP server
		.setCredentials("username", "password") # Change to your credentials
		.setMode(FTP_MODE_PASSIVE)
		.setTimeout(60, 30)
		.connect()
	 
	? "✓ Connected successfully!" + nl

	# Create a test file
	? "Creating test file..."
	write("chain_test.txt", "Testing method chaining with Ring FTP!")
	? "✓ Test file created" + nl
	
	# Chain multiple operations together
	? "Performing chained operations..."
	ftp.mkdir("chained_folder")
		.upload("chain_test.txt", "chained_folder/uploaded.txt")
		.rename("chained_folder/uploaded.txt", "chained_folder/final.txt")
	 
	? "✓ Created folder, uploaded, and renamed file!" + nl
	 
	# Get file size and download
	? "Getting file size and downloading..."
	size = ftp.getFileSize("chained_folder/final.txt")
	? "File size: " + size + " bytes"
	 
	ftp.download("chained_folder/final.txt", "downloaded_chain.txt")
	? "✓ Downloaded file!" + nl
	 
	# Chain cleanup operations
	? "Cleaning up..."
	ftp.delete("chained_folder/final.txt")
		.rmdir("chained_folder")
		.close()
	 
	? "✓ Cleanup complete and connection closed!" + nl
	? copy("=", 50)
	? "Method chaining example completed successfully!"
	? copy("=", 50)
	 
catch
	? "Error occurred: " + cCatchError
	? "FTP Error: " + ftp.getError()
	ftp.close()
end
