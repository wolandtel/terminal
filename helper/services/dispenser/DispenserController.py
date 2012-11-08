# -*- coding: utf-8 -*-

import json

from common.Service import Service

from services.dispenser.DispenserAdapter import DispenserAdapter
from services.dispenser.DispenserAdapter import DispenserError

class DispenserController (Service):
	
	def __init__ (self, model, device, timeout = 0.5):
		
		Service.__init__(self)
		try:
			self.__dispenser = DispenserAdapter(dispenser_model = model, port = device, timeout = timeout)
		except Exception, e:
			self.__dispenser = None
			self.io.error(1, e)
	
	def purge (self):
		
		if not self.__dispenser:
			self.io.error(2, e)
			return 1
		
		try:
			self.__dispenser.connect()
		except DispenserError as e:
			self.logger.error('Couldn`t connect to dispenser: %s' % str(e.args))
			self.io.error(3, e)
			return 2
		
		return self.__dispenser.purge()
	
	def dispense (self, cassettes, amount):
		
		if not self.__dispenser:
			self.io.error(2, e)
			return 1
		
		self.logger.debug('DispenserController: New dispense request received')
		
		dispense_info = {}
		
		try:
			self.__dispenser.connect()
		except DispenserError as e:
			self.logger.error('Couldn`t connect to dispenser: %s' % str(e.args))
			self.io.error(3, e)
			return 2
		
		cassettes = json.loads(cassettes)
		dispensed = 0
		dCassettes = {}
		for ci, cinfo in cassettes.iteritems():
			dCassettes[int(ci)] = cinfo
		
		try:
			self.__dispenser.init()
		except:
			pass
		
		try:
			self.__dispenser.status()
		except DispenserError as e:
			self.logger.error('Couldn`t get dispenser status: %s' % str( e.args ))
			self.io.error(4, e)
			return 3
		
		try:
			dispense_info = self.__dispenser.dispense(amount, dCassettes)
			self.logger.debug('DispenserController: dispense results is %s' % dispense_info)
			
			self.__dispenser.serial.close()
			del self.__dispenser
			
			# calculate balances
			dispense_info['error_cause'] = "%02X" % dispense_info['error_cause']
			for ci, cinfo in cassettes.iteritems():
				for k, v in dispense_info.iteritems():
					if cinfo['position'] in k:
						if '_10' in k:
							notes = int(v) * 10
						elif '_1' in k:
							notes = int(v)
#						print "notes = %d, v = %s, k = %s" % (notes, v, k)
						
						if '_exit_requested_' in k:
							dispensed += (int(cinfo['denomination']) * notes)
						elif ('_chk_requested_' in k) or ('_rejects_' in k):
							cinfo['charging'] -= notes
			
			self.logger.debug('DispenserController: dispensed amount is %s' % dispensed)
			
		except DispenserError as e:
			self.logger.debug('Error catched while dispensing %s' % str(e.args))
			self.io.error(6, e)
			return 4
		
		self.io.out({"code": 0, "dispensed": dispensed, "cassettes": cassettes})
		return 0

