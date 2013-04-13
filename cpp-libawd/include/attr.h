#ifndef _LIBAWD_ATTR_H
#define _LIBAWD_ATTR_H

#include "ns.h"
#include "awd_types.h"

#define ATTR_RETURN_NULL AWD_field_ptr _ptr; _ptr.v = NULL; return _ptr;

typedef awd_uint16 awd_propkey;

class AWDAttr
{
    protected:
        AWD_field_type type;
        AWD_field_ptr value;
        awd_uint32 value_len;

        virtual void write_metadata(int)=0;

    public:
        void write_attr(int, bool);

        void set_val(AWD_field_ptr, awd_uint32, AWD_field_type);
        AWD_field_ptr get_val(awd_uint32 *, AWD_field_type *);
        awd_uint32 get_val_len();
};


/**
 * User attributes.
*/
class AWDUserAttr : 
    public AWDAttr
{
    private:
        const char *key;
        awd_uint16 key_len;
        AWDNamespace *ns;
        
    protected:
        void write_metadata(int);

    public:
        AWDUserAttr *next;

        AWDUserAttr(AWDNamespace *, const char*, awd_uint16);
        ~AWDUserAttr();

        AWDNamespace *get_ns();
        const char *get_key();
        awd_uint16 get_key_len();
};


class AWDUserAttrList {
    private:
        AWDUserAttr *first_attr;
        AWDUserAttr *last_attr;

        AWDUserAttr *find(AWDNamespace *, const char *, awd_uint16);

    public:
        AWDUserAttrList();
        ~AWDUserAttrList();

        awd_uint32 calc_length(bool);
        void write_attributes(int, bool);

        AWD_field_ptr get_val_ptr(AWDNamespace *ns, const char *, awd_uint16);
        bool get(AWDNamespace *, const char *, awd_uint16, AWD_field_ptr *, awd_uint32 *, AWD_field_type *);
        void set(AWDNamespace *, const char *, awd_uint16, AWD_field_ptr, awd_uint32, AWD_field_type);

        //void add_namespaces(AWD *);
};



/**
 * Numeric attributes ("properties")
*/
class AWDNumAttr : 
    public AWDAttr
{
    protected:
        void write_metadata(int);

    public:
        awd_propkey key;

        AWDNumAttr *next;

        AWDNumAttr();
};


class AWDNumAttrList {
    private:
        AWDNumAttr *first_attr;
        AWDNumAttr *last_attr;

        AWDNumAttr *find(awd_propkey);

    public:
        AWDNumAttrList();
        ~AWDNumAttrList();
        awd_uint32 calc_length(bool);
        void write_attributes(int, bool);

        AWD_field_ptr get_val_ptr(awd_propkey);
        bool get(awd_propkey, AWD_field_ptr *, awd_uint32 *, AWD_field_type *);
        void set(awd_propkey, AWD_field_ptr, awd_uint32, AWD_field_type);
};




/**
 * "Attribute element", any element (such as a block, or parts of block
 * like skeleton joints) which can have attributes.
*/
class AWDAttrElement
{
    protected:
        AWDAttrElement();
        ~AWDAttrElement();

        AWDNumAttrList *properties;
        AWDUserAttrList *user_attributes;

        awd_uint32 calc_attr_length(bool, bool, bool);

    public:
        bool get_attr(AWDNamespace *, const char *, awd_uint16, AWD_field_ptr *, awd_uint32 *, AWD_field_type *);
        void set_attr(AWDNamespace *, const char *, awd_uint16, AWD_field_ptr, awd_uint32, AWD_field_type);
};

#endif
