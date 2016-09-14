

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <ipmitool/bswap.h>
#include <ipmitool/helper.h>
#include <ipmitool/ipmi.h>
#include <ipmitool/log.h>
#include <ipmitool/ipmi_intf.h>
#include <ipmitool/ipmi_strings.h>
#include <ipmitool/ipmi_chassis.h>




int is_valid_param(const char *input_param, uint8_t *uchr_ptr, const char *label) 
{
	if (input_param == NULL || label == NULL) {
		lprintf(LOG_ERROR, "ERROR: NULL pointer passed.");
		return (-1);
	}
	if (str2uchar(input_param, uchr_ptr) == 0)
		return 0;

	lprintf(LOG_ERR, "Given %s \"%s\" is invalid.", label, input_param);
	return (-1);
}

uint8_t * ipmi_ppix_raw(struct ipmi_intf * intf, int argc, char ** argv)
{
	int i;
    printf("Inputted Raw Request: \n");

    char bit[4];
    char *index;
    for(i = 0; i < argc; i++){
        if(i%16 == 0)
            printf("\n");
        printf(" %s", *(argv + i));
    }

        
    printf("\n\n");

	struct ipmi_rs * rsp;
	struct ipmi_rq req;
	uint8_t netfn, cmd, lun;
	uint16_t netfn_tmp = 0;
	//int i;
	uint8_t data[256];


	if (argc == 1 && strncmp(argv[0], "help", 4) == 0) {
		ipmi_raw_help();
		return 0;
	}
	else if (argc < 2) {
		lprintf(LOG_ERR, "Not enough parameters given.");
		ipmi_raw_help();
		return (-1);
	}
	else if (argc > sizeof(data))
	{
		lprintf(LOG_NOTICE, "Raw command input limit (256 bytes) exceeded");
		return -1;
	}


	lun = intf->target_lun;
	netfn_tmp = str2val(argv[0], ipmi_netfn_vals);
	if (netfn_tmp == 0xff) {
		if (is_valid_param(argv[0], &netfn, "netfn") != 0)
			return (-1);
	} else {
		if (netfn_tmp >= UINT8_MAX) {
			lprintf(LOG_ERR, "Given netfn \"%s\" is out of range.", argv[0]);
			return (-1);
		}
		netfn = netfn_tmp;
	}

	if (is_valid_param(argv[1], &cmd, "command") != 0)
		return (-1);

	memset(data, 0, sizeof(data));
	memset(&req, 0, sizeof(req));
	req.msg.netfn = netfn;
	req.msg.lun = lun;
	req.msg.cmd = cmd;
	req.msg.data = data;

	for (i=2; i<argc; i++) {
		uint8_t val = 0;

		if (is_valid_param(argv[i], &val, "data") != 0)
			return (-1);

		req.msg.data[i-2] = val;
		req.msg.data_len++;
	}

	lprintf(LOG_INFO, 
           "RAW REQ (channel=0x%x netfn=0x%x lun=0x%x cmd=0x%x data_len=%d)",
           intf->target_channel & 0x0f, req.msg.netfn,req.msg.lun , 
           req.msg.cmd, req.msg.data_len);

	printbuf(req.msg.data, req.msg.data_len, "RAW REQUEST");

	rsp = intf->sendrecv(intf, &req);

	if (rsp == NULL) {
		lprintf(LOG_ERR, "Unable to send RAW command "
			"(channel=0x%x netfn=0x%x lun=0x%x cmd=0x%x)",
			intf->target_channel & 0x0f, req.msg.netfn, req.msg.lun, req.msg.cmd);
		return -1;
	}
	if (rsp->ccode > 0) {
		lprintf(LOG_ERR, "Unable to send RAW command "
			"(channel=0x%x netfn=0x%x lun=0x%x cmd=0x%x rsp=0x%x): %s",
			intf->target_channel & 0x0f, req.msg.netfn, req.msg.lun, req.msg.cmd, rsp->ccode,
			val2str(rsp->ccode, completion_code_vals));
		return -1;
	}

	lprintf(LOG_INFO, "RAW RSP (%d bytes)", rsp->data_len);


    return (rsp->data);

}

