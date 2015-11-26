/* Copyright (c) 2015 Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <parse_utils.h>

struct input_field file_field;
char line_data[MAX_LINE_SIZE];
extern struct g_data_t gd;

static parse_struct_t parse_table[] = {
	{ "PLATFORM", FIELD_PLATFORM },
	{ "ENTRY_POINT", FIELD_ENTRY_POINT },
	{ "PUB_KEY", FIELD_PUB_KEY },
	{ "KEY_SELECT", FIELD_KEY_SELECT },
	{ "IMAGE_1", FIELD_IMAGE_1 },
	{ "IMAGE_2", FIELD_IMAGE_2 },
	{ "IMAGE_3", FIELD_IMAGE_3 },
	{ "IMAGE_4", FIELD_IMAGE_4 },
	{ "IMAGE_5", FIELD_IMAGE_5 },
	{ "IMAGE_6", FIELD_IMAGE_6 },
	{ "IMAGE_7", FIELD_IMAGE_7 },
	{ "IMAGE_8", FIELD_IMAGE_8 },
	{ "FSL_UID_0", FIELD_FSL_UID_0 },
	{ "FSL_UID_1", FIELD_FSL_UID_1 },
	{ "OEM_UID_0", FIELD_OEM_UID_0 },
	{ "OEM_UID_1", FIELD_OEM_UID_1 },
	{ "OEM_UID_2", FIELD_OEM_UID_2 },
	{ "OEM_UID_3", FIELD_OEM_UID_3 },
	{ "OEM_UID_4", FIELD_OEM_UID_4 },
	{ "OUTPUT_HDR_FILENAME", FIELD_OUTPUT_HDR_FILENAME },
	{ "MP_FLAG", FIELD_MP_FLAG },
	{ "ISS_FLAG", FIELD_ISS_FLAG },
	{ "LW_FLAG", FIELD_LW_FLAG },
	{ "PRI_KEY", FIELD_PRI_KEY },
	{ "IMAGE_HASH_FILENAME", FIELD_IMAGE_HASH_FILENAME },
	{ "RSA_SIGN_FILENAME", FIELD_RSA_SIGN_FILENAME },
	{ "RCW_PBI_FILENAME", FIELD_RCW_PBI_FILENAME },
	{ "BOOT1_PTR", FIELD_BOOT1_PTR },
	{ "VERBOSE", FIELD_VERBOSE },

};

#define NUM_FIELDS (sizeof(parse_table) / sizeof(parse_struct_t))

enum input_field_t index_from_field(char *field)
{
	int i;
	for (i = 0; i < NUM_FIELDS; i++) {
		if (strcmp(parse_table[i].field_name, field) == 0)
			return parse_table[i].index;
	}
	return FIELD_UNKNOWN_MAX;
}

/***************************************************************************
 * Function	:	STR_TO_UL
 * Arguments	:	str - String
 *			base - Base (Decimal, HEX)
 * Return	:	unsigned long value
 * Description	:	Converts the string to unsigned long value
 ***************************************************************************/
unsigned long STR_TO_UL(char *str, int base)
{
	unsigned long val;
	char *endptr;
	char *neg;

	/* To distinguish success/failure for strtoul*/
	errno = 0;

	/* Checking for negative values*/
	neg = str;
	if (strchr(neg, '-') != NULL) {
		printf("Field is populated incorrectly with negative value\n");
		exit(EXIT_FAILURE);
	}

	/* Convert string to unsigned long*/
	val = strtoul(str, &endptr, base);

	/* Some invalid character is there in the field value */
	if (*endptr != '\0') {
		printf("Field is populated incorrectly with value %s in %s\n",
			endptr, str);
		exit(EXIT_FAILURE);
	}

	/* Check for various possible errors */
	if (((errno == ERANGE) && (val == ULONG_MAX)) ||
	    (errno != 0 && val == 0)) {
		printf("Field populated incorrectly with value %s\n", endptr);
		exit(EXIT_FAILURE);
	}

	if (*endptr == '\0')
		return val;

	exit(EXIT_FAILURE);
}

/***************************************************************************
 * Function	:	STR_TO_ULL
 * Arguments	:	str - String
 *			base - Base (Decimal, HEX)
 * Return	:	unsigned long value
 * Description	:	Converts the string to unsigned long long value
 ***************************************************************************/
