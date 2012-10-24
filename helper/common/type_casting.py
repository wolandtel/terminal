# -*- coding: utf-8 -*-
import struct

def to_bytes(seq):
    """convert a sequence to a bytes type"""
    b = bytearray()
    for item in seq:
        b.append(item)
    return bytes(b)

def str2bytes(s):
    bytes=[]
    for i in s:
        bytes = bytes + [ord(i)]
    return bytes

def int2bytes(n):
    bytes=[]
    for i in struct.pack('H',n):
        bytes = bytes + [ord(i)]
    return bytes

def byte2hex( byte_str ):
     return ''.join( [ "%X " % ord( x ) for x in byte_str ] ).strip()

def byte2dec(byte_str):
    return hex2dec( byte2hex(byte_str) )

def byte2str(bytes):
    s = ''
    for i in bytes:
        s = s + chr(i)
    return s

def dec2hex(n):
    return "%X" % n

def hex2dec(x):
    return int("%s" % x, 16)

def hex2ascii2dec(hex):
    return chr(hex)

def ints2hex(list):
    s=''
    for i in list:
        s = s+':' + hex(i)[2:4]
    return s