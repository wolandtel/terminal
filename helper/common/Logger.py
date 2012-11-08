# -*- coding: utf-8 -*-

import time
from common.Singleton import Singleton

class Logger (Singleton):
	
	levels		=	{
		'silent'	: 0,
		'debug'		: 10,
		'info'		: 20,
		'warning'	: 30,
		'error'		: 40,
		'critical'	: 50
	}
	
	__log_file = None
	__configs  = {'datefmt': '%H:%M:%S', 'format': '[{ts}] {msg}', 'level': levels['silent']}
	
	def __init__ (self, fd = None, fn = None):
		
		if not self._first:
			return
		
		if fd != None:
			self.__log_file = fd
		elif fn != None:
			self.__log_file = open(fn, 'aw+')
	
	def __del__ (self):
		
		if self.__log_file != None:
			self.__log_file.close()
	
	def configure (self, **args):
		for key, value in args.items():
			if key == 'fd':
				self.__log_file = value
			elif key == 'fn':
				self.__log_file = open(value, 'aw+')
			else:
				self.__configs[key] = value
	
	def critical(self, message):
		self.__write('critical', message)
	
	def error(self, message):
		self.__write('error', message)
	
	def warning(self, message):
		self.__write('warning', message)
	
	def info(self, message):
		self.__write('info', message)
	
	def debug(self, message):
		self.__write('debug', message)
	
	def __write(self, level, message):
		
		if self.__log_file == None:
			return
		
		levelname	= level
		level		= self.levels[level]
		
		if self.__configs['level'] and level >= self.__configs['level']:
			ts = time.strftime(self.__configs['datefmt']) 
			
			message = self.__configs['format'].format(ts = ts, msg = message, level = levelname)
			
			self.__log_file.write(message + "\n")
