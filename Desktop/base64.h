

void build_decoding_table();

char *base64_encode(const unsigned char *data,
                    size_t input_length,
                    size_t *output_length);