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

#include <stdio.h>
#include <axiom.h>
#include <axis2_util.h>
#include <axiom_soap.h>
#include <axis2_client.h>

#define MAX_COUNT  10

axiom_node_t *
build_om_programatically(
    const axis2_env_t *env,
    axis2_char_t *text);

int main(int argc, char** argv)
{
    const axis2_env_t *env = NULL;
    const axis2_char_t *address = NULL;
    const axis2_char_t *to = NULL;
    axis2_endpoint_ref_t* endpoint_ref = NULL;
    axis2_endpoint_ref_t* target_epr = NULL;
    axis2_options_t *options = NULL;
    const axis2_char_t *client_home = NULL;
    axis2_svc_client_t* svc_client = NULL;
    axiom_node_t *payload = NULL;
    axis2_status_t status = AXIS2_FAILURE;
    axis2_property_t *property = NULL;
    int count = 0;
   
    /* Set up the environment */
    env = axis2_env_create_all("rm_ping.log", AXIS2_LOG_LEVEL_TRACE);

    /* Set end point reference of echo service */
    /*address = "http://127.0.0.1:8888/axis2/services/RMSampleService";*/
    address = "http://127.0.0.1:5555/axis2/services/RMSampleService";
    /*to = "http://127.0.0.1:8080/axis2/services/RMSampleService";*/
    to = "http://127.0.0.1:9090/axis2/services/RMSampleService";
    if (argc > 1 )
    {
        address = argv[1];
    }
    if (AXIS2_STRCMP(address, "-h") == 0)
    {
        printf("Usage : %s [endpoint_url]\n", argv[0]);
        printf("use -h for help\n");
        return 0;
    }
    printf ("Using endpoint : %s\n", address);
    
    /* Create EPR with given address */
    endpoint_ref = axis2_endpoint_ref_create(env, to);
    target_epr = axis2_endpoint_ref_create(env, address);

    /* Setup options */
    options = axis2_options_create(env);
    AXIS2_OPTIONS_SET_TO(options, env, endpoint_ref);
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_VALUE(property, env, target_epr);
    AXIS2_OPTIONS_SET_PROPERTY(options, env, AXIS2_TARGET_EPR, property);
    /*AXIS2_OPTIONS_SET_ACTION(options, env, "urn:wsrm:Ping");*/

    /* Set up deploy folder. It is from the deploy folder, the configuration is 
     * picked up using the axis2.xml file.
     * In this sample client_home points to the Axis2/C default deploy folder. 
     * The client_home can be different from this folder on your system. For 
     * example, you may have a different folder (say, my_client_folder) with its 
     * own axis2.xml file. my_client_folder/modules will have the modules that 
     * the client uses
     */
    client_home = AXIS2_GETENV("AXIS2C_HOME");
    if (!client_home)
        client_home = "../../deploy";

    /* Create service client */
    svc_client = axis2_svc_client_create(env, client_home);
    if (!svc_client)
    {
        printf("Error creating service client\n");
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Stub invoke FAILED: Error code:"
                  " %d :: %s", env->error->error_number,
                        AXIS2_ERROR_GET_MESSAGE(env->error));
    }

    /* Set service client options */
    AXIS2_SVC_CLIENT_SET_OPTIONS(svc_client, env, options);    
    
    /* Engage addressing module */
    AXIS2_SVC_CLIENT_ENGAGE_MODULE(svc_client, env, AXIS2_MODULE_ADDRESSING);
    
    /* Build the SOAP request message payload using OM API.*/
    AXIS2_SVC_CLIENT_ENGAGE_MODULE(svc_client, env, "sandesha2");
    
    /* Send request */
    payload = build_om_programatically(env, "ping1");
    status = AXIS2_SVC_CLIENT_SEND_ROBUST(svc_client, env, payload);
    if(status)
        printf("\nping client invoke SUCCESSFUL!\n");
    payload = NULL;
    
    payload = build_om_programatically(env, "ping2");
    status = AXIS2_SVC_CLIENT_SEND_ROBUST(svc_client, env, payload);
    if(status)
        printf("\nping client invoke SUCCESSFUL!\n");
    payload = NULL;

    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_VALUE_TRUE);
    AXIS2_OPTIONS_SET_PROPERTY(options, env, "Sandesha2LastMessage", 
            property);
    payload = build_om_programatically(env, "ping3");
    status = AXIS2_SVC_CLIENT_SEND_ROBUST(svc_client, env, payload);
    if(status)
        printf("\nping client invoke SUCCESSFUL!\n");
     /** Wait till callback is complete. Simply keep the parent thread running
       until our on_complete or on_error is invoked */

    AXIS2_SLEEP(MAX_COUNT);
   
    if (svc_client)
    {
        AXIS2_SVC_CLIENT_FREE(svc_client, env);
        svc_client = NULL;
    }
    return 0;
}

/* build SOAP request message content using OM */
axiom_node_t *
build_om_programatically(
    const axis2_env_t *env,
    axis2_char_t *text)
{
    axiom_node_t *ping_om_node = NULL;
    axiom_element_t* ping_om_ele = NULL;
    axiom_node_t *text_om_node = NULL;
    axiom_element_t* text_om_ele = NULL;
    axiom_namespace_t *ns1 = NULL;
    axis2_char_t *buffer = NULL;
    
    ns1 = axiom_namespace_create (env, "http://tempuri.org/", "ns1");
    ping_om_ele = axiom_element_create(env, NULL, "ping", ns1, &ping_om_node);
    text_om_ele = axiom_element_create(env, ping_om_node, "Text", ns1, &text_om_node);
    AXIOM_ELEMENT_SET_TEXT(text_om_ele, env, text, text_om_node);
    
    buffer = AXIOM_NODE_TO_STRING(ping_om_node, env);
    printf("\nSending OM node in XML : %s \n",  buffer); 

    return ping_om_node;
}
