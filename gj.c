#include <u.h>
#include <libc.h>
#include <json.h>

char*
readfile(int fd)
{
	char *b;
	int s, r, n;

	n = 0;
	s = 4096;
	b = malloc(s);
	if(b==nil)
		sysfatal("malloc: %r");
	for(;;){
		r = read(fd, b+n, s-n);
		if(r<0)
			sysfatal("read: %r");
		if(r==0)
			break;
		n += r;
		if(n==s){
			s *= 1.5;
			b = realloc(b, s);
			if(b==nil)
				sysfatal("realloc: %r");
		}
	}
	b[n] = 0;
	return b;			
}

void
walk(char *path, JSON *json)
{
	int i;
	char *s;
	JSONEl *e;

	print("%s = ", path);
	switch(json->t){
	case JSONNull:
		print("null;\n");
		break;
	case JSONBool:
		print("%s;\n", json->n ? "true": "false");
		break;
	case JSONNumber:
		print("%f;\n", json->n);
		break;
	case JSONString:
		print("\"%s\";\n", json->s);
		break;
	case JSONArray:
		print("[];\n");
		for(i=0, e=json->first; e!=nil; i++, e=e->next){
			s = smprint("%s[%d]", path, i);
			walk(s, e->val);
			free(s);
		}
		break;
	case JSONObject:
		print("{};\n");
		for(e=json->first; e!=nil; e=e->next){
			s = smprint("%s.%s", path, e->name);
			walk(s, e->val);
			free(s);
		}
		break;
	}
}

void
main(int argc, char *argv[])
{
	int fd;
	char *s;
	JSON *json;

	fd = 0;
	ARGBEGIN{
	}ARGEND;
	if(argc==1){
		fd = open(*argv, OREAD);
		if(fd<=0)
			sysfatal("open: %r");
	}
	s = readfile(fd);
	close(fd);
	json = jsonparse(s);
	if(json==nil)
		sysfatal("jsonparse: %r");
	walk("json", json);
	jsonfree(json);
	exits(nil);
}
