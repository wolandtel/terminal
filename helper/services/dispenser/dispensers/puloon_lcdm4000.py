# -*- coding: utf-8 -*-

from common.Logger import Logger
from services.dispenser.dispensers.__proto__ import Dispenser

'''

	public static $cassetes_names = array(
		  self::CASSETE_ID_UPPER				=> "upper",
		  self::CASSETE_ID_UPPER_REJECT		 => "upper_reject",
		  self::CASSETE_ID_LOWER				=> "lower",
		  self::CASSETE_ID_LOWER_REJECT		 => "lower_reject",
		  self::CASSETE_ID_MIDDLE_UPPER		 => "m_upper",
		  self::CASSETE_ID_MIDDLE_UPPER_REJECT  => "m_upper_reject",
		  self::CASSETE_ID_MIDDLE_LOWER		 => "m_lower",
		  self::CASSETE_ID_MIDDLE_LOWER_REJECT  => "m_lower_reject",
		);
'''
class Dispenser_LCDM4000(Dispenser):

	cassets_info	= {
		0 : {'command' : 0x45, 'denomination' : 0, 'charging' : 0, 'position' : 'upper'},
		1 : {'command' : 0x55, 'denomination' : 0, 'charging' : 0, 'position' : 'lower'},
		4 : {'command' : 0x55, 'denomination' : 0, 'charging' : 0, 'position' : 'm_upper'},
		5 : {'command' : 0x55, 'denomination' : 0, 'charging' : 0, 'position' : 'm_lower'}
	}

	bps						 = 3.5   # Banknotes per second
	cassets_sleep_time		  = 3	 # Sleep time between dispensing of first and second cassets

	''' Limits for dispensing '''
	dispense_limit_single_10	= 6
	dispense_limit_single_1	 = 9
	dispense_limit_both_10	  = 6
	dispense_limit_both_1	   = 9
	dispense_limit			  = 100   # Limit banknotes count in transaction

	''' protocol commands '''
	c_purge					 = 0x44  # Initialize the dispenser
	c_dispense_upper			= 0x45  # Dispense banknotes from upper cash box
	c_status					= 0x46
	c_rom_version			   = 0x47
	c_dispense_lower			= 0x55  # Dispense banknotes from lower cash box
	c_dispense_upper_lower	  = 0x56
	c_test_dispense_upper	   = 0x76  # Test upper dispense
	c_test_dispense_lower	   = 0x77  # Test lower dispense

	''' Override '''
	p_id						= 0x30  # Communication ID
	


	@property
	def command_init(self):
		return [
			0x04,0x30,0x02,0x44,0x03,
			0x04^0x30^0x02^0x44^0x03
		]

	@property
	def command_status(self):
		return [
			0x04,0x30,0x02,0x50,0x03,
			0x04^0x30^0x02^0x50^0x03
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

		logger = Logger()
		logger.debug('Dispenser_LCDM4000: Request to dispense will be converted from single to multi dispense')

		CASSETE_ID_UPPER		= 0
		CASSETE_ID_LOWER		= 1
		CASSETE_ID_MIDDLE_UPPER = 4
		CASSETE_ID_MIDDLE_LOWER = 5

		cassets_dispense = {
			CASSETE_ID_UPPER		: 0,
			CASSETE_ID_LOWER		: 0,
			CASSETE_ID_MIDDLE_UPPER : 0,
			CASSETE_ID_MIDDLE_LOWER : 0,
		}

		cassets_dispense[id_cash_box] = count
		return self.command_dispense_several(cassets_info, cassets_dispense)


	'''
		const CASSETE_ID_UPPER				= 0;
		const CASSETE_ID_LOWER				= 1;
		const CASSETE_ID_UPPER_REJECT		 = 2;
		const CASSETE_ID_LOWER_REJECT		 = 3;
		const CASSETE_ID_MIDDLE_UPPER		 = 4;
		const CASSETE_ID_MIDDLE_LOWER		 = 5;
		const CASSETE_ID_MIDDLE_UPPER_REJECT  = 6;
		const CASSETE_ID_MIDDLE_LOWER_REJECT  = 7;
	'''
	def command_dispense_several(self, cassets_info, cassets_dispense):
		logger = Logger()
		logger.debug('Dispenser_LCDM4000: Amount will be dispensed from several cassettes')

		# check limits
		upper_count   = int(cassets_dispense.get(0,0))
		m_upper_count = int(cassets_dispense.get(4,0))
		m_lower_count = int(cassets_dispense.get(5,0))
		lower_count   = int(cassets_dispense.get(1,0))

		logger.debug('Dispenser_LCDM4000: upper_count   : %d'  % upper_count)
		logger.debug('Dispenser_LCDM4000: m_upper_count : %d'  % m_upper_count)
		logger.debug('Dispenser_LCDM4000: m_lower_count : %d'  % m_lower_count)
		logger.debug('Dispenser_LCDM4000: lower_count   : %d'  % lower_count)

		'''
			The number of bills to be dispensed from cassette + 0x20
		'''
		upper_count	+= 0x20
		m_upper_count  += 0x20
		m_lower_count  += 0x20
		lower_count	+= 0x20

		return [
			0x04,0x30,0x02,0x52,upper_count,m_upper_count,m_lower_count,lower_count,0x20,0x20,0x20,0x03,
			0x04^0x30^0x02^0x52^upper_count^m_upper_count^m_lower_count^lower_count^0x20^0x20^0x20^0x03
		]

	def validate_init_response(self, status_frame):
		return True

	def validate_purge_response(self, status_frame):
		return True

	def validate_status_response(self, status_frame):
		logger = Logger()
		logger.debug('Dispenser_LCDM4000: Validate status_frame %s. Len is %d' % (status_frame, len(status_frame)))

		# Pop ACK
		status_frame.pop(0)

		response	= {
			'status'		: True,
			'error_cause'   : "%X" % status_frame[5],
			'sensor_0'	  : "%X" % status_frame[6],  # Upper
			'sensor_1'	  : "%X" % status_frame[9],  # Middle upper
			'sensor_2'	  : "%X" % status_frame[12], # Middle lower
			'sensor_3'	  : "%X" % status_frame[15], # Lower
		}
		return response

	def validate_dispense_response(self, response_frame):
		logger = Logger()
		logger.debug('Dispenser_LCDM4000: validate "Dispense" response frame %s' % (["0x%02X" % x for x in response_frame]))

		template = {
			'status'					: False,
			'error_cause'			   : response_frame[4],
			'miss'					  : response_frame[5],
			'upper_exit_requested_1'	: response_frame[6] - 0x20,
			'upper_exit_requested_10'   : 0,
			'upper_chk_requested_1'	 : response_frame[6] - 0x20,
			'upper_chk_requested_10'	: 0,
			'upper_rejects_1'		   : chr(response_frame[7] - 0x20),
			'upper_rejects_10'		  : 0,
			'type_1'					: chr(response_frame[8]),

			'm_upper_exit_requested_1'  : response_frame[9] - 0x20,
			'm_upper_exit_requested_10' : 0,
			'm_upper_chk_requested_1'   : response_frame[9] - 0x20,
			'm_upper_chk_requested_10'  : 0,
			'm_upper_rejects_1'		 : chr(response_frame[10] - 0x20),
			'm_upper_rejects_10'		: 0,
			'type_2'					: chr(response_frame[11]),

			'm_lower_exit_requested_1'  : response_frame[12] - 0x20,
			'm_lower_exit_requested_10' : 0,
			'm_lower_chk_requested_1'   : response_frame[12] - 0x20,
			'm_lower_chk_requested_10'  : 0,
			'm_lower_rejects_1'		 : chr(response_frame[13] - 0x20),
			'm_lower_rejects_10'		: 0,
			'type_3'					: chr(response_frame[14]),

			'lower_exit_requested_1'	: response_frame[15] - 0x20,
			'lower_exit_requested_10'   : 0,
			'lower_chk_requested_1'	 : response_frame[15] - 0x20,
			'lower_chk_requested_10'	: 0,
			'lower_rejects_1'		   : chr(response_frame[16] - 0x20),
			'lower_rejects_10'		  : 0,
			'type_4'					: chr(response_frame[17]),

			'rsv'		   : 0,
			'etx'		   : 0,
			'bcc'		   : 0
		}

		for k,v in template.items():
			logger.debug(' > %s => %s' % (k,v))

		return template

		if len(response_frame) == 21:
			logger.debug('Allright. Len response_frame is 21')
		else:
			logger.debug('Allright. Len response_frame is %s. Frame is %s' % (len(response_frame), (["%02x" % x for x in response_frame]) ) )

		for k, v in template.items():
			logger.debug(" ]]] %s: %s" % (k, v))
		return False

		# Error while dispensing
		if len(response_frame) == 7:
			logger.debug('Dispenser_LCDM4000: len(response_frame) == 7')
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
			logger.debug('Dispenser_LCDM4000: upper cassete validator')
			chk_sensor_exit_10  = 'upper_chk_requested_10'
			chk_sensor_exit_1   = 'upper_chk_requested_1'
			exit_requested_10   = 'upper_exit_requested_10'
			exit_requested_1	= 'upper_exit_requested_1'
			rejects_10		  = 'upper_rejects_10'
			rejects_1		   = 'upper_rejects_1'
			status			  = 'upper_status'

		elif response_frame[3] == 0x55:
			logger.debug('Dispenser_LCDM4000: lower cassete validator')
			chk_sensor_exit_10  = 'lower_chk_requested_10'
			chk_sensor_exit_1   = 'lower_chk_requested_1'
			exit_requested_10   = 'lower_exit_requested_10'
			exit_requested_1	= 'lower_exit_requested_1'
			rejects_10		  = 'lower_rejects_10'
			rejects_1		   = 'lower_rejects_1'
			status			  = 'lower_status'
		else:
			logger.debug('Dispenser_LCDM4000: UNKNOWN cassete')
			logger.debug('Dispenser_LCDM4000: response_frame[3] is %s' % response_frame[3])
			logger.debug('Dispenser_LCDM4000: response_frame[3] is %X' % response_frame[3])


		template[chk_sensor_exit_10]	= chr(response_frame[4])
		template[chk_sensor_exit_1]	 = chr(response_frame[5])
		template[exit_requested_10]	 = chr(response_frame[6])
		template[exit_requested_1]	  = chr(response_frame[7])
		template[rejects_10]			= chr(response_frame[10])
		template[rejects_1]			 = chr(response_frame[11])
		template['error_cause']		 = response_frame[8]
		template[status]				= response_frame[9]

		logger.debug('Dispenser_LCDM4000: Validate response_frame to dispense_single : %s' % response_frame)

		return template


	@staticmethod
	def __validate_dispense_both_response( response_frame, template ):
		logger = Logger()
		logger.debug('Dispenser_LCDM4000: Validate response_frame to dispense_both : %s' % response_frame)

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