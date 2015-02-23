#ifndef _LIBAWD_AWD_TYPES_H
#define _LIBAWD_AWD_TYPES_H

#include <vector>
#include <string>

namespace AWD
{
	/** \namespace AWD::TYPES
	*	\brief Basic data types defined by libAWD
	*/
	namespace TYPES{}

	/** \namespace AWD::BLOCKS
	*	\brief All classes inside the BLOCKS namespace derive from AWDBlock and NamedElementBase and optionally implement more Baseclasses. This blocks will be written into the file.
	*/
	namespace BLOCKS{}
	
	/** \namespace AWD::BLOCK
	*	\brief The namespace used for BLOCK utilities and types
	*/
	namespace BLOCK{}
	
	/** \namespace AWD::ATTR
	*	\brief The namespace used for the AWDAttributes
	*/
	namespace ATTR{}

	/** \namespace AWD::MATERIAL
	*	\brief The namespace used for Material utilities and types
	*/
	namespace MATERIAL{}

	/** \namespace AWD::BASE
	*	\brief All classes inside the BASE namespace are pure baseclasses, that should not be instiantiated directly
	*/
	namespace BASE{}

	/** \namespace AWD::GEOM
	*	\brief The namespace used for Geometry utilitils and types
	*/
	namespace GEOM{}

	/** \namespace AWD::ANIM
	*	\brief The namespace used for ANIM utilitils and types
	*/
	namespace ANIM{}

	/** \namespace AWD::SETTINGS
	*	\brief The namespace used for Settings utilitils and types
	*/
	namespace SETTINGS{}
	
	/** \namespace AWD::CAMERA
	*	\brief The namespace used for Camera utilitils and types
	*/
	namespace CAMERA{}

	/** \namespace AWD::FILES
	*	\brief The namespace used for FILE utilitils and types
	*/
	namespace FILES{}
		
	/** \namespace AWD::LIGHTS
	*	\brief The namespace used for Light utilitils and types
	*/
	namespace LIGHTS{}

	
	/** \enum result
	 * \brief type of result . Use AWD::GetMessageForMessageCode, to get the std::string for this result
	 */
	enum class result {
		// Numeric types		
		AWD_SUCCESS = 1,				///<	AWD-opperation was succesful
		AWD_ERROR,						///<	AWD-opperation failed, but no specific error-code was encountered
		AWDFILE_ERROR_GETTING_TEMPFILE,			///<	Error while writing bytes to disc
		AWDFILE_ERROR,			///<	Error while writing bytes to disc
		AWDFILE_BLOCKINSTANCE_NOT_FOUND,			///<	Error while writing bytes to disc
		FILEREADER_ERROR_OPENING_FILE,			///<	Error while writing bytes to disc
		FILEREADER_ERROR_CLOSING_FILE,			///<	Error while writing bytes to 
		FILEREADER_ERROR_READING_BYTES,			///<	Error while writing bytes to disc
		FILEREADER_ERROR_SETTING_POS,			///<	Error while writing bytes to disc
		FILEREADER_ERROR_GETTING_POS,			///<	Error while writing bytes to disc 
		WRITE_ERROR,				///<	Error while writing bytes to disc
		WRITE_WARNING,				///<	Warning while writing bytes to disc
		FILE_IS_INVALID,			///<	Error - A AWDFile could not be created, because a file already exists
		FILE_CONTAINS_INVALID_DATA,			///<	Error - A AWDFile could not be created, because a file already exists
		FILE_ALREADY_EXISTS,		///<	Error - A AWDFile could not be created, because a file already exists
		FILE_NOT_FOUND,				///<	AWD-opperation failed, because a AWDFile was not found for a given path
		FILE_PATH_INVALID,			///<	Error - A AWDFile could not be created, because a path was invalid
		FILE_IS_NOT_EMPTY,			///<	Error - A AWDFile could not be created, because a path was invalid
		BLOCK_INVLAID,				///<	AWD-opperation failed, because a given AWDBLock was not found for a given path
		READ_WARNING,				///<	Warning while reading Bytes from disc
		READ_ERROR,					///<	Error while reading Bytes from disc
		RESULT_STRING_NOT_FOUND,	///<	Undefined state - should never happen
		DIFFERENT_PROPERTY_VALUE,
		SAME_PROPERTY_VALUE,
		UNKNOWN_PROPERTY_DATA_TYPE,
		PATH_CONTAINS_LESS_THAN_THREE_SEGMENTS,
		PATH_NOT_CLOSING,
		COULD_NOT_DECIDE_ON_CURVE_TYPE,
		SUBDIVIDED_PATHES,
		NO_BLOCKS_FOUND,

	};

