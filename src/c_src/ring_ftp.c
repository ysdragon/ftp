/*
 * Ring FTP Extension
 *
 * Copyright (c) 2025 Youssef Saeed <youssefelkholey@gmail.com>
 *
 * License: MIT
 */

#include "ring.h"
#define FTP_CLIENT_IMPLEMENTATION
#include "ftpclient.h"

#define RING_FTP_POINTER "ring_ftp_t*"

typedef struct RingFTPProgressData
{
	VM *pVM;
	char *cFuncName;
} ring_ftp_progress_data_t;

static char *ring_ftp_string_strdup(void *pState, const char *cStr)
{
	char *cString;
	unsigned int nSize = strlen(cStr);
	cString = (char *)ring_state_malloc(pState, nSize + 1);
	memcpy(cString, cStr, nSize);
	cString[nSize] = '\0';
	return cString;
}

void ring_ftp_progress_data_free(void *pState, void *pPointer)
{
	ring_ftp_progress_data_t *pData = (ring_ftp_progress_data_t *)pPointer;
	if (pData)
	{
		if (pData->cFuncName)
		{
			ring_state_free(pState, pData->cFuncName);
			pData->cFuncName = NULL;
		}
		ring_state_free(pState, pPointer);
	}
}

void ring_ftp_client_free(void *pState, void *pPointer)
{
	ftp_client_t *client = (ftp_client_t *)pPointer;
	if (client)
	{

		if (client->config.progress_user_data)
		{
			ring_ftp_progress_data_free(pState, client->config.progress_user_data);
			client->config.progress_user_data = NULL;
		}

		ftp_client_destroy(client);
	}
}

int ring_ftp_progress_callback(void *user_data, double download_total, double download_now, double upload_total,
							   double upload_now)
{
	ring_ftp_progress_data_t *pData = (ring_ftp_progress_data_t *)user_data;

	if (!pData || !pData->pVM || !pData->cFuncName)
	{
		return 0;
	}

	VM *pVM = pData->pVM;
	int nResult = 0;

	if (!pVM->pRingState)
	{
		return 0;
	}

	ring_vm_mutexlock(pVM);

	int nSP_before = pVM->nSP;
	int nFuncSP_before = pVM->nFuncSP;
	int nCallListSize_before = RING_VM_FUNCCALLSCOUNT;

	if (!ring_vm_loadfunc2(pVM, pData->cFuncName, RING_FALSE))
	{

		pVM->nSP = nSP_before;
		pVM->nFuncSP = nFuncSP_before;
		ring_vm_mutexunlock(pVM);
		return 0;
	}

	RING_VM_STACK_PUSHNVALUE(download_total);
	RING_VM_STACK_PUSHNVALUE(download_now);
	RING_VM_STACK_PUSHNVALUE(upload_total);
	RING_VM_STACK_PUSHNVALUE(upload_now);

	ring_vm_call2(pVM);

	while (RING_VM_FUNCCALLSCOUNT > nCallListSize_before)
	{
		ring_vm_fetch(pVM);
		if (pVM->lActiveError)
		{
			break;
		}
	}

	if (pVM->lActiveError)
	{
		ring_vm_error(pVM, "Error occurred within the FTP progress callback function.");
		pVM->lActiveError = 0;
		nResult = 0;
	}
	else if (pVM->nSP > nSP_before && RING_VM_STACK_ISNUMBER)
	{
		nResult = (int)RING_VM_STACK_READN;
	}

	pVM->nSP = nSP_before;
	pVM->nFuncSP = nFuncSP_before;

	ring_vm_mutexunlock(pVM);

	return nResult;
}

/*
 * FTP Functions
 */

RING_FUNC(ring_ftp_global_init)
{
	if (RING_API_PARACOUNT != 0)
	{
		RING_API_ERROR(RING_API_BADPARACOUNT);
		return;
	}
	RING_API_RETNUMBER(ftp_global_init());
}

RING_FUNC(ring_ftp_global_cleanup)
{
	if (RING_API_PARACOUNT != 0)
	{
		RING_API_ERROR(RING_API_BADPARACOUNT);
		return;
	}
	ftp_global_cleanup();
}

