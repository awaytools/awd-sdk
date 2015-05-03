#include "blocks/cube_texture.h"
#include "utils/awd_types.h"
#include "base/block.h"

#include "base/attr.h"
#include "base/state_element_base.h"

#include "files/file_writer.h"
#include "utils/settings.h"
#include "blocks/bitmap_texture.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCK;
using namespace AWD::BLOCKS;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;

CubeTexture::CubeTexture(std::string& name) :
	AWDBlock(block_type::CUBE_TEXTURE, name),
	
	AttrElementBase()
{
	this->saveType = storage_type::UNDEFINEDTEXTYPE;
}

CubeTexture::CubeTexture():
	AWDBlock(block_type::CUBE_TEXTURE),
	AttrElementBase()
{
}
CubeTexture::~CubeTexture()
{
}

TYPES::state CubeTexture::validate_block_state()
{
	return TYPES::state::VALID;
}
std::vector<BitmapTexture *> 
CubeTexture::get_texture_blocks()
{
	return this->texture_blocks;
}
void
CubeTexture::set_texture_blocks(std::vector<BitmapTexture *>  texture_blocks)
{
	this->texture_blocks = texture_blocks;
}

storage_type
CubeTexture::get_tex_type()
{
	return this->saveType;
}
void
CubeTexture::set_tex_type(storage_type texType)
{
	this->saveType = texType;
}

TYPES::UINT32
CubeTexture::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	/*if(!this->get_isValid())
		return 0;*/
	TYPES::UINT32 len;

	len = TYPES::UINT32(sizeof(TYPES::UINT16) + this->get_name().size()); //name
	len += sizeof(TYPES::UINT8); //save type (external/embbed)
	if (this->texture_blocks.size()==1){
		this->type=block_type::CUBE_TEXTURE_ATF;
	}
	else if (this->texture_blocks.size()==6){
		this->type=block_type::CUBE_TEXTURE;
	}
	else {
		//TODO: HANDLE ERROR HERE; OR DO IT BEFORE THIS
	}
	/*
	for(BitmapTexture *awdBitmap : this->texture_blocks){
		//len += awdBitmap->calc_bytes_length(this->saveType);		
	}*/
	len += this->calc_attr_length(true, true, settings);
	return len;
}

result
CubeTexture::collect_dependencies(FILES::AWDFile * target_file, BLOCK::instance_type instance_type)
{
	// Do nothing
	return result::AWD_SUCCESS;
}

result
CubeTexture::write_body(FileWriter * fileWriter, BlockSettings *settings, FILES::AWDFile* file)
{
	TYPES::UINT8 saveType = (TYPES::UINT8)this->saveType;
	fileWriter->writeUINT8(saveType);
	fileWriter->writeSTRING(this->get_name(), FILES::write_string_with::LENGTH_AS_UINT16);

	/*
	for(BitmapTexture *awdBitmap : this->texture_blocks){
		//awdBitmap->write_for_CubeTex(fileWriter,this->saveType);		
	}
	*/
	this->properties->write_attributes(fileWriter, settings);
	this->user_attributes->write_attributes(fileWriter, settings);
	return result::AWD_SUCCESS;
}