	namespace TYPES{
		// POD types
		typedef char	INT8;
		typedef short	INT16;
		typedef int		INT32;

		typedef unsigned char	UINT8;
		typedef unsigned short	UINT16;
		typedef unsigned int	UINT32;

		typedef float	F32;
		typedef double	F64;

		typedef unsigned int	BADDR;
		typedef unsigned char	NS_ID;

		typedef unsigned int	COLOR;

		typedef std::string	SUBGEOM_ID_STRING;

		#define AWD_TRUE	1
		#define AWD_FALSE	0
		#define AWD_NULL	0

		/** \union union_ptr
		 * \brief A union used to convert between all different awd data-types. 
		 */
		typedef union {
			void *v;
			bool *b;
			TYPES::INT8 *i8;
			TYPES::INT16 *i16;
			TYPES::INT32 *i32;
			TYPES::UINT8 *ui8;
			TYPES::UINT16 *ui16;
			TYPES::UINT32 *ui32;
			TYPES::F32 *F32;
			TYPES::F64 *F64;
			TYPES::F64 *mtx;	// do we need this duplicate ?
			TYPES::F64 *vec;	// do we need this duplicate ?
			COLOR *col;
			BADDR *addr;
			char *str;
		} union_ptr ;
	
		/** \enum storage_pecision_category
		 * \brief 
		 */
		enum class storage_precision_category {
			
			UNDEFINED_STORAGE_PRECISION=0,
			MATRIX_VALUES,		
			GEOMETRY_VALUES,			
			PROPERIES_VALUES,			
			ATTRIBUTES_VALUES,			
			FORCE_PRECISION,			
			FORCE_FILESIZE,	
		};

		/** \enum project_type
		 * \brief project_type defines the initial behaviour of AWDProject. it is not stored anywere.
		 */
		enum class project_type {

			SINGLE_FILE_EXPORT=0,		///<	Export a single AWDFile
			SINGLE_FILE_IMPORT,			///<	Import A single AWDFile
			MULTI_FILE_PROJECT,			///<	Multiple AWDFile
		};

		/** \enum property_storage_typey
		 * \brief property_storage_typey defines if a property needs to be saved differently for each file, or if it can be saved the same for all files
		 */
		enum class property_storage_type {
			// Numeric 
			STATIC_PROPERTY=0,			///<	Property is the same for all awdFile that contains the AWDBlock
			SCALED_PROPERTY,			///<	Property must be scaled whenever the file is saved
			UPDATED_PROPERTY,			///<	Property must be recreated each time the file is saved (e.g. block-adress needs to be recalculetd)
		};
		
		enum class filled_region_type {
			// Numeric 
			STANDART_FILL=0,					
			FILL_CONVERTED_FROM_STROKE,				
			GENERATED_FONT_OUTLINES,			
		};

		/** \enum state
		 * \brief state defines the state of a class that implements StateElementBase
		 */
		enum class state {
			// Numeric 
			VALID=0,				///<	Valid state - all is good for this element			
			EMPTY=1,				///<	Valid state - all is good for this element
			INVALID=2,				///<	Invalid state - the element is 
			INVALID_INDIREKT=3,		///<	Undefined state - should never happen
		};
		/** \enum process_state
		 * \brief state defines the process_state of a class that implements StateElementBase
		 */
		enum class process_state {
			// Numeric 
			UNPROCESSED=0,					///<	The element needs processing		
			PROCESSED=1,					///<	The element has been processed
			PROCESS_ACTIV=2,				///<	The element is currently in process
		};

