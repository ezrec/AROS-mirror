#ifndef PATTERNMATCHER_H
#define PATTERNMATCHER_H

struct PatternMatcherMsg
{
	struct Message msg;
	char *pattern;
	char *string;
	WORD rc;
};

extern ULONG patternmatchermask;

void InitPatternMatcher(void);
void CleanupPatternMatcher(void);

void HandlePatternMatcher(void);
BOOL SendPatternMatcherMsg(char *pattern,char *string);

#endif

