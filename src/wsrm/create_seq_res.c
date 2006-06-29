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
#include <sandesha2/sandesha2_create_seq_res.h>
#include <sandesha2/sandesha2_constants.h>
#include <axiom_soap_body.h>

/** 
 * @brief CreateSeqenceResponse struct impl
 *	Sandesha2 IOM CreateSeqenceResponse
 */
typedef struct sandesha2_create_seq_res_impl sandesha2_create_seq_res_impl_t;  
  
struct sandesha2_create_seq_res_impl
{
	sandesha2_create_seq_res_t create_seq_res;
	sandesha2_identifier_t *identifier;
	sandesha2_accept_t *accept;
	sandesha2_expires_t *expires;
	axis2_char_t *rm_ns_val;
    axis2_char_t *addr_ns_val;
};

#define SANDESHA2_INTF_TO_IMPL(create_seq_res) \
						((sandesha2_create_seq_res_impl_t *)(create_seq_res))

/***************************** Function headers *******************************/
axis2_char_t* AXIS2_CALL 
sandesha2_create_seq_res_get_namespace_value (
						sandesha2_iom_rm_element_t *create_seq_res,
						const axis2_env_t *env);
    
void* AXIS2_CALL 
sandesha2_create_seq_res_from_om_node(
                        sandesha2_iom_rm_element_t *create_seq_res,
                    	const axis2_env_t *env, axiom_node_t *om_node);
    
axiom_node_t* AXIS2_CALL 
sandesha2_create_seq_res_to_om_node(sandesha2_iom_rm_element_t *create_seq_res,
                    	const axis2_env_t *env, void *om_node);
                    	
axis2_bool_t AXIS2_CALL 
sandesha2_create_seq_res_is_namespace_supported(
						sandesha2_iom_rm_element_t *create_seq_res, 
                        const axis2_env_t *env, 
						axis2_char_t *namespace);

axis2_status_t AXIS2_CALL
sandesha2_create_seq_res_set_identifier(
                        sandesha2_create_seq_res_t *create_seq_res,
                    	const axis2_env_t *env, 
                        sandesha2_identifier_t *identifier);