int ipmi_ppix_enable(struct ipmi_intf * intf, char *password)
{
    int rc;
    int length = 0;
    uint8_t pass[49];

    while(*(password + length)){
        pass[length] = (uint8_t) *(password + length); // convert char to UINT8
        length++;
    }
    char * byte_packet[100] = { // Corresponding to TPM/TXT Enable Command
        "0x2e",
        "0x91",
        "0x57",
        "0x01",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x01",
        "0x27",
        "0x00",
        "0x00",
        "0x00",
        "0x24",
        "0x4f",
        "0x58",
        "0x50",
        "0x27",
        "0x00",
        "0x20",
        "0x00",
        "0x01",
        "0x29",
        "0x03",
        "0xff",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x40",
        "0x00",
        "0x00",
        "0x40",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x01",
        "0x07",
        "0x00",
        "0x01",
        "0x80",  // Here
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",      
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",        
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00"  
    };

    int i;
    char format[5];
    char *tepm;
    for(i = 0; i < length; i++){
        sprintf(format, "0x%02x", pass[i]); // UINT to char format STRING
        byte_packet[51 + i] = malloc(strlen(format) + 1);
        strcpy(byte_packet[51 + i], format);
    }

    int size;
    size = 51 + length + 1;
    uint8_t *rsp;
    printf("\n");
    rsp = ipmi_ppix_raw(intf, size, byte_packet);
    rc = ipmi_decode_enable(rsp, 35);

    printf("\nOutputted Response: \n");

    for(i = 0; i < 35; i++){
        if(i%16 == 0)
            printf("\n");
        printf(" %02x", *(rsp + i));
    }
    printf("\n\n");

    //rc = 0;
    return rc;
}

int ipmi_ppix_discovery(struct ipmi_intf * intf)
{
	int rc;

    char * byte_packet[] = { // Corresponding to TPM/TXT Discovery Command
        
        "0x2e",
        "0x90",
        "0x57",
        "0x01",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x20"
        };

    int size;
    size = 10;
    //size = sizeof(byte_packet)/sizeof(byte_packet[0]);

    uint8_t *rsp;
    printf("\n");

    rsp = ipmi_ppix_raw(intf, size, byte_packet); // Prints Input Request

    printf("Outputted Response: \n"); // Prints Output Request
    int i;
    for(i = 0; i < 35; i++){
        if(i%16 == 0)
            printf("\n");
        printf(" %02x", *(rsp + i));
    }
    printf("\n\n");

    rc = ipmi_decode_discovery(rsp, 35); // Verbose



    return rc;
    
}

