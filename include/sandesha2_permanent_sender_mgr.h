/*
 * Copyright 2004,2005 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SANDESHA2_PERMANENT_SENDER_MGR_H
#define SANDESHA2_PERMANENT_SENDER_MGR_H

/**
 * @file sandesha2_permanent_sender_mgr.h
 * @brief Sandesha Permanat Sender Manager Interface
 */

#include <axis2_allocator.h>
#include <axis2_env.h>
#include <axis2_error.h>
#include <axis2_string.h>
#include <axis2_utils.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct sandesha2_storage_mgr;
struct axis2_ctx;

/** @defgroup sandesha2_permanent_sender_mgr Permanat Sender Manager
  * @ingroup sandesha2
  * @{
  */

AXIS2_EXTERN struct sandesha2_sender_mgr * AXIS2_CALL
sandesha2_permanent_sender_mgr_create(
    const axis2_env_t *env,
    struct sandesha2_storage_mgr *storage_mgr,
    struct axis2_ctx *ctx);

/** @} */
#ifdef __cplusplus
}
#endif
#endif /* SANDESHA2_PERMANENT_SENDER_MGR_H */