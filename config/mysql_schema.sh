#!/bin/bash
drop database sandesha2_svr_db;
drop database sandesha2_client_db;
create database sandesha2_svr_db;
create database sandesha2_client_db;

connect sandesha2_svr_db;
create table create_seq(create_seq_msg_id varchar(100) primary key, 
    internal_seq_id varchar(200), seq_id varchar(200), create_seq_msg_store_key varchar(100),
    ref_msg_store_key varchar(100)) engine=InnoDB;
create table invoker(msg_ctx_ref_key varchar(100) primary key, 
    msg_no long, seq_id varchar(200), is_invoked boolean) engine=InnoDB;
create table sender(msg_id varchar(100) primary key, 
    msg_ctx_ref_key varchar(100), internal_seq_id varchar(200), sent_count int, 
    msg_no long, send boolean, resend boolean, time_to_send long, msg_type int, 
    seq_id varchar(200), wsrm_anon_uri varchar(100), to_address varchar(100)) engine=InnoDB;
create table next_msg(seq_id varchar(200) primary key, 
    ref_msg_key varchar(100), polling_mode boolean, msg_no long) engine=InnoDB;
create table seq_property(id varchar(200) , 
    seq_id varchar(200), name varchar(200), value varchar(200)) engine=InnoDB;
create table msg(stored_key varchar(200) primary key,
    msg_id varchar(200), soap_env_str text, soap_version int, transport_out varchar(100),
    op varchar(100), svc varchar(100), svc_grp varchar(100), op_mep varchar(100), to_url varchar(200),
    reply_to varchar(200), transport_to varchar(200), execution_chain_str varchar(200), flow int, 
    msg_recv_str varchar(200), svr_side boolean, in_msg_store_key varchar(200), prop_str varchar(8192), 
    action varchar(200)) engine=InnoDB;

connect sandesha2_client_db;
create table create_seq(create_seq_msg_id varchar(100) primary key, 
    internal_seq_id varchar(200), seq_id varchar(200), create_seq_msg_store_key varchar(100),
    ref_msg_store_key varchar(100)) engine=InnoDB;
create table invoker(msg_ctx_ref_key varchar(100) primary key, 
    msg_no long, seq_id varchar(200), is_invoked boolean) engine=InnoDB;
create table sender(msg_id varchar(100) primary key, 
    msg_ctx_ref_key varchar(100), internal_seq_id varchar(200), sent_count int, 
    msg_no long, send boolean, resend boolean, time_to_send long, msg_type int, 
    seq_id varchar(200), wsrm_anon_uri varchar(100), to_address varchar(100)) engine=InnoDB;
create table next_msg(seq_id varchar(200) primary key, 
    ref_msg_key varchar(100), polling_mode boolean, msg_no long) engine=InnoDB;
create table seq_property(id varchar(200) , 
    seq_id varchar(200), name varchar(200), value varchar(200)) engine=InnoDB;
create table msg(stored_key varchar(200) primary key,
    msg_id varchar(200), soap_env_str text, soap_version int, transport_out varchar(100),
    op varchar(100), svc varchar(100), svc_grp varchar(100), op_mep varchar(100), to_url varchar(200),
    reply_to varchar(200), transport_to varchar(200), execution_chain_str varchar(200), flow int, 
    msg_recv_str varchar(200), svr_side boolean, in_msg_store_key varchar(200), prop_str varchar(8192), 
    action varchar(200)) engine=InnoDB;
create table response(seq_id varchar(200), 
    response_str text, msg_no int, soap_version int) engine=InnoDB;