int ipmi_decode_discovery(uint8_t *in, int size_input)
{
    
    //Prints out the Input



    if (*in == 0x57 && *(in + 1) == 0x01 && *(in + 2) == 0x00)
    {
        printf("Intel Manufacturer ID: Confirmed\n\t%02x %02x %02x \n", *(in), *(in + 1), *(in + 2));
    }
    else
    {
        printf("Manufacturer ID: Unknown\n\t%02x %02x %02x \n", *(in), *(in + 1), *(in + 2));
    }

    printf("Signature:\n\t%02x %02x %02x %02x\n", *(in+3), *(in + 4), *(in + 5), *(in + 6));
    printf("Total Length(Decimal):\n\t%02d\n", *(in + 7));
    printf("Header Length(Decimal): \n\t%02d\n", *(in + 9));
    printf("Version: \n\t%02d\n", *(in + 11));
    printf("Check Sum(Decimal): \n\t%02d\n", *(in + 12));
    printf("Task and Result: \n\t%02x\n", *(in + 13));
    uint8_t task_result = *(in + 13);
    uint8_t mask = 0x80;
    uint8_t index;
    uint8_t status;

    index = task_result & mask;
    index = index >> 7;

    if (index == 0)
    {
        printf("\tTask not successfully performed\n");
    }
    else if (index == 1)
    {
        printf("\tTask successfully performed\n");
        status = *(in + 14);
        printf("Status: %02d\n", *(in + 14));
        switch(status){
            case 0x00:
                printf("\tSuccess\n");
                break;
            case 0x01:
                printf("\tInvalid Task\n");
                break;
            case 0x10:
                printf("\tTXT not supported\n");
                break;
            case 0x11:
                printf("\tTXT enable error\n");
                break;
            case 0x20:
                printf("\tTPM not supported\n");
                break;
            case 0x21:
                printf("\tTPM enable error\n");
                break;
            case 0x22:
                printf("\tTPM clear error\n");
                break;
            case 0x30:
                printf("\tAuthentication failure\n");
                break;
            case 0x31:
                printf("\tSpecified password string type not supported\n");
                break;
            case 0x32:
                printf("\tSpecified SHA type not supported\n");
                break;
            default:
                printf("\tUnknown error\n");
        }


    }
    else 
    {
        printf("Error: Task and Result invalid input\n");
    }


    //printf("Password Attribute: %02x %02x\n", *(in + 15), *(in + 16));

    index = *(in + 17); // TXT Support
    mask = 0x01;
    index = index & mask;

    //printf(" %02x\n", index);
    printf("TPM/TXT Support Status: %02x \n", *(in + 17));
    switch(index){

    case 1:
        printf("\tTXT is supported\n");
        break;
    case 0:
        printf("\tTXT is not supported\n");
        break;
    default:
        printf("\tInvalid Output.  TXT Support Discovery Failed\n");
    }
    index = *(in + 17); //  TPM Support
    mask = 0x06;
    index = index & mask;
    index = index >> 1;
    //printf("TPM/: %02x\n", index);
    switch(index){
        case 0:
            printf("\tTPM is not supported\n");
            break;
        case 1:
            printf("\tdTPM is supported\n");
            break;
        case 2:
            printf("\tfTPM is supported\n");
            break;
        case 3:
            printf("\tdTPM and fTPM are supported\n");
            break;
        default:
            printf("\tInvalid Output.  TPM Discovery Failed\n");
    }



    index = *(in + 19); // TXT Enabled
    mask = 0x01;
    index = mask & index;
    printf("TPM/TXT Enable Status: %02x\n", *(in + 19));
    switch(index){
        case 0:
            printf("\tTXT is not enabled:\n");
            break;
        case 1:
            printf("\tTXT is enabled:\n");
            break;
        default:
            printf("\tInalid Output. TXT Enabled Discovery Failed");

    }
    index = *(in + 19); //  TPM Enabled
    mask = 0x06;
    index = mask & index;
    index = index >> 1;
    switch(index){
        case 0:
            printf("\tTPM not enabled:\n");
            break;
        case 1:
            printf("\tdTPM enabled:\n");
            break;
        case 2:
            printf("\tfTPM enabled:\n");
            break;
        default:
            printf("\tInvalid Output.  TPM Discovery Failed\n");
    }
    printf("\n");




    return 0;
}
    
int ipmi_decode_enable(uint8_t *in, int size_input)
{

    uint8_t task_result = *(in + 13);
    uint8_t mask = 0x80;
    uint8_t index;
    uint8_t status;

    index = task_result & mask;
    index = index >> 7;

    if (index == 0)
    {
        printf("\tTask not successfully performed\n");
    }
    else if (index == 1)
    {
        printf("\tTask successfully performed\n");
        status = *(in + 14);
        printf("Status: %02d\n", *(in + 14));
        switch(status){
            case 0x00:
                printf("\tStatus: Success\n");
                break;
            case 0x01:
                printf("\tStatus: Invalid Task\n");
                break;
            default:
                printf("\tStatus: Unknown error\n");
        }


    }
    else 
    {
        printf("Error: Task and Result invalid input\n");
    }

    return 0;

}

int ipmi_ppix_TPM_clear(struct ipmi_intf * intf, char *password)
{
    int rc;



    int length = 0; //  number of char of pw
    uint8_t pass[49]; // store password char to ascii -> hex

    while(*(password + length)){
        pass[length] = (uint8_t) *(password + length); // convert char to UINT8
        length++;
    }
    char * byte_packet[100] = { // Corresponding to TPM/TXT Discovery Command
        
        "0x2e",
        "0x91",
        "0x57",
        "0x01",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x01",
        "0x27",
        "0x00",
        "0x00",
        "0x00",
        "0x24",
        "0x4f",
        "0x58",
        "0x50",
        "0x27",
        "0x00",
        "0x20",
        "0x00",
        "0x01",
        "0x28",
        "0x04",
        "0xff",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x40",
        "0x00",
        "0x00",
        "0x40",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x01",
        "0x07",
        "0x00",
        "0x01",
        "0x80", // Here
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",      
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",        
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00"  


        //"0x69", // ASCII i = 0x69
        //"0x00"  // Padded 0x00
                    };

    int i;
    char format[5];
    char *temp;
    for(i = 0; i < length; i++){ 

        sprintf(format, "0x%02x", pass[i]); // UINT to char format STRING
        byte_packet[51 + i] = malloc(strlen(format) + 1);
        strcpy(byte_packet[51 + i], format);
    }


    int size;
    size = 51 + length + 1;



    uint8_t *rsp;
    printf("\n");

    rsp = ipmi_ppix_raw(intf, size, byte_packet);
    //free(byte_packet);
    rc = ipmi_decode_TPM_clear(rsp, 35);

    printf("Outputted Response: \n");

    for(i = 0; i < 35; i++){
        if(i%16 == 0)
            printf("\n");
        printf(" %02x", *(rsp + i));
    }
    printf("\n\n");

    return rc;
}

