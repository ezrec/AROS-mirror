
import sys, commands, os, time, config, os.path

gpFile = 'size2plot.gp'

def generateGP (outputfile, sizefile, title, xaxis = None):
    dict = {
	'outputfile': outputfile,
	'sizefile': sizefile,
	'title': title,
	'xaxis': '',
    }

    if xaxis:
	startDay, interval = xaxis
	dict['xaxis'] = '''set xrange ["%s":]\nset xtics nomirror "%s", %d\n''' % (startDay, startDay, interval)
    else:
	dict['xaxis'] = '''set xtics nomirror %d\n''' % (3600*24*180)
    
    fh = open (gpFile, 'w')
    print dict
    fh.write ('''# This is a gnuplot file
# It creates a picture of the current size of the CVS repository in
set xlabel "Date"
set ylabel "Size in MB" -1,0
set title "%(title)s"
set xdata time
set format x "%%d.%%m.%%Y"
set format y "%%6.2f"
set timefmt "%%d.%%m.%%Y"
set grid
set terminal png small color
set size 1,0.5
set output "%(outputfile)s"
%(xaxis)s#set ytics nomirror 28.0, 1.0
set nomxtics
plot '%(sizefile)s' using 1:($2/1024) title "" with lines
''' % dict)
    fh.close ()

def convert (outputfile, sizefile, title, xaxis = None):
    generateGP (outputfile, sizefile, title, xaxis)

    status, output = commands.getstatusoutput ('%s %s' % (config.GNUPLOT, gpFile))
    if status:
	sys.exit (os.WEXITSTATUS (status))

    os.remove (gpFile)

outputfile, sizefile = sys.argv[1:3]

intervall = 3600*24*15
start=time.time()-(intervall*6)
date = time.strftime ('%d.%m.%Y', time.localtime(start))

titles = {
    'aros': 'Size of AROS sources',
    'contrib': 'Size of contributed sources',
}

title = titles[os.path.basename (outputfile)]

convert (outputfile + '_size.png', sizefile, title, (date, intervall))
title = title + ' (since the beginning)'
convert (outputfile + '_allsize.png', sizefile, title)
