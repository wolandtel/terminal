# -*- coding: utf-8 -*-

class Singleton (object):
	
	_first = True
	__instance = None
	
	def __new__ (cls, *args):
		
		if cls.__instance:
			cls._first = False
		else:
			cls.__instance = object.__new__(cls)
		
		return cls.__instance
