#!/bin/sh

rm -rf \
	aclocal.m4 \
	autom4te.cache \
	compile \
	config.guess \
	config.h.in \
	config.h.in~ \
	config.sub \
	configure \
	depcomp \
	install-sh \
	ltmain.sh \
	missing

for d in . lib simpleclient simpleclient/iaxcomm simpleclient/iaxphone \
         simpleclient/testcall simpleclient/tkphone simpleclient/WinIAX \
	 simpleclient/wx
do
	rm $d/Makefile.in 
done