sandesha2_identifier_t * AXIS2_CALL
sandesha2_create_seq_res_get_identifier(
                        sandesha2_create_seq_res_t *create_seq_res,
                    	const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_create_seq_res_set_accept(sandesha2_create_seq_res_t *create_seq_res,
                    	const axis2_env_t *env, sandesha2_accept_t *accept);

sandesha2_accept_t * AXIS2_CALL
sandesha2_create_seq_res_get_accept(sandesha2_create_seq_res_t *create_seq_res,
                    	const axis2_env_t *env);
                        
axis2_status_t AXIS2_CALL
sandesha2_create_seq_res_set_expires(sandesha2_create_seq_res_t *create_seq_res,
                    	const axis2_env_t *env, sandesha2_expires_t *expires);

sandesha2_expires_t * AXIS2_CALL
sandesha2_create_seq_res_get_expires(sandesha2_create_seq_res_t *create_seq_res,
                    	const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_create_seq_res_to_soap_env(sandesha2_iom_rm_part_t *create_seq_res,
                        const axis2_env_t *env, 
                        axiom_soap_envelope_t *envelope);  

axis2_status_t AXIS2_CALL 
sandesha2_create_seq_res_free (sandesha2_iom_rm_element_t *create_seq_res,
						const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_create_seq_res_t* AXIS2_CALL
sandesha2_create_seq_res_create(const axis2_env_t *env,  axis2_char_t *rm_ns_val, 
					    axis2_char_t *addr_ns_val)
{
    sandesha2_create_seq_res_impl_t *create_seq_res_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, rm_ns_val, NULL);
    AXIS2_PARAM_CHECK(env->error, addr_ns_val, NULL);
    
    if(AXIS2_FALSE == sandesha2_create_seq_res_is_namespace_supported(
                        (sandesha2_iom_rm_element_t*)create_seq_res_impl, env, 
                        rm_ns_val))
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_UNSUPPORTED_NS, 
                            AXIS2_FAILURE);
        return NULL;
    }    
    create_seq_res_impl =  (sandesha2_create_seq_res_impl_t *)AXIS2_MALLOC 
                        (env->allocator, 
                        sizeof(sandesha2_create_seq_res_impl_t));
	
    if(NULL == create_seq_res_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    create_seq_res_impl->rm_ns_val = NULL;
    create_seq_res_impl->addr_ns_val = NULL;
    create_seq_res_impl->identifier = NULL;
    create_seq_res_impl->accept = NULL;
    create_seq_res_impl->expires = NULL;
    create_seq_res_impl->create_seq_res.ops = NULL; 
    create_seq_res_impl->create_seq_res.part.ops = NULL;
    create_seq_res_impl->create_seq_res.part.element.ops = NULL;
    
    create_seq_res_impl->create_seq_res.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_create_seq_res_ops_t));
    if(NULL == create_seq_res_impl->create_seq_res.ops)
	{
		sandesha2_create_seq_res_free((sandesha2_iom_rm_element_t*)
                         create_seq_res_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    create_seq_res_impl->create_seq_res.part.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_iom_rm_part_ops_t));
    if(NULL == create_seq_res_impl->create_seq_res.part.ops)
	{
		sandesha2_create_seq_res_free((sandesha2_iom_rm_element_t*)
                         create_seq_res_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    create_seq_res_impl->create_seq_res.part.element.ops = AXIS2_MALLOC(
        env->allocator, sizeof(sandesha2_iom_rm_element_ops_t));
    if(NULL == create_seq_res_impl->create_seq_res.part.element.ops)
	{
		sandesha2_create_seq_res_free((sandesha2_iom_rm_element_t*)
                         create_seq_res_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    create_seq_res_impl->rm_ns_val = (axis2_char_t *)AXIS2_STRDUP(rm_ns_val, env);
    create_seq_res_impl->addr_ns_val = (axis2_char_t *)AXIS2_STRDUP(addr_ns_val, 
                        env);
    
    create_seq_res_impl->create_seq_res.part.element.ops->get_namespace_value = 
                        sandesha2_create_seq_res_get_namespace_value;
    create_seq_res_impl->create_seq_res.part.element.ops->from_om_node = 
    					sandesha2_create_seq_res_from_om_node;
    create_seq_res_impl->create_seq_res.part.element.ops->to_om_node = 
    				    sandesha2_create_seq_res_to_om_node;
    create_seq_res_impl->create_seq_res.part.element.ops->is_namespace_supported
                        = sandesha2_create_seq_res_is_namespace_supported;
    create_seq_res_impl->create_seq_res.part.ops->to_soap_env = 
    					sandesha2_create_seq_res_to_soap_env;
    create_seq_res_impl->create_seq_res.ops->set_identifier = 
    					sandesha2_create_seq_res_set_identifier;
    create_seq_res_impl->create_seq_res.ops->get_identifier = 
    					sandesha2_create_seq_res_get_identifier;
    create_seq_res_impl->create_seq_res.ops->set_accept = 
    					sandesha2_create_seq_res_set_accept;
    create_seq_res_impl->create_seq_res.ops->get_accept = 
    					sandesha2_create_seq_res_get_accept;
    create_seq_res_impl->create_seq_res.ops->set_expires = 
    					sandesha2_create_seq_res_set_expires;
    create_seq_res_impl->create_seq_res.ops->get_expires = 
    					sandesha2_create_seq_res_get_expires;
    create_seq_res_impl->create_seq_res.part.element.ops->free = 
    					sandesha2_create_seq_res_free;
                        
	return &(create_seq_res_impl->create_seq_res);
}


axis2_status_t AXIS2_CALL 
sandesha2_create_seq_res_free (sandesha2_iom_rm_element_t *create_seq_res, 
						const axis2_env_t *env)
{
    sandesha2_create_seq_res_impl_t *create_seq_res_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    create_seq_res_impl = SANDESHA2_INTF_TO_IMPL(create_seq_res);
    
    if(NULL != create_seq_res_impl->addr_ns_val)
    {
        AXIS2_FREE(env->allocator, create_seq_res_impl->addr_ns_val);
        create_seq_res_impl->addr_ns_val = NULL;
    }
    if(NULL != create_seq_res_impl->rm_ns_val)
    {
        AXIS2_FREE(env->allocator, create_seq_res_impl->rm_ns_val);
        create_seq_res_impl->rm_ns_val = NULL;
    }
    if(NULL != create_seq_res->ops)
    {
        AXIS2_FREE(env->allocator, create_seq_res->ops);
        create_seq_res->ops = NULL;
    }
    if(NULL != create_seq_res_impl->create_seq_res.ops)
    {
        AXIS2_FREE(env->allocator, create_seq_res_impl->create_seq_res.ops);
        create_seq_res_impl->create_seq_res.ops = NULL;
    }
    if(NULL != create_seq_res_impl->create_seq_res.part.ops)
    {
        AXIS2_FREE(env->allocator, create_seq_res_impl->create_seq_res.part.ops);
        create_seq_res_impl->create_seq_res.part.ops = NULL;
    }
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(create_seq_res));
	return AXIS2_SUCCESS;
}

axis2_char_t* AXIS2_CALL 
sandesha2_create_seq_res_get_namespace_value (
						sandesha2_iom_rm_element_t *create_seq_res, 
                        const axis2_env_t *env)
{
	sandesha2_create_seq_res_impl_t *create_seq_res_impl = NULL;
	AXIS2_ENV_CHECK(env, NULL);
	
	create_seq_res_impl = SANDESHA2_INTF_TO_IMPL(create_seq_res);
	return create_seq_res_impl->rm_ns_val;
}


void* AXIS2_CALL 
sandesha2_create_seq_res_from_om_node(
                        sandesha2_iom_rm_element_t *create_seq_res,
                    	const axis2_env_t *env, axiom_node_t *om_node)
{
	sandesha2_create_seq_res_impl_t *create_seq_res_impl = NULL;
    axiom_element_t *om_element = NULL;
    axiom_element_t *csr_part = NULL;
    axiom_element_t *exp_part = NULL;
    axiom_element_t *acc_part = NULL;
    axiom_node_t *csr_node = NULL;
    axiom_node_t *exp_node = NULL;
    axiom_node_t *acc_node = NULL;
    axis2_qname_t *csr_qname = NULL;
    axis2_qname_t *exp_qname = NULL;
    axis2_qname_t *acc_qname = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, om_node, NULL);
    
    create_seq_res_impl = SANDESHA2_INTF_TO_IMPL(create_seq_res);
    om_element =AXIOM_NODE_GET_DATA_ELEMENT(om_node, env);
    if(NULL == om_element)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_OM_ELEMENT,
                        AXIS2_FAILURE);
        return NULL;
    }
    csr_qname = axis2_qname_create(env, 
                        SANDESHA2_WSRM_COMMON_CREATE_SEQUENCE_RESPONSE,
                         create_seq_res_impl->rm_ns_val, NULL);
    if(NULL == csr_qname)
    {
        return NULL;
    }
    csr_part = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(om_element, env,
                        csr_qname, om_node, &csr_node);
    if(NULL == csr_part)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_OM_ELEMENT,
                        AXIS2_FAILURE);
        return NULL;
    }
    create_seq_res_impl->identifier = sandesha2_identifier_create(env, 
                        create_seq_res_impl->rm_ns_val);
    if(NULL == create_seq_res_impl->identifier)
    {
        return NULL;
    }
    if(NULL == SANDESHA2_IOM_RM_ELEMENT_FROM_OM_NODE(
                        ((sandesha2_iom_rm_element_t *)
                        create_seq_res_impl->identifier), env, csr_node))
    {
        return NULL;
    }
    exp_qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_EXPIRES, 
                        create_seq_res_impl->rm_ns_val, NULL);
    if(NULL == exp_qname)
    {
        return NULL;
    }
    exp_part = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(csr_part, env,
                        csr_qname, csr_node, &exp_node);
    if(NULL != exp_part)
    {
        create_seq_res_impl->expires = sandesha2_expires_create(env, 
            create_seq_res_impl->rm_ns_val);
        if(NULL == create_seq_res_impl->expires)
        {
            return NULL;
        }
        if(NULL == SANDESHA2_IOM_RM_ELEMENT_FROM_OM_NODE(
                        ((sandesha2_iom_rm_element_t*)
                        create_seq_res_impl->expires), env, csr_node))
        {
            return NULL;
        }
    }
    acc_qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_ACCEPT, 
                        create_seq_res_impl->rm_ns_val, NULL);
    if(NULL == acc_qname)
    {
        return NULL;
    } 
    acc_part = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(csr_part, env, 
                        acc_qname, csr_node, &acc_node);
    if(NULL != acc_part)
    {
        create_seq_res_impl->accept = sandesha2_accept_create(env, 
                        create_seq_res_impl->rm_ns_val, 
                        create_seq_res_impl->addr_ns_val);
        if(NULL == create_seq_res_impl->accept)
        {
            return NULL;
        }
        if(NULL == SANDESHA2_IOM_RM_ELEMENT_FROM_OM_NODE(
                        ((sandesha2_iom_rm_element_t*)
                        create_seq_res_impl->accept), env, csr_node))
        {
            return NULL;
        }
    } 
    return create_seq_res;
}


