
/*
 * Copyright (C) Daeun Jeong
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include "ngx_http_jda_test_module.h"


#define NGX_FALSE	0
#define NGX_TRUE	1

#define JDA_CONF_DEBUG(fmt, args...)	\
	ngx_log_error(NGX_LOG_NOTICE, ngx_cycle->log, 0, "[JDA_CONF]: "fmt" (%s.%d)", ##args, __FUNCTION__, __LINE__);
#define JDA_HTTP_DEBUG(fmt, args...)	\
	ngx_log_debug(NGX_LOG_DEBUG_HTTP, r->connection->log, 0, "[JDA_HTTP]: "fmt" (%s.%d)", ##args, __FUNCTION__, __LINE__);


typedef struct {
	ngx_flag_t		jda_test;
} ngx_http_jda_test_loc_conf_t;


static ngx_int_t
ngx_http_jda_test_v1_variable(ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t
ngx_http_jda_test_handler(ngx_http_request_t *r);

static void *ngx_http_jda_test_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_jda_test_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);

static ngx_int_t
ngx_http_jda_test_add_variables(ngx_conf_t *cf);
static ngx_int_t
ngx_http_jda_test_init(ngx_conf_t *cf);


ngx_module_t	ngx_http_jda_test_module;


static ngx_command_t	ngx_http_jda_test_commands[] = {
	{ ngx_string("jda_test"),
	  NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
	  ngx_conf_set_flag_slot,
	  NGX_HTTP_LOC_CONF_OFFSET,
	  offsetof(ngx_http_jda_test_loc_conf_t, jda_test),
	  NULL },

	ngx_null_command
};


static ngx_http_module_t	ngx_http_jda_test_module_ctx = {
	ngx_http_jda_test_add_variables,	 	/* preconfiguration */
	ngx_http_jda_test_init,					/* postconfiguration */

	NULL,									/* create main configuration */
	NULL,									/* init main configuration */

	NULL,									/* create server configuration */
	NULL,									/* merge server configuration */

	ngx_http_jda_test_create_loc_conf,		/* create location configuration */
	ngx_http_jda_test_merge_loc_conf,		/* merge location configuration */
};


ngx_module_t	ngx_http_jda_test_module = {
	NGX_MODULE_V1,
	&ngx_http_jda_test_module_ctx,			/* module context */
	ngx_http_jda_test_commands,				/* module directives */
	NGX_HTTP_MODULE,						/* module type */
	NULL,									/* init master */
	NULL,									/* init module */
	NULL,									/* init process */
	NULL,									/* init thread */
	NULL,									/* exit thread */
	NULL,									/* exit process */
	NULL,									/* exit master */
	NGX_MODULE_V1_PADDING
};


static ngx_http_variable_t	ngx_http_jda_test_vars[] = {
	{ ngx_string("jda_test_v1"), NULL,
	  ngx_http_jda_test_v1_variable, 0, 0, 0 },

	ngx_http_null_variable
};


static ngx_int_t
ngx_http_jda_test_v1_variable(ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data)
{
	ngx_str_t	uri = ngx_null_string;

	JDA_HTTP_DEBUG("");

	ngx_jda_strcopy(r->pool, &r->uri, &uri);

	JDA_HTTP_DEBUG("uri=\"%V\"", &uri);

	v->len = uri.len;
	v->valid = 1;
	v->no_cacheable = 0;
	v->not_found = 0;
	v->data = uri.data;

	return NGX_OK;
}


static void *
ngx_http_jda_test_create_loc_conf(ngx_conf_t *cf)
{
	ngx_http_jda_test_loc_conf_t	*conf = NULL;

	JDA_CONF_DEBUG("");

	conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_jda_test_loc_conf_t));
	if (conf == NULL) {
		return NULL;
	}

	conf->jda_test = NGX_CONF_UNSET;

	return conf;
}

static ngx_int_t
ngx_http_jda_test_handler(ngx_http_request_t *r)
{
	ngx_http_jda_test_loc_conf_t	*jtlcf;

	JDA_HTTP_DEBUG("");

	jtlcf = ngx_http_get_module_loc_conf(r, ngx_http_jda_test_module);

	if (r->internal || !jtlcf->jda_test) {
		return NGX_DECLINED;
	}

	JDA_HTTP_DEBUG("request uri=\"%V\"", &r->uri);

	return NGX_DECLINED;
}

static char *
ngx_http_jda_test_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
	ngx_http_jda_test_loc_conf_t	*prev = parent;
	ngx_http_jda_test_loc_conf_t	*conf = child;

	JDA_CONF_DEBUG("");

	ngx_conf_merge_value(conf->jda_test,
						 prev->jda_test, 1);

	return NGX_CONF_OK;
}

static ngx_int_t
ngx_http_jda_test_add_variables(ngx_conf_t *cf)
{
	ngx_http_variable_t	*var, *v;

	JDA_CONF_DEBUG("");

	for (v = ngx_http_jda_test_vars; v->name.len; v++) {
		var = ngx_http_add_variable(cf, &v->name, v->flags);
		if (var == NULL) {
			return NGX_ERROR;
		}

		JDA_CONF_DEBUG("var.name=\"%V\"", &var->name);

		var->get_handler = v->get_handler;
		var->data = v->data;
	}

	return NGX_OK;
}

static ngx_int_t
ngx_http_jda_test_init(ngx_conf_t *cf)
{
	ngx_http_handler_pt			*h;
	ngx_http_core_main_conf_t	*cmcf;

	JDA_CONF_DEBUG("");

	cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

	h = ngx_array_push(&cmcf->phases[NGX_HTTP_REWRITE_PHASE].handlers);
	if (h == NULL) {
		return NGX_ERROR;
	}

	*h = ngx_http_jda_test_handler;

	return NGX_OK;
}
