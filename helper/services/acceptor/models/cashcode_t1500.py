# -*- coding: utf-8 -*-

import time
import random

from threading import Thread

from common.type_casting import str2bytes
from common.type_casting import int2bytes
from common.type_casting import ints2hex
from common.type_casting import hex2dec
from common.type_casting import dec2hex
from common.Logger import Logger

from libs import serial
from services.acceptor.AcceptorError import AcceptorError

### !WARNING! ### Timeout parameter is buggy

class Cashcode_T1500 (Thread):
	
	parity			= 'N'
	stopbits		= 1
	bitesize		= 8
	baudrate		= 9600
	serial_timeout	= .5
	
	p_ack			= 0x00
	p_nck			= 0xFF
	p_sync			= 0x02
	p_adr			= 0x03
	
	c_reset			= 0x30
	c_status		= 0x31
	c_security		= 0x32
	c_poll			= 0x33
	c_bill_types	= 0x34
	c_stack			= 0x35
	c_return		= 0x36
	c_ident			= 0x37
	c_hold			= 0x38
	c_barcode		= 0x39
	c_barcode_extract	= 0x3A
	c_bill_table	= 0x41
	c_download		= 0x50
	c_crc32			= 0x51
	c_stats			= 0x60
	
	r_power_up				= 0x10
	r_power_up_validator	= 0x11
	r_power_up_stacker		= 0x12
	
	r_initialize	= 0x13
	r_idling		= 0x14
	r_accepting		= 0x15
	r_stacking		= 0x17
	r_returning		= 0x18
	r_unit_disabled	= 0x19
	r_holding		= 0x1A
	r_device_busy	= 0x1B
	
	#generic reject codes
	rj_reject		= 0x1C
	
	rj_drop_casset_full	= 0x41
	rj_drop_casset_out	= 0x42
	rj_validator_jammed	= 0x43
	rj_drop_casset_jammed	= 0x44
	rj_cheated			= 0x45
	rj_pause			= 0x46
	rf_failure			= 0x47
	
	#events
	re_escrow			= 0x80
	re_stacked			= 0x81
	re_returned			= 0x82

	# Errors
	e_insertation		= 0x60

	bill_types = {
		2: 10,
		3: 50,
		4: 100,
		5: 500,
		6: 1000,
		7: 5000
	}


	CRC_TABLE = [
		0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
		0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
		0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
		0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
		0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
		0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
		0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
		0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
		0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
		0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
		0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
		0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
		0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
		0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
		0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
		0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
		0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
		0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
		0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
		0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
		0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
		0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
		0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
		0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
		0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
		0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
		0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
		0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
		0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
		0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
		0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
		0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
	]

	__prefix	= '[Cashcode_t1500]'

	last_action = 0
	time_start	= 0
	timeout		= 0
	sleep_time	= .5
	running		= False
	events		= []
	
	def __init__ (self, device, baudrate = 9600, sleep_time = .5, timeout = 0):
		
		self.device, self.timeout = device, timeout
		self.logger = Logger()
		
		if baudrate:
			self.baudrate = baudrate
		
		if sleep_time > 0:
			self.sleep_time = sleep_time
		
		Thread.__init__(self)
	
	def connect(self):
		self.running = True
		
		try:
			self.serial = serial.serial_for_url(
				self.device,
				self.baudrate,
				parity	= self.parity,
				timeout = self.serial_timeout
			)
			
			self.check_status()
			self.logger.debug("Okay. Adapter has been connected to CASHCODE SM with data %s:%s" % (self.device, self.baudrate))
			
		except serial.SerialException as e:
			message = 'Error while connecting to bill acceptor'
			self.logger.warning(message)
			raise AcceptorError(message = message, code = 500)
		except Exception as e:
			message = 'Error while connecting to bill acceptor. Error details: %s' % e
			self.logger.warning(message)
			raise AcceptorError(message = message, code = 500)
		
		self.full_reset()
	
	def disconnect(self):
		self.running = False
		
		try:
			self.logger.debug('[Cashcode_T1500]: reset')
			self.full_reset()
			time.sleep(3)
			self.serial.close()
		except:
			self.logger.debug('[Cashcode_T1500]: stopping with error')
			pass
		self.logger.debug('[Cashcode_T1500]: serial port has been closed')

	def run(self):

		self.logger.debug('[Cashcode_T1500]: thread has ran')
		self.logger.debug('[Cashcode_T1500]: timeout is %s' % self.timeout)
		
		total_summ = 0
		response	= None
		self.last_action = time.time()
		self.enable_bill_types()
		self.check_status()

		while self._running(self.timeout):
			time.sleep(self.sleep_time)
			
			# Если родительский поток не забрал все сообщения
			if len( self.events ):
				continue
			
			try:
				response = self.wait(
					wait = [self.re_stacked],
					skip = [self.r_idling, self.r_initialize, self.r_accepting, self.r_stacking],
					timeout = self.timeout
				)
			except AcceptorError as e:
				message = 'Error while waiting %s. Response is %s. Error is %s' % (self.re_stacked, response, str(e.args))
				self.logger.debug(message)
				continue
			
			try:
				amount = self.bill_types[response[1]]
				total_summ += amount
				
				self.logger.debug('Response is %s' % response)
				self.logger.debug('Accepted amount is %d' % amount)
				self.send_ack()
				self.events.append({'id_trx': 0, 'count': 1, 'value': amount, 'total_amount': total_summ, 'sleep_time': self.sleep_time})
			except:
				pass
		
		self.logger.debug('[Cashcode_T1500]: run stop operations')
		self.disconnect()
	
	def pop_message(self):
		if not len(self.events):
			return False
		return self.events.pop(0)
	
	def _running(self, timeout):
		timestamp = int(time.time())
		
		if not self.running:
			self.logger.debug("Acceptor thread stopped by flag")
			return False
		
		if timeout > 0 and timestamp - self.last_action > timeout:
			self.logger.debug("Acceptor thread stopped by timeout")
			#print "timestamp: %s, last_action: %s, timeout: %s" % (timestamp, self.last_action, timeout)
			return False
		
		return True
	
	def __calc_crc(self, data):
		crc = 0;
		for i in data:
			crc = self.CRC_TABLE[ (crc ^ i) & 0xFF] ^ (crc >> 8);
		return crc;
	
	def _read_nbytes(self, length):
		return str2bytes(self.serial.read(length))
	
	def _read_byte(self):
		return ord(self.serial.read(1))
	
	#reads next 2 bytes (crc length)
	def _read_crc(self):
		return self._read_nbytes(2)
	
	#reads response packey, verify crc, check header(sync, adr)
	def _read_response(self):
		header = self._read_nbytes(3)
		
		try:
			message = 'Header error. Is Cashcode connected to %s? Header is %s' % (self.device, header)
			if (header[0] <> self.p_sync or header[1] <> self.p_adr):
				self.logger.debug( message )
				raise AcceptorError( message = message, code = 500 )
		except IndexError:
			message = 'Header error. Header is %s' % ( header )
			self.logger.debug( message )
			raise AcceptorError( message = message, code = 500 )
		
		packet	= self._read_nbytes( header[2] - 3 - 2 )
		crc	= self._read_crc()
		checkCrc = int2bytes( self.__calc_crc(header + packet) )
		
		if crc != checkCrc:
			self.logger.debug('%s: _read_response. Crc not satisfied their=%s , my=%s' % (self.__prefix, crc, checkCrc))
		