int ipmi_decode_TPM_clear(uint8_t *in, int size_input)
{

    printf("Task and Result: %02x\n", *(in + 13));
    uint8_t task_result = *(in + 13);
    uint8_t mask = 0x80;
    uint8_t index;
    uint8_t status;

    index = task_result & mask;
    index = index >> 7;
    // need to return -1 when password incompatible.  Will return later!/////////////////////////
    if (index == 0)
    {
        printf("\tTask not successfully performed\n");
    }
    else if (index == 1)
    {
        printf("\tTask successfully performed\n");
        status = *(in + 14);
        printf("Status: %02x\n", *(in + 14));
        switch(status){
            case 0x00:
                printf("\tStatus: Success\n");
                break;
            case 0x01:
                printf("\tStatus: Invalid Task\n");
                break;
            default:
                printf("\tStatus: Unknown error\n");
        }


    }
    else 
    {
        printf("Error: Task and Result invalid input\n");
    }


    printf("\n");


    return 0;
}

int ipmi_decode_TXT(uint8_t *in, int size_input)
{


    printf("Task and Result: %02x\n", *(in + 13));
    uint8_t task_result = *(in + 13);
    uint8_t mask = 0x80;
    uint8_t index;
    uint8_t status;

    index = task_result & mask;
    index = index >> 7;

    if (index == 0)
    {
        printf("\tTask not successfully performed\n");
    }
    else if (index == 1)
    {
        printf("\tTask successfully performed\n");
        status = *(in + 14);
        printf("Status: %02d\n", *(in + 14));
        switch(status){
            case 0x00:
                printf("\tStatus: Success\n");
                break;
            case 0x01:
                printf("\tStatus: Invalid Task\n");
                break;
            case 0x10:
                printf("\tStatus: TXT not supported\n");
                break;
            case 0x11:
                printf("\tStatus: TXT enable error\n");
                break;
            case 0x20:
                printf("\tStatus: TPM not supported\n");
                break;
            case 0x21:
                printf("\tStatus: TPM enable error\n");
                break;
            case 0x22:
                printf("\tStatus: TPM clear error\n");
                break;
            case 0x30:
                printf("\tStatus: Authentication failure\n");
                break;
            case 0x31:
                printf("\tStatus: Specified password string type not supported\n");
                break;
            case 0x32:
                printf("\tStatus: Specified SHA type not supported\n");
                break;
            default:
                printf("\tStatus: Unknown error\n");
        }


    }
    else 
    {
        printf("Error: Task and Result invalid input\n");
    }


    index = *(in + 17); // TXT Support
    mask = 0x01;
    index = index & mask;

    //printf(" %02x\n", index);
    printf("TXT Status:\n");
    switch(index){

    case 1:
        printf("\tTXT is supported\n");
        break;
    case 0:
        printf("\tTXT is not supported\n");
        break;
    default:
        printf("\tInvalid Output.  TXT Support Discovery Failed\n");
    }
    
    index = *(in + 19); // TXT Enabled
    mask = 0x01;
    index = mask & index;
    switch(index){
        case 0:
            printf("\tTXT is not enabled\n");
            break;
        case 1:
            printf("\tTXT is enabled\n");
            break;
        default:
            printf("\tInalid Output. TXT Enabled Discovery Failed");

    }

    printf("\n");
    return 0;
}

