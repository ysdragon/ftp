# Example 06: Secure FTPS Connection
# This example demonstrates how to connect to an FTP server
# using SSL/TLS for secure file transfers (FTPS)

load "ftp.ring"

# Create FTP client instance
ftp = new FTP()

try
	# Connect using FTPS with SSL/TLS
	? "Connecting to FTPS server with SSL/TLS..."
	ftp {
        setHost("localhost", 21) # Change to your FTPS server
        setCredentials("username", "password") # Change to your credentials
		
		# Enable SSL/TLS for both control and data connections
		setSSL(FTP_SSL_ALL, 1)  # 1 = verify SSL certificate
		
		# Optional: To skip SSL certificate verification (not recommended for production)
		# setSSL(FTP_SSL_ALL, 0)
		
		setMode(FTP_MODE_PASSIVE)
		setTimeout(60, 30)
		setVerbose(1)  # Enable verbose output to see SSL/TLS details
		
		# Connect
		connect()
		
		? "✓ Secure connection established!" + nl
		
		# Create a test file
		? "Creating test file..."
		write("secure_test.txt", "This file will be transferred securely via FTPS.")
		? "✓ Test file created" + nl

		# Upload file securely
		? "Uploading file via FTPS..."
		upload("secure_test.txt", "secure_uploaded.txt")
		? "✓ File uploaded securely!" + nl
		
		# Download file securely
		? "Downloading file via FTPS..."
		download("secure_uploaded.txt", "secure_downloaded.txt")
		? "✓ File downloaded securely!" + nl
		
		# Clean up
		? "Cleaning up..."
		delete("secure_uploaded.txt")
		? "✓ Remote file deleted" + nl
		
		# Close the connection
		close()
	}
	
	? "✓ Secure connection closed successfully!" + nl
	? copy("=", 50)
	? "FTPS example completed successfully!"
	? copy("=", 50)	
catch
	? "Error occurred: " + cCatchError
	? "FTP Error: " + ftp.getError()
end
