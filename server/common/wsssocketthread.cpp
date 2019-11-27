#include "pch.h"
#include "wsssocketthread.h"
#include "common_api.h"
#include "log.h"
#ifndef WIN32
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif
WSSSocketThread::WSSSocketThread()
{

}

WSSSocketThread::~WSSSocketThread()
{
	ERR_free_strings();
	if(m_ctx != nullptr)
		SSL_CTX_free(m_ctx);
}

bool WSSSocketThread::Init()
{
	SSL_library_init();
	SSL_load_error_strings();
	m_ctx = SSL_CTX_new(SSLv23_method());//SSLv23_server_method or SSLv23_client_method
	if (nullptr == m_ctx)
	{
		WRITE_ERROR_LOG("SSL_CTX_new error");
		return false;
	}
	if(m_clientMethod)
		return CSocketThread::Init();
	SSL_CTX_set_verify(m_ctx, SSL_VERIFY_NONE, nullptr);
	if (1 != SSL_CTX_use_certificate_file(m_ctx, m_cert.data(), SSL_FILETYPE_PEM))
	{
		WRITE_ERROR_LOG("SSL_CTX_use_certificate_file error");
		return false;
	}
	if (1 != SSL_CTX_use_certificate_chain_file(m_ctx, m_cert.data()))
	{
		WRITE_ERROR_LOG("SSL_CTX_use_certificate_chain_file error");
		return false;
	}
	if (1 != SSL_CTX_use_PrivateKey_file(m_ctx, m_key.data(), SSL_FILETYPE_PEM))
	{
		WRITE_ERROR_LOG("SSL_CTX_use_PrivateKey_file error");
		return false;
	}
	if (1 != SSL_CTX_check_private_key(m_ctx))
	{
		WRITE_ERROR_LOG("SSL_CTX_check_private_key error");
		return false;
	}
	return CSocketThread::Init();
}

SSL* WSSSocketThread::GetNewSSL()
{
	return SSL_new(m_ctx);
}