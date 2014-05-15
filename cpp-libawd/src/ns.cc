#include <stdlib.h>
#include "ns.h"
#include "util.h"

#include "platform.h"

AWDNamespace::AWDNamespace(const char *uri, awd_uint16 uri_len) :
    AWDBlock(NAMESPACE)
{
    this->uri_len = uri_len;
    if (uri != NULL) {
        this->uri = (char*)malloc(this->uri_len+1);
        strncpy_s(this->uri, this->uri_len, uri, _TRUNCATE);
        this->uri[this->uri_len] = 0;
    }
    this->handle = 0;
}

AWDNamespace::~AWDNamespace()
{
    if (this->uri_len>0) {
        free(this->uri);
        this->uri_len=0;
        this->uri = NULL;
    }
}

awd_nsid
AWDNamespace::get_handle()
{
    return this->handle;
}

void
AWDNamespace::set_handle(awd_nsid handle)
{
    this->handle = handle;
}

char *
AWDNamespace::get_uri(int *len)
{
    *len = this->uri_len;
    return this->uri;
}

awd_uint32
AWDNamespace::calc_body_length(BlockSettings * curBlockSetting)
{
    if(!this->get_isValid())
        return 0;
    return sizeof(awd_nsid) + sizeof(awd_uint16) + this->uri_len;
}

void
AWDNamespace::write_body(int fd, BlockSettings * curBlockSettings)
{
    write(fd, &(this->handle), sizeof(awd_nsid));
    awdutil_write_varstr(fd, this->uri, this->uri_len);
}