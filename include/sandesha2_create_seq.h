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
 
#ifndef SANDESHA2_CREATE_SEQ_H
#define SANDESHA2_CREATE_SEQ_H

/**
  * @file sandesha2_create_seq.h
  * @brief 
  */

#include <axis2_utils_defines.h>
#include <axis2_env.h>
#include <sandesha2_iom_rm_part.h>
#include <sandesha2_error.h>
#include <sandesha2_acks_to.h>
#include <sandesha2_expires.h>
#include <sandesha2_seq_offer.h>


#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup sandesha2_create_seq
 * @ingroup sandesha2_wsrm
 * @{
 */
    
typedef struct sandesha2_create_seq sandesha2_create_seq_t;
 
/**
 * @brief sandesha2_create_seq
 *    sandesha2_create_seq
 */
AXIS2_DECLARE_DATA struct sandesha2_create_seq
{
    sandesha2_iom_rm_part_t part;
};

AXIS2_EXTERN sandesha2_create_seq_t* AXIS2_CALL
sandesha2_create_seq_create(
    const axis2_env_t *env, 
    axis2_char_t *addr_ns_value,
    axis2_char_t *rm_ns_value);
                    	
sandesha2_acks_to_t * AXIS2_CALL
sandesha2_create_seq_get_acks_to(
    sandesha2_create_seq_t *create_seq,
    const axis2_env_t *env);

axis2_status_t AXIS2_CALL                 
sandesha2_create_seq_set_acks_to(
    sandesha2_create_seq_t *create_seq,
    const axis2_env_t *env, 
    sandesha2_acks_to_t *acks_to);

sandesha2_seq_offer_t * AXIS2_CALL
sandesha2_create_seq_get_seq_offer(
    sandesha2_create_seq_t *create_seq,
    const axis2_env_t *env);

axis2_status_t AXIS2_CALL                 
sandesha2_create_seq_set_seq_offer(
    sandesha2_create_seq_t *create_seq,
    const axis2_env_t *env, 
    sandesha2_seq_offer_t *seq_offer);
 

/** @} */
#ifdef __cplusplus
}
#endif

#endif                          /* SANDESHA2_CREATE_SEQ_H */

