# -*- coding: utf-8 -*-

import os
import sys
import shutil
import re
import json



tp_exe = "...TexturePacker.exe"

export_dir = "Export\\"
resources_dir = "Resources\\"

out_dir = "Export_TP\\"
out_res = "Resources_TP\\"
out_opt = [
"--data %s{n}.plist",
"--sheet %s{n}.png",
"--format cocos2d-original",
"--algorithm MaxRects",
"--multipack",
"--padding 1",
"--disable-rotation",
"--pack-mode Good",
"--size-constraints AnySize",
# "--png-opt-level 0",
# "--ignore-files *",
out_res
]


GBK	  = lambda _str: _str.encode("gbk")
UPATH = lambda path: path if isinstance(path, unicode) else unicode(path, "utf-8")
UJOIN = lambda prefix, path: UPATH(os.path.join(UPATH(prefix), UPATH(path)))
UOPEN = lambda path, file, mode: codecs.open(UJOIN(path, file), mode, 'utf-8')
GOPEN = lambda path, file, mode: codecs.open(UJOIN(path, file), mode, 'gbk')

def LOG(path, data):
	print GBK(data)
	print >>open(UPATH(path), "a"), GBK(data)

def COPY(src_dir, dst_dir, fpath):
	fpath = fpath.replace("/", "\\")
	dirs = fpath.split("\\")[:-1]
	org_dir = os.getcwd()
	os.chdir(UPATH(dst_dir))
	for d in dirs:
		if len(d) <= 0: continue
		if not os.path.isdir(d):
			os.makedirs(d)
		os.chdir(d)
	os.chdir(org_dir)
	
	if os.path.isfile(UJOIN(src_dir, fpath)):
		shutil.copy(UJOIN(src_dir, fpath), UJOIN(dst_dir, fpath))

def find_files_in_dir(dir_path, sub_dir = None):

	if None == sub_dir:
		sub_dir = u".\\"

	os.chdir(dir_path)

	if not os.path.isdir(dir_path):
		return None

	file_list = []
	sub_path_list = os.listdir(sub_dir)
	for sub_path_name in sub_path_list:
		if len(sub_path_name) <= 0: continue
		if sub_path_name[0] == '.': continue

		sub_path = UJOIN(sub_dir, sub_path_name)
		if os.path.isdir(sub_path):
			file_list.extend(find_files_in_dir(dir_path, sub_path))
		elif os.path.isfile(sub_path):
			file_list.append(sub_path[2:])

	return file_list

def open_json(json_file):
	fp = open(json_file, "r")
	out_data = json.loads(fp.read())
	return out_data

def out_json(json_file, json_body):
	fp = open(json_file, "w")
	out_data = json.dumps(json_body , indent=2, ensure_ascii=False)
	out_data = out_data.decode('GBK').encode('UTF-8')
	print >>fp, out_data


def get_png_files(file_path):
	fp = open(file_path, "r")
	lines = fp.readlines()

	out_files = []
	for line in lines:
		key_path = re.search(r"<key>.*?png</key>", line)
		if key_path:
			out_files.append(UPATH(key_path.group()[5:-6]))
	return out_files

LOG("tp_get.log", tp_exe)

work_path = os.getcwd()
file_list = find_files_in_dir(".\\")

data_name = ""
json_path = ""
json_data = None
png_files = []

if not os.path.isdir(out_dir):
	os.makedirs(out_dir)

if not os.path.isdir(out_res):
	os.makedirs(out_res)

for fi in file_list:
	if fi.startswith(export_dir) and fi.endswith(".ExportJson"):
		LOG("tp_get.log", fi)
		json_path = fi
		json_data = open_json(fi)
		data_name = json_path.replace(".ExportJson", "")
		data_name = data_name.replace("Export\\", "")
		COPY(export_dir, out_dir, "%s.ExportJson" % data_name)
	elif fi.startswith(export_dir) and fi.endswith(".plist"):
		LOG("tp_get.log", fi)
		png_files.extend(get_png_files(fi))

for fpng in png_files:
	LOG("tp_get.log", fpng)
	COPY(resources_dir, out_res, fpng)

tp_cmd = tp_exe
tp_cmd = "%s %s" % (tp_cmd, out_opt[0] % UJOIN(out_dir, data_name))
tp_cmd = "%s %s" % (tp_cmd, out_opt[1] % UJOIN(out_dir, data_name))
for x in out_opt[2:]:
	tp_cmd = "%s %s" % (tp_cmd, x)

os.chdir(work_path)
LOG("tp_get.log", tp_cmd)
os.system(GBK(tp_cmd))



