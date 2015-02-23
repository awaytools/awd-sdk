var awd_specs =
[
    [ "Part I: Introduction", "awd_specs_1.html", [
      [ "What is AWD?", "awd_specs_1.html#awd_pt1_1", null ],
      [ "What is Away3D?", "awd_specs_1.html#awd_pt1_2", null ],
      [ "AWD Design intentions", "awd_specs_1.html#awd_pt1_3", null ],
      [ "What about old-school AWD?", "awd_specs_1.html#awd_pt1_4", null ],
      [ "How to read this document", "awd_specs_1.html#awd_pt1_5", [
        [ "For encoder implementers", "awd_specs_1.html#awd_pt1_5_1", null ],
        [ "For parser implementers", "awd_specs_1.html#awd_pt1_5_2", null ],
        [ "For those looking to extend the format", "awd_specs_1.html#awd_pt1_5_3", null ]
      ] ],
      [ "Terminology", "awd_specs_1.html#awd_pt1_6", [
        [ "AWD Project", "awd_specs_1.html#awd_pt1_6_1", null ],
        [ "Block / AWDBlock", "awd_specs_1.html#awd_pt1_6_2", null ],
        [ "Field", "awd_specs_1.html#awd_pt1_6_3", null ],
        [ "Element", "awd_specs_1.html#awd_pt1_6_4", null ],
        [ "Vector", "awd_specs_1.html#awd_pt1_6_5", null ]
      ] ]
    ] ],
    [ "Part II: File format specification", "awd_specs_2.html", [
      [ "Top-level structure of an AWD document", "awd_specs_2.html#awd_pt2_1", [
        [ "File header", "awd_specs_2.html#awd_pt2_file_header", [
          [ "Flags", "awd_specs_2.html#awd_pt2_file_header_1", null ]
        ] ],
        [ "File body", "awd_specs_2.html#awd_pt2_1_2", null ]
      ] ],
      [ "Streaming AWD", "awd_specs_2.html#awd_pt2_2", null ],
      [ "Compression", "awd_specs_2.html#awd_pt2_3", [
        [ "ZLIB/Deflate compression", "awd_specs_2.html#awd_pt2_3_1", null ],
        [ "LZMA compression", "awd_specs_2.html#awd_pt2_3_2", null ]
      ] ],
      [ "Field types and special values", "awd_specs_2.html#awd_pt2_4", [
        [ "Storage-Precision", "awd_specs_2.html#awd_pt2_4_1", [
          [ "MatrixSP", "awd_specs_2.html#awd_pt2_4_1_1", null ],
          [ "GeomSP", "awd_specs_2.html#awd_pt2_4_1_2", null ],
          [ "PropsSP", "awd_specs_2.html#awd_pt2_4_1_3", null ],
          [ "AttrSP", "awd_specs_2.html#awd_pt2_4_1_4", null ]
        ] ],
        [ "Field type identification", "awd_specs_2.html#awd_pt2_4_2", null ],
        [ "Numbers", "awd_specs_2.html#awd_pt2_4_3", [
          [ "Endianness", "awd_specs_2.html#awd_pt2_4_3_1", null ],
          [ "Integers", "awd_specs_2.html#awd_pt2_4_3_2", null ],
          [ "Floating-point numbers", "awd_specs_2.html#awd_pt2_4_3_3", null ]
        ] ],
        [ "Booleans and true/false values", "awd_specs_2.html#awd_pt2_4_4", null ],
        [ "Byte arrays", "awd_specs_2.html#awd_pt2_4_5", [
          [ "ConstString", "awd_specs_2.html#awd_pt2_4_5_1", null ],
          [ "VarString", "awd_specs_2.html#awd_pt2_4_5_2", null ]
        ] ],
        [ "Lists", "awd_specs_2.html#awd_pt2_4_6", [
          [ "ConstList", "awd_specs_2.html#awd_pt2_4_6_1", null ]
        ] ],
        [ "Vectors and matrices", "awd_specs_2.html#awd_pt2_4_7", [
          [ "Vectors and matrices are serialized as a one-dimensional list of floating point numbers. The context defines the size of the vector or matrix.", "awd_specs_2.html#awd_pt2_4_7_1", null ],
          [ "MxN matrix", "awd_specs_2.html#awd_pt2_4_7_2", null ]
        ] ],
        [ "Addresses", "awd_specs_2.html#awd_pt2_4_8", null ],
        [ "Colors", "awd_specs_2.html#awd_pt2_4_9", null ],
        [ "Attributes and properties", "awd_specs_2.html#awd_pt2_4_10", [
          [ "Attribute lists", "awd_specs_2.html#awd_pt2_4_10_1", null ],
          [ "Numeric attributes (“properties”)", "awd_specs_2.html#awd_pt2_4_10_2", null ],
          [ "Text attributes (“user attributes”)", "awd_specs_2.html#awd_pt2_4_10_3", null ]
        ] ],
        [ "TypedPropertiesLists", "awd_specs_2.html#awd_pt2_4_11", [
          [ "TypedProperties-IDs", "awd_specs_2.html#awd_pt2_4_11_1", null ]
        ] ],
        [ "References and null values", "awd_specs_2.html#awd_pt2_4_12", null ]
      ] ],
      [ "The Data Block concept", "awd_specs_2.html#awd_pt2_data_block_concept", [
        [ "Anatomy of a block", "awd_specs_2.html#awd_pt2_9", [
          [ "Block Header", "awd_specs_2.html#awd_pt2_9_1", null ],
          [ "Block flags", "awd_specs_2.html#awd_pt2_9_1_1", null ]
        ] ],
        [ "Block IDs and addressing", "awd_specs_2.html#awd_pt2_9_2", null ],
        [ "Block namespaces", "awd_specs_2.html#awd_pt2_9_3", null ]
      ] ],
      [ "Block types", "awd_specs_2.html#awd_pt2_10", null ],
      [ "Geometry blocks", "awd_specs_2.html#awd_pt2_11", [
        [ "TriangleGeometry (ID 1)", "awd_specs_2.html#awd_pt2_11_1", [
          [ "TriangleGeometry Properties", "awd_specs_2.html#specs_geometry_properties", null ],
          [ "Sub-geometry", "awd_specs_2.html#awd_pt2_11_1_1", null ],
          [ "Structure of a subgeometry in &AWD", "awd_specs_2.html#specs_subgeometry", null ],
          [ "SubGeometry Properties", "awd_specs_2.html#specs_subgeometry_properties", null ],
          [ "Data streams", "awd_specs_2.html#specs_datastreams", null ],
          [ "Data stream types", "awd_specs_2.html#specs_datastream_types", null ],
          [ "Content structure of 3D vertex positions, vertex normals, and vertex tangents data streams", "awd_specs_2.html#awd_pt2_11_1_3", null ],
          [ "Content structure of face index data streams", "awd_specs_2.html#awd_pt2_11_1_4", null ],
          [ "Content structure of UV coordinate and 2D vertex positions data streams", "awd_specs_2.html#awd_pt2_11_1_5", null ],
          [ "Content structure of Joint index and weight streams", "awd_specs_2.html#awd_pt2_11_1_6", null ],
          [ "Content structure of Combined 3D vertex position data streams with length 13", "awd_specs_2.html#awd_pt2_11_1_8", null ],
          [ "Content structure of Combined 2D vertex position data streams with length 9", "awd_specs_2.html#awd_pt2_11_1_7", null ]
        ] ],
        [ "PrimitiveGeometry (ID 11)", "awd_specs_2.html#awd_pt2_11_2", [
          [ "Primitive Types", "awd_specs_2.html#primitive_types", null ],
          [ "PlaneGeometry (Primitve-Type = 1)", "awd_specs_2.html#awd_pt2_11_2_1", null ],
          [ "CubeGeometry (Primitve-Type = 2)", "awd_specs_2.html#awd_pt2_11_2_2", null ],
          [ "SphereGeometry (Primitve-Type = 3)", "awd_specs_2.html#awd_pt2_11_2_3", null ],
          [ "CylinderGeometry (Primitve-Type = 4)", "awd_specs_2.html#awd_pt2_11_2_4", null ],
          [ "ConeGeometry (Primitve-Type = 5)", "awd_specs_2.html#awd_pt2_11_2_5", null ],
          [ "CapsuleGeometry (Primitve-Type = 6)", "awd_specs_2.html#awd_pt2_11_2_6", null ],
          [ "TorusGeometry (Primitve-Type = 7)", "awd_specs_2.html#awd_pt2_11_2_7", null ]
        ] ]
      ] ],
      [ "Scene object blocks", "awd_specs_2.html#awd_pt2_12", [
        [ "Scene Header", "awd_specs_2.html#awd_pt2_12_1", null ],
        [ "Scene (ID 21)", "awd_specs_2.html#awd_pt2_12_2", null ],
        [ "Container (ID 22)", "awd_specs_2.html#awd_pt2_12_3", [
          [ "Container Properties.", "awd_specs_2.html#container_properties", null ]
        ] ],
        [ "MeshInstance (ID 23)", "awd_specs_2.html#awd_pt2_12_4", null ],
        [ "Skybox (ID 31)", "awd_specs_2.html#awd_pt2_12_5", null ],
        [ "Light (ID 41)", "awd_specs_2.html#awd_pt2_12_6", null ],
        [ "Camera (ID 42)", "awd_specs_2.html#awd_pt2_12_7", null ],
        [ "TextureProjector (ID 43)", "awd_specs_2.html#awd_pt2_12_8", null ],
        [ "LightPicker (ID 51)", "awd_specs_2.html#awd_pt2_12_9", null ]
      ] ],
      [ "Material blocks", "awd_specs_2.html#awd_pt2_13", [
        [ "Material block (ID 81)", "awd_specs_2.html#awd_pt2_13_1", [
          [ "Material types", "awd_specs_2.html#awd_pt2_material_types", null ],
          [ "Material properties", "awd_specs_2.html#awd_pt2_material_properties", null ],
          [ "Shading Methods", "awd_specs_2.html#awd_pt2_material_shading_methods", null ]
        ] ],
        [ "BitmapTexture block (ID 82)", "awd_specs_2.html#awd_pt2_13_2", null ],
        [ "CubeTexture block (ID 83)", "awd_specs_2.html#awd_pt2_13_3", null ],
        [ "SharedMethod-Block(ID 91)", "awd_specs_2.html#awd_pt2_13_4", null ],
        [ "EffectMethods", "awd_specs_2.html#awd_pt2_13_5", null ],
        [ "EffectMethodsShadowMapMethod-Block (ID 92)", "awd_specs_2.html#awd_pt2_13_6", null ]
      ] ],
      [ "Animation blocks", "awd_specs_2.html#awd_pt2_14", [
        [ "Skeleton (ID 101)", "awd_specs_2.html#awd_pt2_14_1", [
          [ "Skeleton joint", "awd_specs_2.html#awd_pt2_14_1_1", null ]
        ] ],
        [ "SkeletonPose (ID 102)", "awd_specs_2.html#awd_pt2_14_2", [
          [ "Skeleton pose joint transform", "awd_specs_2.html#awd_pt2_14_2_1", null ]
        ] ],
        [ "SkeletonAnimation (ID 103)", "awd_specs_2.html#awd_pt2_14_3", [
          [ "Skeleton animation frames", "awd_specs_2.html#awd_pt2_14_3_1", null ]
        ] ],
        [ "MeshPose (ID 111) / MeshPoseAnimation (ID 112)", "awd_specs_2.html#awd_pt2_14_4", null ],
        [ "AnimationSet (ID 113)", "awd_specs_2.html#awd_pt2_14_5", null ],
        [ "Animator (ID 122)", "awd_specs_2.html#awd_pt2_14_6", [
          [ "Animator-Type", "awd_specs_2.html#awd_pt2_animator_type", null ],
          [ "UV Animation Frame", "awd_specs_2.html#awd_pt2_uvanim_frame", null ]
        ] ],
        [ "TimeLine", "awd_specs_2.html#awd_pt2_14_7", [
          [ "AWDTimeLineFrame", "awd_specs_2.html#awd_pt2_14_7_1", null ]
        ] ],
        [ "AWDFrameCommand", "awd_specs_2.html#awd_pt2_14_8", [
          [ "Remove Object Command", "awd_specs_2.html#awd_pt2_14_8_1", null ],
          [ "Add Sound Command", "awd_specs_2.html#awd_pt2_add_sound_command", null ],
          [ "Update Object Command", "awd_specs_2.html#awd_pt2_14_8_3", null ],
          [ "Update Object Command -  header", "awd_specs_2.html#awd_pt2_14_8_34", null ],
          [ "Update Object Command -  Body", "awd_specs_2.html#awd_pt2_14_8_35", null ]
        ] ]
      ] ],
      [ "Sound Blocks", "awd_specs_2.html#awd_pt2_15", [
        [ "Sound", "awd_specs_2.html#awd_pt2_15_1", null ]
      ] ],
      [ "Text BLocks", "awd_specs_2.html#awd_pt2_16", [
        [ "Font", "awd_specs_2.html#awd_pt2_16_1", [
          [ "FontStyle", "awd_specs_2.html#awd_pt2_font_style", null ],
          [ "FontShape", "awd_specs_2.html#awd_pt2_font_shape", null ]
        ] ],
        [ "Text-Format Block", "awd_specs_2.html#awd_pt2_16_2", [
          [ "Text-format Element", "awd_specs_2.html#awd_pt2_test_format", null ]
        ] ],
        [ "TextElement", "awd_specs_2.html#awd_pt2_16_3", [
          [ "Text-Paragraph", "awd_specs_2.html#awd_pt2_paragraph", null ],
          [ "TextRun", "awd_specs_2.html#awd_pt2_textrun", null ],
          [ "TextField-Properties", "awd_specs_2.html#awd_pt2_textfiel_propsh", null ]
        ] ],
        [ "Namespace blocks (ID 254)", "awd_specs_2.html#awd_pt2_15_2", null ],
        [ "Meta-data blocks (ID 255)", "awd_specs_2.html#awd_pt2_15_3", null ]
      ] ]
    ] ],
    [ "Part III: Using AWD", "awd_specs_3.html", [
      [ "Official AWD tools", "awd_specs_3.html#awd_pt3_1", [
        [ "AWD SDK", "awd_specs_3.html#awd_pt3_1_1", null ],
        [ "PyAWD - AWD for Python", "awd_specs_3.html#awd_pt3_2", null ],
        [ "Official importer/exporter implementations", "awd_specs_3.html#awd_pt3_1_3", null ]
      ] ],
      [ "AWD Limitations", "awd_specs_3.html#awd_pt3_3", null ],
      [ "AWD Structure examples", "awd_specs_3.html#awd_pt3_4", null ]
    ] ]
];