RING_FUNC(ring_ftp_client_create)
{
	if (RING_API_PARACOUNT != 0)
	{
		RING_API_ERROR(RING_API_BADPARACOUNT);
		return;
	}
	ftp_client_t *client = ftp_client_create();
	if (client)
	{
		RING_API_RETMANAGEDCPOINTER(client, RING_FTP_POINTER, ring_ftp_client_free);
	}
	else
	{
		RING_API_RETSTRING("");
	}
}

RING_FUNC(ring_ftp_client_set_host)
{
	if (RING_API_PARACOUNT != 3 || !RING_API_ISPOINTER(1) || !RING_API_ISSTRING(2) || !RING_API_ISNUMBER(3))
	{
		RING_API_ERROR("Usage: ftp_client_set_host(client_ptr, host_string, port_number)");
		return;
	}
	ftp_client_t *client = (ftp_client_t *)RING_API_GETCPOINTER(1, RING_FTP_POINTER);
	if (!client)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}
	RING_API_RETNUMBER(ftp_client_set_host(client, RING_API_GETSTRING(2), (int)RING_API_GETNUMBER(3)));
}

RING_FUNC(ring_ftp_client_set_credentials)
{
	if (RING_API_PARACOUNT != 3 || !RING_API_ISPOINTER(1) || !RING_API_ISSTRING(2) || !RING_API_ISSTRING(3))
	{
		RING_API_ERROR("Usage: ftp_client_set_credentials(client_ptr, username_string, password_string)");
		return;
	}
	ftp_client_t *client = (ftp_client_t *)RING_API_GETCPOINTER(1, RING_FTP_POINTER);
	if (!client)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}
	RING_API_RETNUMBER(ftp_client_set_credentials(client, RING_API_GETSTRING(2), RING_API_GETSTRING(3)));
}

RING_FUNC(ring_ftp_client_set_mode)
{
	if (RING_API_PARACOUNT != 2 || !RING_API_ISPOINTER(1) || !RING_API_ISNUMBER(2))
	{
		RING_API_ERROR("Usage: ftp_client_set_mode(client_ptr, mode_number)");
		return;
	}
	ftp_client_t *client = (ftp_client_t *)RING_API_GETCPOINTER(1, RING_FTP_POINTER);
	if (!client)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}
	ftp_client_set_mode(client, (ftp_mode_t)(int)RING_API_GETNUMBER(2));
}

RING_FUNC(ring_ftp_client_set_ssl)
{
	if (RING_API_PARACOUNT != 3 || !RING_API_ISPOINTER(1) || !RING_API_ISNUMBER(2) || !RING_API_ISNUMBER(3))
	{
		RING_API_ERROR("Usage: ftp_client_set_ssl(client_ptr, ssl_mode_number, verify_number)");
		return;
	}
	ftp_client_t *client = (ftp_client_t *)RING_API_GETCPOINTER(1, RING_FTP_POINTER);
	if (!client)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}
	ftp_client_set_ssl(client, (ftp_ssl_mode_t)(int)RING_API_GETNUMBER(2), (int)RING_API_GETNUMBER(3));
}

RING_FUNC(ring_ftp_client_set_timeout)
{
	if (RING_API_PARACOUNT != 3 || !RING_API_ISPOINTER(1) || !RING_API_ISNUMBER(2) || !RING_API_ISNUMBER(3))
	{
		RING_API_ERROR("Usage: ftp_client_set_timeout(client_ptr, timeout_seconds, connect_timeout_seconds)");
		return;
	}
	ftp_client_t *client = (ftp_client_t *)RING_API_GETCPOINTER(1, RING_FTP_POINTER);
	if (!client)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}
	ftp_client_set_timeout(client, (long)RING_API_GETNUMBER(2), (long)RING_API_GETNUMBER(3));
}

