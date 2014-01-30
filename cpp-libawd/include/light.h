#ifndef _LIBAWD_LIGHT_H
#define _LIBAWD_LIGHT_H

#include "scene.h"

#define PROP_LIGHT_RADIUS 1
#define PROP_LIGHT_FALLOFF 2
#define PROP_LIGHT_COLOR 3
#define PROP_LIGHT_SPECULAR 4
#define PROP_LIGHT_DIFFUSE 5
#define PROP_LIGHT_AMBIENT_COLOR 7
#define PROP_LIGHT_AMBIENT 8

#define PROP_LIGHT_SHADOWMAPPER 9
#define PROP_LIGHT_SHADOWMAPPER_DEPTHMAPSIZE 10
#define PROP_LIGHT_SHADOWMAPPER_COVERAGE 11
#define PROP_LIGHT_SHADOWMAPPER_NUMCASCADES 12

#define PROP_LIGHT_DIRX 21
#define PROP_LIGHT_DIRY 22
#define PROP_LIGHT_DIRZ 23

typedef enum {
    AWD_LIGHT_UNDEFINED = 0,
    AWD_LIGHT_POINT,
    AWD_LIGHT_DIR,
} AWD_light_type;

class AWDLight :
    public AWDSceneBlock
{
    private:
        AWD_light_type type;
		awd_float32 radius;
		awd_float32 falloff;
		awd_color color;
		awd_float32 specular;
		awd_float32 diffuse;
		awd_color ambientColor;
		awd_float32 ambient;
		awd_float32 dirX;
		awd_float32 dirY;
		awd_float32 dirZ;
		AWDBlock * shadowMethod;
		
    protected:
        void prepare_and_add_dependencies(AWDBlockList *);
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int, BlockSettings *);

    public:
        AWDLight(const char *, awd_uint16);
        ~AWDLight();
		void set_light_type(AWD_light_type);
		AWD_light_type get_light_type();
		void set_radius(awd_float32);
		awd_float32 get_radius();
		void set_falloff(awd_float32);
		awd_float32 get_falloff();
		void set_color(awd_color);
		awd_color get_color();
		void set_specular(awd_float32);
		awd_float32 get_specular();
		void set_diffuse(awd_float32);
		awd_float32 get_diffuse();
		void set_ambientColor(awd_color);
		awd_color get_ambientColor();
		void set_ambient(awd_float32);
		awd_float32 get_ambient();
		void set_directionVec(awd_float32, awd_float32,awd_float32);
		void set_shadowMethod(AWDBlock *);
		AWDBlock * get_shadowMethod();

};

class AWDLightPicker :
    public AWDBlock,
    public AWDNamedElement,
    public AWDAttrElement
{
    private:
        AWDBlockList * lights;

    protected:
        void prepare_and_add_dependencies(AWDBlockList *);
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int, BlockSettings *);

    public:
        AWDLightPicker(const char *, awd_uint16);
        ~AWDLightPicker();
		void set_lights(AWDBlockList *);
		AWDBlockList * get_lights();
		AWDBlock * get_shadowLight();

};
#endif