int ipmi_ppix_TXT_discovery(struct ipmi_intf * intf)
{
	int rc;

    char * byte_packet[] = { // Corresponding to TPM/TXT Discovery Command
        
        "0x2e",
        "0x90",
        "0x57",
        "0x01",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x20"
        };

    int size;
    size = 10;
    //size = sizeof(byte_packet)/sizeof(byte_packet[0]);

    uint8_t *rsp;
    printf("\n");

    rsp = ipmi_ppix_raw(intf, size, byte_packet);

    rc = ipmi_decode_TXT(rsp, 35);

    printf("Outputted Response: \n");
    int i;
    for(i = 0; i < 35; i++){
        if(i%16 == 0)
            printf("\n");
        printf(" %02x", *(rsp + i));
    }
    printf("\n\n");

    return rc;
}

int ipmi_ppix_TPM_discovery(struct ipmi_intf * intf)
{
	int rc;

    char * byte_packet[] = { // Corresponding to TPM/TXT Discovery Command
        
        "0x2e",
        "0x90",
        "0x57",
        "0x01",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x20"
        };

    int size;
    size = 10;
    //size = sizeof(byte_packet)/sizeof(byte_packet[0]);

    uint8_t *rsp;
    printf("\n");

    rsp = ipmi_ppix_raw(intf, size, byte_packet);

    rc = ipmi_decode_TPM(rsp, 35);

    printf("Outputted Response: \n");
    int i;
    for(i = 0; i < 35; i++){
        if(i%16 == 0)
            printf("\n");
        printf(" %02x", *(rsp + i));
    }
    printf("\n\n");

    return rc;
}

int ipmi_decode_TPM(uint8_t *in, int size_input)
{


    printf("Task and Result: %02x\n", *(in + 13));
    uint8_t task_result = *(in + 13);
    uint8_t mask = 0x80;
    uint8_t index;
    uint8_t status;

    index = task_result & mask;
    index = index >> 7;

    if (index == 0)
    {
        printf("\tTask not successfully performed\n");
    }
    else if (index == 1)
    {
        printf("\tTask successfully performed\n");
        status = *(in + 14);
        printf("Status: %02d\n", *(in + 14));
        switch(status){
            case 0x00:
                printf("\tStatus: Success\n");
                break;
            case 0x01:
                printf("\tStatus: Invalid Task\n");
                break;
            case 0x10:
                printf("\tStatus: TXT not supported\n");
                break;
            case 0x11:
                printf("\tStatus: TXT enable error\n");
                break;
            case 0x20:
                printf("\tStatus: TPM not supported\n");
                break;
            case 0x21:
                printf("\tStatus: TPM enable error\n");
                break;
            case 0x22:
                printf("\tStatus: TPM clear error\n");
                break;
            case 0x30:
                printf("\tStatus: Authentication failure\n");
                break;
            case 0x31:
                printf("\tStatus: Specified password string type not supported\n");
                break;
            case 0x32:
                printf("\tStatus: Specified SHA type not supported\n");
                break;
            default:
                printf("\tStatus: Unknown error\n");
        }


    }
    else 
    {
        printf("Error: Task and Result invalid input\n");
    }
    printf("TPM Status:\n");
    index = *(in + 17); //  TPM Support
    mask = 0x06;
    index = index & mask;
    index = index >> 1;
    //printf("index value in hex: %02x\n", index);
    switch(index){
        case 0:
            printf("\tTPM is not supported\n");
            break;
        case 1:
            printf("\tdTPM is supported\n");
            break;
        case 2:
            printf("\tfTPM is supported\n");
            break;
        case 3:
            printf("\tdTPM and fTPM are supported\n");
            break;
        default:
            printf("\tInvalid Output.  TPM Discovery Failed\n");
    }

    index = *(in + 19); //  TPM Enabled
    mask = 0x06;
    index = mask & index;
    index = index >> 1;
    switch(index){
        case 0:
            printf("\tTPM not enabled\n");
            break;
        case 1:
            printf("\tdTPM enabled\n");
            break;
        case 2:
            printf("\tfTPM enabled\n");
            break;
        default:
            printf("\tInvalid Output.  TPM Discovery Failed\n");
    }
    printf("\n");


    return 0;
}

