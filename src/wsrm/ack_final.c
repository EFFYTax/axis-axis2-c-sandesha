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
 
#include <sandesha2/sandesha2_ack_final.h>
#include <sandesha2/sandesha2_constants.h>
/** 
 * @brief AckFinal struct impl
 *	Sandesha2 IOM AckFinal
 */
typedef struct sandesha2_ack_final_impl sandesha2_ack_final_impl_t;  
  
struct sandesha2_ack_final_impl
{
	sandesha2_ack_final_t ack_final;
	axis2_char_t *ns_val;
};

#define SANDESHA2_INTF_TO_IMPL(ack_final) \
						((sandesha2_ack_final_impl_t *)(ack_final))

/***************************** Function headers *******************************/
axis2_char_t* AXIS2_CALL 
sandesha2_ack_final_get_namespace_value (sandesha2_iom_rm_element_t *ack_final,
						const axis2_env_t *env);
    
void* AXIS2_CALL 
sandesha2_ack_final_from_om_node(sandesha2_iom_rm_element_t *ack_final,
                    	const axis2_env_t *env, axiom_node_t *om_node);
    
axiom_node_t* AXIS2_CALL 
sandesha2_ack_final_to_om_node(sandesha2_iom_rm_element_t *ack_final,
                    	const axis2_env_t *env, void *om_node);
                    	
axis2_bool_t AXIS2_CALL 
sandesha2_ack_final_is_namespace_supported(sandesha2_iom_rm_element_t *ack_final,
                    	const axis2_env_t *env, axis2_char_t *namespace);

axis2_status_t AXIS2_CALL 
sandesha2_ack_final_free (sandesha2_iom_rm_element_t *ack_final, 
                        const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_ack_final_t* AXIS2_CALL
sandesha2_ack_final_create(const axis2_env_t *env,  axis2_char_t *ns_val)
{
    sandesha2_ack_final_impl_t *ack_final_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, ns_val, NULL);
    
    if(AXIS2_FALSE == sandesha2_ack_final_is_namespace_supported(
                        (sandesha2_iom_rm_element_t*)ack_final_impl,
                        env, ns_val))
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_UNSUPPORTED_NS, 
                            AXIS2_FAILURE);
        return NULL;
    }    
    ack_final_impl =  (sandesha2_ack_final_impl_t *)AXIS2_MALLOC 
                        (env->allocator, sizeof(sandesha2_ack_final_impl_t));
	
    if(NULL == ack_final_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    ack_final_impl->ns_val = NULL;
    ack_final_impl->ack_final.element.ops = NULL;
     
    ack_final_impl->ack_final.element.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_iom_rm_element_ops_t));
    if(NULL == ack_final_impl->ack_final.element.ops)
	{
		sandesha2_ack_final_free((sandesha2_iom_rm_element_t*)ack_final_impl,
                        env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    ack_final_impl->ns_val = (axis2_char_t *)AXIS2_STRDUP(ns_val, env);
    
    ack_final_impl->ack_final.element.ops->get_namespace_value = 
                        sandesha2_ack_final_get_namespace_value;
    ack_final_impl->ack_final.element.ops->from_om_node = 
    					sandesha2_ack_final_from_om_node;
    ack_final_impl->ack_final.element.ops->to_om_node = 
    					sandesha2_ack_final_to_om_node;
    ack_final_impl->ack_final.element.ops->is_namespace_supported = 
    					sandesha2_ack_final_is_namespace_supported;
    ack_final_impl->ack_final.element.ops->free = sandesha2_ack_final_free;
                        
	return &(ack_final_impl->ack_final);
}


axis2_status_t AXIS2_CALL 
sandesha2_ack_final_free (sandesha2_iom_rm_element_t *ack_final, 
                        const axis2_env_t *env)
{
    sandesha2_ack_final_impl_t *ack_final_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    ack_final_impl = SANDESHA2_INTF_TO_IMPL(ack_final);
    
    if(NULL != ack_final_impl->ns_val)
    {
        AXIS2_FREE(env->allocator, ack_final_impl->ns_val);
        ack_final_impl->ns_val = NULL;
    }
    if(NULL != ack_final_impl->ack_final.element.ops)
    {
        AXIS2_FREE(env->allocator, ack_final_impl->ack_final.element.ops);
        ack_final_impl->ack_final.element.ops = NULL;
    }
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(ack_final));
	return AXIS2_SUCCESS;
}

