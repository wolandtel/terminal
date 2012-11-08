# -*- coding: utf-8 -*-

import time

from common.Logger import Logger
from common.type_casting import byte2hex

from services.dispenser.dispensers.__proto__ import DispenserError
from services.dispenser.dispensers.puloon_lcdm4000 import Dispenser_LCDM4000

from libs import serial
from libs.serial.serialutil import SerialException

class DispenserAdapter:
	
	DISPENSERS = {
		'PULOON-LCDM1000' : {'class' : 'Dispenser_LCDM1000', 'path' : 'services.dispenser.dispensers.puloon_lcdm1000'},
		'PULOON-LCDM2000' : {'class' : 'Dispenser_LCDM2000', 'path' : 'services.dispenser.dispensers.puloon_lcdm2000'},
		'PULOON-LCDM4000' : {'class' : 'Dispenser_LCDM4000', 'path' : 'services.dispenser.dispensers.puloon_lcdm4000'}
	}
	
	serial = None	   # Serial Object
	dispenser = None	# Dispenser model implementation
	
	def __init__(self, dispenser_model, port = False, baudrate = False, timeout = .6, parity = False):
		
		if dispenser_model not in self.DISPENSERS:
			raise DispenserError('%s is not supported. See DispenserController.DISPENSERS for available dispensers.')
		
		self.logger = Logger()
		self.dispenser = self.__import_class(
			self.DISPENSERS[dispenser_model]['path'],
			self.DISPENSERS[dispenser_model]['class']
		)()
		
		# Set data
		if port:
			self.dispenser.port = port
		
		if baudrate:
			self.dispenser.baudrate = baudrate
		
		if timeout:
			self.dispenser.timeout = timeout
		
		if parity:
			self.dispenser.parity = parity
	
	@staticmethod
	def __import_class(class_path, class_name):
		try:
			module =  __import__(class_path, fromlist = '*')
			return getattr(module, class_name)
		
		except Exception as e:
			message = "DispenserAdapter: Couldnt load class %s from path %s With error: %s" % (class_name, class_path, str(e.args))
			
			if __debug__:
				print message
			raise ImportError(message)
	
	def connect(self):
		''' Connect to serial port '''
		self.logger.debug('DispenserAdapter: port is %s'	 % self.dispenser.port)
		self.logger.debug('DispenserAdapter: parity is %s'   % self.dispenser.parity)
		self.logger.debug('DispenserAdapter: baudrate is %s' % self.dispenser.baudrate)
		self.logger.debug('DispenserAdapter: timeout is %s'  % self.dispenser.timeout)
		
		try:
			self.serial = serial.serial_for_url(
				self.dispenser.port,
				self.dispenser.baudrate,
				parity  = self.dispenser.parity,
				timeout = self.dispenser.timeout
			)
		except SerialException as e:
			raise DispenserError('Error while connecting to dispenser', 500)
		
		try:
			self.status()
		except:
			pass
	
	def status(self):
		
		self.logger.debug('DispenserAdapter: get_status command received')
		
		response = self.__send(
			command   = self.dispenser.command_status,
			validator = self.dispenser.validate_status_response
		)
		
		self.__write( [self.dispenser.p_ack] )
		
		if isinstance(self.dispenser, Dispenser_LCDM4000):
			''' Read the EOT for LCDM4000 '''
			try:
				self.__read( timeout = .6 )
			except Exception as e:
				self.logger.debug('DispenserAdapter::status [forr Puloon_LCDM4000] error catched. [%s]' % str(e.args))
		
		if not response:
			return False
		
		if 'status' in response:
			self.logger.debug('DispenserAdapter: [CMD STATUS] result: %s' % response['status'])
			self.logger.debug('DispenserAdapter: [CMD STATUS] info  : %s' % response)
			return response['status']
		
		return False
	
	def init(self):
		
		self.logger.debug('DispenserAdapter: init command received')
		
		response = self.__send(
			command = self.dispenser.command_init,
			validator = self.dispenser.validate_init_response
		)
		
		self.__write( [self.dispenser.p_ack] )
		
		if isinstance(self.dispenser, Dispenser_LCDM4000):
			''' Read the EOT for LCDM4000 '''
			time.sleep(2.1)
