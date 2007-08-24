# create all executables

all : F_Allocs F_Classes F_Dump F_TPG F_Make F_Changes

% : %.c
	  SC $<

F_Allocs : F_Allocs.c
F_Classes : F_Classes.c
F_Dump : F_Dump.c
F_TPG : F_TPG.c
F_Vars : F_Vars.c
F_Make : F_Make.c
F_Changes : F_Changes.c

delete :
	  @   Delete F_Allocs F_Classes F_Dump F_TPG F_Make F_Changes *.(o|lnk)

clean :
	  Delete *.(o|lnk)
