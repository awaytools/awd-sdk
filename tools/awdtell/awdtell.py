#!/usr/bin/env python

import sys
import getopt
import struct
import zlib

#from pyawd import core


BLOCKS      = 0x1
GEOMETRY    = 0x2
SCENE       = 0x4
ANIMATION   = 0x8

include = 0
offset = 0
indent_level = 0
wide_geom = False
wide_geom = True

BT_MESH_DATA = 1
BT_CONTAINER = 22
BT_MESH_INST = 23
BT_SKELETON = 101
BT_SKELPOSE = 102
BT_SKELANIM = 103


def printl(str=''):
    global indent_level

    pad = ' ' * indent_level * 2
    print('%s%s' % (pad, str))



def print_header(data):
    compressions = ('uncompressed', 'deflate (file-level)', 'lzma (file-level)')
    header = struct.unpack_from('<BBHBI', data, 3)

    if header[3] < len(compressions):
        compression = compressions[header[3]]
    else:
        compression = '<error> %s' % hex(header[3])

    printl('version:      %d.%d' % (header[0], header[1]))
    printl('compression:  %s' % compression)
    printl('body size:    %d (%s)' % (header[4], hex(header[4])))
    printl()

    return (header[3], header[2] & 2, header[2] & 4)

def read_var_str(data, offs=0):
    len = struct.unpack_from('<H', data, offs)
    str = struct.unpack_from('%ds' % len[0], data, offs+2)

    return str[0]

def print_properties(data):
    global indent_level

    offs = 0

    printl()
    props_len = struct.unpack_from('<I', data, offs)[0]
    offs += 4
    if props_len > 0:
        printl('PROPERTIES: (%db)' % props_len)
        props_end = offs + props_len

        indent_level += 1
        while (offs < props_end):
            prop_key, prop_len = struct.unpack_from('<HH', data, offs)
            offs += 4
            prop_end = offs + prop_len
            val_str = ''
            while (offs < prop_end):
                val_str += '%02x ' % struct.unpack_from('<B', data, offs)[0]
                offs += 1

            printl('%d: %s' % (prop_key, val_str))

        indent_level -= 1

    return offs

def print_user_attributes(data):
    global indent_level

    offs = 0
    attr_len = struct.unpack_from('<I', data, offs)[0]
    offs += 4

    if attr_len > 0:
        printl('USER ATTRIBUTES (%db)' % attr_len)

    return offs
                


def print_skeleton(data):
    global indent_level

    name = read_var_str(data)
    offs = 2+len(name)

    num_joints = struct.unpack_from('<H', data, offs)[0]
    offs += 2

    printl('NAME: %s' % name)
    printl('JOINTS: %d' % num_joints)

    offs += print_properties(data[offs:])

    indent_level += 1
    joints_printed = 0
    while offs < len(data) and joints_printed < num_joints:
        joint_id, parent_id = struct.unpack_from('<HH', data, offs)
        offs += 4

        joint_name = read_var_str(data, offs)
        printl('JOINT %s (id=%d, parent=%d)' % (
            joint_name, joint_id, parent_id))

        offs += (2 + len(joint_name))
        mtx = read_mtx(data, offs)

        indent_level += 1
        print_matrix(mtx)
        offs += 48
        offs += 4 # (No properties)
        offs += print_user_attributes(data[offs:])
        indent_level -= 1

        joints_printed += 1

    indent_level -= 1

    offs += print_user_attributes(data[offs:])
        


def print_skelpose(data):
    global indent_level 

    offs = 0

    pose_name = read_var_str(data, offs)
    offs += (2 + len(pose_name))

    num_joints = struct.unpack_from('<H', data, offs)[0]
    offs += 2

    printl('NAME: %s' % pose_name)
    printl('NUM TRANSFORMS: %d' % num_joints)

    offs += print_properties(data[offs:])

    indent_level += 1
    for j_idx in range(num_joints):
        has_transform = struct.unpack_from('B', data, offs)[0]
        printl('Transform')
        indent_level += 1
        offs += 1
        if has_transform == 1:
            mtx = read_mtx(data, offs)
            print_matrix(mtx)
            offs += 48
        else:
            indent_level += 1
            printl('No transformation of this joint')
            indent_level -= 1
        indent_level -= 1
            

    indent_level -= 1

    offs += print_user_attributes(data[offs:])

def read_scene_data(data):
    parent = struct.unpack_from('<I', data)[0]
    matrix = read_mtx(data, 4)
    name = read_var_str(data, 52)

    return (parent, matrix, name, 54+len(name))

def print_container(data):
    global indent_level

    parent, matrix, name, offs = read_scene_data(data)
    printl('NAME: %s' % name)
    printl('PARENT ID: %d' % parent)
    printl('TRANSFORM MATRIX:')
    print_matrix(matrix)


def print_mesh_instance(data):
    global indent_level

    parent, matrix, name, offs = read_scene_data(data)
    data_id = struct.unpack_from('<I', data, offs)[0]

    printl('NAME: %s' % name)
    printl('DATA ID: %d' % data_id)
    printl('PARENT ID: %d' % parent)
    printl('TRANSFORM MATRIX:')
    print_matrix(matrix)


def read_mtx(data, offset):
    if wide_mtx:
        matrix = struct.unpack_from('<12d', data, offset) 
    else:
        matrix = struct.unpack_from('<12f', data, offset) 

    return matrix

