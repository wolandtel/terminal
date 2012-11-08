# -*- coding: utf-8 -*-

from pprint import pprint
from common.Logger import Logger
from services.dispenser.dispensers.__proto__ import Dispenser


class Dispenser_LCDM1000(Dispenser):

	cassets_info	= {
		0 : {'command' : 0x45, 'denomination' : 0, 'charging' : 0, 'position' : 'upper'},
	}

	bps						 = 3.5   # Banknotes per second
	cassets_sleep_time		  = 3	 # Sleep time between dispensing of first and second cassets

	''' Limits for dispensing '''
	dispense_limit_single_10	= 6
	dispense_limit_single_1	 = 9
	dispense_limit_both_10	  = 6
	dispense_limit_both_1	   = 9
	dispense_limit			  = 60   # Limit banknotes count in transaction

	''' protocol commands '''
	c_purge					 = 0x44  # Initialize the dispenser
	c_dispense_upper			= 0x45  # Dispense banknotes from upper cash box
	c_status					= 0x46
	c_rom_version			   = 0x47
	c_dispense_lower			= 0x55  # Dispense banknotes from lower cash box
	c_dispense_upper_lower	  = 0x56
	c_test_dispense_upper	   = 0x76  # Test upper dispense
	c_test_dispense_lower	   = 0x77  # Test lower dispense


	@property
	def command_init(self):
		return [
			0x04,0x50,0x02,0x44,0x03,
			0x04^0x50^0x02^0x44^0x03
		]

	@property
	def command_status(self):
		return [
			0x04,0x50,0x02,0x46,0x03,
			0x04^0x50^0x02^0x46^0x03
		]

	@property
	def command_purge(self):
		return [
			0x04,0x50,0x02,0x44,0x03,
			0x04^0x50^0x02^0x44^0x03
		]

	@property
	def command_test_dispense(self):
		return [
			0x04,0x50,0x02,0x76,0x03,
			0x04^0x50^0x02^0x76^0x03
		]

	@property
	def command_rom_version(self):
		return [
			0x04,0x50,0x02,0x47,0x03,
			0x04^0x50^0x02^0x47^0x03
		]


	def command_dispense_single(self, cassets_info, id_cash_box, count):
		logger   = Logger()
		count_10 = min(int(count * 1. / 10), self.dispense_limit_single_10)
		count_1  = min(count - count_10 * 10, self.dispense_limit_single_1)
		
		logger.debug( 'Corrected counts: 10 (%d), 1 (%d)' % (count_10, count_1) )

		count_10 = ord("%s" % count_10)
		count_1 = ord("%s" % count_1)

		return [
			0x04,0x50,0x02,0x45,count_10,count_1,0x03,
			0x04^0x50^0x02^0x45^count_10^count_1^0x03
		]


	def validate_init_response(self, status_frame):
		return True

	def validate_purge_response(self, status_frame):
		return True

	def validate_status_response(self, status_frame):
		logger = Logger()
		logger.debug('Dispenser_LCDM1000: Validate status_frame %s. Len is %d' % (status_frame, len(status_frame)))

		# Pop ACK
		status_frame.pop(0)

		response	= {
			'status'		: True,
			'error_cause'   : "%X" % status_frame[5] ,
			'sensor_0'	  : "%X" % status_frame[6],
			'sensor_1'	  : "%X" % status_frame[7]
		}
		return response

	def validate_dispense_response(self, response_frame):
		logger = Logger()
		logger.debug('Dispenser_LCDM1000: validate "Dispense" response frame %s' % response_frame)

		template = {
			'status'					: False,
			'error_cause'			   : 0,
			'upper_chk_requested_10'	: 0,
			'upper_chk_requested_1'	 : 0,
			'upper_exit_requested_10'   : 0,
			'upper_exit_requested_1'	: 0,
			'upper_rejects_10'		  : 0,
			'upper_rejects_1'		   : 0,
			'upper_status'			  : 0,
			'lower_chk_requested_10'	: 0,
			'lower_chk_requested_1'	 : 0,
			'lower_exit_requested_10'   : 0,
			'lower_exit_requested_1'	: 0,
			'lower_rejects_10'		  : 0,
			'lower_rejects_1'		   : 0,
			'lower_status'			  : 0
		}


		# Hook
		response_frame[3] = int(response_frame[3])

		# Error while dispensing
		if len(response_frame) == 7:
			logger.debug('Dispenser_LCDM1000: len(response_frame) == 7')
			return False

		if len(response_frame) == 21:
			return self.__validate_dispense_both_response(response_frame, template)

		if response_frame[3] in [0x45, 0x55]:
			return self.__validate_dispense_single_response(response_frame, template)

	@staticmethod
	def __validate_dispense_single_response(response_frame, template):
		logger = Logger()
		response	= {}

		if response_frame[3] == 0x45:
			logger.debug('Dispenser_LCDM1000: upper cassete validator')
			chk_sensor_exit_10  = 'upper_chk_requested_10'
			chk_sensor_exit_1   = 'upper_chk_requested_1'
			exit_requested_10   = 'upper_exit_requested_10'
			exit_requested_1	= 'upper_exit_requested_1'
			rejects_10		  = 'upper_rejects_10'
			rejects_1		   = 'upper_rejects_1'
			status			  = 'upper_status'

		elif response_frame[3] == 0x55:
			logger.debug('Dispenser_LCDM1000: lower cassete validator')
			chk_sensor_exit_10  = 'lower_chk_requested_10'
			chk_sensor_exit_1   = 'lower_chk_requested_1'
			exit_requested_10   = 'lower_exit_requested_10'
			exit_requested_1	= 'lower_exit_requested_1'
			rejects_10		  = 'lower_rejects_10'
			rejects_1		   = 'lower_rejects_1'
			status			  = 'lower_status'
		else:
			logger.debug('Dispenser_LCDM1000: UNKNOWN cassete')
			logger.debug('Dispenser_LCDM1000: response_frame[3] is %s' % response_frame[3])
			logger.debug('Dispenser_LCDM1000: response_frame[3] is %X' % response_frame[3])

		template[chk_sensor_exit_10]	= chr(response_frame[4])
		template[chk_sensor_exit_1]	 = chr(response_frame[5])
		template[exit_requested_10]	 = chr(response_frame[6])
		template[exit_requested_1]	  = chr(response_frame[7])
		template[rejects_10]			= chr(response_frame[10])
		template[rejects_1]			 = chr(response_frame[11])
		template['error_cause']		 = response_frame[8]
		template[status]				= response_frame[9]

		logger.debug('Dispenser_LCDM1000: Validate response_frame to dispense_single : %s' % response_frame)
		return template

	@staticmethod
	def __validate_dispense_both_response( response_frame, template ):
		logger = Logger()
		logger.debug('Dispenser_LCDM1000: Validate response_frame to dispense_both : %s' % response_frame)

		template['status']				  = response_frame[12] in [0x30, 0x31]
		template['error_cause']			 = response_frame[12]

		template['upper_chk_requested_10']  = chr(response_frame[4])
		template['upper_chk_requested_1']   = chr(response_frame[5])
		template['upper_exit_requested_10'] = chr(response_frame[6])
		template['upper_exit_requested_1']  = chr(response_frame[7])

		template['lower_chk_requested_10']  = chr(response_frame[8])
		template['lower_chk_requested_1']   = chr(response_frame[9])
		template['lower_exit_requested_10'] = chr(response_frame[10])
		template['lower_exit_requested_1']  = chr(response_frame[11])

		template['upper_rejects_10']		= chr(response_frame[15])
		template['upper_rejects_1']		 = chr(response_frame[16])
		template['upper_status']			= chr(response_frame[13])

		template['lower_rejects_10']		= chr(response_frame[17])
		template['lower_rejects_1']		 = chr(response_frame[18])
		template['lower_status']			= chr(response_frame[14])

		return template