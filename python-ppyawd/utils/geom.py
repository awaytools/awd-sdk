
import ppyawd
from ppyawd.geom import AWDSubGeom
class AWDGeomUtil(object):
    def __init__(self):
        self._vertices = []
        self.normal_threshold = 0.0

    def append_vert_data(self, index, pos, uv=None, norm=None, joint_wheights=None, joint_indices=None, force_hard = False, material=None):
        if uv is None:
            uv = [0,0]
        if norm is None:
            norm = [0,0,0]

        v = (index, pos[0], pos[1], pos[2], uv[0], uv[1], norm[0], norm[1], norm[2], force_hard, joint_wheights, joint_indices, material)

        self._vertices.append(v)

    def build_geom(self, mesh_data):
        '''### to DO:
            - for each triangle:        calculate the facenormal
                                        for each point check for shared point, and calculate angle between shared point
                                        if angle is a phong break, check for next shared points angle
                                        if the point is merged because of angle, but has different uv, merge the normals
                                        
                                        split the points and triangle by the material applied to the points(should be the same for all points of a triangles)
                                        
        '''
        matDic={}
        subMeshes=[]
        vertices=[]
        indices=[]
        uvs=[]
        normals=[]
        #vertCnt=0
        #while vertCnt<len(self._vertices):
        #    vertCnt+=1
        maxCount=165000
        for vert in self._vertices:
            mat=matDic.get(str(vert[12]),None)
            if mat is None:
                newSubMesh=[]
                subMeshes.append(newSubMesh)
                matDic[str(vert[12])]=True
            if int(len(vertices)/3)<maxCount:
                indices.append(int(len(vertices)/3))
                vertices.append(vert[1])
                vertices.append(vert[2])
                vertices.append(vert[3])
                uvs.append(vert[4])
                uvs.append(vert[5])
                normals.append(vert[6])
                normals.append(vert[7])
                normals.append(vert[8])
            #print (vert[1])
        
        for subGeo in subMeshes:    
            sub = AWDSubGeom()
            sub.add_stream(ppyawd.geom.STR_VERTICES, vertices)
            sub.add_stream(ppyawd.geom.STR_TRIANGLES, indices)
            sub.add_stream(ppyawd.geom.STR_UVS, uvs)
            sub.add_stream(ppyawd.geom.STR_VERTEX_NORMALS, normals)    
            mesh_data.add_sub_geom(sub)   
            

