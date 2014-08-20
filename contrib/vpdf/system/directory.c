
#define AROS_ALMOST_COMPATIBLE


#include <exec/lists.h>
#include <exec/types.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <private/vapor/vapor.h>

#include "directory.h"
#include "dlist.h"
#include "functions.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>

/*
 * Helper functions
 */

char *directoryGetNodeName(struct directory *dir, struct directorynode *node)
{
	char *name = NULL;

	if (dir != NULL && node != NULL)
	{
		int l = strlen(dir->basename) + 1 + strlen(node->name) + 1;
		
		name = malloc(l);

		if (name != NULL)
		{
			strcpy(name, dir->basename);
			AddPart(name, node->name, l);
		}
	}

	return name;
}

void directoryFreeNodeName(char *name)
{
	free(name);
}

struct directorynode *directoryFindNode(struct directory *dir, char *name)
{
	struct directorynode *n;

	if (name[ strlen(name) - 1 ] == '/')
	{
		name[ strlen(name) - 1 ] = 0;
	}

	if (FilePart(name) != NULL && strlen(FilePart(name)) > 0)
		name = FilePart(name);


	ITERATELIST(n, &dir->list)
	{
		if (0 == stricmp(n->name, name))
			return n;
	}

	return NULL;

}


char *directoryGetParentName(char *dir)
{
	char path[ 1024 ];
	char *newpath = NULL;
	BPTR lock;

	if (dir == NULL || strlen(dir) == 0)
		return NULL;

	strncpy(path, dir, sizeof(path));

	lock = Lock(path, ACCESS_READ);

	if (lock)
	{
		/* we are in a valid path. use the "good" way */

		BPTR parentLock = ParentDir(lock);
		UnLock(lock);

		if (parentLock && NameFromLock(parentLock, path, sizeof(path)))
		{
			newpath = strdup(path);
		}

		if (parentLock)
			UnLock(parentLock);
	}
	else
	{
		/* we are in some strange path. try to find parent ourself */

		if (strlen(path))
		{
			char *prevPath;

			if (path[ strlen(path) - 1 ] == '/')
			{
				path[ strlen(path) - 1 ] = 0;
			}

			prevPath = PathPart(path);

			path[ prevPath - path ] = 0;

			newpath = strdup(path);

		}
	}

	return newpath;
}

/*
 * Sort directory tree (single level!).
 */

static int dirnodecompare(struct MinNode *n1, struct MinNode *n2)
{
	struct directorynode *dn1 = (struct directorynode*)n1;
	struct directorynode *dn2 = (struct directorynode*)n2;

	if ((dn1->dir && dn2->dir) || (!dn1->dir && !dn2->dir) )
	{
		return stricmp(dn1->name, dn2->name);
	}
	else
	{
		if (dn1->dir && !dn2->dir)
			return -1;
		else
			return 1;
	}
}

static void directorySort(struct directory *dir)
{
	mergesortlist(&dir->list, dirnodecompare);
}


/*
 * Scan directory tree.
 */

struct directory *directoryScan(const char *path, int flags)
{
	BPTR dirLock = Lock(path, ACCESS_READ);
	struct FileInfoBlock fib;
	struct directory *directory = malloc(sizeof(struct directory) + strlen(path) + 1);

	if (dirLock == NULL || directory == NULL)
	{
		free(directory);

		if (dirLock)
			UnLock(dirLock);

		return NULL;
	}

	NEWLIST(&directory->list);
	strcpy(directory->basename, path);

	if (Examine(dirLock, &fib))
	{
		if (isDir(&fib))
		{
			/* fine. we can start examining the entries */

			while(ExNext(dirLock, &fib))
			{
				/* build new name node */

				struct directorynode *n = malloc(sizeof(struct directorynode) + strlen(fib.fib_FileName) + 1);

				if (n)
				{
					strcpy(n->name, fib.fib_FileName);
					n->dir = isDir(&fib);
					n->subdir = NULL;

					if (n->dir && (flags & DIRECTORY_SCAN_RECURSIVE))
					{
						char *fullpath = directoryGetNodeName(directory, n);

						if (fullpath)
						{
							n->subdir = directoryScan(fullpath, flags);
							directoryFreeNodeName(fullpath);
						}
					}

					ADDTAIL(&directory->list, n);

				}
			}
		}

		UnLock(dirLock);

	}

	if (flags & DIRECTORY_SORT)
		directorySort(directory);

	/*
	 * Debug dump.
	 */

