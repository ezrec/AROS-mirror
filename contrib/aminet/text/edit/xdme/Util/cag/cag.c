
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct node
{
    char name[32];
    char title[32];
    char body[256];
};

struct cmd
{
    char nodes[40];
    char name[32];
    char args[80];
    char text[256];
};

struct desc
{
    char cmds[40];
    int begin;
    int len;
};

char header[1024];

#define MAXNODES    40
#define MAXCMDS     300
#define MAXDESCS    50

struct node * node;
struct cmd * cmd;
struct desc * desc;

struct cmd * cmdtable[MAXCMDS];

int nnodes, ncmds, ndescs, lineno;

int cmp_nodes (const struct node * n1, const struct node * n2)
{
    return (strcmp (n1->title, n2->title));
}

int cmp_cmds (const struct cmd * c1, const struct cmd * c2)
{
    return (strcmp (c1->name, c2->name));
}

int cmp_cmdtable (const struct cmd ** c1, const struct cmd ** c2)
{
    return (strcmp ((*c1)->name, (*c2)->name));
}

void add_node (char * line)
{
    char * name, * longname;

    if (nnodes >= MAXNODES)
	exit (10);

    line += 5;
    while (isspace (*line)) line ++;

    name = strtok (line, "\t ");
    longname = strtok (NULL, "\n");

    strcpy (node[nnodes].name, name);
    strcpy (node[nnodes].title, longname);
    node[nnodes].body[0] = 0;

    nnodes ++;
} /* add_node */


void add_cmd (char * line)
{
    char * nodes, * name, * args;
    int t;

    if (ncmds >= MAXCMDS)
	exit (10);

    line += 4;
    while (isspace (*line)) line ++;
    if (!*line)
    {
	fprintf (stderr, "#cmd: unexpected EOL in line %d (%d)\n", lineno, __LINE__);
	exit (10);
    }

    nodes = line;
    while (!isspace (*line) && *line) line ++;

    if (!*line)
    {
	fprintf (stderr, "#cmd: unexpected EOL in line %d (%d)\n", lineno, __LINE__);
	exit (10);
    }

    *line = 0;
    line ++;

    while (isspace (*line)) line ++;
    if (!*line)
    {
	fprintf (stderr, "#cmd: unexpected EOL in line %d (%d)\n", lineno, __LINE__);
	exit (10);
    }

    name = line;
    while (!isspace (*line) && *line) line ++;

    if (!*line)
    {
	fprintf (stderr, "#cmd: unexpected EOL in line %d (%d)\n", lineno, __LINE__);
	exit (10);
    }

    *line = 0;
    line ++;

    while (isspace (*line) && *line) line ++;
    if (!*line)
    {
	fprintf (stderr, "#cmd: unexpected EOL in line %d (%d)\n", lineno, __LINE__);
	exit (10);
    }

    if (*line != '-')
    {
	args = line;

	while (*line)
	{
	    if (isspace(*line) && line[1] == '-')
		break;

	    line ++;
	}

	if (!*line)
	{
	    fprintf (stderr, "#cmd: unexpected EOL in line %d (%d)\n", lineno, __LINE__);
	    exit (10);
	}

	*line = 0;

	for (t=strlen(args)-1; t>=0; t--)
	    if (!isspace (args[t]))
		break;

	args[t+1] = 0;
	line += 2;
    }
    else
    {
	args = line;
	*line = 0;
	line ++;
    }

    while (isspace (*line)) line ++;
    if (!*line)
    {
	fprintf (stderr, "#cmd: unexpected EOL in line %d (%d)\n", lineno, __LINE__);
	exit (10);
    }

    strcpy (cmd[ncmds].nodes, nodes);
    strcpy (cmd[ncmds].name, name);
    strcpy (cmd[ncmds].args, args);
    strcpy (cmd[ncmds].text, line);

    ncmds ++;
} /* add_node */

