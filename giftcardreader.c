/*
 * Gift Card Reading Application
 * Original Author: Shoddycorp's Cut-Rate Contracting
 * Comments added by: Justin Cappos (JAC) and Brendan Dolan-Gavitt (BDG)
 * Maintainer:
 * Date: 8 July 2020
 */


#include "giftcard.h"

#include <stdio.h>
#include <strings.h>

// interpreter for THX-1138 assembly
void animate(char *msg, unsigned char *program) {
    unsigned char regs[16];
    char *mptr = msg;
    unsigned char *pc = program;
    int i = 0;
    int zf = 0;
    while (1) {
        unsigned char op, arg1, arg2;
        op = *pc;
        arg1 = *(pc+1);
        arg2 = *(pc+2);
        switch (*pc) {
            case 0x00:
                break;
            case 0x01:
                regs[arg1] = *mptr;
                break;
            case 0x02:
                *mptr = regs[arg1];
                break;
            case 0x03:
                mptr += (char)arg1;
                break;
            case 0x04:
                regs[arg2] = arg1;
                break;
            case 0x05:
                regs[arg1] ^= regs[arg2];
                zf = !regs[arg1];
                break;
            case 0x06:
                regs[arg1] += regs[arg2];
                zf = !regs[arg1];
                break;
            case 0x07:
                puts(msg);
                break;
            case 0x08:
                goto done;
            case 0x09:
                pc += (char)arg1;
                break;
            case 0x10:
                if (zf) pc += (char)arg1;
                break;
        }
        pc+=3;
        if (pc > program+256) break;
    }
done:
    return;
}

void print_gift_card_info(struct this_gift_card *thisone) {
	struct gift_card_data *gcd_ptr;
	struct gift_card_record_data *gcrd_ptr;
	struct gift_card_amount_change *gcac_ptr;
    struct gift_card_program *gcp_ptr;

	gcd_ptr = thisone->gift_card_data;
	printf("   Merchant ID: %32.32s\n",gcd_ptr->merchant_id);
	printf("   Customer ID: %32.32s\n",gcd_ptr->customer_id);
	printf("   Num records: %d\n",gcd_ptr->number_of_gift_card_records);
	for(int i=0;i<gcd_ptr->number_of_gift_card_records; i++) {
  		gcrd_ptr = (struct gift_card_record_data *) gcd_ptr->gift_card_record_data[i];
		if (gcrd_ptr->type_of_record == 1) {
			printf("      record_type: amount_change\n");
			gcac_ptr = gcrd_ptr->actual_record;
			printf("      amount_added: %d\n",gcac_ptr->amount_added);
			if (gcac_ptr->amount_added>0) {
				printf("      signature: %32.32s\n",gcac_ptr->actual_signature);
			}
		}	
		else if (gcrd_ptr->type_of_record == 2) {
			printf("      record_type: message\n");
			printf("      message: %s\n",(char *)gcrd_ptr->actual_record);
		}
		else if (gcrd_ptr->type_of_record == 3) {
            gcp_ptr = gcrd_ptr->actual_record;
			printf("      record_type: animated message\n");
            printf("      message: %s\n", gcp_ptr->message);
            printf("  [running embedded program]  \n");
            animate(gcp_ptr->message, gcp_ptr->program);
		}
	}
	printf("  Total value: %d\n\n",get_gift_card_value(thisone));
}

// Added to support web functionalities
void gift_card_json(struct this_gift_card *thisone) {
    struct gift_card_data *gcd_ptr;
    struct gift_card_record_data *gcrd_ptr;
    struct gift_card_amount_change *gcac_ptr;
    gcd_ptr = thisone->gift_card_data;
    printf("{\n");
    printf("  \"merchant_id\": \"%32.32s\",\n", gcd_ptr->merchant_id);
    printf("  \"customer_id\": \"%32.32s\",\n", gcd_ptr->customer_id);
    printf("  \"total_value\": %d,\n", get_gift_card_value(thisone));
    printf("  \"records\": [\n");
	for(int i=0;i<gcd_ptr->number_of_gift_card_records; i++) {
        gcrd_ptr = (struct gift_card_record_data *) gcd_ptr->gift_card_record_data[i];
        printf("    {\n");
        if (gcrd_ptr->type_of_record == 1) {
            printf("      \"record_type\": \"amount_change\",\n");
            gcac_ptr = gcrd_ptr->actual_record;
            printf("      \"amount_added\": %d,\n",gcac_ptr->amount_added);
            if (gcac_ptr->amount_added>0) {
                printf("      \"signature\": \"%32.32s\"\n",gcac_ptr->actual_signature);
            }
        }
        else if (gcrd_ptr->type_of_record == 2) {
			printf("      \"record_type\": \"message\",\n");
			printf("      \"message\": \"%s\"\n",(char *)gcrd_ptr->actual_record);
        }
        else if (gcrd_ptr->type_of_record == 3) {
            struct gift_card_program *gcp = gcrd_ptr->actual_record;
			printf("      \"record_type\": \"animated message\",\n");
			printf("      \"message\": \"%s\",\n",gcp->message);
            // programs are binary so we will hex for the json
            char *hexchars = "01234567890abcdef";
            char program_hex[512+1];
            program_hex[512] = '\0';
            int i;
            for(i = 0; i < 256; i++) {
                program_hex[i*2] = hexchars[((gcp->program[i] & 0xf0) >> 4)];
                program_hex[i*2+1] = hexchars[(gcp->program[i] & 0x0f)];
            }
			printf("      \"program\": \"%s\"\n",program_hex);
        }
        if (i < gcd_ptr->number_of_gift_card_records-1)
            printf("    },\n");
        else
            printf("    }\n");
    }
    printf("  ]\n");
    printf("}\n");
}

