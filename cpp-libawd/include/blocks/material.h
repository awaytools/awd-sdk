/** @file */
#ifndef _LIBAWD_MATERIAL_H
#define _LIBAWD_MATERIAL_H

#include <vector>

#include "utils/awd_types.h"

#include "base/block.h"
#include "base/attr.h"

#include "files/file_writer.h"
#include "utils/settings.h"

#include "blocks/bitmap_texture.h"
#include "elements/shading.h"
#include "blocks/light.h"
#include "blocks/light_picker.h"
#include "blocks/effect_method.h"
#include "base/state_element_base.h"

namespace AWD
{
	namespace BLOCKS
	{
	
		/** \class Material
		*	\brief Descripes a AWDBlock for [Away3D Material](http://www.away3d.com) as defined by AWDSpecs: \ref awd_pt2_13_1
		*
		*/
		class Material :
			public BASE::AWDBlock,
			
			public BASE::AttrElementBase
		{
			private:
				MATERIAL::type material_type;

				SETTINGS::BlockSettings* sub_geom_settings;
				bool isCreated;//will be true, once the mtl is converted to awd
				bool isClone;
				bool isDefault;

				std::vector<MATERIAL::ShadingMethod*> shadingMethods;
		
				std::vector<BLOCKS::EffectMethod*> effectMethods;

				std::vector<BLOCKS::Material*> materialClones;

				// vars for material properties
				TYPES::COLOR color;
				BLOCKS::BitmapTexture *texture;
				BLOCKS::BitmapTexture *normalTexture;
				bool multiPass;//specialID
				bool smooth;
				bool mipmap;
				bool both_sides;
				bool premultiplied;
				int blendMode;
				TYPES::F32 alpha;
				bool alpha_blending;
				TYPES::F32 alpha_threshold;
				bool repeat;
				GEOM::VECTOR4D color_components;
				TYPES::F32 ambientStrength;
				TYPES::COLOR ambientColor;
				BLOCKS::BitmapTexture *ambientTexture;
				TYPES::F32 specularStrength;
				TYPES::UINT16 glossStrength;
				TYPES::COLOR specularColor;
				BLOCKS::BitmapTexture *specTexture;
				BLOCKS::LightPicker *lightPicker;
				BASE::AWDBlock *animator;
				BASE::AWDBlock *shadowMethod;
				GEOM::MATRIX2x3* uv_transform_mtx;
				TYPES::F64 * texture_atlas_area;

			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();				
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);

			public:
				Material(std::string&);
				Material();
				~Material();
				TYPES::INT32 focalPoint;
				GEOM::VECTOR4D get_interpolated_color(TYPES::UINT32);
				MATERIAL::GradientSpread gradient_spread;
				std::vector<TYPES::UINT32> gradient_positions;
				std::vector<GEOM::VECTOR4D> gradient_colors;
				TYPES::F64 texture_u;
				TYPES::F64 texture_v;
				TYPES::F64 texture_u_max;
				TYPES::F64 texture_v_max;
				void set_material_type(MATERIAL::type);
				MATERIAL::type get_material_type();
				void set_sub_geom_settings(SETTINGS::BlockSettings*);
				SETTINGS::BlockSettings* get_sub_geom_settings();
				void set_isClone(bool);
				bool get_isClone();
				void set_isDefault(bool);
				bool get_isDefault();
				GEOM::VECTOR4D get_color_components();				
				void set_color_components(TYPES::F64 r,TYPES::F64 g, TYPES::F64 b, TYPES::F64 a);

				void set_mappingChannel(int);
				int get_mappingChannel();
				void set_secondMappingChannel(int);
				int get_secondMappingChannel();
				void set_is_faceted(bool);
				bool get_is_faceted();
				void set_isCreated(bool);
				bool get_isCreated();

				void set_color(TYPES::COLOR);
				TYPES::COLOR get_color();
				void set_texture(BitmapTexture *);
				BitmapTexture *get_texture();
				void set_normalTexture(BitmapTexture *);
				BitmapTexture *get_normalTexture();
				void set_multiPass(bool);
				bool get_multiPass();
				void set_smooth(bool);
				bool get_smooth();
				void set_mipmap(bool);
				bool get_mipmap();
				void set_both_sides(bool);
				bool get_both_sides();
				void set_premultiplied(bool);
				bool get_premultiplied();
				void set_blendMode(int);
				int get_blendMode();
				void set_alpha(TYPES::F32);
				TYPES::F32 get_alpha();
				void set_alpha_blending(bool);
				bool get_alpha_blending();
				void set_alpha_threshold(TYPES::F32);
				TYPES::F32 get_alpha_threshold();
				void set_repeat(bool);
				bool get_repeat();
				void set_ambientStrength(TYPES::F32);
				TYPES::F32 get_ambientStrength();
				void set_ambientColor(TYPES::COLOR);
				TYPES::COLOR get_ambientColor();
				void set_ambientTexture(BitmapTexture *);
				BitmapTexture *get_ambientTexture();
				void set_specularStrength(TYPES::F32);
				TYPES::F32 get_specularStrength();
				void set_glossStrength(TYPES::UINT16);
				TYPES::UINT16 get_glossStrength();
				void set_specularColor(TYPES::COLOR);
				TYPES::COLOR get_specularColor();
				void set_specTexture(BitmapTexture *);
				BitmapTexture *get_specTexture();
				void set_lightPicker(LightPicker *);
				LightPicker *get_lightPicker();
				void set_animator(BASE::AWDBlock *);
				BASE::AWDBlock *get_animator();
				void set_shadowMethod(BASE::AWDBlock *);
				BASE::AWDBlock *get_shadowMethod();

				std::vector<Material*>& get_materialClones();

				void set_effectMethods(std::vector<EffectMethod*>);
				std::vector<EffectMethod*> get_effectMethods();
				//void set_shadingMethods(std::vector<MATERIAL::ShadingMethod*>);
				//std::vector<MATERIAL::ShadingMethod*> get_shadingMethods();

				void set_uv_transform_mtx(TYPES::F64 *);
				GEOM::MATRIX2x3* get_uv_transform_mtx();

				Material* get_unique_material(LightPicker *, BASE::AWDBlock *, std::vector<AWDBlock*>&);

				void add_method(MATERIAL::ShadingMethod *);
				void resetShadingMethods();
		};
	}
}
#endif
