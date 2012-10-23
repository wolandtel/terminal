# -*- coding: utf-8 -*-

import time

class Logger:
	
	levels		=	{
		'silent'	: 0,
		'debug'		: 10,
		'info'		: 20,
		'warning'	: 30,
		'error'		: 40,
		'critical'	: 50
	}
	
	__instance = None
	__log_file = None
	__configs  = {'datefmt': None, 'format': None, 'filename': None, 'level': levels['silent']}
	
	@staticmethod
	def get_instance():
		if not Logger.__instance:
			Logger.__instance = Logger()
		return Logger.__instance
	
	def configure(self, **args):
		for key, value in args.items():
			self.__configs[key] = value
		self.__log_file = open(self.__configs['filename'], 'aw+')
	
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
		
		levelname	= level
		level		= self.levels[level]
		
		if self.__configs['level'] and level >= self.__configs['level']:
			data = {}
			data['asctime']		= time.strftime(self.__configs['datefmt'])
			data['message']		= message
			data['levelname']	= levelname
			
			message = self.__configs['format'] % data
			
			log = open(self.__configs['filename'], 'aw+')
			log.write(message + "\n")
			log.close()
