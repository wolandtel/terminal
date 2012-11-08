# -*- coding: utf-8 -*-

import time

from common.Service import Service

from services.acceptor.models.cashcode_t1500 import Cashcode_T1500

class AcceptorController (Service):
	
	running = False
	adapter = None
	timeout = 0
	
	def __init__ (self, device, baudrate, timeout = 0):
		
		Service.__init__(self)
		
		self.timeout = timeout
		try:
			self.adapter = Cashcode_T1500(device, baudrate, .1)
			self.adapter.connect()
		except Exception as e:
			self.io.error(1, e)
			self.adapter = None
	
	def start (self):
		
		if self.adapter == None:
			self.io.out({'code': 2})
			return
		
		sleep_time	= 1.
		total_sum	= 0
		
		last_action = time.time()
		self.running = True
		
		self.adapter.start()
		
		self.logger.debug('Acceptor in use: %s' % self.running)
		self.running = True
		
		self.io.out({'code': 0, 'event': 'started'})
		
		try:
			while self._running(last_action):
				time.sleep(sleep_time)
				event = self.adapter.pop_message()
				
				if event:
					last_action = time.time()
					total_sum += event['value']
					self.io.out({'code': 0, 'event': 'received', 'amount': total_sum}) # Сообщаем сумму
			
		except KeyboardInterrupt:
			pass
		
		except Exception as e:
			self.io.error(3, e)
		
		finally:
			try:
				self.logger.debug('[AcceptorController::start]: stopping')
				self.stop()
				
				while len(self.adapter.events):
					self.logger.debug( '[AcceptorController::start]: self.adapter.events has items. Processing...' )
					self.adapter.pop_message()
				
				del self.adapter
			except Exception as e:
				self.io.error(4, e)
				return
		
		self.io.out({'code': 0, 'event': 'finished'})
		self.logger.debug('[AcceptorController::start]: receive has been stopped')
	
	def stop (self):
		
		if not self.running:
			return
		
		self.running = False
		if self.adapter != None:
			try:
				self.adapter.disconnect()
			except Exception as e:
				self.io.error(5, e)
	
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
	

