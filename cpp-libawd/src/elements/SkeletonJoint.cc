#include "blocks/skeleton.h"

using namespace AWD;
using namespace AWD::MATERIAL;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::ANIM;
using namespace AWD::SETTINGS;



SkeletonJoint::SkeletonJoint(std::string& name, TYPES::F64 *bind_mtx) :
	
	AttrElementBase()
{
	this->bind_mtx = NULL;

	set_bind_mtx(bind_mtx);
}

SkeletonJoint::~SkeletonJoint()
{
	/*
	SkeletonJoint *cur;

	cur = this->first_child;
	while (cur) {
		SkeletonJoint *next = cur->next;
		cur->next = NULL;
		delete cur; // Will remove it's children too (recursively)
		cur = next;
	}

	if (this->bind_mtx) {
		free(this->bind_mtx);
		this->bind_mtx = NULL;
	}
	*/
}

TYPES::UINT32
SkeletonJoint::get_id()
{
	return this->id;
}

void
SkeletonJoint::set_parent(SkeletonJoint *joint)
{
	this->parent = joint;
}

SkeletonJoint *
SkeletonJoint::get_parent()
{
	return this->parent;
}

void
SkeletonJoint::set_bind_mtx(TYPES::F64 *bind_mtx)
{
	if (this->bind_mtx) {
		free(this->bind_mtx);
	}
	/*
	this->bind_mtx = bind_mtx;

	if (this->bind_mtx == NULL) {
		this->bind_mtx = awdutil_id_mtx4x4(NULL);
	}
	*/
}

TYPES::F64 *
SkeletonJoint::get_bind_mtx()
{
	return this->bind_mtx;
}

SkeletonJoint *
SkeletonJoint::add_child_joint(SkeletonJoint *joint)
{
	/*
	if (joint != NULL) {
		if (joint->get_parent() != NULL) {
			// TODO: Remove from old parent
		}

		// Find place in list of children
		if (this->first_child == NULL) {
			this->first_child = joint;
		}
		else {
			this->last_child->next = joint;
		}

		joint->set_parent(this);
		this->last_child = joint;
		this->last_child->next = NULL;
		this->num_children++;
	}
	*/
	return NULL;
}

int
SkeletonJoint::calc_length(BlockSettings * blockSettings)
{
	/*
	int len;
	SkeletonJoint *child;

	// id + parent + name varstr + matrix
	len = TYPES::UINT32(sizeof(TYPES::UINT16) + sizeof(TYPES::UINT16) +
		sizeof(TYPES::UINT16) + this->get_name().size() +
		MTX43_SIZE(blockSettings->get_wide_matrix()));

	len += this->calc_attr_length(true,true, blockSettings);

	child = this->first_child;
	while (child) {
		len += child->calc_length(blockSettings);
		child = child->next;
	}
	*/

	return 0;
}

int
SkeletonJoint::calc_num_children()
{
	/*
	SkeletonJoint *child;

	num_children = this->num_children;
	child = this->first_child;
	while (child) {
		num_children += child->calc_num_children();
		child = child->next;
	}
	*/
	return 0;
}

int
SkeletonJoint::write_joint(FileWriter* fileWriter, TYPES::UINT32 id, BlockSettings * settings)
{
	/*
	int num_written;
	TYPES::UINT32 child_id;
	SkeletonJoint *child;
	TYPES::UINT16 par_id_be;
	TYPES::UINT16 id_be;

	this->id = id;

	// Convert numbers to big-endian
	id_be = this->id;
	if (this->parent)
		par_id_be = this->parent->id;
	else par_id_be = 0;

	// Write this joint
	fileWriter->writeUINT16(id_be);
	fileWriter->writeUINT16(par_id_be);
	fileWriter->writeSTRING(this->get_name(), 1);
	if (!settings->get_wide_matrix()){
		
		fileWriter->writeFLOAT32multi((TYPES::F32*)this->bind_mtx, 9);
		TYPES::F32 offX=TYPES::F32(this->bind_mtx[9] * settings->get_scale());
		TYPES::F32 offY=TYPES::F32(this->bind_mtx[10] *settings->get_scale());
		TYPES::F32 offZ=TYPES::F32(this->bind_mtx[11]*settings->get_scale());
		fileWriter->writeFLOAT32(offX);
		fileWriter->writeFLOAT32(offY);
		fileWriter->writeFLOAT32(offZ);
	}
	else{
		fileWriter->writeFLOAT64multi((TYPES::F64*)this->bind_mtx, 9);
		TYPES::F64 offX=TYPES::F64(this->bind_mtx[9] * settings->get_scale());
		TYPES::F64 offY=TYPES::F64(this->bind_mtx[10] *settings->get_scale());
		TYPES::F64 offZ=TYPES::F64(this->bind_mtx[11]*settings->get_scale());
		fileWriter->writeFLOAT64(offX);
		fileWriter->writeFLOAT64(offY);
		fileWriter->writeFLOAT64(offZ);
	}

	//  TODO: Write attributes
	this->properties->write_attributes(fileWriter, settings);
	this->user_attributes->write_attributes(fileWriter, settings);
	// Write children
	child_id = id+1;
	num_written = 1;
	child = this->first_child;
	while (child) {
		int num_children_written;

		num_children_written = child->write_joint(fileWriter, child_id, settings);

		child_id += num_children_written;
		num_written += num_children_written;

		child = child->next;
	}
	*/
	return 0;
}
