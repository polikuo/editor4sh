# editor4sh
A FLTK editor with syntax highlighting for sh / bash

Icon Source:
[Clipartmax](https://www.clipartmax.com/max/m2H7G6d3Z5m2i8N4/)
# Installation
To compile the editor, please setup the build environment for [FLTK](http://www.fltk.org/software.php)

Recommended additional CXXFLAGS to shrink binary size:
<pre><code>./configure CXXFLAGS='-Os -s -pipe'
make</code></pre>

If configure is not presented, run
<pre><code>./autogen.sh</code></pre>
# Tiny Core

If you happen to be using [Tiny Core Linux](http://tinycorelinux.net/)

Using Makefile.tc is better
<pre><code>tce-load -w compiletc fltk-1.3-dev
tce-load -i compiletc fltk-1.3-dev
make -f Makefile.tc
sudo make -f Makefile.tc install
</code></pre>
