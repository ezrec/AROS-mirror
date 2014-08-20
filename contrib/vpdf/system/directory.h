#ifndef SYSTEM_DIRECTORY_H
#define SYSTEM_DIRECTORY_H

#include <exec/lists.h>

struct directory
{
	struct MinList list;
	char basename[0];
};


struct directorynode
{
	struct MinNode n;
	int dir;
	struct directory *subdir;
	char name[0];
};

#define DIRECTORY_SCAN_RECURSIVE  1
#define DIRECTORY_SORT  2

typedef enum DirectoryTraverseResult
{
	DIRECTORY_TRAVERSE_ABORT = -1,
	DIRECTORY_TRAVERSE_ABORTDIR = -2,
	DIRECTORY_TRAVERSE_CONTINUE = 1,
	DIRECTORY_TRAVERSE_OK = 0,
	DIRECTORY_TRAVERSE_ERROR = -3,
} DirectoryTraverseResult;

struct directory *directoryScan(const char *path, int flags);
DirectoryTraverseResult	directoryTraverse(const char *path, int flags, DirectoryTraverseResult (*callback)(struct FileInfoBlock *fib, char *path, int level, void *data), void *data);
void directoryFree(struct directory *dir);

char *directoryGetNodeName(struct directory *dir, struct directorynode *node);
void directoryFreeNodeName(char *name);
struct directorynode *directoryFindNode(struct directory *dir, char *name);
char *directoryGetParentName(char *dir);

int	dirCreatePath(char *path);

#endif