unsigned long long STR_TO_ULL(char *str, int base)
{
	unsigned long val;
	char *endptr;
	char *neg;

	/* To distinguish success/failure for strtoul*/
	errno = 0;

	/* Checking for negative values*/
	neg = str;
	if (strchr(neg, '-') != NULL) {
		printf("Field is populated incorrectly with negative value\n");
		exit(EXIT_FAILURE);
	}

	/* Convert string to unsigned long*/
	val = strtoull(str, &endptr, base);

	/* Some invalid character is there in the field value */
	if (*endptr != '\0') {
		printf("Field is populated incorrectly with value %s in %s\n",
			endptr, str);
		exit(EXIT_FAILURE);
	}

	/* Check for various possible errors */
	if (((errno == ERANGE) && (val == ULLONG_MAX)) ||
	    (errno != 0 && val == 0)) {
		printf("Field populated incorrectly with value %s\n", endptr);
		exit(EXIT_FAILURE);
	}

	if (*endptr == '\0')
		return val;

	exit(EXIT_FAILURE);
}


/***************************************************************************
 * Functions from Parsing of Input File
 ****************************************************************************/
int cal_line_size(FILE *fp)
{
	int ctr = 0;
	int ch = 'a';
	while (ch != EOF) {
		if ((ch == '\n') && (ctr != 1))
			return ctr;

		ch = fgetc(fp);
		ctr++;
	}
	return 0;
}


void get_field_from_file(char *line, char *field_name)
{
	int i = 0;
	char delims[] = ",;=";
	char *result = NULL;

	result = strtok(line, delims);
	while (result != NULL) {
		result = strtok(NULL, delims);
		file_field.value[i] = result;
		i++;
	}
	file_field.count = i - 1;
}


void remove_whitespace(char *line)
{
	char *p1;
	char *p2 = line;
	p1 = line;
	while (*p1 != 0) {
		if (*p1 == '{' || *p1 == '}' || *p1 == '[' || *p1 == ']' ||
		    isspace(*p1) || *p1 == '(' || *p1 == ')') {
			++p1;
		} else {
			*p2++ = *p1++;
		}
	}
	*p2 = 0;
}


void find_value_from_file(char *field_name, FILE *fp)
{
	int line_size = 0;
	int i = 0;

	for (i = 0; i < 64; i++)
		file_field.value[i] = NULL;

	file_field.count = 0;

	fseek(fp, 0, SEEK_SET);
	line_size = cal_line_size(fp);
	fseek(fp, -line_size, SEEK_CUR);

	while (fread(line_data, 1, line_size, fp)) {
		*(line_data + line_size) = '\0';
		remove_whitespace(line_data);
		if ((strstr(line_data, field_name)) && (*line_data != '#')) {
			get_field_from_file(line_data, field_name);
			return;
		}
		line_size = cal_line_size(fp);

		fseek(fp, -line_size, SEEK_CUR);
	}
	file_field.count = -1;
}

