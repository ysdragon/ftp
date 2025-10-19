<div align="center">

# Ring FTP

[ring]: https://img.shields.io/badge/Made_with_❤️_for-Ring-2D54CB?style=for-the-badge
[license]: https://img.shields.io/github/license/ysdragon/ftp?style=for-the-badge&logo=opensourcehardware&label=License&logoColor=C0CAF5&labelColor=414868&color=8c73cc

[![][ring]](https://ring-lang.net/)
[![][license]](https://github.com/ysdragon/ftp/blob/master/LICENSE)

**A comprehensive FTP client library for Ring with support for FTP/FTPS file transfers, directory management, and progress tracking.**

</div>

## ✨ Features

- Full FTP client functionality (upload, download, list, delete, rename, etc.)
- Support for both passive and active transfer modes
- SSL/TLS support for secure FTPS connections
- Progress callbacks for tracking file transfers
- Directory management (create, remove, list)
- Custom FTP command execution
- Configurable timeouts and verbose debugging
- Cross-platform support (Windows, Linux, macOS, FreeBSD)

## 📦 Installation

This package can be installed using the Ring Package Manager (**RingPM**):

```
ringpm install ftp from ysdragon
```

## 💡 Usage

First, load the library in your Ring script:

```ring
load "ftp.ring"
```

### Basic File Upload

To connect to an FTP server and upload a file:

```ring
ftp = new FTP()

try
	ftp {
		setHost("ftp.example.com", 21)
		setCredentials("username", "password")
		connect()
		upload("local_file.txt", "remote_file.txt")
		close()
	}
	? "Upload successful!"
	
catch
	? "Error: " + ftp.getError()
end
```

### File Download

To download a file from an FTP server:

```ring
ftp = new FTP()

try
	ftp {
		setHost("ftp.example.com", 21)
		setCredentials("username", "password")
		connect()
		download("remote_file.txt", "local_file.txt")
		close()
	}
	? "Download complete!"
	
catch
	? "Error: " + ftp.getError()
end
```

### Directory Operations

```ring
ftp = new FTP()

try
	ftp {
		setHost("ftp.example.com", 21)
		setCredentials("username", "password")
		connect()
		
		# List directory contents
		listing = listDir("/")
		? listing
		
		# Create a directory
		mkdir("new_folder")
		
		# Remove a directory
		rmdir("old_folder")
		
		# Delete a file
		delete("unwanted_file.txt")
		
		# Rename a file
		rename("old_name.txt", "new_name.txt")
		
		# Get file size
		size = getFileSize("remote_file.txt")
		? "File size: " + size + " bytes"
		
		close()
	}
	
catch
	? "Error: " + ftp.getError()
end
```

### Progress Tracking

Track upload/download progress with callbacks:

```ring
ftp = new FTP()

try
	ftp {
		setHost("ftp.example.com", 21)
		setCredentials("username", "password")
		setProgressCallback(:myProgressCallback)
		connect()
		upload("large_file.zip", "large_file.zip")
		close()
	}
	
catch
	? "Error: " + ftp.getError()
end

func myProgressCallback(download_total, download_now, upload_total, upload_now)
	if upload_total > 0
		percent = (upload_now / upload_total) * 100
		? "Upload Progress: " + percent + "%"
	ok
	if download_total > 0
		percent = (download_now / download_total) * 100
		? "Download Progress: " + percent + "%"
	ok
```

### Advanced Configuration

```ring
ftp = new FTP()

try
	ftp {
		setHost("ftp.example.com", 21)
		setCredentials("username", "password")
		
		# Set passive mode (default)
		setMode(FTP_MODE_PASSIVE)
		
		# Enable SSL/TLS for secure transfers
		setSSL(FTP_SSL_ALL, 1)  # Verify SSL certificate
		
		# Set timeouts (transfer timeout, connection timeout in seconds)
		setTimeout(60, 30)
		
		# Enable verbose debugging output
		setVerbose(1)
		
		connect()
		
		# Execute custom FTP command
		response = executeCommand("PWD")
		? "Current directory: " + response
		
		close()
	}
	
catch
	? "Error: " + ftp.getError()
end
```

## ⚙️ Configuration Options

### Transfer Modes
- **FTP_MODE_PASSIVE**: Passive mode (default, recommended for most firewalls)
- **FTP_MODE_ACTIVE**: Active mode

### SSL/TLS Options
- **FTP_SSL_NONE**: No SSL/TLS (plain FTP)
- **FTP_SSL_TRY**: Try SSL/TLS, fallback to plain FTP if unavailable
- **FTP_SSL_CONTROL**: SSL/TLS for control connection only
- **FTP_SSL_ALL**: SSL/TLS for both control and data connections (FTPS)

### Timeouts
- Configure both transfer and connection timeouts using `setTimeout(transfer_seconds, connect_seconds)`
- Default timeout values are used if not specified

## 📚 API Reference

### FTP Class Methods

#### Configuration Methods

##### `setHost(host, port)`
Sets the FTP server host and port.

**Parameters:**
- `host` (string): The FTP server hostname or IP address
- `port` (number): The FTP server port (usually 21)

**Returns:** self (for method chaining)

**Example:**
```ring
ftp.setHost("ftp.example.com", 21)
```

##### `setCredentials(username, password)`
Sets the username and password for authentication.

**Parameters:**
- `username` (string): FTP username
- `password` (string): FTP password

**Returns:** self (for method chaining)

**Example:**
```ring
ftp.setCredentials("myuser", "mypass")
```

##### `setMode(mode)`
Sets the transfer mode (passive or active).

**Parameters:**
- `mode` (number): FTP_MODE_PASSIVE or FTP_MODE_ACTIVE

**Returns:** self (for method chaining)

**Example:**
```ring
ftp.setMode(FTP_MODE_PASSIVE)
```

##### `setSSL(sslMode, verify)`
Configures SSL/TLS settings.

**Parameters:**
- `sslMode` (number): FTP_SSL_NONE, FTP_SSL_TRY, FTP_SSL_CONTROL, or FTP_SSL_ALL
- `verify` (number): 1 to verify SSL certificate, 0 to skip verification

**Returns:** self (for method chaining)

**Example:**
```ring
ftp.setSSL(FTP_SSL_ALL, 1)
```

##### `setTimeout(timeout, connectTimeout)`
Sets operation and connection timeouts.

**Parameters:**
- `timeout` (number): Transfer timeout in seconds
- `connectTimeout` (number): Connection timeout in seconds

**Returns:** self (for method chaining)

**Example:**
```ring
ftp.setTimeout(60, 30)
```

##### `setVerbose(verbose)`
Enables or disables verbose debugging output.

**Parameters:**
- `verbose` (number): 1 to enable, 0 to disable

**Returns:** self (for method chaining)

**Example:**
```ring
ftp.setVerbose(1)
```

#### Callback Methods

##### `setProgressCallback(callbackFuncName)`
Sets a function to track transfer progress.

**Parameters:**
- `callbackFuncName` (string): Name of the callback function

**Returns:** self (for method chaining)

**Example:**
```ring
ftp.setProgressCallback(:myProgressCallback)

func myProgressCallback(download_total, download_now, upload_total, upload_now)
	# Your progress handling code
```

##### `clearProgressCallback()`
Removes the active progress callback.

**Returns:** self (for method chaining)

#### Action Methods

##### `connect()`
Connects to the configured FTP server.

**Returns:** self (for method chaining)

**Example:**
```ring
ftp.connect()
```

##### `upload(localPath, remotePath)`
Uploads a local file to the remote server.

**Parameters:**
- `localPath` (string): Path to the local file
- `remotePath` (string): Destination path on the remote server

**Returns:** self (for method chaining)

**Example:**
```ring
ftp.upload("local.txt", "remote.txt")
```

##### `download(remotePath, localPath)`
Downloads a remote file to a local path.

**Parameters:**
- `remotePath` (string): Path to the remote file
- `localPath` (string): Destination path on the local system

**Returns:** self (for method chaining)

**Example:**
```ring
ftp.download("remote.txt", "local.txt")
```

##### `listDir(remotePath)`
Lists the contents of a remote directory.

**Parameters:**
- `remotePath` (string): Path to the remote directory

**Returns:** String containing directory listing

**Example:**
```ring
listing = ftp.listDir("/")
? listing
```

##### `mkdir(remotePath)`
Creates a directory on the remote server.

**Parameters:**
- `remotePath` (string): Path for the new directory

**Returns:** self (for method chaining)

**Example:**
```ring
ftp.mkdir("new_folder")
```

##### `rmdir(remotePath)`
Removes a directory from the remote server.

**Parameters:**
- `remotePath` (string): Path to the directory to remove

**Returns:** self (for method chaining)

**Example:**
```ring
ftp.rmdir("old_folder")
```

##### `delete(remotePath)`
Deletes a file from the remote server.

**Parameters:**
- `remotePath` (string): Path to the file to delete

**Returns:** self (for method chaining)

**Example:**
```ring
ftp.delete("unwanted.txt")
```

##### `rename(oldPath, newPath)`
Renames a file or directory on the remote server.

**Parameters:**
- `oldPath` (string): Current path
- `newPath` (string): New path

**Returns:** self (for method chaining)

**Example:**
```ring
ftp.rename("old.txt", "new.txt")
```

##### `getFileSize(remotePath)`
Gets the size of a remote file in bytes.

**Parameters:**
- `remotePath` (string): Path to the remote file

**Returns:** File size in bytes (number)

**Example:**
```ring
size = ftp.getFileSize("file.txt")
```

##### `executeCommand(command)`
Executes a custom FTP command.

**Parameters:**
- `command` (string): FTP command to execute

**Returns:** Server response string

**Example:**
```ring
response = ftp.executeCommand("PWD")
```

#### Utility Methods

##### `getError()`
Returns the last error message from the client.

**Returns:** Error message string

**Example:**
```ring
? ftp.getError()
```

##### `close()`
Disconnects and cleans up the client instance.

**Example:**
```ring
ftp.close()
```

### Constants

#### Error Codes
- `FTP_OK`: Operation successful
- `FTP_ERROR_INIT`: Initialization error
- `FTP_ERROR_CONNECTION`: Connection error
- `FTP_ERROR_AUTH`: Authentication error
- `FTP_ERROR_TRANSFER`: Transfer error
- `FTP_ERROR_FILE_NOT_FOUND`: File not found
- `FTP_ERROR_MEMORY`: Memory allocation error
- `FTP_ERROR_INVALID_PARAM`: Invalid parameter
- `FTP_ERROR_CURL`: libcurl error
- `FTP_ERROR_FILE_IO`: File I/O error
- `FTP_ERROR_TIMEOUT`: Timeout error

## 🛠️ Development

If you wish to contribute to the development of Ring FTP or build it from the source, follow these steps.

### Prerequisites

-   **CMake**: Version 3.16 or higher.
-   **C Compiler**: A C compiler compatible with your platform (e.g., GCC, Clang, MSVC).
-   **libcurl**: libcurl development libraries with SSL/TLS support.
-   **[Ring](https://ring-lang.net/) Source Code**: You will need to have the Ring language source code available on your machine.

### Build Steps

1.  **Clone the Repository:**
	```sh
	git clone https://github.com/ysdragon/ftp.git --recursive
	```

	> **Note**
	> If you installed the library via RingPM, you can skip this step.

2.  **Set the `RING` Environment Variable:**
	This variable must point to the root directory of the Ring language source code.

	-   **Windows (Command Prompt):**
		```cmd
		set RING=X:\path\to\ring
		```
	-   **Windows (PowerShell):**
		```powershell
		$env:RING = "X:\path\to\ring"
		```
	-   **Unix-like Systems (Linux, macOS or FreeBSD):**
		```bash
		export RING=/path/to/ring
		```

3.  **Configure with CMake:**
	Create a build directory and run CMake from within it.
	```sh
	mkdir build
	cd build
	cmake ..
	```

4.  **Build the Project:**
	Compile the source code using the build toolchain configured by CMake.
	```sh
	cmake --build .
	```

	The compiled library will be available in the `lib/<os>/<arch>` directory.

## 🤝 Contributing

Contributions are always welcome! If you have suggestions for improvements or have identified a bug, please feel free to open an issue or submit a pull request.

## 📄 License

This project is licensed under the MIT License. See the [`LICENSE`](LICENSE) file for more details.