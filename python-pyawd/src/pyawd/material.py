from pyawd import core


class AWDBitmapTexture(core.AWDAttrElement, core.AWDBlockBase):
    EXTERNAL = 0
    EMBED = 1

    def __init__(self, type=0, name='', url=None):
        super(AWDBitmapTexture, self).__init__()
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
        self.type = type
        self.name = name
        self.texture = texture
        self.repeat = False
        self.alpha_blending = False
        self.alpha_threshold = 0.0
        self.color = 0
        self.alpha = 1.0


