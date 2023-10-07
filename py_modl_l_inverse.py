# ******************************************************************
# ----------------- TECHNICAL UNIVERSITY OF KOSICE -----------------
# ---Department of Electronics and Multimedia Telecommunications ---
# -------- FACULTY OF ELECTRICAL ENGINEERING AND INFORMATICS -------
# ------------ THIS CODE IS A PART OF A MASTER'S THESIS ------------
# ------------------------- Master thesis --------------------------
# -----------------Patrik Zelenak & Milos Drutarovsky --------------
# ---------------------------version 0.1.3 -------------------------
# --------------------------- 30-09-2023 ---------------------------
# ******************************************************************

from typing import Any
from enum import Enum

class INTERPRETATION(Enum):
	U32 = [32,8] # set this option to perform int/bytes conversion
	U16 = [16,16] # set this option to perform int/field_elem conversion
	Unique = [9,32]

# Converter class is suppose to convert from hexa-list representation to number (inteeger)
# and vice versa.
class Converter:
    # Function hexToNum converts hexa-list into inteeger
    # @[in]  input      -> hexa-list
    # @[in]  interpret  -> INTERPRETATION class element
    # @[in]  b          -> boolean, if True print result into console
    # @[out]            -> inteeger
    #
    # example: Converter.hexToNum(input_hexa_list,INTERPRETATION.U32,False)
	def hexToNum(input:list[int], interpret:INTERPRETATION, b): # if useed properly we can use it for unpack25519/pack25519 convertion compatible with TweetNaCl
		sum:int = 0

		for i in range(interpret.value[0]):
			m:int = 2**(interpret.value[1]*i)

			sum += input[i]*m

		if b:
			print(sum)
		return sum

    # Function numToHex converts inteeger into hexa-list 
    # Function hexToNum converts hexa-list into inteeger
    # @[in]  input      -> inteeger
    # @[in]  interpret  -> INTERPRETATION class element
    # @[in]  b          -> boolean, if True print result into console
    # @[out]            -> hexa-list
    #
    # example: Converter.numToHex(input_inteeger,INTERPRETATION.U32,False) 
	def numToHex(BIG_NUMBER:int, interpret:INTERPRETATION, b):
		result:list[str]=[]
		for i in range(interpret.value[0]-1,-1,-1):
			  m = 2**(interpret.value[1]*i)

			  a = BIG_NUMBER // m
			  BIG_NUMBER = BIG_NUMBER % m

			  result.append(a)

		result = result[::-1]

		if b == True:
			for i in range(len(result)):
			  if i%4==0:
			    print()
			  print(hex(result[i]), end=", ")
			print("\n")

		return result

#---------- CONSTANTS ---------
Lm2: list[int] =  [0xeb, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58,
        0xd6, 0x9c, 0xf7, 0xa2, 0xde, 0xf9, 0xde, 0x14,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10]

Lm2_num:int = Converter.hexToNum(Lm2,INTERPRETATION.U32,False)

#------------------------------

#-------- arithmetic --------

def scalar_bit(in_a, i):
	in_a_str = bin(in_a)[2::][::-1]
	if i<0:
		return 0
	return int(in_a_str[i])

def XOR(a,b):
	# a xor b
	if len(a)!=len(b):
		raise ValueError(f"a:{len(a)} in not eq to b{len(b)}")
	if not isinstance(a[0],int):
		a = [int(i,16) for i in a]
	if not isinstance(b[0],int):
		b = [int(i,16) for i in b]

	return [a[i]^b[i] for i in range(len(a))]

def mul_l(a,b):
		return (a*b) % (Lm2_num + 2)

def inverse_mod_l(a_in):
	m_inv:int = 1

	for i in range(252,-1,-1):
		m_inv = mul_l(m_inv,m_inv)

		if scalar_bit(Lm2_num,i):
			m_inv = mul_l(m_inv,a_in)

	return m_inv

#------------------------------

## TEST OF FUNCTION modl_l_inverse()
def generate_modl_l_inverse_test_vector(IN_hex:list[int], k_hex: int, rounds:int):
	k = [k_hex for i in range(32)]
	k_num = Converter.hexToNum(k,INTERPRETATION.U32,False)
	IN = Converter.hexToNum(IN_hex,INTERPRETATION.U32,False)

	print("Calculating ...")
	for i in range(rounds):
		r = inverse_mod_l(IN)

		r_32 = Converter.numToHex(r,INTERPRETATION.U32,False)
		
		IN = Converter.hexToNum(XOR(r_32,k),INTERPRETATION.U32,False)

	print("Result:")
	Converter.numToHex(IN,INTERPRETATION.U32,True)


## Feel free to change these variables

# iner[32] is input/output variable used in our test of
# function modl_l_inverse.
iner: list[int] = [
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
]

# testing_constant is used in 'masking' part of modl_l_inverse test
testing_constant: int = 0x57
# rounds sets amount of for-loop cycles
rounds:int = 1000

generate_modl_l_inverse_test_vector(iner,testing_constant,rounds)



















"""
INTERNAL COMMENTS FOR ME:

r_hex = [0x0a2c131b,0xed9ce5a3,0x086329a7,0x2106215d,
        0xffffffeb,0xffffffff,0xffffffff,0xffffffff,0xf]

r_num = Converter.hexToNum(r_hex,INTERPRETATION.Unique,True)

int("0xfffffffffffffffffffffffffbeffffffd51260127a923683a5ec9deb131c2a",16)
7237005577332262213973186563041544692391246655028413809887688208983268727850


x = ['5cf5d3ed', '5812631a', 'a2f79cd6', '14def9de','00000000', '00000000', '00000000', '10000000']

print(x)
def getLE(ele):
	elem = ele.split('0x')[1] if '0x' in ele else ele
	# reverse bytes
	return ''.join([elem[i:i+2] for i in range(0, len(elem), 2)][::-1])


['0a2c131b','ed9ce5a3','086329a7','2106215d',
'ffffffeb','ffffffff','ffffffff','ffffffff','0f']
"""