void add_desc (char * line, FILE * fh)
{
    int len = strlen (line);

    line[len-1] = 0;

    if (ndescs >= MAXNODES)
	exit (10);

    line += 5;
    while (isspace (*line)) line ++;

    strcpy (desc[ndescs].cmds, line);
    desc[ndescs].begin = ftell (fh);

    if (ndescs)
	desc[ndescs-1].len = desc[ndescs].begin - len - desc[ndescs-1].begin;

    ndescs ++;
} /* add_desc */

int isinlist (char * elem, char * list)
{
    char * pos;
    int len;

    pos = strstr (list, elem);
    len = strlen (elem);

    /* is inside a list, if it is the first word or the last one or
       delimited by ',' */
    return (pos && (pos == list || pos[-1] == ',') && !isalpha(pos[len]));
} /* isinlist */

char fmt_buf[100];
#define MAXWIDTH 78
#define INDENT 20

void fmt_cmd (FILE * out, struct cmd * cmd)
{
    int t, len, i;
    char * last_space, * curr, * text;

    for (t=0; t<ndescs; t++)
	if (isinlist (cmd->name, desc[t].cmds))
	    break;

    if (t != ndescs) /* found */
    {
	fprintf (out, "@{\" %s \" LINK \"desc%d\"}", cmd->name, t);
	len = strlen (cmd->name)+3;

	if (cmd->args[0])
	{
	    fprintf (out, " @{B}%s@{UB}", cmd->args);
	    len += strlen (cmd->args)+1;
	}
    }
    else
    {
	fprintf (out, "@{B}%s", cmd->name);
	len = strlen (cmd->name);

	if (cmd->args[0])
	{
	    fprintf (out, " %s", cmd->args);
	    len += strlen (cmd->args)+1;
	}

	fprintf (out, "@{UB}");
    }

    if (len >= INDENT)
    {
	fputc (' ', out);
	len ++;
    }
    else while (len < INDENT)
    {
	fputc (' ', out);
	len ++;
    }

    last_space = NULL;
    curr = fmt_buf;
    text = cmd->text;

    while (*text)
    {
	if (len > MAXWIDTH)
	{
	    *curr = 0;

	    if (!last_space)
		last_space = curr;
	    else
		last_space[-1] = 0;

	    fprintf (out, "%s\n", fmt_buf);

	    strcpy (fmt_buf, last_space);

	    for (len=0; len < INDENT; len++)
		fputc (' ', out);

	    last_space = NULL;

	    i = strlen (fmt_buf);
	    curr = fmt_buf + i;
	    len += i;
	}
	else
	{
again:
	    *curr = *text ++;

	    if (isspace (*curr))
		last_space = curr+1;
	    else if (*curr == '@' && *text == '{')
	    {
		curr ++;

		while (*text != '}')
		{
		    *curr ++ = *text ++;
		}

		goto again; /* copy } too */
	    }

	    len ++;
	    curr ++;
	}
    }

    *curr = 0;

    fprintf (out, "%s", fmt_buf);
} /* fmt_cmd */