int ipmi_ppix_TPM_refresh(struct ipmi_intf * intf, char *password)
{
 int rc;
    int length = 0;
    uint8_t pass[49];

    while(*(password + length)){
        pass[length] = (uint8_t) *(password + length); // convert char to UINT8
        length++;
    }
    char * byte_packet[100] = { // Corresponding to TPM/TXT Enable Command
        "0x2e",
        "0x91",
        "0x57",
        "0x01",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x01",
        "0x27",
        "0x00",
        "0x00",
        "0x00",
        "0x24",
        "0x4f",
        "0x58",
        "0x50",
        "0x27",
        "0x00",
        "0x20",
        "0x00",
        "0x01",
        "0x27",
        "0x05",
        "0xff",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x40",
        "0x00",
        "0x00",
        "0x40",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x01",
        "0x07",
        "0x00",
        "0x01",
        "0x80",  // Here
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",      
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",        
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00"  
    };

    int i;
    char format[5];
    char *tepm;
    for(i = 0; i < length; i++){
        sprintf(format, "0x%02x", pass[i]); // UINT to char format STRING
        byte_packet[51 + i] = malloc(strlen(format) + 1);
        strcpy(byte_packet[51 + i], format);
    }

    int size;
    size = 51 + length + 1;
    uint8_t *rsp;
    printf("\n");
    rsp = ipmi_ppix_raw(intf, size, byte_packet);
    rc = ipmi_decode_TPM_refresh(rsp, 35);

    printf("Outputted Response: \n");

    for(i = 0; i < 35; i++){
        if(i%16 == 0)
            printf("\n");
        printf(" %02x", *(rsp + i));
    }
    printf("\n\n");

    return rc;

}

int ipmi_decode_TPM_refresh(uint8_t *in, int size_input)
{
    uint8_t task_result = *(in + 13);
    uint8_t mask = 0x80;
    uint8_t index;
    uint8_t status;

    index = task_result & mask;
    index = index >> 7;

    if (index == 0)
    {
        printf("\tTask not successfully performed\n");
    }
    else if (index == 1)
    {
        printf("\tTask successfully performed\n");
        status = *(in + 14);
        printf("Status: %02d\n", *(in + 14));
        switch(status){
            case 0x00:
                printf("\tStatus: Success\n");
                break;
            case 0x01:
                printf("\tStatus: Invalid Task\n");
                break;
            default:
                printf("\tStatus: Unknown error\n");
        }
    }
    else 
    {
        printf("Error: Task and Result invalid input\n");
    }

    return 0;
}

int ipmi_ppix_full_refresh(struct ipmi_intf * intf, char *password)
{
    int rc;
    int length = 0;
    uint8_t pass[49];

    while(*(password + length)){
        pass[length] = (uint8_t) *(password + length); // convert char to UINT8
        length++;
    }
    char * byte_packet[100] = { // Corresponding to TPM/TXT Enable Command
        "0x2e",
        "0x91",
        "0x57",
        "0x01",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x01",
        "0x27",
        "0x00",
        "0x00",
        "0x00",
        "0x24",
        "0x4f",
        "0x58",
        "0x50",
        "0x27",
        "0x00",
        "0x20",
        "0x00",
        "0x01",
        "0x26",
        "0x06",
        "0xff",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x40",
        "0x00",
        "0x00",
        "0x40",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x01",
        "0x07",
        "0x00",
        "0x01",
        "0x80",  // Here
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",      
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",        
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00",
        "0x00"  
    };

    int i;
    char format[5];
    char *tepm;
    for(i = 0; i < length; i++){
        sprintf(format, "0x%02x", pass[i]); // UINT to char format STRING
        byte_packet[51 + i] = malloc(strlen(format) + 1);
        strcpy(byte_packet[51 + i], format);
    }

    int size;
    size = 51 + length + 1;
    uint8_t *rsp;
    printf("\n");
    rsp = ipmi_ppix_raw(intf, size, byte_packet);
    rc = ipmi_decode_full_refresh(rsp, 35);

    printf("Outputted Response: \n");

    for(i = 0; i < 35; i++){
        if(i%16 == 0)
            printf("\n");
        printf(" %02x", *(rsp + i));
    }
    printf("\n\n");

    return rc;

}

int ipmi_decode_full_refresh(uint8_t *in, int size_input)
{
    uint8_t task_result = *(in + 13);
    uint8_t mask = 0x80;
    uint8_t index;
    uint8_t status;

    index = task_result & mask;
    index = index >> 7;

    if (index == 0)
    {
        printf("\tTask not successfully performed\n");
    }
    else if (index == 1)
    {
        printf("\tTask successfully performed\n");
        status = *(in + 14);
        printf("Status: %02d\n", *(in + 14));
        switch(status){
            case 0x00:
                printf("\tStatus: Success\n");
                break;
            case 0x01:
                printf("\tStatus: Invalid Task\n");
                break;
            default:
                printf("\tStatus: Unknown error\n");
        }
    }
    else 
    {
        printf("Error: Task and Result invalid input\n");
    }

    return 0;
}

