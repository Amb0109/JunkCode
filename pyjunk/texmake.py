import sys
import os

if len(sys.argv) < 2:
	print 'No input file.'
	sys.exit()

if sys.argv[1].endswith('tex'):
	filen = sys.argv[1]
	tocfile = sys.argv[1][:-4] + '.toc'
	auxfile = sys.argv[1][:-4] + '.aux'
	logfile = sys.argv[1][:-4] + '.log'
	dvifile = sys.argv[1][:-4] + '.dvi'
	pdffile = sys.argv[1][:-4] + '.pdf'
	if os.path.isfile(tocfile):
		os.remove(tocfile)
	if os.path.isfile(auxfile):
		os.remove(auxfile)
	if os.path.isfile(logfile):
		os.remove(logfile)
	if os.path.isfile(dvifile):
		os.remove(dvifile)
	if os.path.isfile(pdffile):
		os.remove(pdffile)
	cmdline = 'latex -src-specials ' + filen
	os.system(cmdline)
	for i in range(2,len(sys.argv)):
		if sys.argv[i] == '-pdf':
			cmdline = 'pdflatex ' + filen
			os.system(cmdline)
		if sys.argv[i] == '-ps':
			cmdline = 'dvips ' + dvifile
			os.system(cmdline)
		elif sys.argv[i] == '-c':
			if os.path.isfile(tocfile):
				os.remove(tocfile)
			if os.path.isfile(auxfile):
				os.remove(auxfile)
			if os.path.isfile(logfile):
				os.remove(logfile)


