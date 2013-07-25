import struct
import sys
from ppyawd import core
class AWDWriter(object):
    def __init__(self):
        self.__block_addr = 1
        self.byteBody=bytes()

    def write(self, awd, file):
        def write_blocks(blocks):
            for b in blocks:
                self.byteBody+=b.write_block()
                #print len(self.byteBody)
                     
        def addToExportBlockList(blocks,awdData):
            for b in blocks:
                addToExportList(awdData,b)
              
        metaBlock=core.AWDMetaData()
        awd.introBlocks.append(metaBlock)
        addToExportBlockList(awd.introBlocks,awd)
        addToExportBlockList(awd.scene_blocks,awd)
        addToExportBlockList(awd.texture_blocks,awd)
        addToExportBlockList(awd.material_blocks,awd)
        addToExportBlockList(awd.tri_geom_blocks,awd)
        addToExportBlockList(awd.uvanim_blocks,awd)
        addToExportBlockList(awd.skelanim_blocks,awd)
        addToExportBlockList(awd.skelpose_blocks,awd)
        addToExportBlockList(awd.skeleton_blocks,awd)
        
        outputBits=struct.pack("3s", b"AWD")
        outputBits+=struct.pack('< B',2)
        outputBits+=struct.pack('< B',1)
        outputBits+=struct.pack('< H',0)#awd.flags)
        outputBits+=struct.pack('< B',awd.compression)
        print ("AWDCompression = "+str(awd.compression))
        write_blocks(awd.exportBlockList)
        
        outputBody=bytes()
        #if awd.compression==1:
        #    pass#outputBody = zlib.compress(self.byteBody, 9)
        #else:
            #print "UNCOMPRESSED"
        outputBody = self.byteBody
        outputBits+=struct.pack('< I',len(outputBody))+outputBody
        
        print (len(outputBits))
        file.write(outputBits)
        