#		self.logger.debug('%s: _read_response. response(len=%d) dec=%s, hex=%s ' % (self.__prefix, len(packet), packet, ints2hex(packet) ))
		self.validate_statement(packet)
		return packet
	
	def free(self, t = 0.02):
		time.sleep(t)

	''' ******* SEND COMMANDS ************ '''
	def send_single_command(self, cmd, read_response = True):
		packet = [self.p_sync, self.p_adr, 0x06, cmd]
		packet = packet + int2bytes( self.__calc_crc(packet) )
		self.serial.write( serial.to_bytes( packet ) )

		if read_response:
			return self._read_response()
		return True

	def send_command(self, data, read_response = True):
		packet = [ self.p_sync, self.p_adr, 5 + len(data)] + data
		packet = packet+ int2bytes(self.__calc_crc(packet))

		self.serial.write(serial.to_bytes(packet) )

		if read_response:
			return self._read_response()
		return True

	def status(self):
		return self.check_status()

	''' ******* EXACT COMMANDS ************ '''
	def check_status(self):
		status_response = self.send_single_command( self.c_status, True )
		self.logger.debug( '%s: check_status: Status is "%s"' % (self.__prefix, status_response))
		self.send_ack()
		return status_response

	def poll(self):
		return self.send_single_command( self.c_poll )

	def full_reset(self):
		self.logger.debug( '%s: full_reset' % (self.__prefix) )
		return self.__is_ack( self.send_single_command(self.c_reset) )

	''' ******** ALIASES ************** '''
	def init_bill_types(self, b1, b2, b3, b4, b5, b6):
		init_response = self.send_command([self.c_bill_types, b3, b2, b1, b6, b5, b4])
		self.logger.debug( '%s: init_bill_types: Result is "%s"' % (self.__prefix, init_response))
		
		self.free()
		
		self.logger.debug( '%s: init_bill_types: poll' % (self.__prefix))
		self.poll()
		self.free()
		self.send_ack()
		self.wait( wait = [self.r_idling], skip = [self.r_initialize] )
		
		return True

	def enable_bill_types(self):
		self.logger.debug('[Cashcode_t1500->enable_bill_types]: Enable bill types')
		bills = 0
		for i in range(2,8):
			bills = bills | 1 << i
		return self.init_bill_types(bills,0,0,0,0,0)

	def disable_bill_types(self):
			return self.init_bills(0,0,0,0,0,0)

	def receive_start(self, timeout = 10):
		
		self.logger.debug('%s: Starting receive of banknotes.' % self.__prefix)
		self.last_action = time.time()
		
		if( not self.enable_bill_types() ):
			raise Exception('[Cashcode_t1500->receive_start]: Bill types was not setuped')

		while self._running( timeout ):

			response = self.wait(
				wait = [self.re_stacked],
				skip = [self.r_idling, self.r_initialize, self.r_accepting, self.r_stacking]
			)
			
			try:
				amount = self.bill_types[ response[1] ]
				self.logger.debug('Response is %s' % response)
				self.logger.debug('Accepted amount is %d' % amount)
				self.send_ack()
				return amount
			except:
				pass


		return 0

	def receive_commit(self):
		response = self.send_ack()
		self.logger.debug('%s: commit result is %s' % (self.__prefix, response) )
		
	def receive_rollback(self):
		response = self.send_single_command(self.c_return, read_response = True)
		self.logger.debug('%s: eject result is %s' % (self.__prefix, response) )
		
	def send_ack(self):
		return self.send_single_command(self.p_ack, False)

	def send_nck(self):
		return self.send_single_command(self.p_nck, False)

	def bill_table(self):
		response = self.send_single_command(self.c_bill_table)

		for i in range(0,23):
				bytes=response[i*5 : i*5+5]
				print 'bytes: %s, %s' % (bytes, byte2str(bytes[1:4]))

	def __is_ack(self, data):
		return len(data)==1 and data[0]==self.p_ack

	def __is_nck(self, data):
		return len(data)==1 and data[0]==self.p_nck

	def _stacked_handler(self):
		pass

	def _rejected_handler(self):
		pass

	def wait(self, wait=[], skip=[], timeout = 10):

		self.logger.debug('[Cashcode_t1500->wait] start waiting %s' % timeout)

		while self._running( timeout ):

			response = self.poll()

			if(response[0] in wait):
				self.logger.debug('Normal response waited')
				return response

			if(response[0] in skip):
				self.validate_statement( response )
				continue

			message = "[Cashcode_t1500->wait(%s, %s, %s)] got=%s" % (wait, skip, timeout, response)
			self.logger.debug( message )
			raise AcceptorError(message, response)

			self.free(.2)

	def validate_statement(self, packet):

		if packet == [self.r_power_up]:
			self.logger.debug('State is POWER_UP')
			return

		if packet == [self.r_accepting]:
			self.logger.debug('State is ACCEPTING')
			return

		if packet == [self.r_stacking]:
			self.logger.debug('State is STACKING')
			return

		if packet == [self.rj_reject, self.e_insertation]:
			self.logger.debug('State is Error. Details: [rj_reject, e_insertation]')
			return

#		self.logger.debug( 'Statement is unknown' )