		/** \enum data_types
		 * \brief Numeric identifier for all basic data-types contained in TYPES 
		 */
		enum class data_types {
			// Numeric types		
			INT8=1,		///<	Numberic Value			- 1		bytes - AWD::TYPES::INT8
			INT16,		///<	Numberic Value			- 2		bytes - AWD::TYPES::INT16
			INT32,		///<	Numberic Value 			- 4		bytes - AWD::TYPES::INT32
			UINT8,		///<	Numberic Value			- 1		bytes - AWD::TYPES::UINT8
			UINT16,		///<	Numberic Value			- 2		bytes - AWD::TYPES::UIN16
			UINT32,		///<	Numberic Value			- 4		bytes - AWD::TYPES::UINT32
			FLOAT32,	///<	Numberic Value			- 4		bytes - AWD::TYPES::F32
			FLOAT64,	///<	Numberic Value			- 8		bytes - AWD::TYPES::F64	

			// Derived numeric types
			BOOL=21,	///<	Derived Numberic Value	- 1 byte - boolean
			COLOR,			///<	Derived Numberic Value	- 4		bytes - AWD::TYPES::UINT32
			BADDR,			///<	Derived Numberic Value	- 4		bytes - AWD::TYPES::UINT32 \n Represents the Adress of a AWDBlock inside a AWDFile

			// Aggregate/array types
			STRING,			///<	UTF8-encoded string (unicode)
			BYTEARRAY,		///<	BigEndian ByteArray

			// Mathetmatical types
			VECTOR2x1=41,///< Derived Numberic Value	-  8 / 16 byte (depents on storage precision) - Vector2x1
			VECTOR3x1,				///< Derived Numberic Value		- 12 / 24	bytes (depents on storage precision) - Vector3x1
			VECTOR4x1,				///< Derived Numberic Value		- 16 / 32	bytes (depents on storage precision) - Vector4x1
			MTX3x2,					///< Derived Numberic Value		- 24 / 48	bytes (depents on storage precision) - Matrix3x2
			MTX3x3,					///< Derived Numberic Value		- 36 / 72	bytes (depents on storage precision) - Matrix3x3
			MTX4x3,					///< Derived Numberic Value		- 48 / 96	bytes (depents on storage precision) - Matrix4x3 
			MTX4x4,					///< Derived Numberic Value		- 64 / 128	bytes (depents on storage precision) - Matrix4x4 
			MTX5x4,					///< Derived Numberic Value		- 64 / 128	bytes (depents on storage precision) - Matrix4x4 
		};
		
		/** \enum message_type
		 * \brief Numeric identifier for all message types
		 */
		enum class message_type {
			ERROR_MESSAGE,		///<	
			WARNING_MESSAGE,		///<	
			STATUS_MESSAGE,		///<	
		};
		
		/** \struct Message
		*	\brief A Message contains a std::string, and a message_type
		* 
		*/
		struct Message
		{
			std::string message;
			TYPES::message_type type;
			Message(const std::string new_message, TYPES::message_type new_type)
			{
				message=new_message;
				type=new_type;
			}
		};
		

		enum class display_object_mask_type {
			NO_MASKING=0,
			OBJECTS_ARE_MASKS,
			OBJECTS_ARE_MASKED,

		} ;

		/** \struct NameSpace
		*	\brief A nameSpace contains a std::string, and a name_space_handle
		* 
		*/
		struct NameSpace_struct
		{
			std::string name_space;
			TYPES::UINT8 handle;
			NameSpace_struct(const std::string new_name_space, TYPES::UINT8 new_handle)
			{
				name_space = new_name_space;
				handle = new_handle;
			}

		};

	}
	namespace FILES{
	
