# -*- coding: utf-8 -*-

class AcceptorError (Exception):
    def __init__(self, code, message):
        self.code, self.message = code, message

