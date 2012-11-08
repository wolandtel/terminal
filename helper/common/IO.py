# -*- coding: utf-8 -*-

import sys
import json

from common.Singleton import Singleton

class IO (Singleton):
	
	def error (self, code, exception):
		
		self.out({'code': code, 'msg': ('Exception type = %s,\n\targuments = %s' % (type(exception), exception))})
	
	def out (self, map_):
		
		print json.dumps(map_)
		sys.stdout.flush()
