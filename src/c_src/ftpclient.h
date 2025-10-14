/*
 * ftpclient.h - v1.0.0 - Single Header FTP Client Library
 *
 * A lightweight, easy-to-use FTP/FTPS client library for C/C++.
 * Built on top of libcurl with a simple, intuitive API.
 *
 * USAGE:
 *   Do this:
 *      #define FTP_CLIENT_IMPLEMENTATION
 *   before you include this file in *one* C/C++ file to create the implementation.
 *
 *   // i.e. it should look like this:
 *   #include ...
 *   #include ...
 *   #include ...
 *   #define FTP_CLIENT_IMPLEMENTATION
 *   #include "ftpclient.h"
 *
 * FEATURES:
 *   - Upload and download files
 *   - Directory operations (create, remove, list)
 *   - File management (delete, rename, get size)
 *   - SSL/TLS support (FTPS)
 *   - Progress callbacks
 *   - Active and passive modes
 *   - Comprehensive error handling
 *   - Custom FTP command execution
 *
 * DEPENDENCIES:
 *   libcurl (7.20.0 or later)
 *
 * OPTIONAL DEFINES:
 *   #define FTP_MAX_URL_LENGTH 4096     // Default: 2048
 *   #define FTP_BUFFER_SIZE 16384       // Default: 8192
 *
 * LICENSE:
 *   See end of file for license information.
 *
 * AUTHOR:
 *   Youssef Saeed (youssefelkholey@gmail.com)
 */

#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <curl/curl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Configuration macros */
#ifndef FTP_MAX_URL_LENGTH
#define FTP_MAX_URL_LENGTH 2048
#endif