		/** \enum write_string_with
		 * \brief Defines if a string should be saved or read with any length-indicator 
		 */
		enum class write_string_with {
			NO_LENGTH_VALUE=0,		///< Read / Write no length-indicator
			LENGTH_AS_UINT16=1,		///< Read/Write the length as AWD::TYPES::UINT16
			LENGTH_AS_UINT32=2		///< Read/Write the length as AWD::TYPES::UINT32
		};
	}
	namespace MATERIAL{
		
		/** \enum type
		 * \brief The material types.
		 */
		enum class type {
			UNDEFINED = 0,	///< Undefined AWD::BLOCKS::Material Type 
			COLOR_MATERIAL = 1,		///< Color AWD::MATERIAL::Material 
			TEXTURE_MATERIAL = 2,		///< Texture AWD::MATERIAL::Material 
			SOLID_COLOR_MATERIAL = 3,		
			SOLID_TEXTUREATLAS_MATERIAL = 4,		
			LINEAR_GRADIENT_TEXTUREATLAS_MATERIAL = 5,		
			RADIAL_GRADIENT_TEXTUREATLAS_MATERIAL = 6,		
		};
		
		/** \enum shading_type
		 * \brief Types for ShadingMethod
		 */
		enum class shading_type {
			UNDEFINED	= 0,									///< Undefined AWD::MATERIAL::ShadingMethod Type 
			AWD_SHADEMETHOD_ENV_AMBIENT=1,						///< Env Ambient AWD::MATERIAL::ShadingMethod 
			AWD_SHADEMETHOD_DIFFUSE_DEPTH=51,					///< Diffuse Depth AWD::MATERIAL::ShadingMethod 
			AWD_SHADEMETHOD_DIFFUSE_GRADIENT=52,				///< Diffuse Gradient AWD::MATERIAL::ShadingMethod 
			AWD_SHADEMETHOD_DIFFUSE_WRAP=53,					///< Diffuse Wrap AWD::MATERIAL::ShadingMethod 
			AWD_SHADEMETHOD_DIFFUSE_LIGHTMAP=54,				///< Diffuse Lightmap AWD::MATERIAL::ShadingMethod 
			AWD_SHADEMETHOD_DIFFUSE_CELL=55,					///< Diffuse Cell AWD::MATERIAL::ShadingMethod 
			AWD_SHADEMETHOD_DIFFUSE_SUBSURFACESCATTERING=56,	///< Diffuse SubSurfaceScattering AWD::MATERIAL::ShadingMethod 
			AWD_SHADEMETHOD_SPECULAR_ANISOTROPIC=101,			///< Specular Anisotropic AWD::MATERIAL::ShadingMethod 
			AWD_SHADEMETHOD_SPECULAR_PHONG=102,					///< Specular Phong AWD::MATERIAL::ShadingMethod 
			AWD_SHADEMETHOD_SPECULAR_CELL=103,					///< Specular Cell AWD::MATERIAL::ShadingMethod 
			AWD_SHADEMETHOD_SPECULAR_FRESNEL=104,				///< Specular Fresnel AWD::MATERIAL::ShadingMethod 
			AWD_SHADEMETHOD_NORMAL_SIMPLE_WATER=152,			///< Normal Simple Water AWD::MATERIAL::ShadingMethod 
			AWD_SHADOWMETHOD_WRAPPER=998,						///< AWD::BLOCKS::ShadowMethod Warpper AWD::MATERIAL::ShadingMethod (only stores a pointer to a AWD::BLOCKS::ShadowMethod)
			AWD_EFFECTMETHOD_WRAPPER=999,						///< AWD::BLOCKS::EffectMethod Warpper AWD::MATERIAL::ShadingMethod (only stores a pointer to a AWD::BLOCKS::EffectMethod)
		} ;

