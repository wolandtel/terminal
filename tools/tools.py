#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys

from services.acceptor.AcceptorController import AcceptorController

if __name__ != '__main__':
	sys.exit(1)

argn = len(sys.argv)
if argn < 2:
	sys.exit(2)

if sys.argv[1] == 'acceptor':	
	if argn < 4:
		sys.exit(3)
	
	acceptor = AcceptorController(device = sys.argv[2], baudrate = int(sys.argv[3]), timeout = int(sys.argv[4]))
	acceptor.start()

elif sys.argv[1] == 'dispenser':
	pass