#ifndef FTP_BUFFER_SIZE
#define FTP_BUFFER_SIZE 8192
#endif

	/* Error codes */
	typedef enum
	{
		FTP_OK = 0,
		FTP_ERROR_INIT = -1,
		FTP_ERROR_CONNECTION = -2,
		FTP_ERROR_AUTH = -3,
		FTP_ERROR_TRANSFER = -4,
		FTP_ERROR_FILE_NOT_FOUND = -5,
		FTP_ERROR_MEMORY = -6,
		FTP_ERROR_INVALID_PARAM = -7,
		FTP_ERROR_CURL = -8,
		FTP_ERROR_FILE_IO = -9,
		FTP_ERROR_TIMEOUT = -10
	} ftp_error_t;

	/* Transfer mode */
	typedef enum
	{
		FTP_MODE_PASSIVE = 0,
		FTP_MODE_ACTIVE = 1
	} ftp_mode_t;

	/* SSL/TLS options */
	typedef enum
	{
		FTP_SSL_NONE = 0,
		FTP_SSL_TRY = 1,
		FTP_SSL_CONTROL = 2,
		FTP_SSL_ALL = 3
	} ftp_ssl_mode_t;

	/* Progress callback function type */
	typedef int (*ftp_progress_callback_t)(void *user_data, double download_total, double download_now,
										   double upload_total, double upload_now);

	/* Write callback data structure */
	typedef struct
	{
		char *data;
		size_t size;
		size_t capacity;
	} ftp_memory_buffer_t;

	/* FTP client configuration */
	typedef struct
	{
		char host[256];
		int port;
		char username[128];
		char password[128];
		ftp_mode_t mode;
		ftp_ssl_mode_t ssl_mode;
		int verify_ssl;
		long timeout;
		long connect_timeout;
		int verbose;
		ftp_progress_callback_t progress_callback;
		void *progress_user_data;
	} ftp_config_t;

	/* FTP client handle */
	typedef struct
	{
		CURL *curl;
		ftp_config_t config;
		char last_error[512];
		int initialized;
	} ftp_client_t;

	/* API Functions */

	/**
	 * @brief Initialize the FTP client library
	 *
	 * This function must be called once before using any other FTP client functions.
	 * It initializes the underlying libcurl library and prepares the environment
	 * for FTP operations.
	 *
	 * @return FTP_OK (0) on success, FTP_ERROR_INIT (-1) on failure
	 *
	 * @note This function is thread-safe and can be called multiple times, but
	 *       should be matched with an equal number of ftp_global_cleanup() calls.
	 *
	 * @see ftp_global_cleanup()
	 *
	 * Example:
	 * @code
	 * if (ftp_global_init() != FTP_OK) {
	 *     fprintf(stderr, "Failed to initialize FTP library\n");
	 *     return -1;
	 * }
	 * @endcode
	 */
	int ftp_global_init(void);

	/**
	 * @brief Cleanup the FTP client library
	 *
	 * This function cleans up resources allocated by ftp_global_init().
	 * Should be called when the application is done using the FTP client library.
	 *
	 * @note Must be called after all FTP client operations are complete and
	 *       all client handles have been destroyed.
	 *
	 * @see ftp_global_init()
	 *
	 * Example:
	 * @code
	 * // ... perform FTP operations ...
	 * ftp_global_cleanup();
	 * @endcode
	 */
	void ftp_global_cleanup(void);

	/**
	 * @brief Create a new FTP client handle
	 *
	 * Allocates and initializes a new FTP client handle with default configuration.
	 * The handle must be destroyed with ftp_client_destroy() when no longer needed.
	 *
	 * @return Pointer to a new ftp_client_t handle on success, NULL on failure
	 *
	 * @note Default configuration includes:
	 *       - Port: 21
	 *       - Mode: Passive (FTP_MODE_PASSIVE)
	 *       - SSL: None (FTP_SSL_NONE)
	 *       - Username: "anonymous"
	 *       - Password: "user@example.com"
	 *       - Timeout: 60 seconds
	 *       - Connect timeout: 30 seconds
	 *
	 * @see ftp_client_destroy(), ftp_client_init_config()
	 *
	 * Example:
	 * @code
	 * ftp_client_t *client = ftp_client_create();
	 * if (!client) {
	 *     fprintf(stderr, "Failed to create FTP client\n");
	 *     return -1;
	 * }
	 * @endcode
	 */
	ftp_client_t *ftp_client_create(void);

	/**
	 * @brief Initialize FTP configuration with default values
	 *
	 * Sets all configuration parameters to their default values.
	 * Useful for resetting configuration or initializing custom config structures.
	 *
	 * @param config Pointer to the configuration structure to initialize
	 *
	 * @note This function sets:
	 *       - Port: 21
	 *       - Mode: Passive (FTP_MODE_PASSIVE)
	 *       - SSL Mode: None (FTP_SSL_NONE)
	 *       - SSL Verification: Enabled (1)
	 *       - Username: "anonymous"
	 *       - Password: "user@example.com"
	 *       - Timeout: 60 seconds
	 *       - Connect timeout: 30 seconds
	 *       - Verbose: Disabled (0)
	 *
	 * Example:
	 * @code
	 * ftp_config_t config;
	 * ftp_client_init_config(&config);
	 * // Modify specific settings as needed
	 * config.port = 2121;
	 * @endcode
	 */
	void ftp_client_init_config(ftp_config_t *config);

	/**
	 * @brief Set FTP server connection parameters
	 *
	 * Configures the hostname and port for the FTP server connection.
	 *
	 * @param client Pointer to the FTP client handle
	 * @param host Hostname or IP address of the FTP server
	 * @param port Port number (1-65535). Use 21 for standard FTP, 990 for FTPS
	 *
	 * @return FTP_OK (0) on success, FTP_ERROR_INVALID_PARAM (-7) if parameters are invalid
	 *
	 * Example:
	 * @code
	 * if (ftp_client_set_host(client, "ftp.example.com", 21) != FTP_OK) {
	 *     fprintf(stderr, "Failed to set host\n");
	 * }
	 * @endcode
	 */
	int ftp_client_set_host(ftp_client_t *client, const char *host, int port);

	/**
	 * @brief Set FTP authentication credentials
	 *
	 * Configures the username and password for FTP server authentication.
	 *
	 * @param client Pointer to the FTP client handle
	 * @param username Username for authentication (max 127 characters)
	 * @param password Password for authentication (max 127 characters)
	 *
	 * @return FTP_OK (0) on success, FTP_ERROR_INVALID_PARAM (-7) if parameters are invalid
	 *
	 * @note For anonymous FTP, use username "anonymous" and an email address as password
	 *
	 * Example:
	 * @code
	 * if (ftp_client_set_credentials(client, "myuser", "mypassword") != FTP_OK) {
	 *     fprintf(stderr, "Failed to set credentials\n");
	 * }
	 * @endcode
	 */
	int ftp_client_set_credentials(ftp_client_t *client, const char *username, const char *password);

	/**
	 * @brief Set FTP transfer mode
	 *
	 * Configures whether to use active or passive mode for data transfers.
	 *
	 * @param client Pointer to the FTP client handle
	 * @param mode Transfer mode: FTP_MODE_PASSIVE (0) or FTP_MODE_ACTIVE (1)
	 *
	 * @note Passive mode (default) is recommended for most scenarios, especially
	 *       when behind firewalls or NAT. Active mode may be required for some
	 *       legacy FTP servers.
	 *
	 * Example:
	 * @code
	 * ftp_client_set_mode(client, FTP_MODE_PASSIVE);  // Use passive mode
	 * @endcode
	 */
	void ftp_client_set_mode(ftp_client_t *client, ftp_mode_t mode);

	/**
	 * @brief Set SSL/TLS encryption mode
	 *
	 * Configures the level of SSL/TLS encryption for FTP connections (FTPS).
	 *
	 * @param client Pointer to the FTP client handle
	 * @param ssl_mode SSL mode:
	 *                 - FTP_SSL_NONE (0): No encryption
	 *                 - FTP_SSL_TRY (1): Try encryption, fall back to plain if unavailable
	 *                 - FTP_SSL_CONTROL (2): Encrypt control connection only
	 *                 - FTP_SSL_ALL (3): Encrypt both control and data connections
	 * @param verify Whether to verify SSL certificates (1 = verify, 0 = don't verify)
	 *
	 * @note For secure connections, use FTP_SSL_ALL with verify=1. Disabling
	 *       verification (verify=0) makes connections vulnerable to MITM attacks.
	 *
	 * Example:
	 * @code
	 * // Enable full SSL/TLS with certificate verification
	 * ftp_client_set_ssl(client, FTP_SSL_ALL, 1);
	 * @endcode
	 */
	void ftp_client_set_ssl(ftp_client_t *client, ftp_ssl_mode_t ssl_mode, int verify);

	/**
	 * @brief Set connection and transfer timeouts
	 *
	 * Configures timeout values for FTP operations.
	 *
	 * @param client Pointer to the FTP client handle
	 * @param timeout Maximum time (in seconds) for a complete operation (0 = no timeout)
	 * @param connect_timeout Maximum time (in seconds) to establish connection (0 = default)
	 *
	 * @note Only positive values are applied. Zero or negative values are ignored.
	 *       Default timeout is 60 seconds, default connect_timeout is 30 seconds.
	 *
	 * Example:
	 * @code
	 * // Set 120 second operation timeout and 15 second connect timeout
	 * ftp_client_set_timeout(client, 120, 15);
	 * @endcode
	 */
	void ftp_client_set_timeout(ftp_client_t *client, long timeout, long connect_timeout);

	/**
	 * @brief Enable or disable verbose debug output
	 *
	 * Controls whether detailed debug information is printed to stderr.
	 *
	 * @param client Pointer to the FTP client handle
	 * @param verbose 1 to enable verbose output, 0 to disable (default)
	 *
	 * @note Verbose output includes all FTP commands and responses, useful for debugging.
	 *
	 * Example:
	 * @code
	 * ftp_client_set_verbose(client, 1);  // Enable debug output
	 * @endcode
	 */
	void ftp_client_set_verbose(ftp_client_t *client, int verbose);

	/**
	 * @brief Set progress callback for file transfers
	 *
	 * Registers a callback function to receive progress updates during uploads and downloads.
	 *
	 * @param client Pointer to the FTP client handle
	 * @param callback Function pointer to progress callback (NULL to disable)
	 * @param user_data User-defined pointer passed to the callback function
	 *
	 * @note Callback signature: int callback(void *user_data, double dl_total,
	 *       double dl_now, double ul_total, double ul_now)
	 *       Return 0 to continue transfer, non-zero to abort.
	 *
	 * Example:
	 * @code
	 * int my_progress(void *data, double dltotal, double dlnow,
	 *                 double ultotal, double ulnow) {
	 *     if (ultotal > 0) {
	 *         printf("Upload progress: %.1f%%\n", (ulnow / ultotal) * 100.0);
	 *     }
	 *     return 0;  // Continue transfer
	 * }
	 *
	 * ftp_client_set_progress_callback(client, my_progress, NULL);
	 * @endcode
	 */
	void ftp_client_set_progress_callback(ftp_client_t *client, ftp_progress_callback_t callback, void *user_data);

	/**
	 * @brief Test connection to FTP server
	 *
	 * Attempts to connect to the FTP server and authenticate using the configured
	 * credentials. This verifies that the server is reachable and credentials are valid.
	 *
	 * @param client Pointer to the FTP client handle
	 *
	 * @return FTP_OK (0) on success
	 *         FTP_ERROR_INVALID_PARAM (-7) if client is NULL or host not set
	 *         FTP_ERROR_AUTH (-3) if authentication fails
	 *         FTP_ERROR_TIMEOUT (-10) if connection times out
	 *         FTP_ERROR_CONNECTION (-2) for other connection errors
	 *
	 * @note This function lists the root directory to verify the connection.
	 *       Use ftp_client_get_error() to retrieve detailed error messages.
	 *
	 * Example:
	 * @code
	 * int result = ftp_client_connect(client);
	 * if (result != FTP_OK) {
	 *     fprintf(stderr, "Connection failed: %s\n", ftp_client_get_error(client));
	 *     return -1;
	 * }
	 * printf("Connected successfully!\n");
	 * @endcode
	 */
	int ftp_client_connect(ftp_client_t *client);

	/**
	 * @brief Upload a file to the FTP server
	 *
	 * Transfers a file from the local filesystem to the FTP server.
	 *
	 * @param client Pointer to the FTP client handle
	 * @param local_path Path to the local file to upload
	 * @param remote_path Destination path on the FTP server
	 *
	 * @return FTP_OK (0) on success
	 *         FTP_ERROR_INVALID_PARAM (-7) if any parameter is NULL
	 *         FTP_ERROR_FILE_IO (-9) if local file cannot be opened
	 *         FTP_ERROR_TRANSFER (-4) if transfer fails
	 *
	 * @note If progress callback is set, it will be called during the upload.
	 *       Remote directories must exist before uploading to them.
	 *
	 * Example:
	 * @code
	 * int result = ftp_client_upload(client, "local_file.txt", "/upload/remote_file.txt");
	 * if (result != FTP_OK) {
	 *     fprintf(stderr, "Upload failed: %s\n", ftp_client_get_error(client));
	 * }
	 * @endcode
	 */
	int ftp_client_upload(ftp_client_t *client, const char *local_path, const char *remote_path);

	/**
	 * @brief Download a file from the FTP server
	 *
	 * Transfers a file from the FTP server to the local filesystem.
	 *
	 * @param client Pointer to the FTP client handle
	 * @param remote_path Path to the file on the FTP server
	 * @param local_path Destination path on the local filesystem
	 *
	 * @return FTP_OK (0) on success
	 *         FTP_ERROR_INVALID_PARAM (-7) if any parameter is NULL
	 *         FTP_ERROR_FILE_IO (-9) if local file cannot be created
	 *         FTP_ERROR_FILE_NOT_FOUND (-5) if remote file doesn't exist
	 *         FTP_ERROR_TRANSFER (-4) if transfer fails
	 *
	 * @note If progress callback is set, it will be called during the download.
	 *       Partial files are deleted if the download fails.
	 *
	 * Example:
	 * @code
	 * int result = ftp_client_download(client, "/download/file.txt", "local_file.txt");
	 * if (result != FTP_OK) {
	 *     fprintf(stderr, "Download failed: %s\n", ftp_client_get_error(client));
	 * }
	 * @endcode
	 */
	int ftp_client_download(ftp_client_t *client, const char *remote_path, const char *local_path);

	/**
	 * @brief List directory contents on the FTP server
	 *
	 * Retrieves a listing of files and directories in the specified path.
	 *
	 * @param client Pointer to the FTP client handle
	 * @param remote_path Path to the directory on the FTP server
	 * @param output Pointer to receive the allocated string with directory listing
	 *
	 * @return FTP_OK (0) on success, error code on failure
	 *         FTP_ERROR_INVALID_PARAM (-7) if any parameter is NULL
	 *         FTP_ERROR_TRANSFER (-4) if listing fails
	 *
	 * @note The caller is responsible for freeing the allocated output string.
	 *       The format of the listing depends on the FTP server (typically Unix-style ls format).
	 *
	 * Example:
	 * @code
	 * char *listing = NULL;
	 * if (ftp_client_list_dir(client, "/", &listing) == FTP_OK) {
	 *     printf("Directory contents:\n%s\n", listing);
	 *     free(listing);
	 * }
	 * @endcode
	 */
	int ftp_client_list_dir(ftp_client_t *client, const char *remote_path, char **output);

	/**
	 * @brief Create a directory on the FTP server
	 *
	 * Creates a new directory at the specified path on the FTP server.
	 *
	 * @param client Pointer to the FTP client handle
	 * @param remote_path Path where the directory should be created
	 *
	 * @return FTP_OK (0) on success
	 *         FTP_ERROR_INVALID_PARAM (-7) if any parameter is NULL
	 *         FTP_ERROR_TRANSFER (-4) if directory creation fails
	 *
	 * @note Parent directories must exist. This function does not create
	 *       intermediate directories recursively. Some servers may return
	 *       an error if the directory already exists.
	 *
	 * Example:
	 * @code
	 * if (ftp_client_mkdir(client, "/uploads/new_folder") != FTP_OK) {
	 *     fprintf(stderr, "Failed to create directory: %s\n",
	 *             ftp_client_get_error(client));
	 * }
	 * @endcode
	 */
	int ftp_client_mkdir(ftp_client_t *client, const char *remote_path);

	/**
	 * @brief Remove a directory from the FTP server
	 *
	 * Deletes an empty directory from the FTP server.
	 *
	 * @param client Pointer to the FTP client handle
	 * @param remote_path Path to the directory to remove
	 *
	 * @return FTP_OK (0) on success
	 *         FTP_ERROR_INVALID_PARAM (-7) if any parameter is NULL
	 *         FTP_ERROR_TRANSFER (-4) if removal fails
	 *
	 * @note The directory must be empty before it can be removed.
	 *       Use ftp_client_delete() to remove files first if needed.
	 *
	 * Example:
	 * @code
	 * if (ftp_client_rmdir(client, "/uploads/old_folder") != FTP_OK) {
	 *     fprintf(stderr, "Failed to remove directory: %s\n",
	 *             ftp_client_get_error(client));
	 * }
	 * @endcode
	 */
	int ftp_client_rmdir(ftp_client_t *client, const char *remote_path);

	/**
	 * @brief Delete a file from the FTP server
	 *
	 * Removes a file from the FTP server.
	 *
	 * @param client Pointer to the FTP client handle
	 * @param remote_path Path to the file to delete
	 *
	 * @return FTP_OK (0) on success
	 *         FTP_ERROR_INVALID_PARAM (-7) if any parameter is NULL
	 *         FTP_ERROR_TRANSFER (-4) if deletion fails
	 *
	 * @note This operation is permanent and cannot be undone.
	 *       To remove directories, use ftp_client_rmdir() instead.
	 *
	 * Example:
	 * @code
	 * if (ftp_client_delete(client, "/uploads/old_file.txt") != FTP_OK) {
	 *     fprintf(stderr, "Failed to delete file: %s\n",
	 *             ftp_client_get_error(client));
	 * }
	 * @endcode
	 */
	int ftp_client_delete(ftp_client_t *client, const char *remote_path);

	/**
	 * @brief Rename or move a file or directory on the FTP server
	 *
	 * Renames or moves a file or directory from one path to another.
	 *
	 * @param client Pointer to the FTP client handle
	 * @param old_path Current path of the file or directory
	 * @param new_path New path for the file or directory
	 *
	 * @return FTP_OK (0) on success
	 *         FTP_ERROR_INVALID_PARAM (-7) if any parameter is NULL
	 *         FTP_ERROR_TRANSFER (-4) if rename fails
	 *
	 * @note This can be used to move files between directories if the server
	 *       supports it. The destination directory must exist. If a file with
	 *       the new name already exists, behavior depends on the server.
	 *
	 * Example:
	 * @code
	 * // Rename a file
	 * ftp_client_rename(client, "/uploads/old_name.txt", "/uploads/new_name.txt");
	 *
	 * // Move a file to different directory
	 * ftp_client_rename(client, "/temp/file.txt", "/archive/file.txt");
	 * @endcode
	 */
	int ftp_client_rename(ftp_client_t *client, const char *old_path, const char *new_path);

	/**
	 * @brief Get file size on the FTP server
	 *
	 * Retrieves the size of a file on the FTP server without downloading it.
	 *
	 * @param client Pointer to the FTP client handle
	 * @param remote_path Path to the file on the FTP server
	 * @param size Pointer to receive the file size in bytes
	 *
	 * @return FTP_OK (0) on success
	 *         FTP_ERROR_INVALID_PARAM (-7) if any parameter is NULL
	 *         FTP_ERROR_TRANSFER (-4) if operation fails
	 *
	 * @note This uses the FTP SIZE command, which may not be supported by all servers.
	 *       The size value is only updated on success.
	 *
	 * Example:
	 * @code
	 * int64_t file_size;
	 * if (ftp_client_get_filesize(client, "/data/large_file.bin", &file_size) == FTP_OK) {
	 *     printf("File size: %lld bytes\n", (long long)file_size);
	 * }
	 * @endcode
	 */
	int ftp_client_get_filesize(ftp_client_t *client, const char *remote_path, int64_t *size);

	/**
	 * @brief Execute custom FTP command
	 *
	 * Sends a raw FTP command to the server and optionally retrieves the response.
	 *
	 * @param client Pointer to the FTP client handle
	 * @param command Raw FTP command to execute (e.g., "SITE CHMOD 755 file.txt")
	 * @param response Pointer to receive server response (NULL if not needed)
	 *
	 * @return FTP_OK (0) on success
	 *         FTP_ERROR_INVALID_PARAM (-7) if client or command is NULL
	 *         FTP_ERROR_TRANSFER (-4) if command execution fails
	 *
	 * @note The caller is responsible for freeing the response string if provided.
	 *       This is an advanced function - incorrect commands may leave the
	 *       connection in an inconsistent state.
	 *
	 * Example:
	 * @code
	 * char *response = NULL;
	 * if (ftp_client_execute_command(client, "STAT", &response) == FTP_OK) {
	 *     printf("Server status:\n%s\n", response);
	 *     free(response);
	 * }
	 * @endcode
	 */
	int ftp_client_execute_command(ftp_client_t *client, const char *command, char **response);

	/**
	 * @brief Get last error message
	 *
	 * Returns a human-readable description of the last error that occurred.
	 *
	 * @param client Pointer to the FTP client handle
	 *
	 * @return Pointer to error message string. Returns "Invalid client handle" if client is NULL.
	 *
	 * @note The returned string is managed by the client handle and should not be freed.
	 *       The error message is overwritten by subsequent operations.
	 *
	 * Example:
	 * @code
	 * if (ftp_client_upload(client, "file.txt", "/remote.txt") != FTP_OK) {
	 *     fprintf(stderr, "Error: %s\n", ftp_client_get_error(client));
	 * }
	 * @endcode
	 */
	const char *ftp_client_get_error(ftp_client_t *client);

	/**
	 * @brief Destroy FTP client handle and free resources
	 *
	 * Cleans up and deallocates an FTP client handle.
	 *
	 * @param client Pointer to the FTP client handle to destroy
	 *
	 * @note After calling this function, the client pointer is invalid and should not be used.
	 *       It is safe to pass NULL to this function (it will do nothing).
	 *       This does not call ftp_global_cleanup() - that must be called separately.
	 *
	 * Example:
	 * @code
	 * ftp_client_destroy(client);
	 * client = NULL;  // Good practice to avoid dangling pointer
	 * @endcode
	 */
	void ftp_client_destroy(ftp_client_t *client);