		/*
		enum class purpose{
			MATDIFFUSE=0,	///< AWD::BLOCKS::
			MATDIFFUSE=0,	///< AWD::BLOCKS::
			MATSPECULAR,
			MATAMBIENT,
			MATNORMAL,
			SHADINGLIGHTMAP,
			SHADINGGRADIENTDIFFUSE,
			SHADINGSIMPLEWATERNORMAL,
			FXENVMAPMASK,
			FXLIGHTMAP,
			FXMASK,
			FORCUBETEXTURE,
			FORTEXTUREPROJECTOR
		} ;
		*/
		/** \enum cube_tex_direction
		 * \brief Types for EffectMethod
		 */
		enum class  cube_tex_direction{
			POS_X=0,	///< Left texture of AWD::BLOCKS::CubeTexture
			NEG_X,	///< Right texture of AWD::BLOCKS::CubeTexture
			POS_Y,	///< Top texture of AWD::BLOCKS::CubeTexture
			NEG_Y,	///< Bottom texture of AWD::BLOCKS::CubeTexture
			POS_Z,	///< Front texture of AWD::BLOCKS::CubeTexture
			NEG_Z	///< Back texture of AWD::BLOCKS::CubeTexture
		} ;
		/** \enum effect_method_type
		 * \brief A valid CubeTexture must use all 6 of this directions.
		 */
		enum class effect_method_type {
			UNDEFINED=0,						///< Undefined AWD::BLOCKS::EffectMethod Type 
			AWD_FXMETHOD_COLORMATRIX=401,		///< ColorMatrix AWD::BLOCKS::EffectMethod 
			AWD_FXMETHOD_COLORTRANSFORM=402,	///< ColorTranform AWD::BLOCKS::EffectMethod Type 
			AWD_FXMETHOD_ENVMAP=403,			///< Envmap AWD::BLOCKS::EffectMethod Type 
			AWD_FXMETHOD_LIGHTMAP=404,			///< Lightmap AWD::BLOCKS::EffectMethod Type 
			AWD_FXMETHOD_PROJECTIVE_TEXTURE=405,	///< ProjectiveTexture AWD::BLOCKS::EffectMethod Type 
			AWD_FXMETHOD_RIMLIGHT=406,				///< RimLight AWD::BLOCKS::EffectMethod Type 
			AWD_FXMETHOD_ALPHA_MASK=407,			///< AlphaMask AWD::BLOCKS::EffectMethod Type 
			AWD_FXMETHOD_REFRACTION_ENVMAP=408,		///< RefractionEncmap AWD::BLOCKS::EffectMethod Type 
			AWD_FXMETHOD_OUTLINE=409,				///< Outline AWD::BLOCKS::EffectMethod Type 
			AWD_FXMETHOD_FRESNEL_ENVMAP=410,		///< FresnelEnvmap AWD::BLOCKS::EffectMethod Type 
			AWD_FXMETHOD_FOG=411,					///< Fog AWD::BLOCKS::EffectMethod Type 
		} ;
		
	}
	namespace LIGHTS{
		/** \enum shadow_method_type
		 * \brief A valid CubeTexture must use all 6 of this directions.
		 */
		enum class shadow_method_type {
			AWD_SHADOW_UNDEFINED=0,		///< Undefined ShadowMethod Type 
			AWD_FILTERED_SHADOW=1101,	///< Filtered AWD::BLOCKS::ShadowMethod 
			AWD_DITHERED_SHADOW=1102,	///< Ditheres AWD::BLOCKS::ShadowMethod 
			AWD_SOFT_SHADOW=1103,		///< Soft AWD::BLOCKS::ShadowMethod 
			AWD_HARD_SHADOW=1104,		///< Hard AWD::BLOCKS::ShadowMethod 
			AWD_CASCADE_SHADOW=1001,	///< Cascade AWD::BLOCKS::ShadowMethod 
			AWD_NEAR_SHADOW=1002,		///< Near AWD::BLOCKS::ShadowMethod 
		};
		
