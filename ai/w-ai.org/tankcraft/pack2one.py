# -*- encoding: utf-8 -*-

import os, re, copy, time

gra_file = {}
map_file = {}

def get_file_include(fn, file_map, file_gra):

	file_gra[fn] = []

	allinc = re.findall(r"#include.*?\".*?\"", file_map[fn])
	#print allinc

	for inc in allinc:
		hf = re.search(r"\".*?\"", inc).group()[1:-1]
		if file_map.has_key(hf):
			file_gra[fn].append(hf)


def get_h_cpp_files(path, file_map):

	dirlist = os.listdir(path)

	for _dir in dirlist:
		if os.path.isdir(os.path.join(path, _dir)):
			get_h_cpp_files(os.path.join(path, _dir), file_map)
		elif os.path.isfile(os.path.join(path, _dir)):
			#print os.path.splitext(_dir)[1]
			if os.path.splitext(_dir)[1] in ('.h', '.cpp'):
				file_map[_dir] = open(os.path.join(path, _dir), "r").read()


def gen_the_one(file_gra, file_map):
	fi_cnt = len(file_gra)

	hlist = []

	for fi in file_gra:
		for inc in file_gra[fi]:
			if inc in hlist:
				pass
			else:
				hlist.append(inc)

			file_map[fi] = re.sub(r"#include.*?\"" + inc + "\"", "// ** include " + inc, file_map[fi]) 

	print hlist

	fout = open("pack_code.cpp", "w")

	print >>fout, "/*******************************************"
	print >>fout, " * Amb"
	print >>fout, " * amb-bai.me"
	print >>fout, " * amb.main@gmail.com"
	print >>fout, " * "
	print >>fout, " * tankcraft"
	print >>fout, " *", time.strftime('%Y-%m-%d',time.localtime(time.time()))
	print >>fout, " *******************************************/"

	for hf in hlist:
		print >>fout, ""
		print >>fout, "//=========================="
		print >>fout, "//", hf
		print >>fout, ""
		print >>fout, file_map[hf]
		print >>fout, ""
		print >>fout, "//=========================="
		print >>fout, ""

	for cppf in file_map:
		if os.path.splitext(cppf)[1] != '.cpp':
			continue
		print >>fout, ""
		print >>fout, "//=========================="
		print >>fout, "//", cppf
		print >>fout, ""
		print >>fout, file_map[cppf]
		print >>fout, ""
		print >>fout, "//=========================="
		print >>fout, ""




get_h_cpp_files("tankcraft", map_file)
#print map_file

for fi in map_file:
	get_file_include(fi, map_file, gra_file)
#print gra_file

gen_the_one(gra_file, map_file)