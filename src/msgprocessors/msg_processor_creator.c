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
 
#include <sandesha2_msg_processor.h>
#include <sandesha2_seq_ack.h>
#include <sandesha2_ack_requested.h>
#include <sandesha2_create_seq.h>
#include <sandesha2_spec_specific_consts.h>
#include <sandesha2_constants.h>
#include <sandesha2_utils.h>
#include <sandesha2_msg_ctx.h>
#include <sandesha2_create_seq.h>
#include <sandesha2_create_seq_res.h>
#include <sandesha2_terminate_seq.h>
#include <sandesha2_terminate_seq_res.h>
#include <sandesha2_ack_requested.h>
#include <sandesha2_close_seq.h>
#include <sandesha2_close_seq_res.h>
#include <sandesha2_create_seq_msg_processor.h>
#include <axis2_string.h>
#include <stdio.h>
/** 
 * @brief Application Message Processor Creator struct impl
 *	Sandesha2 App Msg Processor Creator
 */
sandesha2_msg_processor_t *AXIS2_CALL   
sandesha2_msg_processor_create_msg_processor(
        const axis2_env_t *env,
        sandesha2_msg_ctx_t *rm_msg_ctx)
{
    int msg_type = -1;

    msg_type = SANDESHA2_MSG_CTX_GET_MSG_TYPE(rm_msg_ctx, env);
    printf("msg_type2:%d\n", msg_type);
    switch(msg_type)
    {
        case SANDESHA2_MSG_TYPE_CREATE_SEQ:
            return (sandesha2_msg_processor_t *) sandesha2_create_seq_msg_processor_create(env);
        case SANDESHA2_MSG_TYPE_TERMINATE_SEQ:
            return (sandesha2_msg_processor_t *) sandesha2_terminate_seq_msg_processor_create(env);
        case SANDESHA2_MSG_TYPE_TERMINATE_SEQ_RESPONSE:
            return (sandesha2_msg_processor_t *) sandesha2_terminate_seq_res_msg_processor_create(env);
        case SANDESHA2_MSG_TYPE_APPLICATION:
            return (sandesha2_msg_processor_t *) sandesha2_app_msg_processor_create(env);
        case SANDESHA2_MSG_TYPE_CREATE_SEQ_RESPONSE:
            return (sandesha2_msg_processor_t *) sandesha2_create_seq_res_msg_processor_create(env);
        case SANDESHA2_MSG_TYPE_ACK:
            return (sandesha2_msg_processor_t *) sandesha2_ack_msg_processor_create(env);
        case SANDESHA2_MSG_TYPE_CLOSE_SEQ:
            return (sandesha2_msg_processor_t *) sandesha2_close_seq_msg_processor_create(env);
        case SANDESHA2_MSG_TYPE_ACK_REQUEST:
            return (sandesha2_msg_processor_t *) sandesha2_ack_req_msg_processor_create(env);
        default :
            return NULL;
    }
    return NULL;
}
