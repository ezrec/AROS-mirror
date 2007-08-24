#!python
#
# File:     F_CreateFCCInfo.py
#
# Date:     2005/05/22
# Author:   Guillaume ROGUEZ <yomgui1@gmail.com>
#

def writeInfoFile(name, version=1, revision=0,
				  copyright='written by Olivier LAVIALE (www.gofromiel.com)',
				  destination='fcc_info.c',
				  date=None,
				  dry=False,
				  verbose=False):

	from time import localtime

	if not date:
		date = "%04u/%02u/%02u" % localtime()[:3]

	version = int(version)
	revision = int(revision)
	name += '.fc'

	if verbose:
		print "Generating FCC info file '%s' (%lu.%lu)..." % (destination, version, revision)

	if not dry:
		f = open(destination, 'w')
		f.write('const char _FCC_VersionString[] = "\\0$VER: %s %lu.%lu (%s) %s";\n' %
				(name, version, revision, date, copyright))
		f.write('const char _FCC_LibName[] = "%s";\n' % name)
		f.write('unsigned long _FCC_Version = %lu;\n' % version)
		f.write('unsigned long _FCC_Revision = %lu;\n' % revision)
		f.close()

if __name__ == "__main__":
	import sys

	args = sys.argv[1:]

	try:
		printusage = args.index('?')
	except:
		printusage = False

	if (len(args) < 4) or printusage:
		print "%s N=NAME/A, V=VER/A/N, R=REV/A/N, C=COPYRIGHT/A/K, DST=DESTINATION/K, DATE/K" % sys.argv[0]
		if not printusage:
			sys.exit(20)
		else:
			try:
				while True:
					args.remove('?')
			except:
				pass

	writeInfoFile(args)