axis2_char_t* AXIS2_CALL 
sandesha2_ack_final_get_namespace_value (sandesha2_iom_rm_element_t *ack_final,
						const axis2_env_t *env)
{
	sandesha2_ack_final_impl_t *ack_final_impl = NULL;
	AXIS2_ENV_CHECK(env, NULL);
	
	ack_final_impl = SANDESHA2_INTF_TO_IMPL(ack_final);
	return ack_final_impl->ns_val;
}


void* AXIS2_CALL 
sandesha2_ack_final_from_om_node(sandesha2_iom_rm_element_t *ack_final,
                    	const axis2_env_t *env, axiom_node_t *om_node)
{
	sandesha2_ack_final_impl_t *ack_final_impl = NULL;
    axis2_qname_t *final_qname = NULL;
    axiom_element_t *om_element = NULL;
    axiom_element_t *final_part = NULL;
    axiom_node_t *final_part_node = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, om_node, NULL);
    
    ack_final_impl = SANDESHA2_INTF_TO_IMPL(ack_final);
    final_qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_FINAL, 
                        ack_final_impl->ns_val, NULL);
    if(NULL == final_qname)
    {
        return NULL;
    }
    om_element = AXIOM_NODE_GET_DATA_ELEMENT(om_node, env); 
    if(NULL == om_element)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_OM_ELEMENT, 
                        AXIS2_FAILURE); 
        return NULL;
    }
    final_part = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(om_element, env,
                        final_qname, om_node, &final_part_node);  
    if(NULL == final_part)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_OM_ELEMENT,
                        AXIS2_FAILURE);
        return NULL;
    }
    return ack_final;
}


axiom_node_t* AXIS2_CALL 
sandesha2_ack_final_to_om_node(sandesha2_iom_rm_element_t *ack_final,
                    	const axis2_env_t *env, void *om_node)
{
	sandesha2_ack_final_impl_t *ack_final_impl = NULL;
	axiom_namespace_t *rm_ns = NULL;
	axiom_element_t *af_element = NULL;
	axiom_node_t *af_node = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, om_node, NULL);
    
    ack_final_impl = SANDESHA2_INTF_TO_IMPL(ack_final);
	rm_ns = axiom_namespace_create(env, ack_final_impl->ns_val,
                        SANDESHA2_WSRM_COMMON_NS_PREFIX_RM);
    if(NULL == rm_ns)
    {
        return NULL;
    }
    af_element = axiom_element_create(env, NULL, SANDESHA2_WSRM_COMMON_FINAL,
                        rm_ns, &af_node);
    AXIOM_NODE_ADD_CHILD((axiom_node_t*)om_node, env, af_node);
    return (axiom_node_t*)om_node;
}

axis2_bool_t AXIS2_CALL 
sandesha2_ack_final_is_namespace_supported(sandesha2_iom_rm_element_t *ack_final,
                    	const axis2_env_t *env, axis2_char_t *namespace)
{
	sandesha2_ack_final_impl_t *ack_final_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    ack_final_impl = SANDESHA2_INTF_TO_IMPL(ack_final);
    if(0 == AXIS2_STRCMP(namespace, SANDESHA2_SPEC_2005_02_NS_URI))
    {
        return AXIS2_FALSE;
    }
    if(0 == AXIS2_STRCMP(namespace, SANDESHA2_SPEC_2005_10_NS_URI))
    {
        return AXIS2_TRUE;
    } 
    return AXIS2_FALSE;
}