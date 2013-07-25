from ppyawd import core
import struct

class AWDBitmapTexture(core.AWDAttrElement, core.AWDBlockBase):
    EXTERNAL = 0
    EMBED = 1

    def __init__(self, type=0, name='', url=None):
        super(AWDBitmapTexture, self).__init__()
        self.blockType=82
        self.type = type
        self.name = name
        self.url = url

    def embed_file(self, path):
        with open(path, 'rb') as f:
            self.__data = f.read()

class AWDMaterial(core.AWDAttrElement, core.AWDBlockBase):
    COLOR = 1
    BITMAP = 2

    def __init__(self, type=0, name='', texture=None):
        super(AWDMaterial, self).__init__()
        self.blockType=81
        self.name = name
        self.type = type
        self.shaderIDs = []
        self.matProps = []
        self.texture = texture
        self.textureID = 0
        self.normalTexture = None
        self.normalTextureID = 0
        self.specularTexture = None
        self.specularTextureID = 0
        self.ambientTexture = None
        self.ambientTextureID = 0
        self.repeat = False
        self.alpha_blending = False
        self.alpha_threshold = 0.0
        self.color = 0
        self.specularColor = 0
        self.specularGloss = 0
        self.specularLevel = 0
        self.ambientColor = 0
        self.ambientLevel = 0
        self.alpha = 1.0
        self.spezialID=0
        self.lightPickerID=0
        self.smooth=0
        self.mipmap=False
        self.bothSides=False
        self.premultiplied=0
        self.blendMode=0
        
    def write_block(self):
        baseBlockBytes=super(AWDMaterial, self).write_block()
        materialBlockBytes=self.writeString(self.name,"H")
        materialBlockBytes+=struct.pack("< B",self.type)
        materialBlockBytes+=struct.pack("< B",len(self.shaderIDs))
        materialBAttributesBytes=bytes()
        for matProperty in self.matProps:
            if matProperty==1:
                materialBAttributesBytes+=struct.pack("< H",matProperty)
                materialBAttributesBytes+=struct.pack("< I",4)
                materialBAttributesBytes+=struct.pack("< B",int(self.color[0]))
                materialBAttributesBytes+=struct.pack("< B",int(self.color[1]))
                materialBAttributesBytes+=struct.pack("< B",int(self.color[2]))
                materialBAttributesBytes+=struct.pack("< B",int(self.color[3]))
            if matProperty==2:
                materialBAttributesBytes+=struct.pack("< H",matProperty)
                materialBAttributesBytes+=struct.pack("< I",4)
                materialBAttributesBytes+=struct.pack("< I",int(self.textureID))
            if matProperty==3:
                materialBAttributesBytes+=struct.pack("< H",matProperty)
                materialBAttributesBytes+=struct.pack("< I",4)
                materialBAttributesBytes+=struct.pack("< I",int(self.normalTextureID))
            if matProperty==4:
                materialBAttributesBytes+=struct.pack("< H",matProperty)
                materialBAttributesBytes+=struct.pack("< I",1)
                materialBAttributesBytes+=struct.pack("< B",int(self.spezialID))
            if matProperty==5:
                materialBAttributesBytes+=struct.pack("< H",matProperty)
                materialBAttributesBytes+=struct.pack("< I",1)
                materialBAttributesBytes+=struct.pack("< B",int(self.smooth))
            if matProperty==6:
                materialBAttributesBytes+=struct.pack("< H",matProperty)
                materialBAttributesBytes+=struct.pack("< I",1)
                materialBAttributesBytes+=struct.pack("< B",int(self.mipmap))
            if matProperty==7:
                materialBAttributesBytes+=struct.pack("< H",matProperty)
                materialBAttributesBytes+=struct.pack("< I",1)
                materialBAttributesBytes+=struct.pack("< B",int(self.bothSides))
            if matProperty==8:
                materialBAttributesBytes+=struct.pack("< H",matProperty)
                materialBAttributesBytes+=struct.pack("< I",1)
                materialBAttributesBytes+=struct.pack("< B",int(self.premultiplied))
            if matProperty==9:
                materialBAttributesBytes+=struct.pack("< H",matProperty)
                materialBAttributesBytes+=struct.pack("< I",1)
                materialBAttributesBytes+=struct.pack("< B",int(self.blendMode))
            if matProperty==10:
                materialBAttributesBytes+=struct.pack("< H",matProperty)
                materialBAttributesBytes+=struct.pack("< I",4)
                materialBAttributesBytes+=struct.pack("< f",self.alpha)
            if matProperty==11:
                materialBAttributesBytes+=struct.pack("< H",matProperty)
                materialBAttributesBytes+=struct.pack("< I",1)
                materialBAttributesBytes+=struct.pack("< B",int(self.alpha_blending))
            if matProperty==12:
                materialBAttributesBytes+=struct.pack("< H",matProperty)
                materialBAttributesBytes+=struct.pack("< I",4)
                materialBAttributesBytes+=struct.pack("< f",float(self.alpha_threshold))
            if matProperty==13:
                materialBAttributesBytes+=struct.pack("< H",matProperty)
                materialBAttributesBytes+=struct.pack("< I",1)
                materialBAttributesBytes+=struct.pack("< B",float(self.repeat))
            #if matProperty==14: is not used atm
            if matProperty==15:
                materialBAttributesBytes+=struct.pack("< H",matProperty)
                materialBAttributesBytes+=struct.pack("< I",4)
                materialBAttributesBytes+=struct.pack("< f",float(self.ambientLevel))
            if matProperty==16:
                materialBAttributesBytes+=struct.pack("< H",matProperty)
                materialBAttributesBytes+=struct.pack("< I",4)
                materialBAttributesBytes+=struct.pack("< B",int(self.ambientColor[0]))
                materialBAttributesBytes+=struct.pack("< B",int(self.ambientColor[1]))
                materialBAttributesBytes+=struct.pack("< B",int(self.ambientColor[2]))
                materialBAttributesBytes+=struct.pack("< B",int(self.ambientColor[3]))
            if matProperty==17:
                materialBAttributesBytes+=struct.pack("< H",matProperty)
                materialBAttributesBytes+=struct.pack("< I",4)
                materialBAttributesBytes+=struct.pack("< I",int(self.ambientTextureID))
            if matProperty==18:
                materialBAttributesBytes+=struct.pack("< H",matProperty)
                materialBAttributesBytes+=struct.pack("< I",4)
                materialBAttributesBytes+=struct.pack("< f",float(self.specularLevel))
            if matProperty==19:
                materialBAttributesBytes+=struct.pack("< H",matProperty)
                materialBAttributesBytes+=struct.pack("< I",4)
                materialBAttributesBytes+=struct.pack("< f",float(self.specularGloss))
            if matProperty==20:
                materialBAttributesBytes+=struct.pack("< H",matProperty)
                materialBAttributesBytes+=struct.pack("< I",4)
                materialBAttributesBytes+=struct.pack("< B",int(self.specularColor[0]))
                materialBAttributesBytes+=struct.pack("< B",int(self.specularColor[1]))
                materialBAttributesBytes+=struct.pack("< B",int(self.specularColor[2]))
                materialBAttributesBytes+=struct.pack("< B",int(self.specularColor[3]))
            if matProperty==21:
                materialBAttributesBytes+=struct.pack("< H",matProperty)
                materialBAttributesBytes+=struct.pack("< I",4)
                materialBAttributesBytes+=struct.pack("< I",int(self.specularTextureID))
            if matProperty==22:
                materialBAttributesBytes+=struct.pack("< H",matProperty)
                materialBAttributesBytes+=struct.pack("< I",4)
                materialBAttributesBytes+=struct.pack("< I",int(self.lightPickerID))

        materialBlockBytes+=struct.pack("< I",len(materialBAttributesBytes))+bytes(materialBAttributesBytes)
        materialBlockBytes+=struct.pack("< I",0)
        return baseBlockBytes+struct.pack("< I",len(materialBlockBytes))+materialBlockBytes