#ifdef FTP_CLIENT_IMPLEMENTATION

	/* Internal helper functions */

	static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp)
	{
		size_t realsize = size * nmemb;
		ftp_memory_buffer_t *mem = (ftp_memory_buffer_t *)userp;

		if (mem->size + realsize + 1 > mem->capacity)
		{
			size_t new_capacity = mem->capacity == 0 ? FTP_BUFFER_SIZE : mem->capacity * 2;
			while (new_capacity < mem->size + realsize + 1)
			{
				new_capacity *= 2;
			}

			char *new_data = (char *)realloc(mem->data, new_capacity);
			if (!new_data)
			{
				return 0;
			}
			mem->data = new_data;
			mem->capacity = new_capacity;
		}

		memcpy(&(mem->data[mem->size]), contents, realsize);
		mem->size += realsize;
		mem->data[mem->size] = 0;

		return realsize;
	}

	static size_t read_file_callback(void *ptr, size_t size, size_t nmemb, void *stream)
	{
		size_t retcode = fread(ptr, size, nmemb, (FILE *)stream);
		return retcode;
	}

	static size_t write_file_callback(void *ptr, size_t size, size_t nmemb, void *stream)
	{
		size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
		return written;
	}

	static int progress_callback_wrapper(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal,
										 curl_off_t ulnow)
	{
		ftp_client_t *client = (ftp_client_t *)clientp;
		if (client->config.progress_callback)
		{
			return client->config.progress_callback(client->config.progress_user_data, (double)dltotal, (double)dlnow,
													(double)ultotal, (double)ulnow);
		}
		return 0;
	}

	static int build_ftp_url(const ftp_client_t *client, const char *remote_path, char *url, size_t url_size)
	{
		const char *protocol = "ftp";

		/*
		 * Calculate required space: protocol (3) + "://" (3) + host (256) + ":" (1) + port (5) + "/" (1) + path + null
		 */
		size_t base_len = strlen(protocol) + 3 + strlen(client->config.host) + 1 + 5 + 1;
		size_t path_len = remote_path ? strlen(remote_path) : 0;
		size_t required_len = base_len + path_len + 1; /* +1 for null terminator */

		/* Check if URL will fit */
		if (required_len > url_size)
		{
			return FTP_ERROR_INVALID_PARAM; /* URL too long */
		}

		/* Build the URL */
		if (remote_path && remote_path[0] == '/')
		{
			snprintf(url, url_size, "%s://%s:%d%s", protocol, client->config.host, client->config.port, remote_path);
		}
		else if (remote_path)
		{
			snprintf(url, url_size, "%s://%s:%d/%s", protocol, client->config.host, client->config.port, remote_path);
		}
		else
		{
			snprintf(url, url_size, "%s://%s:%d/", protocol, client->config.host, client->config.port);
		}

		return FTP_OK;
	}

	static void setup_curl_common(ftp_client_t *client)
	{
		char userpass[256];
		snprintf(userpass, sizeof(userpass), "%s:%s", client->config.username, client->config.password);

		curl_easy_setopt(client->curl, CURLOPT_USERPWD, userpass);
		curl_easy_setopt(client->curl, CURLOPT_TIMEOUT, client->config.timeout);
		curl_easy_setopt(client->curl, CURLOPT_CONNECTTIMEOUT, client->config.connect_timeout);
		curl_easy_setopt(client->curl, CURLOPT_VERBOSE, client->config.verbose ? 1L : 0L);

		/* Transfer mode */
		if (client->config.mode == FTP_MODE_ACTIVE)
		{
			curl_easy_setopt(client->curl, CURLOPT_FTPPORT, "-");
		}
		else
		{
			curl_easy_setopt(client->curl, CURLOPT_FTP_USE_EPSV, 1L);
		}

		/* SSL/TLS settings */
		if (client->config.ssl_mode != FTP_SSL_NONE)
		{
			curl_easy_setopt(client->curl, CURLOPT_USE_SSL, (long)client->config.ssl_mode);
			curl_easy_setopt(client->curl, CURLOPT_SSL_VERIFYPEER, client->config.verify_ssl ? 1L : 0L);
			curl_easy_setopt(client->curl, CURLOPT_SSL_VERIFYHOST, client->config.verify_ssl ? 2L : 0L);
		}

		/* Progress callback */
		if (client->config.progress_callback)
		{
			curl_easy_setopt(client->curl, CURLOPT_XFERINFOFUNCTION, progress_callback_wrapper);
			curl_easy_setopt(client->curl, CURLOPT_XFERINFODATA, client);
			curl_easy_setopt(client->curl, CURLOPT_NOPROGRESS, 0L);
		}
		else
		{
			curl_easy_setopt(client->curl, CURLOPT_NOPROGRESS, 1L);
		}
	}

	static int ftp_client_execute_simple_command(ftp_client_t *client, struct curl_slist *commands,
												 const char *error_prefix)
	{
		if (!client || !client->curl || !commands)
		{
			return FTP_ERROR_INVALID_PARAM;
		}

		/* Reset curl handle to default state */
		curl_easy_reset(client->curl);

		/* Use base URL for command execution */
		char url[FTP_MAX_URL_LENGTH];
		int result = build_ftp_url(client, "/", url, sizeof(url));
		if (result != FTP_OK)
		{
			snprintf(client->last_error, sizeof(client->last_error), "%s: URL too long", error_prefix);
			return result;
		}

		curl_easy_setopt(client->curl, CURLOPT_URL, url);
		setup_curl_common(client);
		curl_easy_setopt(client->curl, CURLOPT_QUOTE, commands);

		ftp_memory_buffer_t buffer = {0};
		curl_easy_setopt(client->curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
		curl_easy_setopt(client->curl, CURLOPT_WRITEDATA, &buffer);

		CURLcode res = curl_easy_perform(client->curl);

		if (buffer.data)
		{
			free(buffer.data);
		}

		if (res != CURLE_OK)
		{
			snprintf(client->last_error, sizeof(client->last_error), "%s: %s", error_prefix, curl_easy_strerror(res));
			return FTP_ERROR_TRANSFER;
		}

		return FTP_OK;
	}

	/* API Implementation */

	int ftp_global_init(void)
	{
		CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
		return (res == CURLE_OK) ? FTP_OK : FTP_ERROR_INIT;
	}

	void ftp_global_cleanup(void)
	{
		curl_global_cleanup();
	}

	ftp_client_t *ftp_client_create(void)
	{
		ftp_client_t *client = (ftp_client_t *)calloc(1, sizeof(ftp_client_t));
		if (!client)
		{
			return NULL;
		}

		client->curl = curl_easy_init();
		if (!client->curl)
		{
			free(client);
			return NULL;
		}

		/* Initialize with defaults */
		ftp_client_init_config(&client->config);
		client->initialized = 1;

		return client;
	}

	void ftp_client_init_config(ftp_config_t *config)
	{
		memset(config, 0, sizeof(ftp_config_t));
		config->port = 21;
		config->mode = FTP_MODE_PASSIVE;
		config->ssl_mode = FTP_SSL_NONE;
		config->verify_ssl = 1;
		config->timeout = 60;
		config->connect_timeout = 30;
		config->verbose = 0;
#ifdef _MSC_VER
		strcpy_s(config->username, sizeof(config->username), "anonymous");
		strcpy_s(config->password, sizeof(config->password), "user@example.com");
#else
		strcpy(config->username, "anonymous");
		strcpy(config->password, "user@example.com");
#endif
	}

	int ftp_client_set_host(ftp_client_t *client, const char *host, int port)
	{
		if (!client || !host)
		{
			return FTP_ERROR_INVALID_PARAM;
		}

#ifdef _MSC_VER
		strncpy_s(client->config.host, sizeof(client->config.host), host, _TRUNCATE);
#else
		strncpy(client->config.host, host, sizeof(client->config.host) - 1);
		client->config.host[sizeof(client->config.host) - 1] = '\0';
#endif

		if (port > 0 && port <= 65535)
		{
			client->config.port = port;
		}

		return FTP_OK;
	}

	int ftp_client_set_credentials(ftp_client_t *client, const char *username, const char *password)
	{
		if (!client || !username || !password)
		{
			return FTP_ERROR_INVALID_PARAM;
		}

#ifdef _MSC_VER
		strncpy_s(client->config.username, sizeof(client->config.username), username, _TRUNCATE);
		strncpy_s(client->config.password, sizeof(client->config.password), password, _TRUNCATE);
#else
		strncpy(client->config.username, username, sizeof(client->config.username) - 1);
		client->config.username[sizeof(client->config.username) - 1] = '\0';

		strncpy(client->config.password, password, sizeof(client->config.password) - 1);
		client->config.password[sizeof(client->config.password) - 1] = '\0';
#endif

		return FTP_OK;
	}

	void ftp_client_set_mode(ftp_client_t *client, ftp_mode_t mode)
	{
		if (client)
		{
			client->config.mode = mode;
		}
	}

	void ftp_client_set_ssl(ftp_client_t *client, ftp_ssl_mode_t ssl_mode, int verify)
	{
		if (client)
		{
			client->config.ssl_mode = ssl_mode;
			client->config.verify_ssl = verify;
		}
	}

	void ftp_client_set_timeout(ftp_client_t *client, long timeout, long connect_timeout)
	{
		if (client)
		{
			if (timeout > 0)
			{
				client->config.timeout = timeout;
			}
			if (connect_timeout > 0)
			{
				client->config.connect_timeout = connect_timeout;
			}
		}
	}

	void ftp_client_set_verbose(ftp_client_t *client, int verbose)
	{
		if (client)
		{
			client->config.verbose = verbose;
		}
	}

	void ftp_client_set_progress_callback(ftp_client_t *client, ftp_progress_callback_t callback, void *user_data)
	{
		if (client)
		{
			client->config.progress_callback = callback;
			client->config.progress_user_data = user_data;
		}
	}

	int ftp_client_connect(ftp_client_t *client)
	{
		if (!client || !client->curl)
		{
			return FTP_ERROR_INVALID_PARAM;
		}

		if (client->config.host[0] == '\0')
		{
			snprintf(client->last_error, sizeof(client->last_error), "Host not set");
			return FTP_ERROR_INVALID_PARAM;
		}

		/* Reset curl handle to default state */
		curl_easy_reset(client->curl);

		char url[FTP_MAX_URL_LENGTH];
		int result = build_ftp_url(client, "/", url, sizeof(url));
		if (result != FTP_OK)
		{
			snprintf(client->last_error, sizeof(client->last_error), "URL too long");
			return result;
		}

		curl_easy_setopt(client->curl, CURLOPT_URL, url);
		setup_curl_common(client);

		/* Just list root to test connection */
		ftp_memory_buffer_t buffer = {0};
		curl_easy_setopt(client->curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
		curl_easy_setopt(client->curl, CURLOPT_WRITEDATA, &buffer);

		CURLcode res = curl_easy_perform(client->curl);

		if (buffer.data)
		{
			free(buffer.data);
		}

		if (res != CURLE_OK)
		{
			snprintf(client->last_error, sizeof(client->last_error), "Connection failed: %s", curl_easy_strerror(res));

			if (res == CURLE_LOGIN_DENIED)
			{
				return FTP_ERROR_AUTH;
			}
			else if (res == CURLE_OPERATION_TIMEDOUT)
			{
				return FTP_ERROR_TIMEOUT;
			}
			return FTP_ERROR_CONNECTION;
		}

		return FTP_OK;
	}

	int ftp_client_upload(ftp_client_t *client, const char *local_path, const char *remote_path)
	{
		if (!client || !client->curl || !local_path || !remote_path)
		{
			return FTP_ERROR_INVALID_PARAM;
		}

		FILE *fp = fopen(local_path, "rb");
		if (!fp)
		{
			snprintf(client->last_error, sizeof(client->last_error), "Cannot open local file: %s", local_path);
			return FTP_ERROR_FILE_IO;
		}

		/* Get file size */
		fseek(fp, 0L, SEEK_END);
		int64_t file_size = ftell(fp);
		fseek(fp, 0L, SEEK_SET);

		/* Reset curl handle to default state */
		curl_easy_reset(client->curl);

		char url[FTP_MAX_URL_LENGTH];
		int result = build_ftp_url(client, remote_path, url, sizeof(url));
		if (result != FTP_OK)
		{
			fclose(fp);
			snprintf(client->last_error, sizeof(client->last_error), "Remote path too long");
			return result;
		}

		curl_easy_setopt(client->curl, CURLOPT_URL, url);
		setup_curl_common(client);

		curl_easy_setopt(client->curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(client->curl, CURLOPT_READFUNCTION, read_file_callback);
		curl_easy_setopt(client->curl, CURLOPT_READDATA, fp);
		curl_easy_setopt(client->curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_size);

		CURLcode res = curl_easy_perform(client->curl);

		fclose(fp);

		if (res != CURLE_OK)
		{
			snprintf(client->last_error, sizeof(client->last_error), "Upload failed: %s", curl_easy_strerror(res));
			return FTP_ERROR_TRANSFER;
		}

		return FTP_OK;
	}

	int ftp_client_download(ftp_client_t *client, const char *remote_path, const char *local_path)
	{
		if (!client || !client->curl || !local_path || !remote_path)
		{
			return FTP_ERROR_INVALID_PARAM;
		}

		FILE *fp = fopen(local_path, "wb");
		if (!fp)
		{
			snprintf(client->last_error, sizeof(client->last_error), "Cannot create local file: %s", local_path);
			return FTP_ERROR_FILE_IO;
		}

		/* Reset curl handle to default state */
		curl_easy_reset(client->curl);

		char url[FTP_MAX_URL_LENGTH];
		int result = build_ftp_url(client, remote_path, url, sizeof(url));
		if (result != FTP_OK)
		{
			fclose(fp);
			snprintf(client->last_error, sizeof(client->last_error), "Remote path too long");
			return result;
		}

		curl_easy_setopt(client->curl, CURLOPT_URL, url);
		setup_curl_common(client);
		curl_easy_setopt(client->curl, CURLOPT_WRITEFUNCTION, write_file_callback);
		curl_easy_setopt(client->curl, CURLOPT_WRITEDATA, fp);

		CURLcode res = curl_easy_perform(client->curl);

		fclose(fp);

		if (res != CURLE_OK)
		{
			snprintf(client->last_error, sizeof(client->last_error), "Download failed: %s", curl_easy_strerror(res));
			remove(local_path); /* Delete partial file */

			if (res == CURLE_REMOTE_FILE_NOT_FOUND)
			{
				return FTP_ERROR_FILE_NOT_FOUND;
			}
			return FTP_ERROR_TRANSFER;
		}

		return FTP_OK;
	}

	int ftp_client_list_dir(ftp_client_t *client, const char *remote_path, char **output)
	{
		if (!client || !client->curl || !output)
		{
			return FTP_ERROR_INVALID_PARAM;
		}

		/* Reset curl handle to default state */
		curl_easy_reset(client->curl);

		char url[FTP_MAX_URL_LENGTH];
		char dir_path[FTP_MAX_URL_LENGTH];

		/* Ensure path ends with / to indicate it's a directory */
		size_t len = strlen(remote_path);
		if (len > 0 && remote_path[len - 1] != '/')
		{
			snprintf(dir_path, sizeof(dir_path), "%s/", remote_path);
		}
		else
		{
			/* Copy as-is if it already ends with / */
			snprintf(dir_path, sizeof(dir_path), "%s", remote_path);
		}

		int result = build_ftp_url(client, dir_path, url, sizeof(url));
		if (result != FTP_OK)
		{
			snprintf(client->last_error, sizeof(client->last_error), "Directory path too long");
			return result;
		}

		curl_easy_setopt(client->curl, CURLOPT_URL, url);
		setup_curl_common(client);

		ftp_memory_buffer_t buffer = {0};
		curl_easy_setopt(client->curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
		curl_easy_setopt(client->curl, CURLOPT_WRITEDATA, &buffer);

		CURLcode res = curl_easy_perform(client->curl);

		if (res != CURLE_OK)
		{
			if (buffer.data)
			{
				free(buffer.data);
			}
			snprintf(client->last_error, sizeof(client->last_error), "Directory listing failed: %s",
					 curl_easy_strerror(res));
			return FTP_ERROR_TRANSFER;
		}

		*output = buffer.data;
		return FTP_OK;
	}

	int ftp_client_mkdir(ftp_client_t *client, const char *remote_path)
	{
		if (!client || !client->curl || !remote_path)
		{
			return FTP_ERROR_INVALID_PARAM;
		}

		struct curl_slist *commands = NULL;
		char cmd[512];
		/* Ensure path is absolute (starts with /) */
		snprintf(cmd, sizeof(cmd), "MKD %s%s", (remote_path[0] == '/') ? "" : "/", remote_path);
		commands = curl_slist_append(commands, cmd);

		int result = ftp_client_execute_simple_command(client, commands, "Create directory failed");

		curl_slist_free_all(commands);
		return result;
	}

	int ftp_client_rmdir(ftp_client_t *client, const char *remote_path)
	{
		if (!client || !client->curl || !remote_path)
		{
			return FTP_ERROR_INVALID_PARAM;
		}

		struct curl_slist *commands = NULL;
		char cmd[512];
		/* Ensure path is absolute (starts with /) */
		snprintf(cmd, sizeof(cmd), "RMD %s%s", (remote_path[0] == '/') ? "" : "/", remote_path);
		commands = curl_slist_append(commands, cmd);

		int result = ftp_client_execute_simple_command(client, commands, "Remove directory failed");

		curl_slist_free_all(commands);
		return result;
	}

	int ftp_client_delete(ftp_client_t *client, const char *remote_path)
	{
		if (!client || !client->curl || !remote_path)
		{
			return FTP_ERROR_INVALID_PARAM;
		}

		struct curl_slist *commands = NULL;
		char cmd[512];
		/* Ensure path is absolute (starts with /) */
		snprintf(cmd, sizeof(cmd), "DELE %s%s", (remote_path[0] == '/') ? "" : "/", remote_path);
		commands = curl_slist_append(commands, cmd);

		int result = ftp_client_execute_simple_command(client, commands, "Delete file failed");

		curl_slist_free_all(commands);
		return result;
	}

	int ftp_client_rename(ftp_client_t *client, const char *old_path, const char *new_path)
	{
		if (!client || !client->curl || !old_path || !new_path)
		{
			return FTP_ERROR_INVALID_PARAM;
		}

		struct curl_slist *commands = NULL;
		char cmd1[512], cmd2[512];
		/* Ensure paths are absolute (start with /) */
		snprintf(cmd1, sizeof(cmd1), "RNFR %s%s", (old_path[0] == '/') ? "" : "/", old_path);
		snprintf(cmd2, sizeof(cmd2), "RNTO %s%s", (new_path[0] == '/') ? "" : "/", new_path);
		commands = curl_slist_append(commands, cmd1);
		commands = curl_slist_append(commands, cmd2);

		int result = ftp_client_execute_simple_command(client, commands, "Rename failed");

		curl_slist_free_all(commands);
		return result;
	}

	int ftp_client_get_filesize(ftp_client_t *client, const char *remote_path, int64_t *size)
	{
		if (!client || !client->curl || !remote_path || !size)
		{
			return FTP_ERROR_INVALID_PARAM;
		}

		/* Reset curl handle to default state */
		curl_easy_reset(client->curl);

		char url[FTP_MAX_URL_LENGTH];
		int result = build_ftp_url(client, remote_path, url, sizeof(url));
		if (result != FTP_OK)
		{
			snprintf(client->last_error, sizeof(client->last_error), "Remote path too long");
			return result;
		}

		curl_easy_setopt(client->curl, CURLOPT_URL, url);
		setup_curl_common(client);

		/* Use NOBODY to get file info without downloading content */
		curl_easy_setopt(client->curl, CURLOPT_NOBODY, 1L);
		curl_easy_setopt(client->curl, CURLOPT_FILETIME, 1L);
		curl_easy_setopt(client->curl, CURLOPT_HEADER, 1L);

		/* Provide write callback to discard any header data */
		ftp_memory_buffer_t buffer = {0};
		curl_easy_setopt(client->curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
		curl_easy_setopt(client->curl, CURLOPT_WRITEDATA, &buffer);

		CURLcode res = curl_easy_perform(client->curl);

		if (buffer.data)
		{
			free(buffer.data);
		}

		if (res != CURLE_OK)
		{
			snprintf(client->last_error, sizeof(client->last_error), "Get file size failed: %s",
					 curl_easy_strerror(res));
			return FTP_ERROR_TRANSFER;
		}

		/* Get file size from curl info */
		curl_off_t filesize;
		res = curl_easy_getinfo(client->curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &filesize);
		if (res == CURLE_OK && filesize >= 0)
		{
			*size = (int64_t)filesize;
			return FTP_OK;
		}

		snprintf(client->last_error, sizeof(client->last_error), "Could not retrieve file size");
		return FTP_ERROR_TRANSFER;
	}

	int ftp_client_execute_command(ftp_client_t *client, const char *command, char **response)
	{
		if (!client || !client->curl || !command)
		{
			return FTP_ERROR_INVALID_PARAM;
		}

		/* Reset curl handle to default state */
		curl_easy_reset(client->curl);

		char url[FTP_MAX_URL_LENGTH];
		int result = build_ftp_url(client, "/", url, sizeof(url));
		if (result != FTP_OK)
		{
			snprintf(client->last_error, sizeof(client->last_error), "URL too long");
			return result;
		}

		curl_easy_setopt(client->curl, CURLOPT_URL, url);
		setup_curl_common(client);

		struct curl_slist *commands = NULL;
		commands = curl_slist_append(commands, command);
		curl_easy_setopt(client->curl, CURLOPT_QUOTE, commands);

		ftp_memory_buffer_t buffer = {0};
		curl_easy_setopt(client->curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
		curl_easy_setopt(client->curl, CURLOPT_WRITEDATA, &buffer);

		CURLcode res = curl_easy_perform(client->curl);

		curl_slist_free_all(commands);

		if (res != CURLE_OK)
		{
			if (buffer.data)
			{
				free(buffer.data);
			}
			snprintf(client->last_error, sizeof(client->last_error), "Command execution failed: %s",
					 curl_easy_strerror(res));
			return FTP_ERROR_TRANSFER;
		}

		if (response)
		{
			*response = buffer.data;
		}
		else if (buffer.data)
		{
			free(buffer.data);
		}

		return FTP_OK;
	}

	const char *ftp_client_get_error(ftp_client_t *client)
	{
		if (!client)
		{
			return "Invalid client handle";
		}
		return client->last_error;
	}

	void ftp_client_destroy(ftp_client_t *client)
	{
		if (client)
		{
			if (client->curl)
			{
				curl_easy_cleanup(client->curl);
			}
			free(client);
		}
	}

#endif /* FTP_CLIENT_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#endif /* FTPCLIENT_H */

/*
 * ------------------------------------------------------------------------------
 * LICENSE
 * ------------------------------------------------------------------------------
 *
 * MIT License
 *
 * Copyright (c) 2025 Youssef Saeed (ysdragon)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ------------------------------------------------------------------------------
 */