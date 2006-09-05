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

#ifndef SANDESHA2_MSG_RETRANS_ADJUSTER_H
#define SANDESHA2_MSG_RETRANS_ADJUSTER_H

/**
 * @file sandesha2_msg_retrans_adjuster.h
 * @brief Sandesha Message Retransmission Adjuster
 */

#include <axis2_allocator.h>
#include <axis2_env.h>
#include <axis2_error.h>
#include <axis2_string.h>
#include <axis2_utils.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_sender_bean.h>
#include <axis2_conf_ctx.h>


#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup sandesha2_msg_retrans_adjuster 
  * @ingroup sandesha2_utils
  * @{
  */

AXIS2_EXTERN axis2_bool_t AXIS2_CALL
sandesha2_msg_retrans_adjuster_adjust_retrans(
        const axis2_env_t *env,
        sandesha2_sender_bean_t *retrans_bean,
        axis2_conf_ctx_t *conf_ctx, 
        sandesha2_storage_mgr_t *storage_mgr);


/** @} */
#ifdef __cplusplus
}
#endif
#endif /* SANDESHA2_MSG_RETRANS_ADJUSTER_H */