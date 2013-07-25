from ppyawd import utils 
from ppyawd import core
import struct

class AWDSceneBlockBase(core.AWDAttrElement, core.AWDBlockBase):
    def __init__(self, name='', transform=None):
        super(AWDSceneBlockBase, self).__init__()
        self.__children = []
        self.__parent = None
        self.name = name
        self.transform = transform
        self.parentID = transform
        self.blockType=22

        if self.transform is None:
            self.transform = utils.AWDMatrix4x4()

    def get_parent(self):
        return self.__parent


    def set_parent(self, parent):
        if self.__parent is not None:
            self.__parent.remove_child(self)

        self.__parent = parent
        if self.__parent is not None:
            self.__parent.add_child(self)
            

    def add_child(self, child):
        if child not in self.__children:
            self.__children.append(child)
            child.set_parent(self)


    def remove_child(self, child):
        child.set_parent(None)
        while child in self.__children:
            self.__children.remove(child)

    def write_block(self):
        baseBlockBytes=super(AWDSceneBlockBase, self).write_block()
        outputBits=struct.pack("< I",self.parentID)
        for value in self.transform.raw_data:
            outputBits+=struct.pack("< f",value)
        outputBits+=self.writeString(self.name,"H")
        return baseBlockBytes, outputBits


class AWDScene(AWDSceneBlockBase):
    def __init__(self, name='', transform=None):
        super(AWDScene, self).__init__(name, transform)


class AWDMeshInst(AWDSceneBlockBase):
    def __init__(self, geom, name="test", transform=None):
        self.geometry = geom 
        self.geoID = 0
        self.materials = []
        self.materialIDs = []
        super(AWDMeshInst, self).__init__(name, transform)
        self.blockType=23
        
    def write_block(self):
        baseBlockBytes,sceneBlockBytes=super(AWDMeshInst, self).write_block()
        sceneBlockBytes+=struct.pack("< I",self.geoID)
        sceneBlockBytes+=struct.pack("< H",len(self.materialIDs))
        for mat in self.materialIDs:
            sceneBlockBytes+=struct.pack("< I",int(mat))        
        sceneBlockBytes+=struct.pack("< I",0)
        sceneBlockBytes+=struct.pack("< I",0)
        return baseBlockBytes+struct.pack("< I",len(sceneBlockBytes))+sceneBlockBytes


class AWDContainer(AWDSceneBlockBase):
    def __init__(self, name='', transform=None):
        super(AWDContainer, self).__init__(name, transform)
        self.blockType=22

    def write_block(self):
        baseBlockBytes, sceneBlockBytes=super(AWDContainer, self).write_block()
        sceneBlockBytes+=struct.pack("< I",0)
        sceneBlockBytes+=struct.pack("< I",0)
        return baseBlockBytes+struct.pack("< I",len(sceneBlockBytes))+sceneBlockBytes