int ipmi_ppix_main(struct ipmi_intf * intf, int argc, char ** argv)
{
    int rc = 0;

    if ((argc == 0) || (strncmp(argv[0], "help", 4) == 0)) { 
		lprintf(LOG_NOTICE, "PPIX Commands:  discovery, enable, TXT, TPM, fullrefresh");
	}
    
    else if (strncmp(argv[0], "discovery", 9) == 0) { //  Discovery prototype function
		rc = ipmi_ppix_discovery(intf);
	}

    else if (strncmp(argv[0], "TXT", 3) == 0) // TXT Commands enable, clear, discovery
    {
        uint8_t ctl = 0;
        if ((argc < 2) || (strncmp(argv[1], "help", 4) == 0)) // HELP
        {
			lprintf(LOG_NOTICE, "TXT Commands: discovery");
			return 0;
		}
        if (strncmp(argv[1], "discovery", 9) == 0) // Discovery
        {
            rc = ipmi_ppix_TXT_discovery(intf);
			return rc;
		}

        else{
            lprintf(LOG_ERR, "Invalid PPIX command: %s", argv[0]);
		    return -1;
        }
    }
    
    else if (strncmp(argv[0], "fullrefresh", 11) == 0) // Enable TPM and TXT
    {
        if ((argc < 2) || (strncmp(argv[1], "help", 4) == 0)) 
            printf("No Password Specified \n");
        else
        {
            char *password;
            password = argv[1];
            rc = ipmi_ppix_full_refresh(intf, password);
            if(rc == 0)
                return rc;
            else if(rc == -1){
                printf("Invalid Password\n");
                return rc;
            }
        }
    }
    else if (strncmp(argv[0], "enable", 6) == 0) { // Enable prototype function
		//uint8_t *in;
        if ((argc < 2) || (strncmp(argv[1], "help", 4) == 0)) 
            printf("No Password Specified \n");
        else
        {
            char *password;
            password = argv[1];
            rc = ipmi_ppix_enable(intf, password);
            if(rc == 0)
                return rc;
            else if(rc == -1){
                printf("Invalid Password\n");
                return rc;
            }
        }
	}

    else if (strncmp(argv[0], "TPM", 3) == 0) // TPM Commands enable, clear, discovery
    {

        if ((argc < 2) || (strncmp(argv[1], "help", 4) == 0))  // HELP
        {
			printf("TPM Commands: discovery, refresh, clear\n");
			return 0;
		}
        if (strncmp(argv[1], "discovery", 9) == 0) // Discovery
        {
            rc = ipmi_ppix_TPM_discovery(intf);
            //printf("TPM discovery Function goes here\n");
			return rc;
		}
   
		else if (strncmp(argv[1], "clear", 7) == 0) // clear
        {
			if ((argc < 3) || (strncmp(argv[2], "help", 4) == 0)) 
            {
                printf("No Password Specified \n");
            }
            else{
                 char *password;
                 password = argv[2];
                 rc = ipmi_ppix_TPM_clear(intf, password);

                 if(rc == 0)
                    return rc;
                 else if (rc == -1){
                    printf("Invalid Password\n");
                    return rc;

                }
            }      
        }

		else if (strncmp(argv[1], "refresh", 7) == 0) // TPM clear and activate
        {
			if ((argc < 3) || (strncmp(argv[2], "help", 4) == 0)) 
            {
                printf("No Password Specified \n");
            }
            else{
                 char *password;
                 password = argv[2];
                 rc = ipmi_ppix_TPM_refresh(intf, password);

                 if(rc == 0)
                    return rc;
                 else if (rc == -1){
                    printf("Invalid Password\n");
                    return rc;

                }
            }      
        }
        else{
            lprintf(LOG_ERR, "Invalid PPIX command: %s", argv[0]);
		    return -1;
        }




    }
    else {
		lprintf(LOG_ERR, "Invalid PPIX command: %s", argv[0]);
		return -1;
	}



    //printf("\nPlease work\n");
    return rc;
}