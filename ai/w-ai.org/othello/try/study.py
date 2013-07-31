import os, re

temp_code = open("temp.cpp", "r").read()

DNA = {
	":A0:" : 10,
	":A1:" : 10,
	":A2:" : 12,
	":A3:" : 7,
	":A4:" : 5,
	":A5:" : 15,
	":A6:" : 18,

	":B0:" : 24,
	":B1:" : 20,
	":B2:" : 46,
	":B3:" : 8,
	":B4:" : 24,
	":B5:" : 80,
	":B6:" : 680
}

def decode(dna):
	res_code = temp_code
	print dna
	for key in dna:
		res_code = res_code.replace(key, str(dna[key]))

	return res_code


def compile_code(code, fname):
	print >>open(fname + ".cpp", "w"), code
	cmd = "g++ " + fname + ".cpp" + " -o " + fname + ".exe"
	print cmd
	os.system(cmd)


def run_game(fA, fB):
	print os.system("run.py " + fA + " " + fB)
	print os.system("run.py " + fB + " " + fA)


def mutation(dna):
	pass


def crossover(dna0, dna1):
	pass


compile_code(decode(DATA), "A")
compile_code(decode(DATA), "B")

run_game("A", "B")