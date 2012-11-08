# -*- coding: utf-8 -*-

from libs import serial
from common.Logger import Logger

class DispenserError(Exception):
	
	def __init__(self, message, code = 0):
		
		self.message, self.code = message, code
	
	def __str__(self):
		
		#return "code = %d, message = '%s'" % (self.code, self.message)
		return repr((self.code, self.message))

class Dispenser:
	
	''' Configuration '''
	port						= '/dev/ttyUSB'
	baudrate					= 9600
	timeout					 = 1
	data_bits				   = 8					 # 8 bits
	stop_bits				   = 1					 # 1 bit
	parity					  = serial.PARITY_NONE	# No parity
	bps						 = 3.5				   # Banknotes per second
	
	''' Limits for dispensing '''
	dispense_limit_single_10	= 6
	dispense_limit_single_1	 = 9
	dispense_limit_both_10	  = 6
	dispense_limit_both_1	   = 9
	dispense_limit			  = 60   # Limit banknotes count for dispensing from 1 cassete

	''' PROTOCOL > CONSTANTS '''
	p_soh					   = 0x01  # Start of Header
	p_stx					   = 0x02  # Start of Text
	p_etx					   = 0x03  # End of Text
	p_eot					   = 0x04  # Start of Transmission
	p_ack					   = 0x06  # ACK
	p_nck					   = 0x15  # NCK
	p_id						= 0x50  # Communication ID
	p_bcc					   = 0x00  # Block Check Character: BCC can be gotten through Exclusive-OR (XOR) from the start of each message to ETX

	@staticmethod
	def get_cassete_by_denomination(cassets, denomination):
		for id_cassete, cassete_info in cassets.items():
			if cassete_info['denomination'] == denomination:
			   return id_cassete
		return -1

	def get_dispense(self, amount, cassets_info):
		self.logger = Logger()
		
		'''
			Method calculates cash dispensing between cassets

			amount:
				Amount to dispense

			cassets_info:
				Cassets infrormation in format {id_cassete : {'command' : 0x45, 'denomination' : 0, 'charging' : 100, 'position' : 'upper'},...}
			return {
				'to_dispense'   : 5,
				'cassets'	   : {
					0 : {'amount' : 100, 'count' : 10, 'denomination'  : 10},
					1 : {'amount' : 1000, 'count' : 10, 'denomination'  : 100},
					...
				}
			}
		'''
		dispense = {'to_dispense': amount, 'cassets': {}}

		# Dynamic cassets information
		cassets = ([cassete_info['denomination'] for cassete_info in cassets_info.values()])
		cassets.sort()

		while dispense['to_dispense'] > 0 and len( cassets ):
			self.logger.debug('[DISPENSER]: Amount to dispense: %s' % dispense['to_dispense'])
			self.logger.debug('[DISPENSER]: Next cassets are available: %s' % cassets_info)

			# select max denomination
			denomination = cassets.pop()
			self.logger.debug( '[DISPENSER]: selected denomination: %s' % denomination )
			id_cassete = self.get_cassete_by_denomination( cassets_info, denomination )

			# if no available cassets
			if id_cassete == -1:
				self.logger.debug('[DISPENSER]: no more cassets are available to dispense amount %s' % dispense['to_dispense'])
				continue

			# check count of banknotes
			banknotes_count = min(
				int( dispense['to_dispense'] * 1. / denomination ),	 # Count of full-dispensing
				cassets_info[id_cassete]['charging'],				   # Count of available banknotes
				self.dispense_limit									 # Dispenser limit
			)

			# If no more banknotes
			if not banknotes_count:
				self.logger.debug( '[DISPENSER]: no such banknotes in cassete #%d' % id_cassete )
				continue

			self.logger.debug( '[DISPENSER]: from selected cassete #%d will exit %d banknotes with denomination %d' % (id_cassete, banknotes_count, denomination) )

			dispense['cassets'][id_cassete] = banknotes_count
			dispense['to_dispense'] -= banknotes_count * denomination

		return dispense