def addToExportList(awd,awdBlock):
    if awdBlock is None:
        return 0
    if awdBlock.blockID>=0:
        return awdBlock.blockID
    else:
        if awdBlock.tagForExport==True:
            if awdBlock.blockType==254:#NameSpace
                pass#doThis=True
            if awdBlock.blockType==255:#metadata
                pass#doThis=True    
            if awdBlock.blockType==1:
                pass#triangelGeometry
            if awdBlock.blockType==11:
                pass#primitiveGeometry
            if awdBlock.blockType==21:
                pass#scene       
            if awdBlock.blockType==22:#container
                pass
                awdBlock.parentID=addToExportList(awd,awdBlock.get_parent())                        
            if awdBlock.blockType==23:#meshinstance
                awdBlock.parentID=addToExportList(awd,awdBlock.get_parent())
                awdBlock.geoID=addToExportList(awd,awdBlock.geometry)          
                awdBlock.materialIDs=[]
                for mat in awdBlock.materials:
                    awdBlock.materialIDs.append(addToExportList(awd,mat))
                          
            if awdBlock.blockType==41:#light
                pass
                    #parentBlock=awd.allAWDBlocks[int(awdBlock.data.dataParentBlockID)]
                    #if parentBlock is not None:
                    #    awdBlock.data.dataParentBlockID=self.addToExportList(awd,parentBlock)
                        
            if awdBlock.blockType==42:#camera
                pass
                    #parentBlock=awd.allAWDBlocks[int(awdBlock.data.dataParentBlockID)]
                    
            if awdBlock.blockType==51:#lightPicker
                pass
                    #awd.lightPickerCounter+=1
                    #awdBlock.data.name+=str(awd.lightPickerCounter)
                    #for light in awdBlock.data.lightList:
                    #    lightID=self.addToExportList(awd,awd.allAWDBlocks[int(light)])
                    #    awdBlock.data.saveLightList.append(lightID)
                    
            if awdBlock.blockType==81:#material
                pass
                    
                    #lightPickerBlock=awd.allAWDBlocks[int(awdBlock.data.lightPicker)]
                    #if lightPickerBlock is not None:
                    #    awdBlock.data.saveMatProps.append(22)
                    #    awdBlock.data.lightPickerID=self.addToExportList(awd,lightPickerBlock)
                    #if awdBlock.data.isCreated==False and awdBlock.data.colorMat==False:
                    #    awdBlock.data.isCreated=True                
                    #    #mainMaterials.createMaterial(awdBlock.data,awd)
                    #if awdBlock.data.saveMatType==2:
                    #    textureBlock=awd.allAWDBlocks[int(awdBlock.data.saveColorTextureID)]
                    #    if textureBlock is not None:
                    #        awdBlock.data.saveColorTextureID=self.addToExportList(awd,textureBlock)
                    #    normalTextureBlock=awd.allAWDBlocks[int(awdBlock.data.saveNormalTextureID)]
                    #    if normalTextureBlock is not None:
                    #        awdBlock.data.saveNormalTextureID=self.addToExportList(awd,normalTextureBlock)
                    #    ambientTextureBlock=awd.allAWDBlocks[int(awdBlock.data.saveAmbientTextureID)]
                    #    if ambientTextureBlock is not None:
                    #        awdBlock.data.saveAmbientTextureID=self.addToExportList(awd,ambientTextureBlock)
                    #    specTextureBlock=awd.allAWDBlocks[int(awdBlock.data.saveSpecTextureID)]
                    #    if specTextureBlock is not None:
                    #        awdBlock.data.saveSpecTextureID=self.addToExportList(awd,specTextureBlock)
                    
            if awdBlock.blockType==82:#texture
                pass
            if awdBlock.blockType==83:#cubetexture
                pass#      awdBlock.dataParentBlockID=addToExportList(awd,parentBlock)                    
            if awdBlock.blockType==101:#skeleton
                pass
            if awdBlock.blockType==102:#skeletonpose
                pass
            if awdBlock.blockType==103:#skeletonanimation
                pass
                    #for pose in awdBlock.data.framesIDSList:
                    #    awdBlock.data.framesIDSList2.append(addToExportList(awd,pose))
            if awdBlock.blockType==121:#uvanimation
                pass
            if awdBlock.blockType==111:#VertexState
                pass
                    #targetMesh=awd.allAWDBlocks[int(awdBlock.data.targetMesh)]
                    #if targetMesh is not None:
                    #    awdBlock.data.saveTargetMesh=addToExportList(awd,targetMesh)

            if awdBlock.blockType==112:#VertexAnimation
                pass
                   #targetMesh=awd.allAWDBlocks[int(awdBlock.data.targetMesh)]
                    #if targetMesh is not None:
                    #    awdBlock.data.saveTargetMesh=addToExportList(awd,targetMesh)
                        
            if awdBlock.blockType==113:#AnimationSet
                pass
                    #awdBlock.data.saveTargetMesh=addToExportList(awd,awdBlock.data.targetMesh)
                    #geoBlock=awd.allAWDBlocks[int(awdBlock.data.targetMesh.data.geoBlockID)]
                    #if len(geoBlock.data.saveSubMeshes[0].weightsBuffer)>0:
                    #    awdBlock.data.jointsPerVert=len(geoBlock.data.saveSubMeshes[0].weightsBuffer)/(len(geoBlock.data.saveSubMeshes[0].vertexBuffer)/3)
                    #print "AnimationNodes in Set = "+str(len(geoBlock.data.saveSubMeshes[0].weightsBuffer))
                    #print "AnimationNodes in Set = "+str(len(geoBlock.data.saveSubMeshes[0].vertexBuffer))
                    #print "AnimationNodes in Set = "+str(awdBlock.data.jointsPerVert)
                    #for anim in awdBlock.data.poseBlockIDs:
                    #    awdBlock.data.savePoseBlockIDs.append(addToExportList(awd,anim))
                 
            if awdBlock.blockType==122:#Animator
                pass
                    #awdBlock.data.saveMeshID=addToExportList(awd,awdBlock.data.targetMeshBlock)
                    #awdBlock.data.saveAnimSetID=addToExportList(awd,awdBlock.data.animSetBlock)
                    #print "awdBlock.data.skeleton "+str(awdBlock.data.skeleton)
                    #if awdBlock.data.skeleton is not None:
                    #    print "awdBlock.data.skeleton "+str(awdBlock.data.skeleton)
                    #    awdBlock.data.saveSkeletonID=addToExportList(awd,awdBlock.data.skeleton)
                    #print "awdBlock.data.saveSkeletonID "+str(awdBlock.data.saveSkeletonID)
                    #awdBlock.saveSkeletonID=addToExportList(awd,awd.skeleton)
                            
            #print "Reordered Block type = "+str(awdBlock.blockType)+"  /  "+  str(awdBlock) 
            awdBlock.blockID=int(len(awd.exportBlockList))
            awd.exportBlockList.append(awdBlock)
            return awdBlock.blockID