RING_FUNC(ring_ftp_client_set_verbose)
{
	if (RING_API_PARACOUNT != 2 || !RING_API_ISPOINTER(1) || !RING_API_ISNUMBER(2))
	{
		RING_API_ERROR("Usage: ftp_client_set_verbose(client_ptr, verbose_flag_number)");
		return;
	}
	ftp_client_t *client = (ftp_client_t *)RING_API_GETCPOINTER(1, RING_FTP_POINTER);
	if (!client)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}
	ftp_client_set_verbose(client, (int)RING_API_GETNUMBER(2));
}

RING_FUNC(ring_ftp_client_set_progress_callback)
{
	if (RING_API_PARACOUNT != 2 || !RING_API_ISPOINTER(1) || !RING_API_ISSTRING(2))
	{
		RING_API_ERROR("Usage: ftp_client_set_progress_callback(client_ptr, function_name_string)");
		return;
	}

	ftp_client_t *client = (ftp_client_t *)RING_API_GETCPOINTER(1, RING_FTP_POINTER);
	const char *funcName = RING_API_GETSTRING(2);

	if (!client)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}

	if (client->config.progress_user_data)
	{
		ring_ftp_progress_data_free(RING_API_STATE, client->config.progress_user_data);
		client->config.progress_user_data = NULL;
	}

	if (strlen(funcName) == 0)
	{
		ftp_client_set_progress_callback(client, NULL, NULL);
		return;
	}

	ring_ftp_progress_data_t *pData = (ring_ftp_progress_data_t *)RING_API_MALLOC(sizeof(ring_ftp_progress_data_t));
	if (!pData)
	{
		RING_API_ERROR(RING_OOM);
		return;
	}

	pData->pVM = (VM *)pPointer;
	pData->cFuncName = ring_ftp_string_strdup(RING_API_STATE, funcName);
	if (!pData->cFuncName)
	{
		RING_API_FREE(pData);
		RING_API_ERROR(RING_OOM);
		return;
	}

	ftp_client_set_progress_callback(client, ring_ftp_progress_callback, pData);
}

RING_FUNC(ring_ftp_client_clear_progress_callback)
{
	if (RING_API_PARACOUNT != 1 || !RING_API_ISPOINTER(1))
	{
		RING_API_ERROR("Usage: ftp_client_clear_progress_callback(client_ptr)");
		return;
	}
	ftp_client_t *client = (ftp_client_t *)RING_API_GETCPOINTER(1, RING_FTP_POINTER);
	if (!client)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}

	if (client->config.progress_user_data)
	{
		ring_ftp_progress_data_free(RING_API_STATE, client->config.progress_user_data);
		client->config.progress_user_data = NULL;
	}

	ftp_client_set_progress_callback(client, NULL, NULL);
}

RING_FUNC(ring_ftp_client_connect)
{
	if (RING_API_PARACOUNT != 1 || !RING_API_ISPOINTER(1))
	{
		RING_API_ERROR("Usage: ftp_client_connect(client_ptr)");
		return;
	}
	ftp_client_t *client = (ftp_client_t *)RING_API_GETCPOINTER(1, RING_FTP_POINTER);
	if (!client)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}
	RING_API_RETNUMBER(ftp_client_connect(client));
}

RING_FUNC(ring_ftp_client_upload)
{
	if (RING_API_PARACOUNT != 3 || !RING_API_ISPOINTER(1) || !RING_API_ISSTRING(2) || !RING_API_ISSTRING(3))
	{
		RING_API_ERROR("Usage: ftp_client_upload(client_ptr, local_path, remote_path)");
		return;
	}
	ftp_client_t *client = (ftp_client_t *)RING_API_GETCPOINTER(1, RING_FTP_POINTER);
	if (!client)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}
	RING_API_RETNUMBER(ftp_client_upload(client, RING_API_GETSTRING(2), RING_API_GETSTRING(3)));
}

RING_FUNC(ring_ftp_client_download)
{
	if (RING_API_PARACOUNT != 3 || !RING_API_ISPOINTER(1) || !RING_API_ISSTRING(2) || !RING_API_ISSTRING(3))
	{
		RING_API_ERROR("Usage: ftp_client_download(client_ptr, remote_path, local_path)");
		return;
	}
	ftp_client_t *client = (ftp_client_t *)RING_API_GETCPOINTER(1, RING_FTP_POINTER);
	if (!client)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}
	RING_API_RETNUMBER(ftp_client_download(client, RING_API_GETSTRING(2), RING_API_GETSTRING(3)));
}

