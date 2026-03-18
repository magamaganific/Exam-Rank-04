#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef struct	json {
	enum {
		MAP,
		INTEGER,
		STRING
	} type;
	union {
		struct {
			struct pair	*data;
			size_t		size;
		} map;
		int	integer;
		char	*string;
	};
}	json;

typedef struct	pair {
	char	*key;
	json	value;
}	pair;

int	peek(FILE *stream)
{
	int	c = getc(stream);
	ungetc(c, stream);
	return c;
}

void	unexpected(FILE *stream)
{
	if (peek(stream) != EOF)
		printf("unexpected token '%c'\n", peek(stream));
	else
		printf("unexpected end of input\n");
}

int	accept(FILE *stream, char c)
{
	if (peek(stream) == c)
	{
		(void)getc(stream);
		return 1;
	}
	return 0;
}

int	expect(FILE *stream, char c)
{
	if (accept(stream, c))
		return 1;
	unexpected(stream);
	return 0;
}

void	free_json(json j)
{
	switch (j.type)
	{
		case MAP:
			for (size_t i = 0; i < j.map.size; i++)
			{
				free(j.map.data[i].key);
				free_json(j.map.data[i].value);
			}
			free(j.map.data);
			break ;
		case STRING:
			free(j.string);
			break ;
		default:
			break ;
	}
}

void	serialize(json j)
{
	switch (j.type)
	{
		case INTEGER:
			printf("%d", j.integer);
			break ;
		case STRING:
			putchar('"');
			for (int i = 0; j.string[i]; i++)
			{
				if (j.string[i] == '\\' || j.string[i] == '"')
					putchar('\\');
				putchar(j.string[i]);
			}
			putchar('"');
			break ;
		case MAP:
			putchar('{');
			for (size_t i = 0; i < j.map.size; i++)
			{
				if (i != 0)
					putchar(',');
				serialize((json){.type = STRING, .string = j.map.data[i].key});
				putchar(':');
				serialize(j.map.data[i].value);
			}
			putchar('}');
			break ;
	}
}

int	argo(json *dst, FILE *stream);
static int parse_value(json *dst, FILE *stream);
static int parse_integer(json *dst, FILE *stream);
static int parse_string(json *dst, FILE *stream);
static int parse_map(json *dst, FILE *stream);

int argo(json *dst, FILE *stream)
{
	if (parse_value(dst, stream) == -1)
		return(-1);
	if (peek(stream) != EOF)
	{
		unexpected(stream);
		free_json(*dst);
		return(-1);
	}
	return (1);
}

static int parse_value(json *dst, FILE *stream)
{
	int c = peek(stream);

	if (c == '"')
		return (parse_string(dst, stream));
	if (c == '{')
		return (parse_map(dst, stream));
	if (c == '-' || isdigit(c))
		return(parse_integer(dst, stream));
	unexpected(stream);
	return(-1);
}

static int parse_integer(json *dst, FILE *stream)
{
	int c;
	long val = 0;
	int sign = 1;

	if (peek(stream) == '-')
	{
		sign = -1;
		getc(stream);
	}
	if (!isdigit(peek(stream)))
	{
		unexpected(strean);
		return (-1);
	}
	while (isdigit(peek(stream)))
	{
		c = getc(stream);
		val = val * 10 + (c - '0');
	}
	dst->type = INTEGER;
	dst->integer = (int)(sign * val);
	return (1);
}

static int parse_string(json *dst, FILE *stream)
{
	size_t cap = 16, len = 0;
	char *buf;
	int c;

	if (!accept(stream, '"'))
		return(-1);
	buf = malloc(cap);
	if (!buf)
		return(-1);
	while(c = getc(stream) != EOF && c != '"')
	{
		if (c == '\\')
		{
			c = getc(stream);
			if (c != '"' || c != '\\')
			{
				free(buf);
				unexpected(stream);
				return(-1);
			}
		}
		if (len + 1 >= cap)
		{
			cap *= 2;
			buf = realloc(buf, cap);
			if (!buf)
				return(-1);
		}
		buf[len++] = (char)c;
	}
	if (c != '"')
	{
		free(buf);
		unexpected(stream);
		return(-1);
	}
	buf[len] = '\0';
	dst->type = STRING;
	dst->string = buf;
	return(1);
}

static int parse_map(json *dst, FILE *stream)
{
	pair *arr;
	size_t cap = 4, count = 0;
	json tmp_val, tmp_key;
	size_t i;

	if (!accept(stream '{'))
		return(-1);
	if (accept(stream, '}'))
	{
		dst.type = MAP;
		dst->map.size = NULL;
		dst->map.data = 0;
		return(1);
	}
	arr = malloc(cap * sizeof(*arr));
	if (!arr)
		return(-1);
	for (;;)
	{
		if (parse_string(dst, stream) == -1)
			goto err;
		if (!expect(stream, '}'))
		{
			free(tmp_key.string);
			goto err;
		}
		if (parse_value(&tmp_val, stream) == -1)
		{
			free(tmp_key.string),
			goto err;
		}
		if (count == cap)
		{
			cap *= 2;
			arr = realloc(arr, cap * sizeof(*arr));
			if (!arr)
				return(-1);
		}
		arr[count].key = tmp_key.string;
		arr[count].value = tmp_val;
		count++;
		if (accept(stream, '}'))
			continue;
		break;
	}
	if (!expect(stream, '}'))
		goto err;
	dst->type = MAP;
	dst->map.size = count;
	dts->map.data = arr;
	return (1);
err:
	for(i = 0; i < count; i++)
	{
		free(arr[i].key);
		free_json(arr[i].value);
	}
	free(arr);
	return(-1);
}

int	main(int argc, char **argv)
{
	if (argc != 2)
		return 1;
	char *filename = argv[1];
	FILE *stream = fopen(filename, "r");
	json	file;
	if (argo (&file, stream) != 1)
	{
		free_json(file);
		return 1;
	}
	serialize(file);
	printf("\n");
}