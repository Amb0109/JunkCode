set autochdir                " 自动cd到当前目录
set tabstop=4                " 设置tab键的宽度
set autoindent               " 自动对齐
set backspace=2              " 设置退格键可用
set smartindent              " 智能自动缩进
set number                   " 显示行号
set mouse=a                  " 启用鼠标
set ruler                    " 右下角显示光标位置的状态行
set incsearch                " 查找book时，当输入/b时会自动找到
set hlsearch                 " 开启高亮显示结果
set incsearch                " 开启实时搜索功能
set nowrapscan               " 搜索到文件两端时不重新搜索
set nocompatible             " 关闭兼容模式
set hidden                   " 允许在有未保存的修改时切换缓冲区
set scrolloff=10             " 设置多余10行滚动
set shiftwidth=4             " 设置缩进宽度
set whichwrap=b,s,<,>,[,]    " 光标自动换行
set guioptions-=T            " 设置隐藏菜单栏
syntax enable                " 打开语法高亮
syntax on                    " 开启文件类型侦测
filetype indent on           " 针对不同的文件类型采用不同的缩进格式
filetype plugin on           " 针对不同的文件类型加载对应的插件
filetype plugin indent on    " 启用自动补全
colorscheme jellybeans


"====encoding
set fileencoding=utf-8
set fileencodings=utf-8,gb18030,utf-16,big5

"============= set path
"set path=.,,/home/amb/文档/vimsrc
"cd /home/amb/文档/vimsrc

"====set font
set guifont=YaHei\ Mono:h10:cANSI
set guifontwide=YaHei\ Mono:h10

"============= set statusbar
set laststatus=2
set statusline=[%F]%y%r%m%*%=%l/%L,%c\ -\ %p%%\ 

"============= quick vimrc
map <silent> ,rc :e /home/amb/.vimrc<cr>
autocmd! bufwritepost .vimrc source /home/amb/.vimrc


