# -*- coding: utf-8 -*-

from common.IO import IO
from common.Logger import Logger

class Service (object):
	
	def __init__ (self):
		
		self.io		= IO()
		self.logger	= Logger()