RING_FUNC(ring_ftp_client_list_dir)
{
	char *output = NULL;
	if (RING_API_PARACOUNT != 2 || !RING_API_ISPOINTER(1) || !RING_API_ISSTRING(2))
	{
		RING_API_ERROR("Usage: ftp_client_list_dir(client_ptr, remote_path)");
		return;
	}
	ftp_client_t *client = (ftp_client_t *)RING_API_GETCPOINTER(1, RING_FTP_POINTER);
	if (!client)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}

	int result = ftp_client_list_dir(client, RING_API_GETSTRING(2), &output);
	if (result == FTP_OK && output)
	{
		RING_API_RETSTRING(output);
		free(output);
	}
	else
	{
		RING_API_RETSTRING("");
	}
}

RING_FUNC(ring_ftp_client_mkdir)
{
	if (RING_API_PARACOUNT != 2 || !RING_API_ISPOINTER(1) || !RING_API_ISSTRING(2))
	{
		RING_API_ERROR("Usage: ftp_client_mkdir(client_ptr, remote_path)");
		return;
	}
	ftp_client_t *client = (ftp_client_t *)RING_API_GETCPOINTER(1, RING_FTP_POINTER);
	if (!client)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}
	RING_API_RETNUMBER(ftp_client_mkdir(client, RING_API_GETSTRING(2)));
}

RING_FUNC(ring_ftp_client_rmdir)
{
	if (RING_API_PARACOUNT != 2 || !RING_API_ISPOINTER(1) || !RING_API_ISSTRING(2))
	{
		RING_API_ERROR("Usage: ftp_client_rmdir(client_ptr, remote_path)");
		return;
	}
	ftp_client_t *client = (ftp_client_t *)RING_API_GETCPOINTER(1, RING_FTP_POINTER);
	if (!client)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}
	RING_API_RETNUMBER(ftp_client_rmdir(client, RING_API_GETSTRING(2)));
}

RING_FUNC(ring_ftp_client_delete)
{
	if (RING_API_PARACOUNT != 2 || !RING_API_ISPOINTER(1) || !RING_API_ISSTRING(2))
	{
		RING_API_ERROR("Usage: ftp_client_delete(client_ptr, remote_path)");
		return;
	}
	ftp_client_t *client = (ftp_client_t *)RING_API_GETCPOINTER(1, RING_FTP_POINTER);
	if (!client)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}
	RING_API_RETNUMBER(ftp_client_delete(client, RING_API_GETSTRING(2)));
}

RING_FUNC(ring_ftp_client_rename)
{
	if (RING_API_PARACOUNT != 3 || !RING_API_ISPOINTER(1) || !RING_API_ISSTRING(2) || !RING_API_ISSTRING(3))
	{
		RING_API_ERROR("Usage: ftp_client_rename(client_ptr, old_path, new_path)");
		return;
	}
	ftp_client_t *client = (ftp_client_t *)RING_API_GETCPOINTER(1, RING_FTP_POINTER);
	if (!client)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}
	RING_API_RETNUMBER(ftp_client_rename(client, RING_API_GETSTRING(2), RING_API_GETSTRING(3)));
}

RING_FUNC(ring_ftp_client_get_filesize)
{
	int64_t size = 0;
	if (RING_API_PARACOUNT != 2 || !RING_API_ISPOINTER(1) || !RING_API_ISSTRING(2))
	{
		RING_API_ERROR("Usage: ftp_client_get_filesize(client_ptr, remote_path)");
		return;
	}
	ftp_client_t *client = (ftp_client_t *)RING_API_GETCPOINTER(1, RING_FTP_POINTER);
	if (!client)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}

	int result = ftp_client_get_filesize(client, RING_API_GETSTRING(2), &size);
	if (result == FTP_OK)
	{
		RING_API_RETNUMBER((double)size);
	}
	else
	{
		RING_API_RETNUMBER(-1);
	}
}

