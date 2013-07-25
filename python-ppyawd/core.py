import struct
import sys

# Compression constants
UNCOMPRESSED = 0
DEFLATE = 1
LZMA = 2


class AWDBlockBase(object):
    blockType=0
    blockID=-1
    tagForExport=True
    def __init__(self):
        super(AWDBlockBase, self).__init__()

    def writeString(self,inputString,lengthByte=None):
        stringAsBytes=str.encode(inputString)#bytes(inputString,"utf-8")
        outputBytes=bytes()
        if lengthByte is not None:
            outputBytes+=struct.pack("< "+str(lengthByte),len (stringAsBytes))
        outputBytes+=stringAsBytes
        return outputBytes
        
    def write_block(self):
        print ("Save Block with BlockID = "+str(self.blockID)+" BlockType = "+str(self.blockType))
        outBytes=struct.pack('<IBBB', self.blockID, 0, self.blockType,0)        
        return outBytes
        
    def write_body(self, file):
        pass # To be overridden



class AWDNamespace(object):
    def __init__(self, handle):
        self.__handle = handle

    def get_handle(self):
        return self.__handle



class AWDAttrElement(object):
    class AWDAttrDict(object):
        def __init__(self):
            self.__nsdict = {}

        def __getitem__(self, ns):
            handle = ns.get_handle()
            if handle not in self.__nsdict:
                self.__nsdict[handle] = {}

            return self.__nsdict[handle]
        
    def __init__(self):
        super(AWDAttrElement, self).__init__()
        self.attributes = self.AWDAttrDict()
        
class AWDMetaData(AWDBlockBase):
    def __init__(self):
        import ppyawd
        self.encoder = 'ppyawd'
        self.encoder_version = '%d.%d.%d%s' % (
            ppyawd.version[1], ppyawd.version[2], ppyawd.version[3], ppyawd.version[4])
        self.generator = "None"
        self.generator_version = "None"
        self.blockType=255
    def write_block(self):
        baseBlockBytes=super(AWDMetaData, self).write_block()
        metaDataPropsBytes=struct.pack("< H",1)
        metaDataPropsBytes+=struct.pack("< I",4)+struct.pack("< I",0)
        metaDataPropsBytes+=struct.pack("< H",2)
        metaDataPropsBytes+=self.writeString(self.encoder,"I")
        metaDataPropsBytes+=struct.pack("< H",3)
        metaDataPropsBytes+=self.writeString(self.encoder_version,"I")
        metaDataPropsBytes+=struct.pack("< H",4)
        metaDataPropsBytes+=self.writeString(self.generator,"I")
        metaDataPropsBytes+=struct.pack("< H",5)
        metaDataPropsBytes+=self.writeString(self.generator_version,"I")
        print ("Metadata = "+str(len(metaDataPropsBytes)))
        metaDataBytes=struct.pack("< I",len(metaDataPropsBytes))+metaDataPropsBytes        
        return baseBlockBytes+struct.pack("< I",len(metaDataBytes))+metaDataBytes


class AWDProp(object):
    def __init__(self, propID, propType, values, defaults=[]):        
        self.propID = propID
        self.propType = propType
        self.values = values
        self.defaults = defaults
        
    def writeBytes(self):
        if str(defaults)!=str(values):
            outputBytes=struct.pack("< H",propID)
            propsbytes=bytes()
            if propType=="STRING":
                pass
            else:
                for value in values:
                    propsbytes=struct.pack("< "+str(propType),4)+struct.pack("< I",0)
            outputBytes+=struct.pack("< I",4)+struct.pack("< I",0)
            
        #if str(self.defaults)!=str(self.values):
        

class AWD(object):

    def __init__(self, compression=0, streaming=False, wide_geom=False, wide_mtx=False):
        self.compression = 0

        self.flags = 0
        if streaming:
            self.flags |= 1
        if wide_geom:
            self.flags |= 2
        if wide_mtx:
            self.flags |= 4
        self.metadata_generator=None
        self.metadata_generatorVersion=None
        self.introBlocks= []
        self.texture_blocks = []
        self.mesh_data_blocks = []
        self.material_blocks = []
        self.tri_geom_blocks = []
        self.uvanim_blocks = []
        self.skelanim_blocks = []
        self.skelpose_blocks = []
        self.skeleton_blocks = []
        self.scene_blocks = []
        self.exportBlockList=[]

    def flush(self, file=sys.stdout):
        from ppyawd import pyio as io
        writer = io.AWDWriter()
        writer.write(self, file)

    def add_texture(self, block):
        self.texture_blocks.append(block)

    def add_material(self, block):
        self.material_blocks.append(block)

    def add_tri_geom(self, block):
        self.tri_geom_blocks.append(block)

    def add_scene_block(self, block):
        self.scene_blocks.append(block)

    def add_uv_anim(self, block):
        self.uvanim_blocks.append(block)

    def add_skeleton(self, block):
        self.skeleton_blocks.append(block)

    def add_skeleton_pose(self, block):
        self.skelpose_blocks.append(block)

    def add_skeleton_anim(self, block):
        self.skelanim_blocks.append(block)


