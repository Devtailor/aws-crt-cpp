/*
 * Copyright 2010-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
#include <aws/crt/io/TlsOptions.h>

#include <aws/io/tls_channel_handler.h>

namespace Aws
{
    namespace Crt
    {
        namespace Io
        {
            TlsContextOptions::~TlsContextOptions()
            {
                if (m_isInit)
                {
                    aws_tls_ctx_options_clean_up(&m_options);
                }
            }

            TlsContextOptions::TlsContextOptions() noexcept : m_isInit(false) { AWS_ZERO_STRUCT(m_options); }

            TlsContextOptions::TlsContextOptions(TlsContextOptions &&other) noexcept
            {
                m_options = other.m_options;
                m_isInit = other.m_isInit;
                AWS_ZERO_STRUCT(other.m_options);
                other.m_isInit = false;
            }

            TlsContextOptions &TlsContextOptions::operator=(TlsContextOptions &&other) noexcept
            {
                if (&other != this)
                {
                    if (m_isInit)
                    {
                        aws_tls_ctx_options_clean_up(&m_options);
                    }

                    m_options = other.m_options;
                    m_isInit = other.m_isInit;
                    AWS_ZERO_STRUCT(other.m_options);
                    other.m_isInit = false;
                }

                return *this;
            }

            TlsContextOptions TlsContextOptions::InitDefaultClient(Allocator *allocator) noexcept
            {
                TlsContextOptions ctxOptions;
                aws_tls_ctx_options_init_default_client(&ctxOptions.m_options, allocator);
                ctxOptions.m_isInit = true;
                return ctxOptions;
            }

            TlsContextOptions TlsContextOptions::InitClientWithMtls(
                const char *certPath,
                const char *pKeyPath,
                Allocator *allocator) noexcept
            {
                TlsContextOptions ctxOptions;
                if (!aws_tls_ctx_options_init_client_mtls_from_path(
                        &ctxOptions.m_options, allocator, certPath, pKeyPath))
                {
                    ctxOptions.m_isInit = true;
                }
                return ctxOptions;
            }

            TlsContextOptions TlsContextOptions::InitClientWithMtls(
                const ByteCursor &cert,
                const ByteCursor &pkey,
                Allocator *allocator) noexcept
            {
                TlsContextOptions ctxOptions;
                if (!aws_tls_ctx_options_init_client_mtls(
                        &ctxOptions.m_options,
                        allocator,
                        const_cast<ByteCursor *>(&cert),
                        const_cast<ByteCursor *>(&pkey)))
                {
                    ctxOptions.m_isInit = true;
                }
                return ctxOptions;
            }
#ifdef __APPLE__
            TlsContextOptions TlsContextOptions::InitClientWithMtlsPkcs12(
                const char *pkcs12Path,
                const char *pkcs12Pwd,
                Allocator *allocator) noexcept
            {
                TlsContextOptions ctxOptions;
                struct aws_byte_cursor password = aws_byte_cursor_from_c_str(pkcs12Pwd);
                if (!aws_tls_ctx_options_init_client_mtls_pkcs12_from_path(
                        &ctxOptions.m_options, allocator, pkcs12Path, &password))
                {
                    ctxOptions.m_isInit = true;
                }
                return ctxOptions;
            }
#endif

            bool TlsContextOptions::IsAlpnSupported() noexcept { return aws_tls_is_alpn_available(); }

            void TlsContextOptions::SetAlpnList(const char *alpn_list) noexcept
            {
                if (m_isInit)
                {
                    aws_tls_ctx_options_set_alpn_list(&m_options, alpn_list);
                }
            }

            void TlsContextOptions::SetVerifyPeer(bool verify_peer) noexcept
            {
                if (m_isInit)
                {
                    aws_tls_ctx_options_set_verify_peer(&m_options, verify_peer);
                }
            }

            void TlsContextOptions::OverrideDefaultTrustStore(const char *caPath, const char *caFile) noexcept
            {
                if (m_isInit)
                {
                    aws_tls_ctx_options_override_default_trust_store_from_path(&m_options, caPath, caFile);
                }
            }

            void TlsContextOptions::OverrideDefaultTrustStore(const ByteCursor &ca) noexcept
            {
                if (m_isInit)
                {
                    aws_tls_ctx_options_override_default_trust_store(&m_options, const_cast<ByteCursor *>(&ca));
                }
            }

            void InitTlsStaticState(Aws::Crt::Allocator *alloc) noexcept { aws_tls_init_static_state(alloc); }

            void CleanUpTlsStaticState() noexcept { aws_tls_clean_up_static_state(); }

            TlsConnectionOptions::TlsConnectionOptions() noexcept : m_lastError(AWS_ERROR_SUCCESS), m_isInit(false) {}

            TlsConnectionOptions::TlsConnectionOptions(aws_tls_ctx *ctx, Allocator *allocator) noexcept
                : m_allocator(allocator), m_lastError(AWS_ERROR_SUCCESS), m_isInit(true)
            {
                aws_tls_connection_options_init_from_ctx(&m_tls_connection_options, ctx);
            }

            TlsConnectionOptions::~TlsConnectionOptions()
            {
                if (m_isInit)
                {
                    aws_tls_connection_options_clean_up(&m_tls_connection_options);
                    m_isInit = false;
                }
            }

            TlsConnectionOptions::TlsConnectionOptions(const TlsConnectionOptions &options) noexcept
            {
                m_isInit = false;

                if (options.m_isInit)
                {
                    m_allocator = options.m_allocator;
                    if (!aws_tls_connection_options_copy(&m_tls_connection_options, &options.m_tls_connection_options))
                    {
                        m_isInit = true;
                    }
                    else
                    {
                        m_lastError = aws_last_error();
                    }
                }
            }

            TlsConnectionOptions &TlsConnectionOptions::operator=(const TlsConnectionOptions &options) noexcept
            {
                if (this != &options)
                {
                    if (m_isInit)
                    {
                        aws_tls_connection_options_clean_up(&m_tls_connection_options);
                    }

                    m_isInit = false;

                    if (options.m_isInit)
                    {
                        m_allocator = options.m_allocator;
                        if (!aws_tls_connection_options_copy(
                                &m_tls_connection_options, &options.m_tls_connection_options))
                        {
                            m_isInit = true;
                        }
                        else
                        {
                            m_lastError = aws_last_error();
                        }
                    }
                }

                return *this;
            }

            TlsConnectionOptions::TlsConnectionOptions(TlsConnectionOptions &&options) noexcept
                : m_isInit(options.m_isInit)
            {
                if (options.m_isInit)
                {
                    m_tls_connection_options = options.m_tls_connection_options;
                    AWS_ZERO_STRUCT(options.m_tls_connection_options);
                    options.m_isInit = false;
                    options.m_allocator = options.m_allocator;
                }
            }

            TlsConnectionOptions &TlsConnectionOptions::operator=(TlsConnectionOptions &&options) noexcept
            {
                if (this != &options)
                {
                    if (options.m_isInit)
                    {
                        m_tls_connection_options = options.m_tls_connection_options;
                        AWS_ZERO_STRUCT(options.m_tls_connection_options);
                        options.m_isInit = false;
                        m_isInit = true;
                        options.m_allocator = options.m_allocator;
                    }
                }

                return *this;
            }

            bool TlsConnectionOptions::SetServerName(ByteCursor &serverName) noexcept
            {
                if (aws_tls_connection_options_set_server_name(&m_tls_connection_options, m_allocator, &serverName))
                {
                    m_lastError = aws_last_error();
                    return false;
                }

                return true;
            }

            bool TlsConnectionOptions::SetAlpnList(const char *alpnList) noexcept
            {
                if (aws_tls_connection_options_set_alpn_list(&m_tls_connection_options, m_allocator, alpnList))
                {
                    m_lastError = aws_last_error();
                    return false;
                }

                return true;
            }

            TlsContext::TlsContext() noexcept : m_ctx(nullptr), m_lastError(AWS_ERROR_SUCCESS) {}

            TlsContext::TlsContext(TlsContextOptions &options, TlsMode mode, Allocator *allocator) noexcept
                : m_ctx(nullptr), m_lastError(AWS_ERROR_SUCCESS)
            {
                if (mode == TlsMode::CLIENT)
                {
                    m_ctx = aws_tls_client_ctx_new(allocator, &options.m_options);
                }
                else
                {
                    m_ctx = aws_tls_server_ctx_new(allocator, &options.m_options);
                }

                if (!m_ctx)
                {
                    m_lastError = aws_last_error();
                }
            }

            TlsContext::~TlsContext()
            {
                if (*this)
                {
                    aws_tls_ctx_destroy(m_ctx);
                }
            }

            TlsContext::TlsContext(TlsContext &&toMove) noexcept : m_ctx(toMove.m_ctx), m_lastError(toMove.m_lastError)
            {
                toMove.m_ctx = nullptr;
                toMove.m_lastError = AWS_ERROR_UNKNOWN;
            }

            TlsContext &TlsContext::operator=(TlsContext &&toMove) noexcept
            {
                if (this == &toMove)
                {
                    return *this;
                }

                m_ctx = toMove.m_ctx;
                m_lastError = toMove.m_lastError;
                toMove.m_ctx = nullptr;
                toMove.m_lastError = AWS_ERROR_UNKNOWN;

                return *this;
            }

            TlsContext::operator bool() const noexcept { return m_ctx && m_lastError == AWS_ERROR_SUCCESS; }

            int TlsContext::LastError() const noexcept { return m_lastError; }

            TlsConnectionOptions TlsContext::NewConnectionOptions() const noexcept
            {
                return TlsConnectionOptions(m_ctx, m_ctx->alloc);
            }
        } // namespace Io
    }     // namespace Crt
} // namespace Aws
