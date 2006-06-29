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
 
#include <sandesha2/sandehsa2_spec_specific_consts.h>
#include <sandesha2/sandehsa2_consts.h>

AXIS2_EXTERN axis2_char_t* AXIS2_CALL
sandesha2_spec_specific_consts_get_spec_ver_str(const axis2_env_t *env,
                        axis2_char_t *ns_val)
{       
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, ns_val, AXIS2_FAILURE);
    
    if(0 == AXIS2_STRCMP(ns_val, SANDESHA2_SPEC_2005_02_NS_URI))
    {
        return SANDESHA2_SPEC_VERSION_1_0;
    }
    else if(0 == AXIS2_STRCMP(ns_val, SANDESHA2_SPEC_2005_10_NS_URI))
    {
        return SANDESHA2_SPEC_VERSION_1_1;
    }
    else
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_NS_URI, 
                        AXIS2_FAILURE);
        return NULL;                                
    }
    return NULL
}

AXIS2_EXTERN axis2_bool_t AXIS2_CALL
sandesha2_spec_specific_consts_is_ack_final_allowed(const axis2_env_t *env, 
                        axis2_char_t *rm_spec_ver)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_spec_ver, AXIS2_FAILURE);
    
    if(0 == AXIS2_STRCMP(rm_spec_ver, SANDESHA2_SPEC_VERSION_1_0))
    {
        return AXIS2_FALSE;
    }
    else if(0 == AXIS2_STRCMP(rm_spec_ver, SANDESHA2_SPEC_VERSION_1_1))
    {
        return AXIS2_TRUE;
    }
    else
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_SPEC_VER, 
                        AXIS2_FAILURE);
        return AXIS2_FALSE;
    }
    return AXIS2_FALSE;
}

AXIS2_EXTERN axis2_bool_t AXIS2_CALL
sandesha2_spec_specific_consts_is_ack_none_allowed(const axis2_env_t *env, 
                        axis2_char_t *rm_spec_ver)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_spec_ver, AXIS2_FAILURE);
    if(0 == AXIS2_STRCMP(rm_spec_ver, SANDESHA2_SPEC_VERSION_1_0))
    {
        return AXIS2_FALSE;
    }
    else if(0 == AXIS2_STRCMP(rm_spec_ver, SANDESHA2_SPEC_VERSION_1_1))
    {
        return AXIS2_TRUE;
    }
    else
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_SPEC_VER, 
                        AXIS2_FAILURE);
        return AXIS2_FALSE;
    }
    return AXIS2_FALSE;   
}