	if (0)
	{
		struct directorynode *n;


		printf("Contents of: <%s>\n", directory->basename);

		ITERATELIST(n, &directory->list)
		{
			printf("<%s>\n", n->name);
		}
	}

	return	directory;
}

DirectoryTraverseResult	directoryTraverse(const char *path, int flags, DirectoryTraverseResult (*callback)(struct FileInfoBlock *fib, char *path, int level, void *data), void *data)
{
	if (flags & DIRECTORY_SORT)
	{
		int rc = DIRECTORY_TRAVERSE_CONTINUE;
		struct directory *directory = directoryScan(path, DIRECTORY_SORT);
		
		if (directory != NULL)
		{
			struct directorynode *n;

			ITERATELIST(n, &directory->list)
			{
				char *fullpath = directoryGetNodeName(directory, n);
				if (fullpath != NULL)
				{
					if (n->dir == FALSE)
						rc = callback(NULL, fullpath, 0, data);	
					else if (flags & DIRECTORY_SCAN_RECURSIVE)
						rc = directoryTraverse(fullpath, flags, callback, data);
							
					directoryFreeNodeName(fullpath);
				}
				else
				{					
					rc = DIRECTORY_TRAVERSE_ERROR;
					break;
				}
				
				if (rc != DIRECTORY_TRAVERSE_CONTINUE)
					break;
			}
			
			directoryFree(directory);
			
		}
		else
		{
			rc = DIRECTORY_TRAVERSE_ERROR;
		}
		
		return rc;
	}
	else
	{

		BPTR dirLock = Lock(path, ACCESS_READ);
		struct FileInfoBlock fib;
		int bufflen = strlen(path) + 109;
		char *fullpath = malloc(bufflen);
		int rc = DIRECTORY_TRAVERSE_CONTINUE;

		if (dirLock == NULL || fullpath == NULL)
		{
			free(fullpath);
			return DIRECTORY_TRAVERSE_ERROR;
		}

		if (Examine(dirLock, &fib))
		{
			if (isDir(&fib))
			{
				/* fine. we can start examining the entries */

				while(rc == DIRECTORY_TRAVERSE_CONTINUE && ExNext(dirLock, &fib))
				{
					strcpy(fullpath, path);
					AddPart(fullpath, fib.fib_FileName, bufflen);

					if (isDir(&fib) && (flags & DIRECTORY_SCAN_RECURSIVE))
					{
						rc = directoryTraverse(fullpath, flags, callback, data);
						if (rc == DIRECTORY_TRAVERSE_OK)
							rc = DIRECTORY_TRAVERSE_CONTINUE;
					}
					if (!isDir(&fib))
						rc = callback(&fib, fullpath, 0, data);
				}
			}

			UnLock(dirLock);
		}

		free(fullpath);

		if (rc == DIRECTORY_TRAVERSE_ABORTDIR || rc == DIRECTORY_TRAVERSE_CONTINUE)
			rc = DIRECTORY_TRAVERSE_OK;

		return rc;
	}
}

void directoryFree(struct directory *dir)
{
	if (dir != NULL)
	{
		struct directorynode *dn;

		while((dn = (struct directorynode*)GetTail(&dir->list)))
		{
			directoryFree(dn->subdir);
			REMOVE(dn);
			free(dn);
		}

		free(dir);
	}
}

int dirCreatePath(char *path)
{
	int	pos = 0;
	char constructedPath[ 1024 ];

	/* check if it doesn't exist */

	{
		BPTR lock;
		char *eoPath = PathPart(path);
		char t = eoPath[ 0 ];

		/* temporarely cut the file from path */

        eoPath[ 0 ] = 0;

		lock = Lock(path, ACCESS_READ);
		if (lock)
		{
			UnLock(lock);

            /* restore file part */

			eoPath[ 0 ] = t;

			return TRUE;
		}

		/* restore file part */

		eoPath[ 0 ] = t;
	}

	constructedPath[ 0 ] = 0;

	do
	{
		char buff[ 256 ];
		pos	= SplitName(path, '/', buff, pos, sizeof(buff));

		if (pos != -1)	 /* last part is filename itself */
		{
			BPTR dLock;

			AddPart(constructedPath, buff, sizeof(constructedPath));

			dLock = CreateDir(constructedPath);

			if (dLock)
			{
				UnLock(dLock);
			}
			else
			{
			}
		}

	} while(pos != -1);

	return TRUE;
}