int fill_gd_input_file(char *field_name, FILE *fp)
{
	int i;
	DWord val64;
	enum input_field_t idx;

	idx = index_from_field(field_name);

	if (idx == FIELD_UNKNOWN_MAX) {
		printf("\n Invalid Field being parsed");
		return FAILURE;
	}

	find_value_from_file(field_name, fp);

	switch (idx) {
	case FIELD_ENTRY_POINT:
		if (file_field.count == 1) {
			val64.whole = STR_TO_ULL(file_field.value[0], 16);
			gd.entry_addr_high = val64.m_halfs.high;
			gd.entry_addr_low = val64.m_halfs.low;
		}
		break;

	case FIELD_PUB_KEY:
		gd.num_srk_entries = file_field.count;
		if (gd.num_srk_entries >= 1) {
			i = 0;
			while (i != gd.num_srk_entries) {
				strcpy(gd.pub_fname[i], file_field.value[i]);
				i++;
				if (i == MAX_NUM_KEY) {
					printf("\n Key Number Limit reached");
					break;
				}
			}
		}
		break;

	case FIELD_PRI_KEY:
		gd.num_pri_key = file_field.count;
		if (gd.num_pri_key >= 1) {
			i = 0;
			while (i != gd.num_pri_key) {
				strcpy(gd.pri_fname[i], file_field.value[i]);
				i++;
				if (i == MAX_NUM_KEY) {
					printf("\n Key Number Limit reached");
					break;
				}
			}
		}
		break;

	case FIELD_KEY_SELECT:
		if (file_field.count == 1)
			gd.srk_sel = STR_TO_UL(file_field.value[0], 16);

		break;

	case FIELD_IMAGE_1:
		if (file_field.count >= 2) {
			strcpy(gd.entries[0].name, file_field.value[0]);
			val64.whole = STR_TO_ULL(file_field.value[1], 16);
			gd.entries[0].addr_high = val64.m_halfs.high;
			gd.entries[0].addr_low = val64.m_halfs.low;
			gd.num_entries++;
		}
		if (file_field.count >= 3) {
			gd.entries[0].dst_addr =
				STR_TO_UL(file_field.value[2], 16);
		}
		break;

	case FIELD_IMAGE_2:
		if (file_field.count >= 2) {
			strcpy(gd.entries[1].name, file_field.value[0]);
			val64.whole = STR_TO_ULL(file_field.value[1], 16);
			gd.entries[1].addr_high = val64.m_halfs.high;
			gd.entries[1].addr_low = val64.m_halfs.low;
			gd.num_entries++;
		}
		if (file_field.count >= 3) {
			gd.entries[1].dst_addr =
				STR_TO_UL(file_field.value[2], 16);
		}
		break;

	case FIELD_IMAGE_3:
		if (file_field.count >= 2) {
			strcpy(gd.entries[2].name, file_field.value[0]);
			val64.whole = STR_TO_ULL(file_field.value[1], 16);
			gd.entries[2].addr_high = val64.m_halfs.high;
			gd.entries[2].addr_low = val64.m_halfs.low;
			gd.num_entries++;
		}
		if (file_field.count >= 3) {
			gd.entries[2].dst_addr =
				STR_TO_UL(file_field.value[2], 16);
		}
		break;

	case FIELD_IMAGE_4:
		if (file_field.count >= 2) {
			strcpy(gd.entries[3].name, file_field.value[0]);
			val64.whole = STR_TO_ULL(file_field.value[1], 16);
			gd.entries[3].addr_high = val64.m_halfs.high;
			gd.entries[3].addr_low = val64.m_halfs.low;
			gd.num_entries++;
		}
		if (file_field.count >= 3) {
			gd.entries[3].dst_addr =
				STR_TO_UL(file_field.value[2], 16);
		}
		break;

	case FIELD_IMAGE_5:
		if (file_field.count >= 2) {
			strcpy(gd.entries[4].name, file_field.value[0]);
			val64.whole = STR_TO_ULL(file_field.value[1], 16);
			gd.entries[4].addr_high = val64.m_halfs.high;
			gd.entries[4].addr_low = val64.m_halfs.low;
			gd.num_entries++;
		}
		if (file_field.count >= 3) {
			gd.entries[4].dst_addr =
				STR_TO_UL(file_field.value[2], 16);
		}
		break;

	case FIELD_IMAGE_6:
		if (file_field.count >= 2) {
			strcpy(gd.entries[5].name, file_field.value[0]);
			val64.whole = STR_TO_ULL(file_field.value[1], 16);
			gd.entries[5].addr_high = val64.m_halfs.high;
			gd.entries[5].addr_low = val64.m_halfs.low;
			gd.num_entries++;
		}
		if (file_field.count >= 3) {
			gd.entries[5].dst_addr =
				STR_TO_UL(file_field.value[2], 16);
		}
		break;

	case FIELD_IMAGE_7:
		if (file_field.count >= 2) {
			strcpy(gd.entries[6].name, file_field.value[0]);
			val64.whole = STR_TO_ULL(file_field.value[1], 16);
			gd.entries[6].addr_high = val64.m_halfs.high;
			gd.entries[6].addr_low = val64.m_halfs.low;
			gd.num_entries++;
		}
		if (file_field.count >= 3) {
			gd.entries[6].dst_addr =
				STR_TO_UL(file_field.value[2], 16);
		}
		break;

	case FIELD_IMAGE_8:
		if (file_field.count >= 2) {
			strcpy(gd.entries[7].name, file_field.value[0]);
			val64.whole = STR_TO_ULL(file_field.value[1], 16);
			gd.entries[7].addr_high = val64.m_halfs.high;
			gd.entries[7].addr_low = val64.m_halfs.low;
			gd.num_entries++;
		}
		if (file_field.count >= 3) {
			gd.entries[7].dst_addr =
				STR_TO_UL(file_field.value[2], 16);
		}
		break;

	case FIELD_FSL_UID_0:
		if (file_field.count == 1) {
			gd.fsluid[0] = STR_TO_UL(file_field.value[0], 16);
			gd.fsluid_flag[0] = 1;
		}
		break;

	case FIELD_FSL_UID_1:
		if (file_field.count == 1) {
			gd.fsluid[1] = STR_TO_UL(file_field.value[0], 16);
			gd.fsluid_flag[1] = 1;
		}
		break;

	case FIELD_OEM_UID_0:
		if (file_field.count == 1) {
			gd.oemuid[0] = STR_TO_UL(file_field.value[0], 16);
			gd.oemuid_flag[0] = 1;
		}
		break;

	case FIELD_OEM_UID_1:
		if (file_field.count == 1) {
			gd.oemuid[1] = STR_TO_UL(file_field.value[0], 16);
			gd.oemuid_flag[1] = 1;
		}
		break;

	case FIELD_OEM_UID_2:
		if (file_field.count == 1) {
			gd.oemuid[2] = STR_TO_UL(file_field.value[0], 16);
			gd.oemuid_flag[2] = 1;
		}
		break;

	case FIELD_OEM_UID_3:
		if (file_field.count == 1) {
			gd.oemuid[3] = STR_TO_UL(file_field.value[0], 16);
			gd.oemuid_flag[3] = 1;
		}
		break;

	case FIELD_OEM_UID_4:
		if (file_field.count == 1) {
			gd.oemuid[4] = STR_TO_UL(file_field.value[0], 16);
			gd.oemuid_flag[4] = 1;
		}
		break;

	case FIELD_OUTPUT_HDR_FILENAME:
		if (file_field.count == 1)
			strcpy(gd.hdr_file_name, file_field.value[0]);
		else
			strcpy(gd.hdr_file_name, DEFAULT_HDR_FILE_NAME);
		break;

	case FIELD_IMAGE_HASH_FILENAME:
		if (file_field.count == 1)
			strcpy(gd.img_hash_file_name, file_field.value[0]);
		else
			strcpy(gd.img_hash_file_name, DEFAULT_HASH_FILE_NAME);
		break;

	case FIELD_RSA_SIGN_FILENAME:
		if (file_field.count == 1)
			strcpy(gd.rsa_sign_file_name, file_field.value[0]);
		else
			strcpy(gd.rsa_sign_file_name, DEFAULT_SIGN_FILE_NAME);
		break;

	case FIELD_RCW_PBI_FILENAME:
		if (file_field.count == 1)
			strcpy(gd.rcw_fname, file_field.value[0]);

		break;

		break;

	case FIELD_BOOT1_PTR:
		if (file_field.count == 1)
			gd.boot1_ptr = STR_TO_UL(file_field.value[0], 16);

		break;

	case FIELD_MP_FLAG:
		if (file_field.count == 1)
			gd.mp_flag = STR_TO_UL(file_field.value[0], 16);

		break;

	case FIELD_ISS_FLAG:
		if (file_field.count == 1)
			gd.iss_flag = STR_TO_UL(file_field.value[0], 16);

		break;

	case FIELD_LW_FLAG:
		if (file_field.count == 1)
			gd.lw_flag = STR_TO_UL(file_field.value[0], 16);

		break;

	case FIELD_VERBOSE:
		if (file_field.count == 1)
			gd.verbose_flag = STR_TO_UL(file_field.value[0], 16);

		break;

	default:
		printf("\n Invalid Field being parsed");
		return FAILURE;
	}

	return SUCCESS;
}

int get_file_size(const char *c)
{
	FILE *fp;
	unsigned char buf[IOBLOCK];
	int bytes = 0;

	fp = fopen(c, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Error in opening the file: %s\n", c);
		return FAILURE;
	}

	while (!feof(fp)) {
		/* read some data */
		bytes += fread(buf, 1, IOBLOCK, fp);
		if (ferror(fp)) {
			fprintf(stderr, "Error in reading file\n");
			fclose(fp);
			exit(EXIT_FAILURE);
		} else if (feof(fp) && (bytes == 0)) {
			break;
		}
	}

	fclose(fp);
	return bytes;
}