int get_gift_card_value(struct this_gift_card *thisone) {
	struct gift_card_data *gcd_ptr;
	struct gift_card_record_data *gcrd_ptr;
	struct gift_card_amount_change *gcac_ptr;
	int ret_count = 0;

	gcd_ptr = thisone->gift_card_data;
	for(int i=0;i<gcd_ptr->number_of_gift_card_records; i++) {
  		gcrd_ptr = (struct gift_card_record_data *) gcd_ptr->gift_card_record_data[i];
		if (gcrd_ptr->type_of_record == 1) {
			gcac_ptr = gcrd_ptr->actual_record;
			ret_count += gcac_ptr->amount_added;
		}	
	}
	return ret_count;
}



/* JAC: input_fd is misleading... It's a FILE type, not a fd */
struct this_gift_card *gift_card_reader(FILE *input_fd) {

	struct this_gift_card *ret_val = malloc(sizeof(struct this_gift_card));

    void *optr;
	void *ptr;

	// Loop to do the whole file
	while (!feof(input_fd)) {

		struct gift_card_data *gcd_ptr;
		/* JAC: Why aren't return types checked? */
		fread(&ret_val->num_bytes, 4,1, input_fd);

		// Make something the size of the rest and read it in
		ptr = malloc(ret_val->num_bytes);
		fread(ptr, ret_val->num_bytes, 1, input_fd);

        optr = ptr-4;

		gcd_ptr = ret_val->gift_card_data = malloc(sizeof(struct gift_card_data));
		gcd_ptr->merchant_id = ptr;
		ptr += 32;	
//		printf("VD: %d\n",(int)ptr - (int) gcd_ptr->merchant_id);
		gcd_ptr->customer_id = ptr;
		ptr += 32;	
		/* JAC: Something seems off here... */
		gcd_ptr->number_of_gift_card_records = *((char *)ptr);
		ptr += 4;

		gcd_ptr->gift_card_record_data = (void *)malloc(gcd_ptr->number_of_gift_card_records*sizeof(void*));

		// Now ptr points at the gift card recrod data
		for (int i=0; i<=gcd_ptr->number_of_gift_card_records; i++){
			//printf("i: %d\n",i);
			struct gift_card_record_data *gcrd_ptr;
			gcrd_ptr = gcd_ptr->gift_card_record_data[i] = malloc(sizeof(struct gift_card_record_data));
			struct gift_card_amount_change *gcac_ptr;
			gcac_ptr = gcrd_ptr->actual_record = malloc(sizeof(struct gift_card_record_data));
            struct gift_card_program *gcp_ptr;
			gcp_ptr = malloc(sizeof(struct gift_card_program));

			gcrd_ptr->record_size_in_bytes = *((char *)ptr);
            //printf("rec at %x, %d bytes\n", ptr - optr, gcrd_ptr->record_size_in_bytes); 
			ptr += 4;	
			//printf("record_data: %d\n",gcrd_ptr->record_size_in_bytes);
			gcrd_ptr->type_of_record = *((char *)ptr);
			ptr += 4;	
            //printf("type of rec: %d\n", gcrd_ptr->type_of_record);

			// amount change
			if (gcrd_ptr->type_of_record == 1) {
				gcac_ptr->amount_added = *((int*) ptr);
				ptr += 4;	

				// don't need a sig if negative
				/* JAC: something seems off here */
				if (gcac_ptr < 0) break;

				gcac_ptr->actual_signature = ptr;
				ptr+=32;
			}
			// message
			if (gcrd_ptr->type_of_record == 2) {
				gcrd_ptr->actual_record = ptr;
				// advance by the string size + 1 for nul
                // BDG: does not seem right
				ptr=ptr+strlen((char *)gcrd_ptr->actual_record)+1;
			}
            // BDG: never seen one of these in the wild
            // text animatino (BETA)
            if (gcrd_ptr->type_of_record == 3) {
                gcp_ptr->message = malloc(32);
                gcp_ptr->program = malloc(256);
                memcpy(gcp_ptr->message, ptr, 32);
                ptr+=32;
                memcpy(gcp_ptr->program, ptr, 256);
                ptr+=256;
                gcrd_ptr->actual_record = gcp_ptr;
            }
		}
	}
	return ret_val;
}

// BDG: why not a local variable here?
struct this_gift_card *thisone;

int main(int argc, char **argv) {
    // BDG: no argument checking?
	FILE *input_fd = fopen(argv[2],"r");
	thisone = gift_card_reader(input_fd);
	if (argv[1][0] == '1') print_gift_card_info(thisone);
    else if (argv[1][0] == '2') gift_card_json(thisone);

	return 0;
}
