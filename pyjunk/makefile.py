import sys
import os

if len(sys.argv) < 2:
	print 'No input file.'
	sys.exit()

if sys.argv[1].endswith('cpp'):
	outfile = sys.argv[1][:-4] + '.exe'
	if os.path.isfile(outfile):
		os.remove(outfile)
	makecmd = "g++ -o "+ outfile + " " + sys.argv[1]
	print makecmd
	if os.system(makecmd) == 0:
		if os.path.isfile(outfile):
			runcmd = outfile
			print runcmd
			os.system(outfile)
	else:
		print 'G++ Complier Error!'
elif sys.argv[1].endswith('java'):
	outfile = sys.argv[1][:-5] + '.class'
	if os.path.isfile(outfile):
		os.remove(outfile)
	makecmd = "javac " + sys.argv[1]
	print makecmd
	if os.system(makecmd) == 0:
		if os.path.isfile(outfile):
			runcmd = 'java ' + sys.argv[1][:-5]
			print runcmd
			os.system(runcmd)
	else:
		print 'JRE Error!'