		/** \enum shadow_mapper_type
		 * \brief Types for ShadowMethod
		 */
		enum class shadow_mapper_type {
			UNDEFINED_SHADOW,
			DIRECTIONAL_SHADOW,
			NEAR_SHADOW,
			CASCADE_SHADOW,
			CUBE_SHADOW,
		};

		/** \enum type
		 * \brief The type of a light.
		 */
		enum class type {
			UNDEFINED = 0,		///< undefined Light Type 
			POINT_LIGHT,		///< Point Light  
			DIRECTIONAL_LIGHT,	///< Directional Light
		} ;
	}
	namespace GEOM{
		/** \union stream_ptr
		 * \brief Used do store data inside a SubGeomstream
		 */
		typedef union  {
			void *v;
			TYPES::INT32 *i32;
			TYPES::UINT32 *ui32;
			TYPES::F64 *F64;
		} stream_ptr;		
	
		/** \enum primitive_type
		 * \brief Types for primitves
		 */
		enum class primitive_type {
			UNDEFINED=0,	///< undefined primitve type
			PLANE=1,		///< undefined primitve type
			CUBE=2,			///< 3D primitve: Cube
			SPHERE=3,		///< 3D primitve: Sphere
			CYLINDER=4,		///< 3D primitve: Cylinder
			CONE=5,			///< 3D primitve: Cone
			CAPSULE=6,		///< 3D primitve: Capsule
			TORUS=7			///< 3D primitve: Torus
		} ;
		
	
		/** \enum stream_target
		 * \brief Defines if stream is a triangle stream or a vertex stream
		 */
		enum class stream_target {
			VERTEX_STREAM=1,			///< The stream contains vertex-data.
			TRIANGLE_STREAM=2,			///< The stream contains index-data
		};
		
		/** \enum geom_smoothing_mode
		 * \brief Defines if stream is a triangle stream or a vertex stream
		 */
		enum class geom_smoothing_mode {
			SMOOTH_NOTHING = 0,			///< PREVENT any smoothing from happening. the vertex-normals might still be affected 
			SMOOTH_ALL,					///< smooth all vertex-normals according to the subGeom-Settings
			BREAK_WITH_MATERIAL,		///< do not smooth vertex-normals, if vertex does not share material
			BREAK_WITH_FACE_GROUP,		///< do not smooth vertex-normals, if vertex does not share face-group - includes BREAK_WITH_MATERIAL
		};

		/** \enum vertex_normals_mode
		 * \brief Defines if stream is a triangle stream or a vertex stream
		 */
		enum class vertex_normals_mode {
			EXPLICIT_VERTEX_NORMALS = 1,	///< just use the normals set on the verticles. libawd performs no addional no smoothing	
			EXPLICIT_FACE_NORMALS,			///< just use the normals set on the faces. libawd performs no addional no smoothing. This a creates a "exploded" geometry.
			PHONG_FACE_NORMALS,				///< calculate smoothing (phong-shading) based on a angle-threshold between face-normals		
			PHONG_VERTEX_NORMALS,			///< calculate smoothing (phong-shading) based on a angle-threshold between vertex-normals
		};
		
		enum class edge_type {
			INNER_EDGE=1,
			OUTTER_EDGE=2,
			CONVEX_EDGE=3,
			CONCAVE_EDGE=4,
			CURVED_EDGE=5
		};
		
		enum class edge_state {
			TEST_INTERSECTING,
			NOT_INTERSECTING,
			MAX_SUBDIVISION,
			SUBDIVIDED,
			SUBDIVIDED_RESOLVED,
		} ;
		enum class intersect_state {
			RESOLVED=1,
			UNCHECKED=2,
			UNSOLVEABLE=3,
		} ;
		/** \enum geom_attr_type
		 * \brief The type of a StreamAttribute
		 */
		enum class data_stream_attr_type {
			POSITION3D=1,			
			POSITION2D=2,		
			NORMAL=3,				
			UV1=4,	
			UV2=5,	
			TANGENT=6,	
			JOINT_INDICIES=7,	
			JOINT_WEIGTHS=8,	
			COLOR=9,		
			CURVE_DATA_2D=10,	
			VERTEX_INDICIES=11,	
			UV_2D=12,	
		};

