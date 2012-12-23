# -*- encoding: utf-8 -*-

import sys, os, shutil
from hashlib import md5


class PatchTool:

	def __init__ (self, path_old = None, path_new = None, log_path = None):
		self.self_path = os.getcwd()
		self.logfile = open(os.path.join(self.self_path, "patchtool.log"), "w")

		self.set_path(path_old, path_new)


		self.OP_MODIFY	= "~"
		self.OP_ADD		= "+"
		self.OP_DEL		= "-"

		self.TP_FILE	= "F"
		self.TP_DIR		= "D"


	def __add_log (self, loginfo, show = True):
		print >>self.logfile, loginfo
		if show:
			print loginfo


	def __add_diff_log (self, diffinfo):
		self.__add_log(diffinfo[0] + diffinfo[1] + "  " + diffinfo[2])


	def __add_copy_log (self, copyinfo):
		self.__add_log("patch:  " + copyinfo)


	def set_path_old (self, path_old):

		if path_old == None:
			self.path_old = self.self_path
			return False
		else:
			if not os.path.isdir(path_old):
				print "Error path: " , path_old
				self.path_old = self.self_path
				return False
			else:
				self.path_old = path_old
				return True


	def set_path_new (self, path_new):

		if path_new == None:
			self.path_new = self.self_path
			return False
		else:
			if not os.path.isdir(path_new):
				print "Error path: " , path_new
				self.path_new = self.self_path
				return False
			else:
				self.path_new = path_new
				return True


	def set_path(self, path_old, path_new):

		isOK = True
		if False == self.set_path_old(path_old): isOK = False
		if False == self.set_path_new(path_new): isOK = False
		return isOK


	def find_list_in_dir(self, path_prefix, path):

		abspath = os.path.join(path_prefix, path)

		if not os.path.isdir(abspath):
			return None

		list_pre = os.listdir(abspath)

		dir_list = []
		file_list = []

		for dirp in list_pre:
			if os.path.isdir(os.path.join(abspath, dirp)):
				dir_list.append(dirp)
			elif os.path.isfile(os.path.join(abspath, dirp)):
				file_list.append(dirp)

		return {"DIR":dir_list, "FILE":file_list}


	def is_same_file(self, file_path, file_path_new = None):
		file_path_old = file_path
		if file_path_new == None:
			file_path_new = file_path_old

		absfile_old = os.path.join(self.path_old, file_path_old)
		absfile_new = os.path.join(self.path_new, file_path_new)

		if not os.path.isfile(absfile_old): return None
		if not os.path.isfile(absfile_new): return None

		if os.path.getsize(absfile_old) != os.path.getsize(absfile_new):
			return False

		file_md5_old = (md5(open(absfile_old, "r").read())).hexdigest()
		file_md5_new = (md5(open(absfile_new, "r").read())).hexdigest()

		return (file_md5_old == file_md5_new)



	def check_diff(self, dir_path = ""):

		dir_list_old = self.find_list_in_dir(self.path_old, dir_path)
		if dir_list_old == None: return False
		dir_list_new = self.find_list_in_dir(self.path_new, dir_path)
		if dir_list_new == None: return False

		file_set = dir_list_old["FILE"][:]
		file_set.extend(dir_list_new["FILE"])
		file_set = set(file_set)

		diff_info = []
		for filep in file_set:
			file_path = os.path.join(dir_path, filep)
			if filep in dir_list_old["FILE"] and filep in dir_list_new["FILE"]:
				if not self.is_same_file(file_path):
					diff_info.append((self.OP_MODIFY, self.TP_FILE, file_path))
					self.__add_diff_log(diff_info[-1])
			elif filep in dir_list_old["FILE"]:
				diff_info.append((self.OP_DEL, self.TP_FILE, file_path))
				self.__add_diff_log(diff_info[-1])
			elif filep in dir_list_new["FILE"]:
				diff_info.append((self.OP_ADD, self.TP_FILE, file_path))
				self.__add_diff_log(diff_info[-1])
			else:
				pass

		subdir_set = dir_list_old["DIR"][:]
		subdir_set.extend(dir_list_new["DIR"])
		subdir_set = set(subdir_set)

		for dirp in subdir_set:
			subdir_path = os.path.join(dir_path, dirp)
			if dirp in dir_list_old["DIR"] and dirp in dir_list_new["DIR"]:
				diff_info.extend(self.check_diff(subdir_path))
			elif dirp in dir_list_old["DIR"]:
				diff_info.append((self.OP_DEL, self.TP_DIR, subdir_path))
				self.__add_diff_log(diff_info[-1])
			elif dirp in dir_list_new["DIR"]:
				diff_info.append((self.OP_ADD, self.TP_DIR, subdir_path))
				self.__add_diff_log(diff_info[-1])
			else:
				pass

		return diff_info
	

	def create_patch(self, diffinfo, sav_path = None):

		if sav_path == None:
			sav_path = os.path.join(self.self_path, "patch")
		else:
			sav_path = os.path.join(sav_path, "patch")

		if diffinfo == None: return None

		if os.path.isdir(sav_path):
			shutil.rmtree(sav_path)
		os.mkdir(sav_path)


		for diff in diffinfo:
			
			if diff[0] == self.OP_ADD or diff[0] == self.OP_MODIFY:
				
				sav_dir = os.path.dirname(os.path.join(sav_path, diff[2]))
				if not os.path.isdir(sav_dir):
					os.makedirs(sav_dir)

				if diff[1] == self.TP_FILE:
					shutil.copy( \
						os.path.join(self.path_new, diff[2]), \
						os.path.join(sav_path, diff[2]))
					self.__add_copy_log(diff[2])
				elif diff[1] == self.TP_DIR:
					shutil.copytree( \
						os.path.join(self.path_new, diff[2]), \
						os.path.join(sav_path, diff[2]))
					self.__add_copy_log(diff[2])


if __name__ == "__main__":

	if len(sys.argv) >= 3:
		path_old = sys.argv[1]
		path_new = sys.argv[2]
	else:
		path_old = raw_input("path_old>> ")
		path_new = raw_input("path_new>> ")

	patchtool = PatchTool(path_old, path_new)

	patchtool.create_patch(patchtool.check_diff())