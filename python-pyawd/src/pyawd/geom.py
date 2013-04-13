from pyawd import core

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

    def add_stream(self, type, data):
        self.__data_streams.append((type,data))

    def __len__(self):
        return len(self.__data_streams)

    def __getitem__(self, index):
        return self.__data_streams[index]


class AWDTriGeom(core.AWDAttrElement, core.AWDBlockBase):
    def __init__(self, name=''):
        super(AWDTriGeom, self).__init__()

        self.name = name
        self.__sub_geoms = []

    def add_sub_geom(self, sub):
        self.__sub_geoms.append(sub)

    def __len__(self):
        return len(self.__sub_geoms)

    def __getitem__(self, index):
        return self.__sub_geoms[index]

