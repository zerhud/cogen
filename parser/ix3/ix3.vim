if exists("b:current_syntax")
	finish
endif


syn keyword ix3Modules module enum record interface
syn keyword ix3Types i32 i64 string list optional date data

syn match ix3Comment "#.*$"
syn match ix3Version "v\d\.\d"
syn region ix3String start='"' end='"'
syn region ix3Block  start='{' end='}' fold transparent contains=ix3Types,ix3Comment,ix3Version

hi def link ix3Modules Statement
hi def link ix3Types   Type
hi def link ix3String  Constant
hi def link ix3Version Constant
hi def link ix3Comment Comment

setlocal foldmethod=syntax
setlocal foldlevel=20
