# Ring FTP Library

# Load Ring FTP Library and Constants
load "ftp.rh"

# Global variables for managing the global state.
$_FTP_Initialized = 0
$_FTP_InstanceCount = 0

class FTP
	
	client
	callbackData
	
	# Initializes the FTP client and global library state.
	func init()
		if $_FTP_Initialized = 0
			ftp_global_init()
			$_FTP_Initialized = 1
		ok
		$_FTP_InstanceCount++
		
		client = ftp_client_create()
		if client = 0 or isNULL(client)
			raise("Could not create FTP client handle.")
		ok
		
	# Sets the FTP server host and port.
	func setHost(host, port)
		if isNULL(host) or not isString(host)
			raise("Host must be a valid string.")
		ok
		if not isNumber(port)
			raise("Port must be a number.")
		ok
		result = ftp_client_set_host(client, host, port)
		if result != FTP_OK
			raise(getError())
		ok
		return self
	
	# Sets the username and password for authentication.
	func setCredentials(username, password)
		if isNULL(username) or not isString(username)
			raise("Username must be a valid string.")
		ok
		if isNULL(password) or not isString(password)
			raise("Password must be a valid string.")
		ok
		result = ftp_client_set_credentials(client, username, password)
		if result != FTP_OK
			raise(getError())
		ok
		return self
	
	# Sets the transfer mode (passive or active).
	func setMode(mode)
		ftp_client_set_mode(client, mode)
		return self
	
	# Configures SSL/TLS settings.
	func setSSL(sslMode, verify)
		ftp_client_set_ssl(client, sslMode, verify)
		return self
	
	# Sets operation and connection timeouts.
	func setTimeout(timeout, connectTimeout)
		ftp_client_set_timeout(client, timeout, connectTimeout)
		return self
	
	# Enables or disables verbose debugging output.
	func setVerbose(verbose)
		ftp_client_set_verbose(client, verbose)
		return self
	
	# Callback Methods
	
	# Sets a function to track transfer progress.
	func setProgressCallback(callbackFuncName)
		if isstring(callbackFuncName)
			callbackData = ftp_client_set_progress_callback(client, callbackFuncName)
		else
			raise("Callback must be a string containing the function name.")
		ok
		return self
	
	# Removes the active progress callback.
	func clearProgressCallback()
		if not isNULL(callbackData)
			ftp_client_clear_progress_callback(client)
			callbackData = NULL
		ok
		return self
	
	# Action Methods
	
	# Connects to the configured FTP server.
	func connect()
		result = ftp_client_connect(client)
		if result != FTP_OK
			raise(getError())
		ok
		return self
	
	# Uploads a local file to the remote server.
	func upload(localPath, remotePath)
		result = ftp_client_upload(client, localPath, remotePath)
		if result != FTP_OK
			raise(getError())
		ok
		return self
	
	# Downloads a remote file to a local path.
	func download(remotePath, localPath)
		result = ftp_client_download(client, remotePath, localPath)
		if result != FTP_OK
			raise(getError())
		ok
		return self
	
	# Lists the contents of a remote directory.
	func listDir(remotePath)
		return ftp_client_list_dir(client, remotePath)
	
	# Creates a directory on the remote server.
	func mkdir(remotePath)
		result = ftp_client_mkdir(client, remotePath)
		if result != FTP_OK
			raise(getError())
		ok
		return self
	
	# Removes a directory from the remote server.
	func rmdir(remotePath)
		result = ftp_client_rmdir(client, remotePath)
		if result != FTP_OK
			raise(getError())
		ok
		return self
	
	# Deletes a file from the remote server.
	func delete(remotePath)
		result = ftp_client_delete(client, remotePath)
		if result != FTP_OK
			raise(getError())
		ok
		return self
	
	# Renames a file or directory on the remote server.
	func rename(oldPath, newPath)
		result = ftp_client_rename(client, oldPath, newPath)
		if result != FTP_OK
			raise(getError())
		ok
		return self
	
	# Gets the size of a remote file in bytes.
	func getFileSize(remotePath)
		size = ftp_client_get_filesize(client, remotePath)
		if size < 0
			raise(getError())
		ok
		return size
	
	# Executes a custom FTP command.
	func executeCommand(command)
		return ftp_client_execute_command(client, command)
	
	# Utility and Cleanup
	
	# Returns the last error message from the client.
	func getError()
		return ftp_client_get_error(client)
	
	# Disconnects and cleans up the client instance.
	func close()
		if not isNULL(client) and client != 0
			clearProgressCallback()
			ftp_client_destroy(client)
			client = NULL
			$_FTP_InstanceCount--
			
			if $_FTP_InstanceCount = 0 and $_FTP_Initialized = 1
				ftp_global_cleanup()
				$_FTP_Initialized = 0
			ok
		ok