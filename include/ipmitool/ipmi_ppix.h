int ipmi_ppix_discovery(struct ipmi_intf * intf);       // Full Discovery
int ipmi_decode_discovery(uint8_t *in, int size_input);

int ipmi_ppix_enable(struct ipmi_intf * intf, char *password); // Enables TPM/TXT
int ipmi_decode_enable(uint8_t *in, int size_input);

int ipmi_ppix_TPM_clear(struct ipmi_intf * intf, char *password);  // Clear TPM
int ipmi_decode_TPM_clear(uint8_t *in, int size_input);

int ipmi_ppix_TXT_discovery(struct ipmi_intf * intf); //  TXT Discovery
int ipmi_decode_TXT(uint8_t *in, int size_input);

int ipmi_ppix_TPM_discovery(struct ipmi_intf * intf); // TPM Discovery
int ipmi_decode_TPM(uint8_t *in, int size_input);

int ipmi_ppix_full_refresh(struct ipmi_intf * intf, char *password);
int ipmi_decode_full_refresh(uint8_t *in, int size_input);

int ipmi_ppix_TPM_refresh(struct ipmi_intf * intf, char *password);
int ipmi_decode_TPM_refresh(uint8_t *in, int size_input);

uint8_t * ipmi_ppix_raw(struct ipmi_intf * intf, int argc, char ** argv); // Raw Function
int is_valid_param(const char *input_param, uint8_t *uchr_ptr, const char *label);

int ipmi_ppix_main(struct ipmi_intf * intf, int argc, char ** argv); //  Main
