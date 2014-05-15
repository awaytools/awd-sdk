#include <cstdio>
#include <string.h>

#include "platform.h"
#include "message.h"
#include "awd.h"

AWDMessageBlock::AWDMessageBlock(char * objName, const char * warningMessage) :
    AWDBlock(MESSAGE)
{
    const char * str_part_1="Warning!\n '";
    const char * str_part_2="'\nMessage: '";
    const char * str_part_3="'\n\n";

    this->message_len = strlen(str_part_1)+strlen(objName)+strlen(str_part_2)+strlen(warningMessage)+strlen(str_part_3)+1;
    this->message = (char *)malloc(this->message_len);

    strcpy(message, str_part_1);
    strcat(message, objName);
    strcat(message, str_part_2);
    strcat(message, warningMessage);
    strcat(message, str_part_3);

    //free(warningMessage);
}

AWDMessageBlock::~AWDMessageBlock() {
 if(this->message_len>0) {
    free(this->message);
    this->message = NULL;
    this->message_len = 0;
 }
}

void
AWDMessageBlock::prepare_and_add_dependencies(AWDBlockList *export_list)
{
}
char *
AWDMessageBlock::get_message()
{
    return this->message;
}
int
AWDMessageBlock::get_message_len()
{
    return this->message_len;
}

awd_uint32
AWDMessageBlock::calc_body_length(BlockSettings * curBlockSettings)
{
    return 0;
}

void
AWDMessageBlock::write_body(int fd, BlockSettings *curBlockSettings)
{
}