		/** \enum stream_type
		 * \brief Types for SubGeometryStream
		 */
		enum class stream_type {
			VERTICES=1,			///< The position for the vertices		
			TRIANGLES=2,		///< The index data for faces			
			UVS=3,				///< The UV for the vertices			
			VERTEX_NORMALS=4,	///< The VertexNormals					
			VERTEX_TANGENTS=5,	///< The VertexTangents					
			JOINT_INDICES=6,	///< The joint-indices for the vertices	
			VERTEX_WEIGHTS=7,	///< The joint-weights for the vertices	
			SUVS=8,				///< The UV for the vertices			
			ALLVERTDATA3D_13F=9,		///< The Combined Vertices Data		
			ALLVERTDATA2D__9F=10,		///< The Combined Vertices Data	
		};
	}
	
	namespace BLOCK{
		
		/** \enum storage_type
		 * \brief Only used on classes that inherit both from blockbase, and from datablock
		 */
		enum class  storage_type{
			EXTERNAL=0,			///< Data will be saved in external file. Only the relative path to the file is written into the AWDFile.
			EMBEDDED,			///< Data will be embbed into the AWDFile.
			UNDEFINEDTEXTYPE	///< Undefined storage type
		} ;
		
		/** \enum instance_type
		 * \brief Describes the type of a instance_type as TYPES::UINT8
		 */
		enum class instance_type {
			BLOCK_EMBBED,				///< Block will be written into the AWDFile including all dependencies (default)
			BLOCK_EXTERN_DEPENDENCIES,	///< Block will be written into the AWDFile, but dependencies are handled as external ressource.
			BLOCK_EXTERN				///< Block will be written into the AWDFile as external ressources.
		} ;
		/** \enum block_type
		 * \brief Type for block. The block_type of a block is set in the constructor of the classes that implement block
		 */
		typedef enum {
			NULL_REF=0,				///< Undefined AWDBlock Type

			// Geometry/data
			TRI_GEOM=1,				///< AWD::BLOCKS::Geometry
			PRIM_GEOM=11,			///< AWD::BLOCKS::Primitive

			// Scene objects
			SCENE=21,				///< AWD::BLOCKS::Scene
			CONTAINER=22,			///< AWD::BLOCKS::ObjectContainer3D
			MESH_INSTANCE=23,		///< AWD::BLOCKS::Mesh

			SKYBOX=31,				///< AWD::BLOCKS::SkyBox

			LIGHT=41,				///< AWD::BLOCKS::Light
			CAMERA=42,				///< AWD::BLOCKS::Camera
			TEXTURE_PROJECTOR=43,	///< AWD::BLOCKS::TextureProjector
			SOUND_SOURCE=44,		///< AWD::BLOCKS::AWDSound

			LIGHTPICKER=51,			///< AWD::BLOCKS::LightPicker

			// Partitioning
			BSP_TREE=61,			///< Not Used
			OCT_TREE=62,			///< Not Used

			// Materials
			SIMPLE_MATERIAL=81,		///< AWD::BLOCKS::Material
			BITMAP_TEXTURE=82,		///< AWD::BLOCKS::BitmapTexture
			CUBE_TEXTURE=83,		///< AWD::BLOCKS::CubeTexture
			CUBE_TEXTURE_ATF=84,	///< AWD::BLOCKS::AWDCubetextureAtf

			EFFECT_METHOD=91,		///< AWD::BLOCKS::EffectMethod
			SHADOW_METHOD=92,		///< AWD::BLOCKS::ShadowMethod
			// Animation
			SKELETON=101,			///< AWD::BLOCKS::Skeleton
			SKELETON_POSE=102,		///< AWD::BLOCKS::SkeletonPose
			SKELETON_ANIM=103,		///< AWD::BLOCKS::SkeletonAnimationClip

			VERTEX_POSE=111,		///< AWD::BLOCKS::VertexPose
			VERTEX_ANIM=112,		///< AWD::BLOCKS::VertexAnimationClip

			ANIMATION_SET=113,		///< AWD::BLOCKS::AnimationSet
			ANIMATOR=122,			///< AWD::BLOCKS::Animator

			UV_ANIM=121,			///< AWD::BLOCKS::UVAnimationClip
			
			TIMELINE=133,
			TEXT_ELEMENT=134,
			FONT=135,
			FORMAT=136,

			// Misc
			NAMESPACES=251,			///< AWD::BLOCKS::NameSpace containing multiple NameSpace
			EXTERNAL_RESSOURCE=252,	///< External Ressource AWDBlock 
			COMMAND=253,			///< AWD::BLOCKS::Command
			NAMESPACE=254,			///< AWD::BLOCKS::NameSpace containing only 1 NameSpace
			METADATA=255,			///< AWD::BLOCKS::MetaData

		}block_type;
		
		
		/** \enum category
		 * \brief The Catergories a AWDBlock can have assigned
		 */
		enum class category {
			SCENE_OBJECT,	///< the block can be part of a scenegraph
			MATERIALS,		///< the block is used to descripe material / shading / texture
			LIGHTS,			///< the block is used to lighting / shadows 
			ANIMATION		///< the block is used to descripe animation
		};
	}
	namespace FONT{
		
