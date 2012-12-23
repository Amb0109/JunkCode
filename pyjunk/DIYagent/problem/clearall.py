import os, re

files = os.listdir(os.getcwd())
for f in files:
	if re.match(r".*?pyc?$", f) and f != "makedata.py" and f != "clearall.py":
		print f + " ??? py"
		os.remove(f)
		continue
	if re.match(r".*?info$", f) and f != "init.info":
		print f + " ??? info"
		os.remove(f)
		continue
	if re.match(r".*?tmp$", f):
		print f + " ??? tmp"
		os.remove(f)
		continue
	if re.match(r".*?cpp~$", f) or re.match(r".*?o$", f) or re.match(r".*?exe$", f):
		print f + " ??? cpp"
		os.remove(f)
		continue
	if re.match(r".*?html$", f):
		print f+" ??? html"
		os.remove(f)
		continue

a = raw_input("FINAL CLEAR (y/n): ")

if a == "y":
	files = os.listdir(os.getcwd())
	for f in files:
		if re.match(r".*?py$", f):
			os.remove(f)
			continue
		if re.match(r".*?info$", f):
			os.remove(f)
			continue

