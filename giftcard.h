/*
 * Gift Card Application
 * Original Author: Shoddycorp's Cut-Rate Contracting
 * Maintainer:
 * Comments added by: Justin Cappos (JAC)
 * Modification Date: 8 July 2020
 */

// The purpose of this app is to read in gift card data.  The format of a gift 
// card looks like this:
//
// thisgiftcard:
//   num_bytes[32 bits]:giftcard_data (see below)
//
// giftcarddata:
//   merchant_id [32 bytes]:customer_id [32 bytes]:num_gift_card_records[32 bits]:gift_card_record_data[variable length]
//
// giftcardrecorddata:
//   num_bytes[32 bits]
//   type_of_record 
//   gift_card_amount_change OR gift_card_message
//
// giftcardamountchange:  Adds or removes money from the card
//   amount_added[32 bits]    Note, amounts are in 1 yen
//   merchant_signature[32 bytes]  USED IFF amount_added is POSITIVE
//
// giftcardmessage:
//   just a null terminated C string.
// 
// giftcardprogram: (BETA)
//   message[32 bytes]
//   program[256 bytes]
//
// So some actual example files might look like this:
//  [bytes][merchant_id][customer_id][num_records][bytes][type][AMOUNT][sig]
//    116     456...       524...         1          44      1   2000   fed...
//  [bytes][merchant_id][customer_id][num_records][bytes][type][AMOUNT][sig][bytes][type][message]
//    124    abc...        123...         2          44      1   1500   fed... 24    2   "Happy Birthday!\0"
//
// [BETA] program spec for animated text feature
// customer requested. Electronic greeting cards use THX-1138 miroprocessor and
// can support small programs to animate the text being displayed. Real processor
// has more opcodes than this but for beta demo just implement these
// programs operate on the message buffer
// note!! it is the gift card producers job to make sure that
// programs in the gift card are VALID
// each "instruction" is 3 bytes
// arithmetic instructions set the zero flag if the result is 0
// opcode (1 byte) arg1 (1 byte) arg2 (1 byte)
// 0x00  no-op (if there is extra space at end of program this should be used)
// 0x01  get current char into register "arg1" (16 registers)
// 0x02  put registar "arg1" into current char
// 0x03  move cursor by "arg1" bytes (can be negative)
// 0x04  put constant "arg1" into register "arg2"
// 0x05  xor register "arg1" with register "arg2" and store result in register "arg1"
// 0x06  add register "arg1" to register "arg2" and store result in register "arg1"
// 0x07  display the current message
// 0x08  end program
// 0x09  jump "arg1" bytes relative to the end of this instruction
// 0x10  if the zero flag is set, jump "arg1" bytes relative to the end of this instruction
// 0x11-0xff not used for now

// BDG note: this program structure looks complex. And I'm worried
//           about that comment that says programs must be "VALID"??
#include<stdlib.h>

struct this_gift_card {
	int num_bytes;
	void *gift_card_data;
}; 

struct gift_card_data {
	char *merchant_id; // always points to 32 bytes of data
	char *customer_id;// always points to 32 bytes of data
	int number_of_gift_card_records;
	void **gift_card_record_data; // many can exist here.
};


struct gift_card_record_data {
	int record_size_in_bytes; // size of this entry, including this field.
	int type_of_record; // 1 for amount change, 2 for message
	void *actual_record; // polymorphism
};



struct gift_card_amount_change {
	int amount_added;
	// always 32 bytes.  Only appears if amount_added is positive
	char *actual_signature; 
};

struct gift_card_program {
    char *message; // 32 bytes
    unsigned char *program; // 128 bytes
};