		enum class baselineshift_type {
			BASELINE_NORMAL=0,
			BASELINE_UP=1,
			BASELINE_DOWN=2
		} ;
		
	}
	namespace ANIM{
		/** \enum anim_type
		 * \brief Types for AWDAnimation
		 */
		enum class anim_type {
			UNDEFINED=0,	///< Undefined type of animation
			SKELETON=1,		///< SkeletonAnimation (Geometry is controlled by a skeleton) 
			VERTEX=2,		///< VertexAnimation (Geometry have animated vertex-attributes)
			UV=3,			///< UVAnimation (Animation of the UV-transform of a SubMesh)
		};
		enum class frame_command_type {
			AWD_FRAME_COMMAND_UNDEFINED=0,
			AWD_FRAME_COMMAND_ADD_LOCAL_RESSOURCE=1,
			AWD_FRAME_COMMAND_ADD_GLOBAL_RESSOURCE=2,
			AWD_FRAME_COMMAND_UPDATE=3,
			AWD_FRAME_COMMAND_REMOVE_OBJECT=4,
			AWD_FRAME_COMMAND_SOUND=5
		};

		enum class frame_label_type{
			AWD_FRAME_LABEL_NONE=1,
			AWD_FRAME_LABEL_NAME=2,
			AWD_FRAME_LABEL_COMMENT=3,
			AWD_FRAME_LABEL_ANCOR=4
		};
	}
	namespace SETTINGS{
		/** \enum compression
		 * \brief Types of compression.
		 */
		enum class compression{
			UNCOMPRESSED = 0,	///< No compression
			ZLIB = 1,			///< ZLIB compression
			LZMA = 2,			///< LZMA compression
		};
	}
	namespace CAMERA{
		/** \enum cam_type
		 * \brief The type of a Camera. TODO: This is not used in AWD yet.
		 */
		enum class cam_type {
			AWD_CAM_FREE = 0,	///< Free Camera
			AWD_CAM_TARGET = 1,	///< Target Camera
			AWD_CAM_HOVER = 2,	///< Hover Camera
		};

		/** \enum lens_type
		 * \brief The type of a Lens
		 */
		enum class lens_type{
			AWD_LENS_PERSPECTIVE = 5001,	///< Perspective Lens
			AWD_LENS_ORTHO = 5002,			///< Ortographic Lens
			AWD_LENS_ORTHOOFFCENTER = 5003,	///< Ortographic Offcenter Lens
		} ;
	}

}
#endif
