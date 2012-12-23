"============build=================
map<F5> :call FileMake()<CR>

function CPPMake()
	let outfile=expand("%:r").".exe"
	let runfile=expand("%:r").".exe"
	if filereadable(outfile)
		let isdel=delete(outfile)
		if (isdel!=0)
			echohl WarningMsg | echo "Error! Cannot write the ".outfile | echohl None
			return
		endif
	endif
	set makeprg=g++\ -o\ %<.exe\ %
	execute "make"
	set makeprg=make
	if filereadable(runfile)
		execute "!".runfile
	else
		echohl WarningMsg | echo "Error!" | echohl None
	endif
endfunction

function JavaMake()
	let outfile=expand("%:r").".class"
	let runfile=expand("%:r")
	if filereadable(outfile)
		let isdel=delete(outfile)
		if (isdel!=0)
			echohl WarningMsg | echo "Error! Cannot write the ".outfile | echohl None
			return
		endif
	endif
	set makeprg=javac\ %
	execute "make"
	set makeprg=make
	if filereadable(outfile)
		execute "!java ".runfile
	else
		echohl WarningMsg | echo "Error!" | echohl None
	endif
endfunction

function PythonMake()
	let runfile=expand("%:t")
	execute "!python ".runfile
endfunction

function FileMake()
	execute "w"
	let ftype=expand("%:e")
	if (ftype=="c" || ftype=="cpp")
		call CPPMake()
	elseif (ftype=="java")
		call JavaMake()
	elseif (ftype=="py")
		call PythonMake()
	endif
endfunction




