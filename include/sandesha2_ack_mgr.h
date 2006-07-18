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
 
#ifndef SANDESHA2_ACK_MGR_H
#define SANDESHA2_ACK_MGR_H

/**
  * @file sandesha2_ack_mgr.h
  * @brief 
  */
#include <axis2_utils_defines.h>
#include <axis2_env.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_msg_ctx.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup sandesha2_ack_mgr
 * @ingroup sandesha2_util
 * @{
 */

AXIS2_EXTERN sandesha2_msg_ctx_t *AXIS2_CALL
sandesha2_ack_mgr_generate_ack_msg(const axis2_env_t *env,
                        sandesha2_msg_ctx_t *ref_rm_msg,
                        axis2_char_t *seq_id,
                        sandesha2_storage_mgr_t *storage_mgr);

/**
 * This is used to get the acked messages of a sequence. If this is an outgoing 
 * message the sequence_identifier should be the internal sequenceID.
 * 
 * @param sequence_identifier
 * @param out_going_msg
 * @return
 */
AXIS2_EXTERN axis2_array_list_t *AXIS2_CALL
sandesha2_ack_mgr_get_client_completed_msgs_list(
        const axis2_env_t *env,
        axis2_char_t *seq_id,
        sandesha2_seq_property_mgr_t *seq_prop_mgr);
 
AXIS2_EXTERN axis2_array_list_t *AXIS2_CALL
sandesha2_ack_mgr_get_svr_completed_msgs_list(
        const axis2_env_t *env,
        axis2_char_t *seq_id,
        sandesha2_seq_property_mgr_t *seq_prop_mgr);

/** @} */
#ifdef __cplusplus
}
#endif

#endif                          /* SANDESHA2_ACK_MGR_H */