axiom_node_t* AXIS2_CALL 
sandesha2_create_seq_res_to_om_node(sandesha2_iom_rm_element_t *create_seq_res,
                    	const axis2_env_t *env, void *om_node)
{
	sandesha2_create_seq_res_impl_t *create_seq_res_impl = NULL;
    axiom_namespace_t *rm_ns = NULL;
    axiom_element_t *csr_element = NULL;
    axiom_node_t *csr_node = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, om_node, NULL);
    
    create_seq_res_impl = SANDESHA2_INTF_TO_IMPL(create_seq_res);
    if(NULL == create_seq_res_impl->identifier)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_TO_OM_NULL_ELEMENT, 
                        AXIS2_FAILURE);
        return NULL;
    }
    rm_ns = axiom_namespace_create(env, create_seq_res_impl->rm_ns_val,
                        SANDESHA2_WSRM_COMMON_NS_PREFIX_RM);
    if(NULL == rm_ns)
    {
        return NULL;
    }
    csr_element = axiom_element_create(env, NULL, 
                        SANDESHA2_WSRM_COMMON_CREATE_SEQUENCE_RESPONSE, rm_ns, 
                        &csr_node);
    if(NULL == csr_element)
    {
        return NULL;
    }
    SANDESHA2_IOM_RM_ELEMENT_TO_OM_NODE(((sandesha2_iom_rm_element_t*)
                        create_seq_res_impl->identifier), env, csr_node);
    if(NULL != create_seq_res_impl->accept)
    {
        SANDESHA2_IOM_RM_ELEMENT_TO_OM_NODE(((sandesha2_iom_rm_element_t*)
                        create_seq_res_impl->accept), env, csr_node);
    }
    if(NULL != create_seq_res_impl->expires)
    {
        SANDESHA2_IOM_RM_ELEMENT_TO_OM_NODE(((sandesha2_iom_rm_element_t*)
                        create_seq_res_impl->expires), env, csr_node);
    }
    AXIOM_NODE_ADD_CHILD((axiom_node_t*)om_node, env, csr_node);
    return (axiom_node_t*)om_node;
}

