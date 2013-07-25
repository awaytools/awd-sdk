from ppyawd import core
import struct 
STR_VERTICES = 1
STR_TRIANGLES = 2
STR_UVS = 3
STR_VERTEX_NORMALS = 4
STR_VERTEX_TANGENTS = 5
STR_JOINT_INDICES = 6
STR_JOINT_WEIGHTS = 7

class AWDSubGeom:
    def __init__(self):
        self.__data_streams = []
        self.dataTypeDic={}
        self.dataTypeDic[STR_VERTICES]="f"
        self.dataTypeDic[STR_TRIANGLES]="H"
        self.dataTypeDic[STR_UVS]="f"
        self.dataTypeDic[STR_VERTEX_NORMALS]="f"
        self.dataTypeDic[STR_VERTEX_TANGENTS]="f"
        self.dataTypeDic[STR_JOINT_INDICES]="H"
        self.dataTypeDic[STR_JOINT_WEIGHTS]="f"

    def add_stream(self, streamType, data):
        self.__data_streams.append((streamType,data))

    def __len__(self):
        return len(self.__data_streams)

    def __getitem__(self, index):
        return self.__data_streams[index]

    def write_block(self):
        outputBits=struct.pack("< I",0)
        streamCount=0
        while streamCount<len(self.__data_streams):
            if len(self.__data_streams[streamCount][1])>0:   
                outputBits+=struct.pack("< B",self.__data_streams[streamCount][0])
                outputBits+=struct.pack("< B",self.__data_streams[streamCount][0])
                streamType=self.dataTypeDic[self.__data_streams[streamCount][0]]
                streamData=self.__data_streams[streamCount][1]                
                streamDataBits=bytes()
                print ("dsklfkldsfnm")
                if self.__data_streams[streamCount][0]==STR_UVS:
                    print ("UVS = ",self.__data_streams[streamCount][1] )
                streamDataBits = streamDataBits.join((struct.pack("< "+str(streamType), val) for val in streamData))    
                outputBits+=struct.pack("< I",len(streamDataBits))+streamDataBits
            streamCount+=1
        outputBits+=struct.pack("< I",0)
        return outputBits

class AWDTriGeom(core.AWDAttrElement, core.AWDBlockBase):
    def __init__(self, name=''):
        super(AWDTriGeom, self).__init__()
        self.blockType=1
        self.name = name
        self.__sub_geoms = []
        self._meshInstances = []


    def write_block(self):
        baseBlockBytes=super(AWDTriGeom, self).write_block()
        triangleGeometrieBlockBytes=bytes()
        triangleGeometrieBlockBytes+=self.writeString(self.name,"H")
        triangleGeometrieBlockBytes+=struct.pack("< H",len(self.__sub_geoms))
        triangleGeometrieBlockBytes+=struct.pack("< I",0)
        subGeoCount=0
        #print(len(self.__sub_geoms))
        while subGeoCount<len(self.__sub_geoms):
            subMeshBlockBytes=self.__sub_geoms[subGeoCount].write_block()
            triangleGeometrieBlockBytes+=struct.pack("< I",int(len(subMeshBlockBytes)-8))+subMeshBlockBytes
            subGeoCount+=1
        triangleGeometrieBlockBytes+=struct.pack("< I",0)
        return baseBlockBytes+struct.pack("< I",len(triangleGeometrieBlockBytes))+triangleGeometrieBlockBytes
        
    def add_sub_geom(self, sub):
        self.__sub_geoms.append(sub)

    def __len__(self):
        return len(self.__sub_geoms)

    def __getitem__(self, index):
        return self.__sub_geoms[index]

