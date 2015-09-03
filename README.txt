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
ssh and sshd started on server

Run
-----
for now (I hope to fix it) you must start it from a terminal, otherwise TERM is not set and some commands do not work.
the run:
build/netshell start 
