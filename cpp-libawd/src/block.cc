#include "awd_types.h"
#include "block.h"
#include "util.h"

#include "platform.h"


AWDBlock::AWDBlock(AWD_block_type type) 
{
    this->type = type;
    
    // TODO: Allow setting flags
    this->flags = 0;
	this->addr = 0;
}


void
AWDBlock::prepare_write()
{
    // Does nothing by default. Can be optionally
    // overriden by sub-classes to take any actions
    // that need to happen before length is calculated
}

size_t
AWDBlock::write_block(int fd, awd_baddr addr)
{
    awd_uint8 ns_addr;
    awd_uint32 length;
    awd_uint32 length_be;
    awd_baddr block_addr_be;

	// TODO: Don't hard-code!
	bool wide_mtx = false;

    this->addr = addr;

    this->prepare_write();
    length = this->calc_body_length(wide_mtx);

    //TODO: Get addr of actual namespace
    ns_addr = 0;

    // Convert to big-endian if necessary
    block_addr_be = UI32(this->addr);
    length_be = UI32(length);

    // Write header
    write(fd, &block_addr_be, sizeof(awd_baddr));
    write(fd, &ns_addr, sizeof(awd_uint8));
    write(fd, &this->type, sizeof(awd_uint8));
    write(fd, &this->flags, sizeof(awd_uint8));
    write(fd, &length_be, sizeof(awd_uint32));

    // Write body using concrete implementation
    // in block sub-classes
    this->write_body(fd, wide_mtx);


    return (size_t)length + 11;
}



awd_baddr
AWDBlock::get_addr()
{
    return this->addr;
}


AWD_block_type
AWDBlock::get_type()
{
    return this->type;
}




AWDBlockList::AWDBlockList()
{
    this->first_block = NULL;
    this->last_block = NULL;
    this->num_blocks = 0;
}

AWDBlockList::~AWDBlockList()
{
    list_block *cur;

    cur = this->first_block;
    while(cur) {
        list_block *next = cur->next;
        cur->next = NULL;
        delete cur->block;
		delete cur;
        cur = next;
    }

    // Already deleted as part 
    // of above loop
    this->first_block = NULL;
    this->last_block = NULL;
}


bool
AWDBlockList::append(AWDBlock *block)
{
    if (!this->contains(block)) {
        list_block *ctr = (list_block *)malloc(sizeof(list_block));
        ctr->block = block;
        if (this->first_block == NULL) {
            this->first_block = ctr;
        }
        else {
            this->last_block->next = ctr;
        }

        this->last_block = ctr;
        this->last_block->next = NULL;
        this->num_blocks++;

        return true;
    }
    else {
        return false;
    }
}


void
AWDBlockList::force_append(AWDBlock *block)
{
    list_block *ctr = (list_block *)malloc(sizeof(list_block));
    ctr->block = block;
    if (this->first_block == NULL) {
        this->first_block = ctr;
    }
    else {
        this->last_block->next = ctr;
    }
    
    this->last_block = ctr;
    this->last_block->next = NULL;
    this->num_blocks++;
}


bool
AWDBlockList::contains(AWDBlock *block)
{
    list_block *cur;

    cur = this->first_block;
    while (cur) {
        if (cur->block == block)
            return true;

        cur = cur->next;
    }

    return false;
}


int
AWDBlockList::get_num_blocks()
{
    return this->num_blocks;
}





AWDBlockIterator::AWDBlockIterator(AWDBlockList *list)
{
    this->list = list;
    this->cur_block = NULL;
    this->reset();
}


void
AWDBlockIterator::reset()
{
    if (this->list != NULL)
        this->cur_block = this->list->first_block;
}

AWDBlock *
AWDBlockIterator::next()
{
    list_block *tmp;

    tmp = this->cur_block;
    if (this->cur_block != NULL)
        this->cur_block = this->cur_block->next;

    if (tmp)
        return tmp->block;
    else return NULL;
}
