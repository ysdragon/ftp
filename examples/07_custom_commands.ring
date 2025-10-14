# Example 07: Custom FTP Commands
# This example demonstrates how to execute custom FTP commands
# using the executeCommand method

load "ftp.ring"

# Create FTP client instance
ftp = new FTP()

try
	# Connect to FTP server
	? "Connecting to FTP server..."
	ftp {
		setHost("ftp.example.com", 21) # Change to your FTP server
		setCredentials("username", "password") # Change to your credentials
		setVerbose(1)  # Enable verbose output
		connect()
		
		? "✓ Connected successfully!" + nl
		
		# Execute PWD (Print Working Directory) command
		? "Executing PWD command..."
		response = executeCommand("PWD")
		? "Current directory: " + response + nl
		
		# Execute SYST (System) command
		? "Executing SYST command..."
		response = executeCommand("SYST")
		? "Server system: " + response + nl
		
		# Execute HELP command
		? "Executing HELP command..."
		response = executeCommand("HELP")
		? "Server help:" + nl + response + nl
		
		# Execute NOOP (No Operation) command - keeps connection alive
		? "Executing NOOP command..."
		response = executeCommand("NOOP")
		? "NOOP response: " + response + nl
		
		# Execute STAT command (Server status)
		? "Executing STAT command..."
		response = executeCommand("STAT")
		? "Server status:" + nl + response + nl
		
		# Close the connection
		close()
	}
	
	? "✓ Connection closed successfully!" + nl
	? copy("=", 50)
	? "Custom commands example completed successfully!"
	? copy("=", 50)
	? nl
	? "Common FTP commands you can execute:"
	? "  PWD  - Print working directory"
	? "  SYST - Get system information"
	? "  HELP - Get available commands"
	? "  STAT - Get server status"
	? "  NOOP - No operation (keep-alive)"
	? "  FEAT - Get server features"
catch
	? "Error occurred: " + cCatchError
	? "FTP Error: " + ftp.getError()
end
