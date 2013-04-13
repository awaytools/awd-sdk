#include <stdlib.h>
#include "ns.h"
#include "util.h"

#include "platform.h"


AWDNamespace::AWDNamespace(const char *uri, awd_uint16 uri_len) :
    AWDBlock(NAMESPACE)
{
    this->uri = uri;
    this->uri_len = uri_len;
}


AWDNamespace::~AWDNamespace()
{
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


const char *
AWDNamespace::get_uri(int *len)
{
    *len = this->uri_len;
    return this->uri;
}


awd_uint32 
AWDNamespace::calc_body_length(bool wide_mtx)
{
    return sizeof(awd_nsid) + sizeof(awd_uint16) + this->uri_len;
}


void 
AWDNamespace::write_body(int fd, bool wide_mtx)
{
    write(fd, &(this->handle), sizeof(awd_nsid));
    awdutil_write_varstr(fd, this->uri, this->uri_len);
}
