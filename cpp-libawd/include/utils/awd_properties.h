#ifndef _LIBAWD_AWD_PROPERTIES_H
#define _LIBAWD_AWD_PROPERTIES_H

namespace AWD
{
	namespace ATTR
	{
		// meta data properties:	
		#define PROP_META_TIMESTAMP 1			///< Property-key ID for setting timestamp on a MetaData
		#define PROP_META_ENCODER_NAME 2		///< Property-key ID for setting Encoder Name on a MetaData
		#define PROP_META_ENCODER_VER 3			///< Property-key ID for setting Encoder Version on a MetaData
		#define PROP_META_GENERATOR_NAME 4		///< Property-key ID for setting Generator Version on a MetaData
		#define PROP_META_GENERATOR_VER 5		///< Property-key ID for setting Generator Version on a MetaData

		// camera properties:	
		#define PROP_CAM_FOV 101				///< Property-key ID for setting Field of View on Camera
		
		#define PROP_TEXTFORMAT_FONTSIZE 1				
		#define PROP_TEXTFORMAT_LETTER_SPACING 2			
		#define PROP_TEXTFORMAT_IS_ROTATED 3	
		#define PROP_TEXTFORMAT_AUTO_KERNING 4	
		#define PROP_TEXTFORMAT_BASELINESHIFT 5	
		#define PROP_TEXTFORMAT_ALIGNMENT 6	
		#define PROP_TEXTFORMAT_INDENT 7	
		#define PROP_TEXTFORMAT_LEFT_MARGIN 8	
		#define PROP_TEXTFORMAT_RIGHT_MARGIN 9	
		#define PROP_TEXTFORMAT_LINE_SPACING 10	
		#define PROP_TEXTFORMAT_COLOR 11	
		
		
		#define PROP_TEXTFIELD_IS_SELECTABLE 1				
		#define PROP_TEXTFIELD_BORDER 3	
		#define PROP_TEXTFIELD_RENDER_HTML 4	
		#define PROP_TEXTFIELD_IS_SCROLLABLE 5	
		#define PROP_TEXTFIELD_TEXTFLOW 7	
		#define PROP_TEXTFIELD_ORIENTATIONMODE 8	
		#define PROP_TEXTFIELD_LINEMODE 9	

		#define PROP_FRAMECOMMAND_DISPLAYMATRIX2D 1				
		#define PROP_FRAMECOMMAND_DISPLAYMATRIX3D 2			
		#define PROP_FRAMECOMMAND_COLORTRANSFORM 3	
		#define PROP_FRAMECOMMAND_BLENDMODE 4	
		#define PROP_FRAMECOMMAND_VISIBLITY 5	
		#define PROP_FRAMECOMMAND_DEPTH 6	
		#define PROP_FRAMECOMMAND_MASK 7

		// material properties:
		#define PROP_MAT_COLOR 1				///< Property-key ID for setting color on Material
		#define PROP_MAT_TEXTURE 2				///< Property-key ID for setting Diffuse-Texture-Address on Material
		#define PROP_MAT_NORMALTEXTURE 3		///< Property-key ID for setting Normal-Texture-Address on Material
		#define PROP_MAT_SPECIAL_ID 4			///< Property-key ID for setting the specialID \todo make this better
		#define PROP_MAT_SMOOTH 5
		#define PROP_MAT_MIPMAP 6
		#define PROP_MAT_BOTHSIDES 7
		#define PROP_MAT_APLHA_PREMULTIPLIED 8
		#define PROP_MAT_BLENDMODE 9
		#define PROP_MAT_ALPHA 10
		#define PROP_MAT_ALPHA_BLENDING 11
		#define PROP_MAT_ALPHA_THRESHOLD 12
		#define PROP_MAT_REPEAT 13
		#define PROP_MAT_CURRENTLY_NOT_USED 14 
		#define PROP_MAT_AMBIENT_LEVEL 15
		#define PROP_MAT_AMBIENTCOLOR 16
		#define PROP_MAT_AMBIENTTEXTURE 17
		#define PROP_MAT_SPECULARLEVEL 18
		#define PROP_MAT_GLOSS 19
		#define PROP_MAT_SPECULARCOLOR 20
		#define PROP_MAT_SPECULARTEXTURE 21
		#define PROP_MAT_LIGHTPICKER 22
		
		#define PROP_SPRITE_REGPOINTX 1
		#define PROP_SPRITE_REGPOINTY 2
		#define PROP_SPRITE_REGSCALEX 3
		#define PROP_SPRITE_REGSCALEY 4
		// primitive properties (todo: remove becasue we have the reusable props)
		#define PROP_PRIM_NUMBER1 101
		#define PROP_PRIM_NUMBER2 102
		#define PROP_PRIM_NUMBER3 103
		#define PROP_PRIM_NUMBER4 104
		#define PROP_PRIM_NUMBER5 105
		#define PROP_PRIM_NUMBER6 106

		#define PROP_PRIM_SCALEU 110
		#define PROP_PRIM_SCALEV 111

		#define PROP_PRIM_INT1 301
		#define PROP_PRIM_INT2 302
		#define PROP_PRIM_INT3 303
		#define PROP_PRIM_INT4 304
		#define PROP_PRIM_INT5 305
		#define PROP_PRIM_INT6 306

		#define PROP_PRIM_BOOL1 701
		#define PROP_PRIM_BOOL2 702
		#define PROP_PRIM_BOOL3 703
		#define PROP_PRIM_BOOL4 704
		#define PROP_PRIM_BOOL5 705
		#define PROP_PRIM_BOOL6 706
	
		// light properties:
		#define PROP_LIGHT_RADIUS 1
		#define PROP_LIGHT_FALLOFF 2
		#define PROP_LIGHT_COLOR 3
		#define PROP_LIGHT_SPECULAR 4
		#define PROP_LIGHT_DIFFUSE 5
		#define PROP_LIGHT_AMBIENT_COLOR 7
		#define PROP_LIGHT_AMBIENT 8
	
		// shadowmapper properties:
		#define PROP_LIGHT_SHADOWMAPPER 9
		#define PROP_LIGHT_SHADOWMAPPER_DEPTHMAPSIZE 10
		#define PROP_LIGHT_SHADOWMAPPER_COVERAGE 11
		#define PROP_LIGHT_SHADOWMAPPER_NUMCASCADES 12
	
		// light properties (directional only):
		#define PROP_LIGHT_DIRX 21
		#define PROP_LIGHT_DIRY 22
		#define PROP_LIGHT_DIRZ 23
		
		#define PROPS_BADDR1 1
		#define PROPS_BADDR2 2
		#define PROPS_BADDR3 3
		#define PROPS_BADDR4 4

		#define PROPS_NUMBER1 101
		#define PROPS_NUMBER2 102
		#define PROPS_NUMBER3 103
		#define PROPS_NUMBER4 104
		#define PROPS_NUMBER5 105
		#define PROPS_NUMBER6 106

		#define PROPS_SCALEU 110
		#define PROPS_SCALEV 111

		#define PROPS_INT1 301
		#define PROPS_INT2 302
		#define PROPS_INT3 303
		#define PROPS_INT4 304
		#define PROPS_INT5 305
		#define PROPS_INT6 306

		#define PROPS_INT8_1 401

		#define PROPS_COLOR1 601
		#define PROPS_COLOR2 602
		#define PROPS_COLOR3 603

		#define PROPS_BOOL1 701
		#define PROPS_BOOL2 702
		#define PROPS_BOOL3 703
		#define PROPS_BOOL4 704
		#define PROPS_BOOL5 705
		#define PROPS_BOOL6 706
	}
}
#endif
