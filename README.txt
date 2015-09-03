Description
============
wshell is a server wich let you open a shell from a web browser.

BEWARE: it's an experiment, it's not secure!!!

Compile
========

Requirements
-------------
scons (I have used v2.1.0.r5357)

g++ (I have used 4.6.3)

to compile:
scons

Libraries
----------
pthread
ssl
crypto

Running
========

Requirements
-------------
linux (I have tested it only on Ubuntu 12.04 64 bit), ssh and sshd started on server

Run
-----
for now (I hope to fix it) you must start it from a terminal, otherwise TERM is not set and some commands do not work.
to run the server:
build/wshell start

then you can open http://localhost:5000
follow link to wshell
