#!/bin/sh

if [ -e Makefile ]
then
	make clean
fi 
rm -f Makefile *.o WireAPI-*.gem WireAPI.so *~
ls
gem build WireAPI.gemspec