axis2_bool_t AXIS2_CALL 
sandesha2_create_seq_res_is_namespace_supported(
						sandesha2_iom_rm_element_t *create_seq_res, 
                        const axis2_env_t *env, 
						axis2_char_t *namespace)
{
	sandesha2_create_seq_res_impl_t *create_seq_res_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    create_seq_res_impl = SANDESHA2_INTF_TO_IMPL(create_seq_res);
    if(0 == AXIS2_STRCMP(namespace, SANDESHA2_SPEC_2005_02_NS_URI))
    {
        return AXIS2_TRUE;
    }
    if(0 == AXIS2_STRCMP(namespace, SANDESHA2_SPEC_2005_10_NS_URI))
    {
        return AXIS2_TRUE;
    }
    return AXIS2_FALSE;
}

axis2_status_t AXIS2_CALL
sandesha2_create_seq_res_set_identifier(
                        sandesha2_create_seq_res_t *create_seq_res,
                    	const axis2_env_t *env, 
                        sandesha2_identifier_t *identifier)
{
	sandesha2_create_seq_res_impl_t *create_seq_res_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    create_seq_res_impl = SANDESHA2_INTF_TO_IMPL(create_seq_res);
    create_seq_res_impl->identifier = identifier;
    return AXIS2_SUCCESS;
}


