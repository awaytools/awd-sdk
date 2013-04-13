from pyawd import utils 
from pyawd import core

class GenericAnimFrame(object):
    def __init__(self, data=None, duration=0):
        self.duration = duration
        self.data = data


class GenericAnim(object):
    def __init__(self, frames):
        super(GenericAnim, self).__init__()
        self.__frames = frames

    def __len__(self):
        return len(self.__frames)

    def __getitem__(self, key):
        idx = int(key)
        return self.__frames[idx]

    def __setitem__(self, key, val):
        idx = int(key)
        if isinstance(val, GenericAnimFrame):
            self.__frames[idx] = val
        else:
            raise ValueError('value must be GenericAnimFrame instance')

    def __contains__(self, item):
        return item in self.__frames


class AWDSkeleton(core.AWDBlockBase, core.AWDAttrElement):
    def __init__(self, name=''):
        super(AWDSkeleton, self).__init__()
        self.name = name
        self.root_joint = None

    def joint_index(self, name=None, joint=None):
        if name is None:
            if joint is not None:
                name = joint.name
            else:
                raise AttributeError('either name or joint argument must be defined.')

        if self.root_joint is None:
            return None
        elif self.root_joint.name == name:
            return self.root_joint
        else:
            def find_name(joints, cur_idx):
                for j in joints:
                    #print('checking joint "%s", idx=%d' % (j.name, cur_idx))
                    if j.name == name:
                        return (cur_idx, cur_idx)
                    else:
                        found_idx, cur_idx = find_name(j._AWDSkeletonJoint__children, cur_idx+1)
                        if found_idx is not None:
                            return (found_idx, cur_idx)

                return (None, cur_idx)

            # Find joint, starting at 2 (1 being the root, which has already
            # been checked outside of the recursion.)
            ret = find_name(self.root_joint._AWDSkeletonJoint__children, 2)
            if ret is not None:
                return ret[0]
        

class AWDSkeletonAnimation(GenericAnim, core.AWDAttrElement, core.AWDBlockBase):
    def __init__(self, name=''):
        self.name = name
        self.__frames = []
        super(AWDSkeletonAnimation, self).__init__(self.__frames)

    def add_frame(self, pose, duration):
        dur = int(duration)
        self.__frames.append(GenericAnimFrame(data=pose, duration=dur))


class AWDSkeletonJoint(core.AWDAttrElement):
    def __init__(self, name='', inv_bind_mtx=None):
        super(AWDSkeletonJoint, self).__init__()
        self.name = name
        self.inv_bind_mtx = inv_bind_mtx

        self.__children = []
        self.__parent = None

        if self.inv_bind_mtx is None:
            self.inv_bind_mtx = utils.AWDMatrix4x4()

    def remove_child_joint(self, child):
        self.__children.remove(child)
        
    def add_child_joint(self, child):
        if child.__parent is not None:
            child.__parent.remove_child_joint(child)

        child.__parent = self
        self.__children.append(child)


class AWDSkeletonPose(core.AWDBlockBase, core.AWDAttrElement):
    def __init__(self, name=''):
        super(AWDSkeletonPose, self).__init__()
        self.name = name
        self.transforms = []

    def add_joint_transform(self, transform=None):
        self.transforms.append(transform)
        

class AWDUVAnimation(GenericAnim, core.AWDAttrElement, core.AWDBlockBase):
    def __init__(self, name=''):
        self.name = name
        self.__frames = []
        super(AWDUVAnimation, self).__init__(self.__frames)

    def add_frame(self, transform, duration):
        dur = int(duration)
        self.__frames.append(GenericAnimFrame(data=transform, duration=dur))