RING_FUNC(ring_ftp_client_execute_command)
{
	char *response = NULL;
	if (RING_API_PARACOUNT != 2 || !RING_API_ISPOINTER(1) || !RING_API_ISSTRING(2))
	{
		RING_API_ERROR("Usage: ftp_client_execute_command(client_ptr, command)");
		return;
	}
	ftp_client_t *client = (ftp_client_t *)RING_API_GETCPOINTER(1, RING_FTP_POINTER);
	if (!client)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}

	int result = ftp_client_execute_command(client, RING_API_GETSTRING(2), &response);
	if (result == FTP_OK && response)
	{
		RING_API_RETSTRING(response);
		free(response);
	}
	else
	{
		RING_API_RETSTRING("");
	}
}

RING_FUNC(ring_ftp_client_get_error)
{
	if (RING_API_PARACOUNT != 1 || !RING_API_ISPOINTER(1))
	{
		RING_API_ERROR("Usage: ftp_client_get_error(client_ptr)");
		return;
	}
	ftp_client_t *client = (ftp_client_t *)RING_API_GETCPOINTER(1, RING_FTP_POINTER);
	if (!client)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}
	RING_API_RETSTRING(ftp_client_get_error(client));
}

RING_FUNC(ring_ftp_client_destroy)
{
	if (RING_API_PARACOUNT != 1 || !RING_API_ISPOINTER(1))
	{
		RING_API_ERROR("Usage: ftp_client_destroy(client_ptr)");
		return;
	}
	ftp_client_t *client = (ftp_client_t *)RING_API_GETCPOINTER(1, RING_FTP_POINTER);
	if (client)
	{

		ftp_client_destroy(client);
		RING_API_SETNULLPOINTER(1);
	}
}

/*
 * FTP Constants
 */

RING_FUNC(ring_get_ftp_ok)
{
	RING_API_RETNUMBER(FTP_OK);
}

RING_FUNC(ring_get_ftp_error_init)
{
	RING_API_RETNUMBER(FTP_ERROR_INIT);
}

RING_FUNC(ring_get_ftp_error_connection)
{
	RING_API_RETNUMBER(FTP_ERROR_CONNECTION);
}

RING_FUNC(ring_get_ftp_error_auth)
{
	RING_API_RETNUMBER(FTP_ERROR_AUTH);
}

RING_FUNC(ring_get_ftp_error_transfer)
{
	RING_API_RETNUMBER(FTP_ERROR_TRANSFER);
}

RING_FUNC(ring_get_ftp_error_file_not_found)
{
	RING_API_RETNUMBER(FTP_ERROR_FILE_NOT_FOUND);
}

RING_FUNC(ring_get_ftp_error_memory)
{
	RING_API_RETNUMBER(FTP_ERROR_MEMORY);
}

RING_FUNC(ring_get_ftp_error_invalid_param)
{
	RING_API_RETNUMBER(FTP_ERROR_INVALID_PARAM);
}

RING_FUNC(ring_get_ftp_error_curl)
{
	RING_API_RETNUMBER(FTP_ERROR_CURL);
}

RING_FUNC(ring_get_ftp_error_file_io)
{
	RING_API_RETNUMBER(FTP_ERROR_FILE_IO);
}

RING_FUNC(ring_get_ftp_error_timeout)
{
	RING_API_RETNUMBER(FTP_ERROR_TIMEOUT);
}

RING_FUNC(ring_get_ftp_mode_passive)
{
	RING_API_RETNUMBER(FTP_MODE_PASSIVE);
}

RING_FUNC(ring_get_ftp_mode_active)
{
	RING_API_RETNUMBER(FTP_MODE_ACTIVE);
}

RING_FUNC(ring_get_ftp_ssl_none)
{
	RING_API_RETNUMBER(FTP_SSL_NONE);
}

RING_FUNC(ring_get_ftp_ssl_try)
{
	RING_API_RETNUMBER(FTP_SSL_TRY);
}

RING_FUNC(ring_get_ftp_ssl_control)
{
	RING_API_RETNUMBER(FTP_SSL_CONTROL);
}

