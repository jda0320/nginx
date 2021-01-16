
/*
 * Copyright (C) Daeun Jeong
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


void
ngx_jda_strcopy(ngx_pool_t *pool, ngx_str_t *src, ngx_str_t *dst)
{
	if (src == NULL || src->len == 0|| src->data == NULL || dst == NULL) {
		return;
	}

	dst->data = ngx_pcalloc(pool, src->len);
	if (dst->data == NULL) {
		return;
	}

	dst->len = src->len;
	memcpy(dst->data, src->data, dst->len);
}
