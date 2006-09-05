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
 
#ifndef SANDESHA2_TERMINATE_SEQ_H
#define SANDESHA2_TERMINATE_SEQ_H

/**
  * @file sandesha2_terminate_seq.h
  * @brief 
  */

#include <axis2_utils_defines.h>
#include <axis2_env.h>
#include <sandesha2_iom_rm_part.h>
#include <sandesha2_identifier.h>
#include <sandesha2_error.h>


#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup sandesha2_terminate_seq
 * @ingroup sandesha2_wsrm
 * @{
 */
    
typedef struct sandesha2_terminate_seq_ops sandesha2_terminate_seq_ops_t;
typedef struct sandesha2_terminate_seq sandesha2_terminate_seq_t;
 
/**
 * @brief TerminateSequence ops struct
 * Encapsulator struct for ops of sandesha2_terminate_seq
 */
AXIS2_DECLARE_DATA struct sandesha2_terminate_seq_ops
{
    sandesha2_identifier_t * (AXIS2_CALL *
        get_identifier)
            (sandesha2_terminate_seq_t *terminate_seq,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_identifier)
            (sandesha2_terminate_seq_t *terminate_seq,
            const axis2_env_t *env, 
            sandesha2_identifier_t *identifier);
};

/**
 * @brief sandesha2_terminate_seq
 *    sandesha2_terminate_seq
 */
AXIS2_DECLARE_DATA struct sandesha2_terminate_seq
{
    sandesha2_iom_rm_part_t part;
    sandesha2_terminate_seq_ops_t *ops;
};

AXIS2_EXTERN sandesha2_terminate_seq_t* AXIS2_CALL
sandesha2_terminate_seq_create(
						const axis2_env_t *env,
					    axis2_char_t *ns_value);
                        
/************************** Start of function macros **************************/
#define SANDESHA2_TERMINATE_SEQ_SET_IDENTIFIER(terminate_seq, env, identifier) \
    ((terminate_seq)->ops->set_identifier (terminate_seq, env, identifier))
#define SANDESHA2_TERMINATE_SEQ_GET_IDENTIFIER(terminate_seq, env) \
    ((terminate_seq)->ops->get_identifier (terminate_seq, env))
/************************** End of function macros ****************************/
/** @} */
#ifdef __cplusplus
}
#endif

#endif                          /* SANDESHA2_TERMINATE_SEQ_H */