sandesha2_identifier_t * AXIS2_CALL
sandesha2_create_seq_res_get_identifier(
                        sandesha2_create_seq_res_t *create_seq_res,
                    	const axis2_env_t *env)
{
	sandesha2_create_seq_res_impl_t *create_seq_res_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    create_seq_res_impl = SANDESHA2_INTF_TO_IMPL(create_seq_res);
    return create_seq_res_impl->identifier;
} 

axis2_status_t AXIS2_CALL
sandesha2_create_seq_res_set_accept(sandesha2_create_seq_res_t *create_seq_res,
                    	const axis2_env_t *env, sandesha2_accept_t *accept)
{
	sandesha2_create_seq_res_impl_t *create_seq_res_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    create_seq_res_impl = SANDESHA2_INTF_TO_IMPL(create_seq_res);
    create_seq_res_impl->accept = accept;
    return AXIS2_SUCCESS;
}


sandesha2_accept_t * AXIS2_CALL
sandesha2_create_seq_res_get_accept(sandesha2_create_seq_res_t *create_seq_res,
                    	const axis2_env_t *env)
{
	sandesha2_create_seq_res_impl_t *create_seq_res_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    create_seq_res_impl = SANDESHA2_INTF_TO_IMPL(create_seq_res);
    return create_seq_res_impl->accept;
}

axis2_status_t AXIS2_CALL
sandesha2_create_seq_res_set_expires(sandesha2_create_seq_res_t *create_seq_res,
                    	const axis2_env_t *env, sandesha2_expires_t *expires)
{
	sandesha2_create_seq_res_impl_t *create_seq_res_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    create_seq_res_impl = SANDESHA2_INTF_TO_IMPL(create_seq_res);
    create_seq_res_impl->expires = expires;
    return AXIS2_SUCCESS;
}


sandesha2_expires_t * AXIS2_CALL
sandesha2_create_seq_res_get_expires(sandesha2_create_seq_res_t *create_seq_res,
                    	const axis2_env_t *env)
{
	sandesha2_create_seq_res_impl_t *create_seq_res_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    create_seq_res_impl = SANDESHA2_INTF_TO_IMPL(create_seq_res);
    return create_seq_res_impl->expires;
}

axis2_status_t AXIS2_CALL
sandesha2_create_seq_res_to_soap_env(sandesha2_iom_rm_part_t *create_seq_res,
                        const axis2_env_t *env, axiom_soap_envelope_t *envelope)
{
    sandesha2_create_seq_res_impl_t *create_seq_res_impl = NULL;
    axiom_node_t *body_node = NULL;
    axis2_qname_t *create_seq_res_qname = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, envelope, AXIS2_FAILURE);
    
    create_seq_res_impl = SANDESHA2_INTF_TO_IMPL(create_seq_res);
    /**
     * Remove if old exists
     */
    create_seq_res_qname = axis2_qname_create(env, 
                        SANDESHA2_WSRM_COMMON_CREATE_SEQUENCE_RESPONSE, 
                        create_seq_res_impl->rm_ns_val, NULL);
    if(NULL == create_seq_res_qname)
    {
        return AXIS2_FAILURE;
    }
    sandesha2_utils_remove_soap_body_part(env, envelope, create_seq_res_qname);
    body_node = AXIOM_SOAP_BODY_GET_BASE_NODE(AXIOM_SOAP_ENVELOPE_GET_BODY(
                        envelope, env), env);
    sandesha2_create_seq_res_to_om_node((sandesha2_iom_rm_element_t*)
                        create_seq_res, env, body_node);
    return AXIS2_SUCCESS;
}