/** @file
 * Copyright (c) 2019, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#include "val_crypto.h"

#define EMPTY_KEY_SLOT      31

typedef struct {
    char                    test_desc[75];
    psa_key_slot_t          key_slot[2];
    psa_key_type_t          key_type;
    uint8_t                 key_data[32];
    uint32_t                key_length;
    psa_key_usage_t         usage;
    psa_algorithm_t         key_alg;
    uint8_t                 nonce[16];
    size_t                  nonce_length;
    uint8_t                 additional_data[32];
    size_t                  additional_data_length;
    uint8_t                 expected_plaintext[30];
    size_t                  plaintext_size;
    uint8_t                 ciphertext[45];
    size_t                  ciphertext_size;
    size_t                  expected_plaintext_length;
    psa_status_t            expected_status;
} test_data;

static test_data check1[] = {
{"Test psa_aead_decrypt - CCM - 16B AES - 13B nounce & 8B addi data\n", {1, 1}, PSA_KEY_TYPE_AES,
{0xD7, 0x82, 0x8D, 0x13, 0xB2, 0xB0, 0xBD, 0xC3, 0x25, 0xA7, 0x62, 0x36, 0xDF,
 0x93, 0xCC, 0x6B},
 AES_16B_KEY_SIZE, PSA_KEY_USAGE_DECRYPT, PSA_ALG_CCM,
{0x00, 0x41, 0x2B, 0x4E, 0xA9, 0xCD, 0xBE, 0x3C, 0x96, 0x96, 0x76, 0x6C, 0xFA},
 13,
{0x0B, 0xE1, 0xA8, 0x8B, 0xAC, 0xE0, 0x18, 0xB1}, 8,
{0x08, 0xE8, 0xCF, 0x97, 0xD8, 0x20, 0xEA, 0x25, 0x84, 0x60, 0xE9, 0x6A, 0xD9,
 0xCF, 0x52, 0x89, 0x05, 0x4D, 0x89, 0x5C, 0xEA, 0xC4, 0x7C}, BUFFER_SIZE,
{0x4C, 0xB9, 0x7F, 0x86, 0xA2, 0xA4, 0x68, 0x9A, 0x87, 0x79, 0x47, 0xAB, 0x80,
 0x91, 0xEF, 0x53, 0x86, 0xA6, 0xFF, 0xBD, 0xD0, 0x80, 0xF8, 0x12, 0x03, 0x33,
 0xD1, 0xFC, 0xB6, 0x91, 0xF3, 0x40, 0x6C, 0xBF, 0x53, 0x1F, 0x83, 0xA4, 0xD8},
 39, 23, PSA_SUCCESS
},

{"Test psa_aead_decrypt - AES-CCM\n", {2, 2}, PSA_KEY_TYPE_AES,
{0x41, 0x89, 0x35, 0x1B, 0x5C, 0xAE, 0xA3, 0x75, 0xA0, 0x29, 0x9E, 0x81, 0xC6,
 0x21, 0xBF, 0x43}, AES_16B_KEY_SIZE, PSA_KEY_USAGE_DECRYPT, PSA_ALG_CCM,
{0x48, 0xc0, 0x90, 0x69, 0x30, 0x56, 0x1e, 0x0a, 0xb0, 0xef, 0x4c, 0xd9, 0x72},
 13,
{0x40, 0xa2, 0x7c, 0x1d, 0x1e, 0x23, 0xea, 0x3d, 0xbe, 0x80, 0x56, 0xb2, 0x77,
 0x48, 0x61, 0xa4, 0xa2, 0x01, 0xcc, 0xe4, 0x9f, 0x19, 0x99, 0x7d, 0x19, 0x20,
 0x6d, 0x8c, 0x8a, 0x34, 0x39, 0x51}, 32,
{0x45, 0x35, 0xd1, 0x2b, 0x43, 0x77, 0x92, 0x8a, 0x7c, 0x0a, 0x61, 0xc9, 0xf8,
 0x25, 0xa4, 0x86, 0x71, 0xea, 0x05, 0x91, 0x07, 0x48, 0xc8, 0xef}, BUFFER_SIZE,
{0x26, 0xc5, 0x69, 0x61, 0xc0, 0x35, 0xa7, 0xe4, 0x52, 0xcc, 0xe6, 0x1b, 0xc6,
 0xee, 0x22, 0x0d, 0x77, 0xb3, 0xf9, 0x4d, 0x18, 0xfd, 0x10, 0xb6, 0xd8, 0x0e,
 0x8b, 0xf8, 0x0f, 0x4a, 0x46, 0xca, 0xb0, 0x6d, 0x43, 0x13, 0xf0, 0xdb, 0x9b,
 0xe9}, 40, 24, PSA_SUCCESS
},

{"Test psa_aead_decrypt - GCM - 16B AES - 12B Nounce & 12B addi data\n",
{4, 4}, PSA_KEY_TYPE_AES,
{0x3d, 0xe0, 0x98, 0x74, 0xb3, 0x88, 0xe6, 0x49, 0x19, 0x88, 0xd0, 0xc3, 0x60,
 0x7e, 0xae, 0x1f}, 16, PSA_KEY_USAGE_DECRYPT, PSA_ALG_GCM,
{0x57, 0x69, 0x0e, 0x43, 0x4e, 0x28, 0x00, 0x00, 0xa2, 0xfc, 0xa1, 0xa3}, 12,
{0x42, 0xf6, 0x7e, 0x3f, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10}, 12,
{0x45, 0x00, 0x00, 0x1c, 0x42, 0xa2, 0x00, 0x00, 0x80, 0x01, 0x44, 0x1f, 0x40,
 0x67, 0x93, 0xb6, 0xe0, 0x00, 0x00, 0x02, 0x0a, 0x00, 0xf5, 0xff, 0x01, 0x02,
 0x02, 0x01}, BUFFER_SIZE,
{0xFB, 0xA2, 0xCA, 0x84, 0x5E, 0x5D, 0xF9, 0xF0, 0xF2, 0x2C, 0x3E, 0x6E, 0x86,
 0xDD, 0x83, 0x1E, 0x1F, 0xC6, 0x57, 0x92, 0xCD, 0x1A, 0xF9, 0x13, 0x0E, 0x13,
 0x79, 0xED, 0x36, 0x9F, 0x07, 0x1F, 0x35, 0xE0, 0x34, 0xBE, 0x95, 0xF1, 0x12,
 0xE4, 0xE7, 0xD0, 0x5D, 0x35},
 44, 28, PSA_SUCCESS
},

{"Test psa_aead_decrypt - DES Key\n", {5, 5}, PSA_KEY_TYPE_DES,
{0x70, 0x24, 0x55, 0x0C, 0x14, 0x9D, 0xED, 0x29}, DES_8B_KEY_SIZE,
 PSA_KEY_USAGE_DECRYPT, PSA_ALG_CCM,
{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B}, 12,
{0xEC, 0x46, 0xBB, 0x63, 0xB0, 0x25, 0x20, 0xC3, 0x3C, 0x49, 0xFD, 0x70}, 12,
{0xB9, 0x6B, 0x49, 0xE2, 0x1D, 0x62, 0x17, 0x41, 0x63, 0x28, 0x75, 0xDB, 0x7F,
 0x6C, 0x92, 0x43, 0xD2, 0xD7, 0xC2}, BUFFER_SIZE,
{0}, 16, 0, PSA_ERROR_NOT_SUPPORTED
},

{"Test psa_aead_decrypt - Empty key slot\n", {6, EMPTY_KEY_SLOT}, PSA_KEY_TYPE_AES,
{0x49, 0x8E, 0xC7, 0x7D, 0x01, 0x95, 0x0D, 0x94, 0x2C, 0x16, 0xA5, 0x3E, 0x99,
 0x5F, 0xC9}, AES_16B_KEY_SIZE, PSA_KEY_USAGE_DECRYPT, PSA_ALG_GCM,
{0x48, 0xc0, 0x90, 0x69, 0x30, 0x56, 0x1e, 0x0a, 0xb0, 0xef, 0x4c, 0xd9, 0x72}, 13,
{0}, 0, "hello world", BUFFER_SIZE,
{0x5D, 0xC1, 0x72, 0x23, 0x66, 0x96, 0xFD, 0xFC, 0x93, 0x06, 0x27, 0x52, 0xC7,
 0x0A, 0xCB, 0x36, 0x55, 0x30, 0xC9, 0x48, 0x8F, 0x5E, 0xA5, 0xB9, 0x51, 0xFB,
 0x4E},
 16, 27, PSA_ERROR_EMPTY_SLOT
},

{"Test psa_aead_decrypt - Zero key slot\n", {7, ZERO_KEY_SLOT}, PSA_KEY_TYPE_AES,
{0x49, 0x8E, 0xC7, 0x7D, 0x01, 0x95, 0x0D, 0x94, 0x2C, 0x16, 0xA5, 0x3E, 0x99,
 0x5F, 0xC9}, AES_16B_KEY_SIZE, PSA_KEY_USAGE_DECRYPT, PSA_ALG_GCM,
{0}, 13, {0}, 0, "hello world", BUFFER_SIZE,
{0x5D, 0xC1, 0x72, 0x23, 0x66, 0x96, 0xFD, 0xFC, 0x93, 0x06, 0x27, 0x52, 0xC7,
 0x0A, 0xCB, 0x36, 0x55, 0x30, 0xC9, 0x48, 0x8F, 0x5E, 0xA5, 0xB9, 0x51, 0xFB,
 0x4E},
 27, 11, PSA_ERROR_INVALID_ARGUMENT
},

{"Test psa_aead_decrypt - Invalid key slot\n", {8, INVALID_KEY_SLOT}, PSA_KEY_TYPE_AES,
{0x49, 0x8E, 0xC7, 0x7D, 0x01, 0x95, 0x0D, 0x94, 0x2C, 0x16, 0xA5, 0x3E, 0x99,
 0x5F, 0xC9}, AES_16B_KEY_SIZE, PSA_KEY_USAGE_DECRYPT, PSA_ALG_GCM,
{0}, 13, {0}, 0, "hello world", BUFFER_SIZE,
{0x5D, 0xC1, 0x72, 0x23, 0x66, 0x96, 0xFD, 0xFC, 0x93, 0x06, 0x27, 0x52, 0xC7,
 0x0A, 0xCB, 0x36, 0x55, 0x30, 0xC9, 0x48, 0x8F, 0x5E, 0xA5, 0xB9, 0x51, 0xFB,
 0x4E},
 27, 11, PSA_ERROR_INVALID_ARGUMENT
},

{"Test psa_aead_decrypt - Unsupported Algorithm\n", {9, 9}, PSA_KEY_TYPE_AES,
{0x49, 0x8E, 0xC7, 0x7D, 0x01, 0x95, 0x0D, 0x94, 0x2C, 0x16, 0xA5, 0x3E, 0x99,
 0x5F, 0xC9}, AES_16B_KEY_SIZE, PSA_KEY_USAGE_DECRYPT, PSA_ALG_CFB,
{0}, 13, {0}, 0, "hello world", BUFFER_SIZE,
{0x5D, 0xC1, 0x72, 0x23, 0x66, 0x96, 0xFD, 0xFC, 0x93, 0x06, 0x27, 0x52, 0xC7,
 0x0A, 0xCB, 0x36, 0x55, 0x30, 0xC9, 0x48, 0x8F, 0x5E, 0xA5, 0xB9, 0x51, 0xFB,
 0x4E},
 27, 11, PSA_ERROR_NOT_SUPPORTED
},

{"Test psa_aead_decrypt - Invalid key usage\n", {10, 10}, PSA_KEY_TYPE_AES,
{0x49, 0x8E, 0xC7, 0x7D, 0x01, 0x95, 0x0D, 0x94, 0x2C, 0x16, 0xA5, 0x3E, 0x99,
 0x5F, 0xC9}, AES_16B_KEY_SIZE, PSA_KEY_USAGE_ENCRYPT, PSA_ALG_GCM,
{0x48, 0xc0, 0x90, 0x69, 0x30, 0x56, 0x1e, 0x0a, 0xb0, 0xef, 0x4c, 0xd9, 0x72}, 13,
{0}, 0, "hello world", BUFFER_SIZE,
{0x5D, 0xC1, 0x72, 0x23, 0x66, 0x96, 0xFD, 0xFC, 0x93, 0x06, 0x27, 0x52, 0xC7,
 0x0A, 0xCB, 0x36, 0x55, 0x30, 0xC9, 0x48, 0x8F, 0x5E, 0xA5, 0xB9, 0x51, 0xFB,
 0x4E},
 27, 11, PSA_ERROR_NOT_PERMITTED
},

{"Test psa_aead_decrypt - Small output buffer size\n", {11, 11}, PSA_KEY_TYPE_AES,
{0x49, 0x8E, 0xC7, 0x7D, 0x01, 0x95, 0x0D, 0x94, 0x2C, 0x16, 0xA5, 0x3E, 0x99,
 0x5F, 0xC9}, AES_16B_KEY_SIZE, PSA_KEY_USAGE_DECRYPT, PSA_ALG_GCM,
{0}, 13, {0}, 0, "hello world", 10,
{0x5D, 0xC1, 0x72, 0x23, 0x66, 0x96, 0xFD, 0xFC, 0x93, 0x06, 0x27, 0x52, 0xC7,
 0x0A, 0xCB, 0x36, 0x55, 0x30, 0xC9, 0x48, 0x8F, 0x5E, 0xA5, 0xB9, 0x51, 0xFB,
 0x4E},
 27, 11, PSA_ERROR_BUFFER_TOO_SMALL
},

{"Test psa_aead_decrypt - Invalid cipher text\n", {12, 12}, PSA_KEY_TYPE_AES,
{0xD7, 0x82, 0x8D, 0x13, 0xB2, 0xB0, 0xBD, 0xC3, 0x25, 0xA7, 0x62, 0x36, 0xDF,
 0x93, 0xCC, 0x6B},
 AES_16B_KEY_SIZE, PSA_KEY_USAGE_DECRYPT, PSA_ALG_CCM,
{0x00, 0x41, 0x2B, 0x4E, 0xA9, 0xCD, 0xBE, 0x3C, 0x96, 0x96, 0x76, 0x6C, 0xFA},
 13,
{0x0B, 0xE1, 0xA8, 0x8B, 0xAC, 0xE0, 0x18, 0xB1}, 8,
{0x08, 0xE8, 0xCF, 0x97, 0xD8, 0x20, 0xEA, 0x25, 0x84, 0x60, 0xE9, 0x6A, 0xD9,
 0xCF, 0x52, 0x89, 0x05, 0x4D, 0x89, 0x5C, 0xEA, 0xC4, 0x7C}, BUFFER_SIZE,
{0x4C, 0xB8, 0x7F, 0x86, 0xA2, 0xA4, 0x68, 0x9A, 0x87, 0x79, 0x47, 0xAB, 0x80,
 0x91, 0xEF, 0x53, 0x86, 0xA6, 0xFF, 0xBD, 0xD0, 0x80, 0xF8, 0x12, 0x03, 0x33,
 0xD1, 0xFC, 0xB6, 0x91, 0xF3, 0x40, 0x6C, 0xBF, 0x53, 0x1F, 0x83, 0xA4, 0xD8},
 39, 23, PSA_ERROR_INVALID_SIGNATURE
},

{"Test psa_aead_decrypt - Invalid cipher text size\n", {13, 13}, PSA_KEY_TYPE_AES,
{0xD7, 0x82, 0x8D, 0x13, 0xB2, 0xB0, 0xBD, 0xC3, 0x25, 0xA7, 0x62, 0x36, 0xDF,
 0x93, 0xCC, 0x6B},
 AES_16B_KEY_SIZE, PSA_KEY_USAGE_DECRYPT, PSA_ALG_CCM,
{0x00, 0x41, 0x2B, 0x4E, 0xA9, 0xCD, 0xBE, 0x3C, 0x96, 0x96, 0x76, 0x6C, 0xFA},
 13,
{0x0B, 0xE1, 0xA8, 0x8B, 0xAC, 0xE0, 0x18, 0xB1}, 8,
{0x08, 0xE8, 0xCF, 0x97, 0xD8, 0x20, 0xEA, 0x25, 0x84, 0x60, 0xE9, 0x6A, 0xD9,
 0xCF, 0x52, 0x89, 0x05, 0x4D, 0x89, 0x5C, 0xEA, 0xC4, 0x7C}, BUFFER_SIZE,
{0x4C, 0xB9, 0x7F, 0x86, 0xA2, 0xA4, 0x68, 0x9A, 0x87, 0x79, 0x47, 0xAB, 0x80,
 0x91, 0xEF, 0x53, 0x86, 0xA6, 0xFF, 0xBD, 0xD0, 0x80, 0xF8, 0x12, 0x03, 0x33,
 0xD1, 0xFC, 0xB6, 0x91, 0xF3, 0x40, 0x6C, 0xBF, 0x53, 0x1F, 0x83, 0xA4},
 38, 23, PSA_ERROR_INVALID_SIGNATURE
},

};