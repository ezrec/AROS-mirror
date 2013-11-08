Rar_Extractor 0.2.1
-------------------
Rar_Extractor allows efficient access to files within RAR archives. It is
meant for programs which transparently access RAR-compressed files and groups
of files. All I/O is done through the Rar_Reader and Rar_Writer interface
classes; no OS calls are made. A C wrapper is included, using the unrarlib
0.4.0 interface (see http://www.unrarlib.org).

It is based on UnRAR 3.5.1 by Alexander L. Roshal. The original sources have
been heavily modified, trimmed down, and purged of all OS-specific calls for
file access and other unnecessary operations. Refer to unrar_license.txt for
full licensing terms. The original unRAR source is available at
http://www.rarlab.com

Licensed under the same license as the original unRAR sources; see
unrar_license.txt.

Website: http://www.slack.net/~ant/libs/
Contact: Shay Green <hotpop.com@blargg> (swap to e-mail)


Getting Started
---------------
Build a program consisting of demo.cpp and all source files in the unrar/
directory. Running the program with an RAR archive "test.rar" in the same
directory should generate a list of the files it contains.

See notes.txt for more information and Rar_Extractor.h and unrarlib/unrarlib.h
for reference.


Files
-----
notes.txt               General notes about the library
changes.txt             Changes since previous releases
unrar_license.txt       unRAR license

demo.cpp                How to use Rar_Extractor in C++
demo_unrarlib.c         How to use libunrar in C

unrarlib/               C wrapper with unrarlib 0.4.0 compatible interface
  unrarlib.h
  unrarlib.cpp

unrar/                  Rar_Extractor sources
  abstract_file.h       Data_Reader/Data_Writer interface
  abstract_file.cpp
  Rar_Extractor.h       Rar_Extractor interface
  Rar_Extractor.cpp
  rar_archive.cpp       Internal source
  rar_archive.hpp
  rar_arcread.cpp
  rar_array.hpp
  rar_common.hpp
  rar_compress.hpp
  rar_crc.cpp
  rar_crc.hpp
  rar_getbits.cpp
  rar_getbits.hpp
  rar_headers.hpp
  rar_model.cpp
  rar_model.hpp
  rar_rarvm.cpp
  rar_rarvm.hpp
  rar_rawread.cpp
  rar_rawread.hpp
  rar_rdwrfn.cpp
  rar_rdwrfn.hpp
  rar_rs.cpp
  rar_rs.hpp
  rar_suballoc.cpp
  rar_suballoc.hpp
  rar_suballoc_inl.h
  rar_unpack.cpp
  rar_unpack.hpp
  rar_unpack15.cpp
  rar_unpack20.cpp
  
-- 
Shay Green <hotpop.com@blargg> (swap to e-mail)