RING_FUNC(ring_get_ftp_ssl_all)
{
	RING_API_RETNUMBER(FTP_SSL_ALL);
}

RING_LIBINIT
{

	RING_API_REGISTER("ftp_global_init", ring_ftp_global_init);
	RING_API_REGISTER("ftp_global_cleanup", ring_ftp_global_cleanup);

	RING_API_REGISTER("ftp_client_create", ring_ftp_client_create);
	RING_API_REGISTER("ftp_client_destroy", ring_ftp_client_destroy);

	RING_API_REGISTER("ftp_client_set_host", ring_ftp_client_set_host);
	RING_API_REGISTER("ftp_client_set_credentials", ring_ftp_client_set_credentials);
	RING_API_REGISTER("ftp_client_set_mode", ring_ftp_client_set_mode);
	RING_API_REGISTER("ftp_client_set_ssl", ring_ftp_client_set_ssl);
	RING_API_REGISTER("ftp_client_set_timeout", ring_ftp_client_set_timeout);
	RING_API_REGISTER("ftp_client_set_verbose", ring_ftp_client_set_verbose);
	RING_API_REGISTER("ftp_client_set_progress_callback", ring_ftp_client_set_progress_callback);
	RING_API_REGISTER("ftp_client_clear_progress_callback", ring_ftp_client_clear_progress_callback);

	RING_API_REGISTER("ftp_client_connect", ring_ftp_client_connect);
	RING_API_REGISTER("ftp_client_upload", ring_ftp_client_upload);
	RING_API_REGISTER("ftp_client_download", ring_ftp_client_download);
	RING_API_REGISTER("ftp_client_list_dir", ring_ftp_client_list_dir);
	RING_API_REGISTER("ftp_client_mkdir", ring_ftp_client_mkdir);
	RING_API_REGISTER("ftp_client_rmdir", ring_ftp_client_rmdir);
	RING_API_REGISTER("ftp_client_delete", ring_ftp_client_delete);
	RING_API_REGISTER("ftp_client_rename", ring_ftp_client_rename);
	RING_API_REGISTER("ftp_client_get_filesize", ring_ftp_client_get_filesize);
	RING_API_REGISTER("ftp_client_execute_command", ring_ftp_client_execute_command);
	RING_API_REGISTER("ftp_client_get_error", ring_ftp_client_get_error);

	RING_API_REGISTER("get_ftp_ok", ring_get_ftp_ok);
	RING_API_REGISTER("get_ftp_error_init", ring_get_ftp_error_init);
	RING_API_REGISTER("get_ftp_error_connection", ring_get_ftp_error_connection);
	RING_API_REGISTER("get_ftp_error_auth", ring_get_ftp_error_auth);
	RING_API_REGISTER("get_ftp_error_transfer", ring_get_ftp_error_transfer);
	RING_API_REGISTER("get_ftp_error_file_not_found", ring_get_ftp_error_file_not_found);
	RING_API_REGISTER("get_ftp_error_memory", ring_get_ftp_error_memory);
	RING_API_REGISTER("get_ftp_error_invalid_param", ring_get_ftp_error_invalid_param);
	RING_API_REGISTER("get_ftp_error_curl", ring_get_ftp_error_curl);
	RING_API_REGISTER("get_ftp_error_file_io", ring_get_ftp_error_file_io);
	RING_API_REGISTER("get_ftp_error_timeout", ring_get_ftp_error_timeout);
	RING_API_REGISTER("get_ftp_mode_passive", ring_get_ftp_mode_passive);
	RING_API_REGISTER("get_ftp_mode_active", ring_get_ftp_mode_active);
	RING_API_REGISTER("get_ftp_ssl_none", ring_get_ftp_ssl_none);
	RING_API_REGISTER("get_ftp_ssl_try", ring_get_ftp_ssl_try);
	RING_API_REGISTER("get_ftp_ssl_control", ring_get_ftp_ssl_control);
	RING_API_REGISTER("get_ftp_ssl_all", ring_get_ftp_ssl_all);
}