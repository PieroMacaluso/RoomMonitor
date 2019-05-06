#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "query_resolver.h"

static inline char *
find_and_terminate(char *str, char find);
static inline char *
find_query(char *str);
struct query_t * query_init(int n_param);
int query_parse(struct query_t *url, char *u);
void query_free(struct query_t *url);
int query_start_parse(struct query_t *url /*char *query, char delimiter, struct yuarel_param *params, int max_params*/);
/**
 * Find a character in a string, replace it with '\0' and return the next
 * character in the string.
 *
 * str: the string to search in.
 * find: the character to search for.
 *
 * Returns a pointer to the character after the one to search for. If not
 * found, NULL is returned.
 */
static inline char *
find_and_terminate(char *str, char find)
{
	str = strchr(str, find);
	if (NULL == str)
	{
		return NULL;
	}

	*str = '\0';
	return str + 1;
}

static inline char *
find_query(char *str)
{
	return find_and_terminate(str, '?');
}

struct query_t * query_init(int n_param)
{
	struct query_t * url;
	url = malloc(sizeof(struct query_t));
	url->npar = n_param;
	url->params = malloc(url->npar*sizeof(struct query_param));
	if (url->params == NULL) return NULL;
	// memset(url, 0, sizeof(struct yuarel));
	return url;
}

/**
 * Parse a URL string to a struct.
 *
 * url: pointer to the struct where to store the parsed URL parts.
 * u:   the string containing the URL to be parsed.
 *
 * Returns 0 on success, otherwise -1.
 */
int query_parse(struct query_t *url, char *u)
{

	// memset(url, 0, sizeof(struct yuarel));

	/* (Query) */
	url->query = find_query(u);
	int ret = query_start_parse(url);

	return ret;
}

void query_free(struct query_t *url)
{
	int i;
	free(url->params);
	free(url);
}

int query_start_parse(struct query_t *url /*char *query, char delimiter, struct yuarel_param *params, int max_params*/)
{
	int i = 0;
	if (NULL == url->query || '\0' == *(url->query))
	{
		return -1;
	}
	char *query = url->query;

	url->params[i++].key = query;
	while (i < url->npar && NULL != (query = strchr(query, '&')))
	{
		*query = '\0';
		url->params[i].key = ++query;
		url->params[i].val = NULL;

		/* Go back and split previous param */
		if (i > 0)
		{
			if ((url->params[i - 1].val = strchr(url->params[i - 1].key, '=')) != NULL)
			{
				*(url->params[i - 1].val)++ = '\0';
			}
		}
		i++;
	}

	/* Go back and split last param */
	if ((url->params[i - 1].val = strchr(url->params[i - 1].key, '=')) != NULL)
	{
		*(url->params[i - 1].val)++ = '\0';
	}

	return i;
}
