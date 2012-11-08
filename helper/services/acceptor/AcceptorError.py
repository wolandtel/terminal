# -*- coding: utf-8 -*-

class AcceptorError (Exception):
	
	def __init__(self, code, message):
		
		self.code, self.message = code, message
	
	def __str__(self):
		
		#return "code = %d, message = '%s'" % (self.code, self.message)
		return repr((self.code, self.message))