def print_matrix(matrix):
    for i in range(0, 11, 3):
        printl('%f %f %f' % (matrix[i], matrix[i+1], matrix[i+2]))


def print_mesh_data(data):
    global indent_level

    name = read_var_str(data)
    offs = (2 + len(name)) # var str 
    num_subs = struct.unpack_from('<H', data, offs)[0]
    offs += 2

    printl('NAME: %s' % name)
    printl('SUB-MESHES: %d' % num_subs)

    offs += print_properties(data[offs:])

    printl()

    subs_printed = 0
    indent_level += 1
    while offs < len(data) and subs_printed < num_subs:
        length = struct.unpack_from('<I', data, offs)[0]
        offs += 4

        printl('SUB-MESH')
        indent_level += 1
        printl('Length:      %d' % length)
        indent_level -= 1

        offs += 4 # TODO: Read numeric properties
        sub_end = offs + length

        indent_level += 1
        while offs < sub_end:
            stream_types = ('', 'VERTEX', 'TRIANGLE', 'UV', 'VERTEX_NORMALS', 'VERTEX_TANGENTS', 'JOINT_INDICES', 'VERTEX_WEIGHTS')
            type, data_type, str_len = struct.unpack_from('<BBI', data, offs)
            offs += 6

            if type < len(stream_types):
                stream_type = stream_types[type]
                if type == 1 or type == 3 or type==4 or type==5 or type==7:
                    if wide_geom:
                        elem_data_format = 'd'
                    else:
                        elem_data_format = 'f'
                    elem_print_format = '%f'

                elif type == 2 or type == 6:
                    if wide_geom:
                        elem_data_format = 'I'
                    else:
                        elem_data_format = 'H'
                    elem_print_format = '%d'
            else:
                stream_type = '<error> %x' % type
            
            printl('STREAM (%s)' % stream_type)
            indent_level += 1
            printl('Length: %d' % str_len)

            str_end = offs + str_len
            while offs < str_end:
                element = struct.unpack_from('<%s' % elem_data_format, data, offs)
                printl(elem_print_format % element[0])
                offs += struct.calcsize(elem_data_format)

            printl()
            indent_level -= 1
                
        subs_printed += 1
        indent_level -= 1

    indent_level -= 1

    offs += print_user_attributes(data[offs:])


def print_next_block(data):
    global indent_level

    block_types = {}
    block_types[BT_MESH_DATA] = 'MeshData'
    block_types[BT_CONTAINER] = 'Container'
    block_types[BT_MESH_INST] = 'MeshInst'
    block_types[BT_SKELETON] =  'Skeleton'
    block_types[BT_SKELPOSE] =  'SkeletonPose'
    block_types[BT_SKELANIM] =  'SkeletonAnimation'

    block_header = struct.unpack_from('<IBBBI', data, offset)

    type = block_header[2]
    flags = block_header[3]
    length = block_header[4]

    if type in block_types:
        block_type = block_types[type]
    else:
        block_type = '<error> %s (%x)' % (type, type)

    printl('BLOCK %s' % block_type)
    indent_level += 1
    printl('NS: %d, ID: %d' % (block_header[1], block_header[0]))
    printl('Flags: %x' % flags)
    printl('Length: %d' % length)

    if type == BT_MESH_INST and include&SCENE:
        printl()
        print_mesh_instance(data[offset+11 : offset+11+length])
    elif type == BT_CONTAINER and include &SCENE:
        printl()
        print_container(data[offset+11 : offset+11+length])
    elif type == BT_MESH_DATA and include&GEOMETRY:
        printl()
        print_mesh_data(data[offset+11 : offset+11+length])
    elif type == BT_SKELETON and include&ANIMATION:
        printl()
        print_skeleton(data[offset+11 : offset+11+length])
    elif type == BT_SKELPOSE and include&ANIMATION:
        printl()
        print_skelpose(data[offset+11 : offset+11+length])


    printl()
    indent_level -= 1
    return 11 + length


if __name__ == '__main__':
    opts, files = getopt.getopt(sys.argv[1:], 'bgsax')

    for opt in opts:
        if opt[0] == '-b':
            include |= BLOCKS
        elif opt[0] == '-g':
            include |= (GEOMETRY | BLOCKS)
        elif opt[0] == '-s':
            include |= (SCENE | BLOCKS)
        elif opt[0] == '-a':
            include |= (ANIMATION | BLOCKS)
        elif opt[0] == '-x':
            include = 0xffff
            

    for file in files:
        f = open(file, 'rb')
        data = f.read()
        printl(file)

        indent_level += 1
        compression, wide_geom, wide_mtx = print_header(data)

        wide_mtx = False

        uncompressed_data = None
        if compression == 0:
            offset = 12
            uncompressed_data = data
        elif compression == 1:
            offset = 0
            data = data[12:]
            uncompressed_data = zlib.decompress(data)
        elif compression == 2:
            import pylzma

            offset = 0
            uncompressed_len = struct.unpack_from('<I', data, 12)[0]
            data = data[16:]
            uncompressed_data = pylzma.decompress(data, uncompressed_len, uncompressed_len)
        else:
            print('unknown compression: %d' % compression)
            sys.exit(-1)
            
        if include & BLOCKS:
            while offset < len(uncompressed_data):
                offset += print_next_block(uncompressed_data)

