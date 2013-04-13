class AWDGeomUtil(object):
    def __init__(self):
        self._vertices = []
        self.normal_threshold = 0.0

    def append_vert_data(self, index, pos, uv=None, norm=None, joint_wheights=None, joint_indices=None, force_hard = False):
        if uv is None:
            uv = [0,0]
        if norm is None:
            norm = [0,0,0]

        v = (index, pos[0], pos[1], pos[2], uv[0], uv[1], norm[0], norm[1], norm[2], force_hard, joint_wheights, joint_indices)

        self._vertices.append(v)

    def build_geom(self, mesh_data):
        from pyawd.cpyawd import util_build_geom


        return util_build_geom(self._vertices, mesh_data, self.normal_threshold)