#			try:
#				self.__read( timeout = .7 )
#			except Exception as e:
#				self.logger.debug('DispenserAdapter::init [for Puloon_LCDM4000] error catched. [%s]' % str(e.args))
		
		self.logger.debug('DispenserAdapter: init finished with result %s' % bool(response))
		
		if response:
			return True
		return False
	
	def purge(self):
		
		self.logger.debug('DispenserAdapter: purge command received')
		
		response = self.__send(
			command   = self.dispenser.command_purge,
			validator = self.dispenser.validate_purge_response
		)
		
		self.__write( [self.dispenser.p_ack] )
		
		if isinstance(self.dispenser, Dispenser_LCDM4000):
			''' Read the EOT for LCDM4000 '''
			try:
				self.__read( timeout = .6 )
			except Exception as e:
				self.logger.debug('DispenserAdapter::purge [for Puloon_LCDM4000] error catched. [%s]' % str(e.args))
		
		if response:
			if 'status' in response:
				self.logger.debug('DispenserAdapter: [CMD PURGE] result: %s' % response['status'])
				self.logger.debug('DispenserAdapter: [CMD PURGE] info  : %s' % response)
				return response['status']
		return False
	
	def dispense(self, amount, cassets_info):
		''' Dispense the bills
			amount:
				amount to dispense
			cassets_info:
				Cassets infrormation in format {id_cassete : {'command' : 0x45, 'denomination' : 0, 'charging' : 100, 'position' : 'upper'},...}
		'''
		dispense_info = self.dispenser.get_dispense( amount, cassets_info )
		command = None
		
		self.logger.debug('DispenserAdapter: Starting dispense of amount %s' % amount)
		
		# Amount is not dispensable
		if dispense_info['to_dispense'] == amount:
			raise DispenserError('Amount is not dispensable', 500)
		
		if len(dispense_info['cassets']) > 1:
			command = self.dispenser.command_dispense_several( cassets_info, dispense_info['cassets'] )
		else:
			id_cassete, banknotes_count = dispense_info['cassets'].popitem()
			command = self.dispenser.command_dispense_single( cassets_info, id_cassete, banknotes_count )
		
		response = self.__send(
			command = command,
			validator = self.dispenser.validate_dispense_response
		)

		if not response:
			raise DispenserError('Error while dispensing (response from dispenser is None)', 500)
		
		self.__write( [self.dispenser.p_ack] )
		
		return response
	
	def __send(self, command, validator = False):
		
		if not self.serial:
			raise DispenserError( 'Serial port is not opened' )
		
		self.logger.debug('DispenserAdapter: send command in hex %s' % (["%02X" % x for x in command]))
		
		''' Пишем, спим полсекундочки и читаем '''
		self.logger.debug( '>> serial.write %s' % (["%02X" % x for x in command]) )
		bytes_written = self.__write(  command )
		
		self.logger.debug( '>> serial.read' )
		bytes_recd = self.__read( timeout = 1)
		self.logger.debug( '>> serial.read done' )
		
		if bytes_recd == [self.dispenser.p_nck]:
			raise DispenserError('DispenserAdapter.__send: bytes_recd == NCK')
		
		if bytes_recd == [self.dispenser.p_ack]:
			self.logger.debug('DispenserAdapter.__send: bytes_recd == ACK')
			bytes_recd = self.__read( timeout = 60)
			readed = (["%02X" % x for x in bytes_recd])
		else:
			if  isinstance(self.dispenser, Dispenser_LCDM4000):
				self.logger.debug( "Dispenser instance is Dispenser_LCDM4000. Fuck this!" )
				if validator:
					return validator(bytes_recd)
			
			readed = (["%02X" % x for x in bytes_recd])
			self.logger.debug('DispenserAdapter.__send [read from dispenser]:  HEX is %s' % readed)
			raise DispenserError('Shit hap in HEX: %s' % readed)
		
		self.logger.debug('>> serial.read in HEX: %s' % readed)
		
#		if isinstance(self.dispenser, Dispenser_LCDM4000):
#			''' Read the EOT for LCDM4000 '''
#			try:
#				self.__read( timeout = .5 )
#			except Exception as e:
#				self.logger.debug('DispenserAdapter::dispense [for Puloon_LCDM4000] error catched. [%s]' % str(e.args))
		
		if validator:
			return validator(bytes_recd)
		
		self.logger.debug('DispenserAdapter: No validator presents. Return invalidated response.')
		return bytes_recd
	
	def __write(self, bytes):
		
		return self.serial.write(serial.to_bytes(bytes))
	
	def __read(self, timeout = 60):
		
		time_start = time.time()
		frame	= []
		data	= ''
		byte	= self.serial.read(1)
		data	+= byte
		
		try:
			first_byte = int(byte2hex( byte ), 16)
			
			self.logger.debug('DispenserAdapter.__read: first byte is: 0x%02X' % first_byte)
			
			if first_byte == self.dispenser.p_nck:
				self.logger.debug('DispenserAdapter.__read: first byte is NCK')
				return [self.dispenser.p_nck]
			
			if first_byte == self.dispenser.p_ack:
				self.logger.debug('DispenserAdapter.__read: first byte is: ACK')
				return [self.dispenser.p_ack]
			
			byte  = self.serial.read(1)
			data += byte
			
			self.logger.debug('DispenserAdapter.__read: Wrong first byte in response 0x%02X' % first_byte )
			self.logger.debug('DispenserAdapter.__read: Next byte in response is 0x%02X' % int(byte2hex( byte )) )
		
		except Exception as e:
			self.logger.debug('DispenserAdapter.__read: %s' % str(e.args) )
			pass
		
		self.logger.debug('DispenserAdapter.__read: first byte is detected')
		
		while True:
			if time.time() - time_start > timeout:
				self.logger.debug('DispenserAdapter.__read: timeout to read. ')
				for _, i in enumerate(serial.to_bytes(data)):
					frame.append( int(byte2hex(i), 16) )
				
				self.logger.debug('DispenserAdapter.__read: full dump of response: %s' % (["0x%02X" % x for x in frame]))
				raise DispenserError(message = 'Timeout to read', code = 500)
			
			try: 
				if int(byte2hex(byte), 16) == self.dispenser.p_etx:
					break
			except Exception as e:
				self.logger.debug('Error in read cycle: %s' % str(e.args) )
				time.sleep( .01 )
			
			byte  = self.serial.read(1)
			data += byte
		
		self.logger.debug('DispenserAdapter.__read: cycle is complete')
		
		data += self.serial.read(1)
		
		for _, i in enumerate(serial.to_bytes(data)):
			frame.append( int(byte2hex(i), 16) )
		
		self.logger.debug('DispenserAdapter.__read: full dump of response: [%s]' % ([x for x in frame]))
		return frame
