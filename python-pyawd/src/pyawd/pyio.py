class AWDWriter(object):
    def __init__(self):
        self.__block_addr = 1

    def write(self, awd, file):
        def write_blocks(blocks):
            for b in blocks:
                b.write_block(file, self.__block_addr)
                self.__block_addr += 1
        
        write_blocks(awd.texture_blocks)
        write_blocks(awd.material_blocks)
        write_blocks(awd.mesh_data_blocks)
        write_blocks(awd.uvanim_blocks)
        write_blocks(awd.skelanim_blocks)
        write_blocks(awd.skelpose_blocks)
        write_blocks(awd.skeleton_blocks)
        write_blocks(awd.scene_blocks)

