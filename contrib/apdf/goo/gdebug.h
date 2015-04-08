//========================================================================
//
// gdebug.h
//
// Copyright 2000-2001 Emmanuel Lesueur
//
//========================================================================

#ifndef GDEBUG_H
#define GDEBUG_H

#if defined(DEBUG_MEM) && defined(__GNUC__)

class GDebug {
public:

  class Context {
  public:
    Context(const char *name1): name(name1), prev(current) { current = this; }
    ~Context() { current = prev; }

    static void traceBack();
    static const char **save();
    static void free(const char **context) { delete [] context; }
    static void print(const char **);
  private:
    static Context *current;
    Context *prev;
    const char *name;
  };

  class Tracker {
    struct Tracked;
    struct TrackedArray;
    static const int size = 1024;
  public:
    Tracker();
    virtual ~Tracker();
    void track(void *);
    void unTrack(void *);
  protected:
    virtual void show(void *) =0;
  private:
    Tracker(const Tracker&);
    Tracker& operator = (const Tracker&);
    TrackedArray *tracked;
  };

};


#define DEBUG_INFO                      GDebug::Context gDebug_Context(__PRETTY_FUNCTION__);

#else

#define DEBUG_INFO

#endif

#endif 

