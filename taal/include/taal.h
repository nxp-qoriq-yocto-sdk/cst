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

#ifndef _TAAL_H_
#define _TAAL_H_

enum cfg_taal {
	TA_3_0 = 0,		/* LS2085 */
	TA_3_1,			/* LS2088, LS1088 */
	TA_UNKNOWN_MAX
};

typedef struct {
	char *plat_name;
	enum cfg_taal ta_type;
} ta_struct_t;

typedef int (*ta_parse)(void);
typedef int (*ta_fill)(void);
typedef int (*ta_create)(void);
typedef int (*ta_img_hash)(void);
typedef int (*ta_srk_hash)(void);
typedef int (*ta_dump)(void);

struct taal_t {
	ta_parse	parse_input_file;
	ta_fill		fill_structure;
	ta_create	create_header;
	ta_img_hash	calc_img_hash;
	ta_srk_hash	calc_srk_hash;
	ta_dump		dump_hdr;
};

/* Initialize taal data structure */
void taal_init(struct taal_t *);

/* Wrappers over target API */
int taal_parse_input_file(struct taal_t *, enum cfg_taal);
int taal_fill_structures(struct taal_t *, enum cfg_taal);
int taal_create_hdr(struct taal_t *, enum cfg_taal);
int taal_calc_img_hash(struct taal_t *, enum cfg_taal);
int taal_calc_srk_hash(struct taal_t *, enum cfg_taal);
int taal_dump_header(struct taal_t *, enum cfg_taal);

/* Get TA from File */
enum cfg_taal get_ta_from_file(char *file_name);

#endif
