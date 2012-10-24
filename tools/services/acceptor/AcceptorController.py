# -*- coding: utf-8 -*-

import time
import glob
import json
import sys
import os.path

from common.Logger import Logger

from services.acceptor.models.cashcode_t1500 import Cashcode_T1500

class AcceptorController:
	
	running = False
	adapter = None
	timeout = 0
	
	def __init__ (self, device, baudrate, timeout = 0):
		
		self.timeout = timeout
		
		try:
			self.logger = Logger.get_instance()
			self.adapter = Cashcode_T1500(device, baudrate, .1)
			self.adapter.connect()
		except Exception, e:
			self.error(1, e)
			self.adapter = None
	
	def start (self):
		
		if self.adapter == None:
			self._out({'code': 2})
			return
		
		sleep_time	= 1.
		total_sum	= 0
		
		last_action = time.time()
		self.running = True
		
		self.adapter.start()
		
		self.logger.debug('Acceptor in use: %s' % self.running)
		self.running = True
		
		self._out({'code': 0, 'event': 'started'})
		
		try:
			while self._running(last_action):
				time.sleep(sleep_time)
				event = self.adapter.pop_message()
				
				if event:
					last_action = time.time()
					total_sum += event['value']
					self._out({'code': 0, 'event': 'received', 'amount': total_sum}) # Сообщаем сумму
			
		except KeyboardInterrupt, e:
			pass
		
		except Exception, e:
			self.error(3, e)
		
		finally:
			try:
				self.logger.debug('[AcceptorController::start]: stopping')
				self.stop()
				
				while len(self.adapter.events):
					self.logger.debug( '[AcceptorController::start]: self.adapter.events has items. Processing...' )
					self.adapter.pop_message()
				
				del self.adapter
			except Exception, e:
				self.error(4, e)
				return
		
		self._out({'code': 0, 'event': 'finished'})
		self.logger.debug('[AcceptorController::start]: receive has been stopped')
	
	def stop (self):
		
		if not self.running:
			return
		
		self.running = False
		if self.adapter != None:
			try:
				self.adapter.disconnect()
			except Exception, e:
				self.error(5, e)
	
	def error (self, code, exception):
		
		self._out({'code': code, 'msg': ('Exception type = %s,\n\targuments = %s' % (type(exception), str(exception.args)))})
	
	def _running (self, last_action):
		
		timestamp = int(time.time())
		last_action = int(last_action)
		
		if not self.running:
			self.logger.debug("Stopped by flag")
			return False
		
		if self.timeout > 0 and timestamp - last_action > self.timeout:
			self.logger.debug("Stopped by timeout. Timeout: %s, last action: %s, time: %s" % (self.timeout, last_action, timestamp))
			self.stop()
		
		return self.running
	
	def _out (self, jObject):
		
		print json.dumps(jObject)
		sys.stdout.flush()
	
