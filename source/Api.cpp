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
#include <aws/crt/Api.h>
#include <aws/crt/StlAllocator.h>
#include <aws/crt/external/cJSON.h>
#include <aws/crt/io/TlsOptions.h>

#include <aws/http/http.h>

namespace Aws
{
    namespace Crt
    {
        Allocator *g_allocator = Aws::Crt::DefaultAllocator();

        static void *s_cJSONAlloc(size_t sz) { return aws_mem_acquire(g_allocator, sz); }

        static void s_cJSONFree(void *ptr) { return aws_mem_release(g_allocator, ptr); }

        static void s_initApi(Allocator *allocator)
        {
            // sets up the StlAllocator for use.
            g_allocator = allocator;
            Io::InitTlsStaticState(allocator);
            aws_http_library_init(allocator);
            aws_mqtt_library_init(allocator);

            cJSON_Hooks hooks;
            hooks.malloc_fn = s_cJSONAlloc;
            hooks.free_fn = s_cJSONFree;
            cJSON_InitHooks(&hooks);
        }

        static void s_cleanUpApi()
        {
            g_allocator = nullptr;
            aws_mqtt_library_clean_up();
            aws_http_library_clean_up();
            Io::CleanUpTlsStaticState();
        }

        ApiHandle::ApiHandle(Allocator *allocator) noexcept { s_initApi(allocator); }

        ApiHandle::ApiHandle() noexcept { s_initApi(DefaultAllocator()); }

        ApiHandle::~ApiHandle() { s_cleanUpApi(); }

        void LoadErrorStrings() noexcept
        {
            aws_load_error_strings();
            aws_io_load_error_strings();
        }

        const char *ErrorDebugString(int error) noexcept { return aws_error_debug_str(error); }

        int LastError() noexcept { return aws_last_error(); }

    } // namespace Crt
} // namespace Aws