int main (int argc, char ** argv)
{
    FILE * in;
    char puffer[1024];
    char line[256];
    int llen;
    int state;
    int t, i, j, nx;

    if (!(in = fopen (argv[1], "r")) )
	exit (10);

    if (!(node = malloc (sizeof (struct node) * MAXNODES)) )
	exit (10);

    if (!(cmd = malloc (sizeof (struct cmd) * MAXCMDS)) )
	exit (10);

    if (!(desc = malloc (sizeof (struct desc) * MAXDESCS)) )
	exit (10);

    state = 0;

    *header = 0;
    *puffer = 0;

    while (fgets (line, sizeof(line), in))
    {
	llen = strlen (line);
	lineno ++;

	switch (state)
	{
	case 0: /* header */
	    if (strncmp ("#toc", line, 4))
		strcat (header, line);
	    else
		state = 1;

	    break;

	case 1: /* parse #node */
	    if (!strncmp ("#node", line, 5))
	    {
		add_node (line);
		state = 2;
	    }

	    break;

	case 2: /* parse node-body until next node or #cmd */
	    if (!strncmp ("#node", line, 5))
	    {
		add_node (line);
		state = 2;
	    }
	    else if (!strncmp ("#cmd", line, 4))
	    {
		add_cmd (line);
		state = 3;
	    }
	    else
		strcat (node[nnodes-1].body, line);

	    break;

	case 3: /* parse #cmd until #long */
	    if (!strncmp ("#cmd", line, 4))
	    {
		add_cmd (line);
	    }
	    else if (!strncmp ("#long", line, 5))
	    {
		add_desc (line, in);
		state = 4;
	    }

	    break;

	case 4: /* parse #long until EOF */
	    if (!strncmp ("#long", line, 5))
	    {
		add_desc (line, in);
		state = 4;
	    }

	    break;
	}
    }

    if (ndescs)
	desc[ndescs-1].len = ftell (in) - llen - desc[ndescs-1].begin;

    rewind (in);

    qsort (node, nnodes, sizeof (struct node),
	    (int (*)(const void *,const void *))cmp_nodes);
    qsort (cmd, ncmds, sizeof (struct cmd),
	    (int (*)(const void *,const void *))cmp_cmds);

    fputs (header, stdout);

    llen = 0;

    for (t=0; t<nnodes; t++)
    {
	i = strlen (node[t].title);

	if (i > llen)
	    llen = i;
    }

    for (t=0; t<nnodes; t++)
	fprintf (stdout, "    @{\" %*s \" LINK \"%s\"}\n", -llen, node[t].title, node[t].name);

    fprintf (stdout, "\n@ENDNODE \"MAIN\"\n\n");

    for (t=0; t<nnodes; t++)
    {
	fprintf (stdout, "@NODE \"%s\" \"%s\"\n", node[t].name, node[t].title);
	fprintf (stdout, "@TOC \"MAIN\"\n\n");
	fputs (node[t].body, stdout);

	nx=0;

	for (i=0; i<ncmds; i++)
	    if (isinlist (node[t].name, cmd[i].nodes))
		cmdtable[nx++] = &cmd[i];

	if (nx)
	{
	    qsort (cmdtable, nx, sizeof (struct cmd *),
		    (int (*)(const void *,const void *))cmp_cmdtable);

	    for (i=0; i<nx; i++)
	    {
		fmt_cmd (stdout, cmdtable[i]);
	    }
	}
	else
	    fprintf (stderr, "Unused nodes: %s\n", node[t].name);

	fprintf (stdout, "@ENDNODE \"%s\"\n\n", node[t].name);
    }

    for (t=0; t<ndescs; t++)
    {
	sprintf (line, "desc%d", t);

	fprintf (stdout, "@NODE \"%s\" \"%s\"\n", line, desc[t].cmds);
	fprintf (stdout, "@TOC \"MAIN\"\n\n");

	for (i=0; i<ncmds; i++)
	    if (isinlist (cmd[i].name, desc[t].cmds))
		fprintf (stdout, "@{B}%s %s@{UB}\n", cmd[i].name, cmd[i].args);

	fseek (in, desc[t].begin, SEEK_SET);
	for (i=desc[t].len; i>0; i-=j)
	{
	    j = i < sizeof (puffer) ? i : sizeof (puffer);

	    j = fread (puffer, 1, j, in);
	    if (j < 0)
		exit (10);

	    fwrite (puffer, 1, j, stdout);
	}
	/* fprintf (stdout, "insert %d bytes from %d\n", desc[t].len, desc[t].begin); */

	fprintf (stdout, "@ENDNODE \"%s\"\n\n", line);
    }

    fprintf (stdout, "@NODE \"index\" \"Index\"\n");
    fprintf (stdout, "@TOC \"MAIN\"\n\n");

    for (t=0; t<ncmds; t++)
	fmt_cmd (stdout, &cmd[t]);

    fprintf (stdout, "@ENDNODE \"index\"\n\n");

    return 0;
} /* main */
