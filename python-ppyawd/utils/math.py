class AWDMatrix3x4(object):
    def __init__(self, raw_data=None):
        self.raw_data = raw_data

        if self.raw_data is None:
            self.raw_data = [1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]


class AWDMatrix2x3(object):
    def __init__(self, raw_data=None):
        self.raw_data = raw_data

        if self.raw_data is None:
            self.raw_data = [1,0,0,1,0,0]


class AWDVector2(object):
    def __init__(self, raw_data=None):
        self.raw_data = raw_data
        if self.raw_data is None:
            self.raw_data = [0,0]


class AWDVector3(object):
    def __init__(self, raw_data=None):
        self.raw_data = raw_data

        if self.raw_data is None:
            self.raw_data = [0, 0, 0]
