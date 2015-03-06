[HEADER]
	VERSION			00.00.00.00
	DESCRIPTION		Dental 
	COPYRIGHT		O R C A
[/HEADER]
 

 
[TEXTURES]
	FILE gum                                        gum.pvr                                     LINEAR-LINEAR-LINEAR
	FILE gum_NM                                     gum_normal.pvr                              LINEAR-LINEAR-LINEAR
    
	FILE lowerJawcrossSectionDiffuse				lowerJawcrossSectionDiffuse.pvr				LINEAR-LINEAR-LINEAR
	FILE lowerJawcrossSectionDiffuse_NM				lowerJawCrossSectionDiffuseNormals2.pvr     LINEAR-LINEAR-LINEAR
    
	FILE skull                                      skull.pvr                                   LINEAR-LINEAR-LINEAR
    FILE skull_NM                                   skull-Normals.pvr                           LINEAR-LINEAR-LINEAR
    
    FILE teethDiffuse                               teethDiffuse.pvr                            LINEAR-LINEAR-LINEAR
	FILE teethDiffuse_NM                            teethNormals.pvr                            LINEAR-LINEAR-LINEAR
    
	FILE teethMolarDiffuse                          teethMolarDiffuse.pvr                       LINEAR-LINEAR-LINEAR
	FILE teethMolarDiffuse_NM                       teethMolarsNormals.pvr                      LINEAR-LINEAR-LINEAR
    
	FILE upperJawCrossSectionDiffuse                upperJawCrossSectionDiffuse.pvr             LINEAR-LINEAR-LINEAR
    FILE upperJawCrossSectionDiffuse_NM             upperJawCrossSectionDiffuseNormals.pvr      LINEAR-LINEAR-LINEAR

[/TEXTURES]


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// vertex shaders
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

[VERTEXSHADER]
	NAME noBoneObjectSpaceVertShader
	[GLSL_CODE]
			attribute highp   vec3 inVertex;
			attribute mediump vec3 inNormal;
			attribute mediump vec2 inTexCoord;
			
			uniform highp   mat4 ViewProjMatrix;
			uniform mediump vec4 inLight0;
			uniform mediump vec4 inLight1;
			uniform mediump vec4 inLight2;
			uniform lowp	int	 LightCount;
			uniform mediump vec3 eyePos;
		
			varying mediump vec3 Light0;
			varying mediump vec3 Light1;
			varying mediump vec3 Light2;
			varying mediump vec2 TexCoord;
			varying mediump vec3 outNormal;
			varying mediump vec3 eyeVector;

			void main()
			{
				highp vec4 position = vec4(inVertex, 1.0);
					
				if(inLight0.w > 0.0){ 
					Light0 = normalize(inLight0.xyz - position.xyz); //Light0.w > 0 tells us that Light0 is the lights position
				}else{
					Light0 = inLight0.xyz;
				}
		
			
				if(inLight1.w > 0.0){ 
					Light1 = normalize(inLight1.xyz - position.xyz); //Light1.w > 0 tells us that Light1 is the lights position
				}else{
					Light1 = inLight1.xyz;
				}
			
				if(inLight2.w > 0.0){ 
					Light2 = normalize(inLight2.xyz - position.xyz); //Light1.w > 0 tells us that Light1 is the lights position
				}else{
					Light2 = inLight2.xyz;
				}

				gl_Position = ViewProjMatrix * position;
			
				TexCoord = inTexCoord;
				outNormal = inNormal;
				eyeVector = normalize(eyePos - position.xyz);
			}
	[/GLSL_CODE]
[/VERTEXSHADER]


[VERTEXSHADER]
	NAME boneVertShader
	[GLSL_CODE]
			attribute vec3 inVertex;
			attribute vec3 inNormal;
			attribute vec3 inTangent;
			attribute vec3 inBiNormal;
			attribute vec2 inTexCoord;
			attribute vec4 inBoneIndex;
			attribute vec4 inBoneWeights;
			
			uniform   mat4 ViewProjMatrix;
			uniform vec4 inLight0;
			uniform vec4 inLight1;
			uniform vec4 inLight2;
			uniform	   int	 LightCount;
			uniform vec3 eyePos;

			uniform	   int	 BoneCount;
			uniform   mat4 BoneMatrixArray[16];
			uniform   mat3 BoneMatrixArrayIT[16];
			
			varying vec3 Light0;
			varying vec3 Light1;
			varying vec3 Light2;
			varying vec2 TexCoord;
			varying vec3 outNormal;
			varying vec3 eyeVector;

			void main()
			{
				vec4 position = vec4(0.0);
				vec3 worldNormal =  inNormal;
				vec3 worldTangent = inTangent;
				vec3 worldBiNormal = inBiNormal;
				
				if(BoneCount > 0)
				{
			
					ivec4 boneIndex = ivec4(inBoneIndex);
					vec4 boneWeights = inBoneWeights;
				
					mat4 boneMatrix;
					mat3 normalMatrix;

					for (int i = 0; i < 4; i++)
					{
					if(i < BoneCount)
						{
							boneMatrix = BoneMatrixArray[boneIndex.x];
							normalMatrix = BoneMatrixArrayIT[boneIndex.x];
			
							position += boneMatrix * vec4(inVertex, 1.0) * boneWeights.x;
							worldNormal += normalMatrix * inNormal * boneWeights.x;
							

							worldTangent += normalMatrix * inTangent * boneWeights.x;
							worldBiNormal += normalMatrix * inBiNormal * boneWeights.x;

							//we 'rotate' the weights to avoid the [] operator
							boneWeights = boneWeights.yzwx;
							boneIndex = boneIndex.yzwx;
			
						
						}
					}		
				}else{
					
					position = vec4(inVertex, 1.0);
				}
				
					normalize(worldTangent);
					normalize(worldBiNormal);
					normalize(worldNormal);
					
					
				// lighting
				vec3 TmpLightDir; 
					
						if(inLight0.w > 0.0){ 
							TmpLightDir = normalize(inLight0.xyz - position.xyz); //Light0.w > 0 tells us that Light0 is the lights position
						}else{
							TmpLightDir = inLight0.xyz;
						}
							//light.xyz is a vector representing the direction to the light
							Light0.x = dot(worldTangent, TmpLightDir);
							Light0.y = dot(worldBiNormal, TmpLightDir);
							Light0.z = dot(worldNormal, TmpLightDir);
							//Light0 is now in tangentspace


				
					
						if(inLight1.w > 0.0){ 
							TmpLightDir = normalize(inLight1.xyz - position.xyz); //Light1.w > 0 tells us that Light1 is the lights position
						}else{
							TmpLightDir = inLight1.xyz;
						}
							//light.xyz is a vector representing the direction to the light
							Light1.x = dot(worldTangent, TmpLightDir);
							Light1.y = dot(worldBiNormal, TmpLightDir);
							Light1.z = dot(worldNormal, TmpLightDir);
							//Light1 is now in tangentspace


					
					
						if(inLight2.w > 0.0){ 
							TmpLightDir = normalize(inLight2.xyz - position.xyz); //Light1.w > 0 tells us that Light1 is the lights position
						}else{
							TmpLightDir = inLight2.xyz;
						}
							//light.xyz is a vector representing the direction to the light
							Light2.x = dot(worldTangent, TmpLightDir);
							Light2.y = dot(worldBiNormal, TmpLightDir);
							Light2.z = dot(worldNormal, TmpLightDir);
							//Light2 is now in tangentspace

				
				
				gl_Position = ViewProjMatrix * position;
			
				// Pass through texcoords
				TexCoord = inTexCoord;
				vec3  TmpOutNormal = inNormal;
							outNormal.x = dot(worldTangent, TmpOutNormal);
							outNormal.y = dot(worldBiNormal, TmpOutNormal);
							outNormal.z = dot(worldNormal, TmpOutNormal);
							//outNormal is now in tangentspace
							
				vec3  TmpEyeDir = normalize(eyePos - position.xyz);
							eyeVector.x = dot(worldTangent, TmpEyeDir);
							eyeVector.y = dot(worldBiNormal, TmpEyeDir);
							eyeVector.z = dot(worldNormal, TmpEyeDir);
							//eyeVector is now in tangentspace
			}
	[/GLSL_CODE]
[/VERTEXSHADER]


[VERTEXSHADER]
	NAME lameVertShader
	[GLSL_CODE]
	attribute vec4 inVertex;

	void main()
	{
		gl_Position = inVertex;
	}
	[/GLSL_CODE]
[/VERTEXSHADER]

 
 // vertex shader, use if there are no bones in the effect/mesh
[VERTEXSHADER]
	NAME noBoneTangentSpaceVertShader
	[GLSL_CODE]
			attribute highp   vec3 inVertex;
			attribute mediump vec3 inNormal;
			attribute mediump vec3 inTangent;
			attribute mediump vec3 inBiNormal;
			attribute mediump vec2 inTexCoord;
			
			uniform highp   mat4 ViewProjMatrix;
			uniform mediump vec4 inLight0;
			uniform mediump vec4 inLight1;
			uniform mediump vec4 inLight2;
			uniform lowp	int	 LightCount;
			uniform mediump vec3 eyePos;
		
			varying mediump vec3 Light0;
			varying mediump vec3 Light1;
			varying mediump vec3 Light2;
			varying mediump vec2 TexCoord;
			varying mediump vec3 outNormal;
			varying mediump vec3 eyeVector;

			void main()
			{
				highp vec4 position = vec4(inVertex, 1.0);
				highp vec3 worldNormal =  inNormal;
				highp vec3 worldTangent = inTangent;
				highp vec3 worldBiNormal = inBiNormal;
				normalize(worldTangent);
				normalize(worldBiNormal);
				normalize(worldNormal);


				// lighting
				mediump vec3 TmpLightDir; 
				if(inLight0.w > 0.0)
				{ 
					TmpLightDir = normalize(inLight0.xyz - position.xyz); //Light0.w > 0 tells us that Light0 is the lights position
				} else {
					TmpLightDir = inLight0.xyz;
				}
				//light.xyz is a vector representing the direction to the light
				Light0.x = dot(worldTangent, TmpLightDir);
				Light0.y = dot(worldBiNormal, TmpLightDir);
				Light0.z = dot(worldNormal, TmpLightDir);
				//Light0 is now in tangentspace
	
	
				if(inLight1.w > 0.0)
				{ 
					TmpLightDir = normalize(inLight1.xyz - position.xyz); //Light1.w > 0 tells us that Light1 is the lights position
				} else {
					TmpLightDir = inLight1.xyz;
				}
				//light.xyz is a vector representing the direction to the light
				Light1.x = dot(worldTangent, TmpLightDir);
				Light1.y = dot(worldBiNormal, TmpLightDir);
				Light1.z = dot(worldNormal, TmpLightDir);
				//Light1 is now in tangentspace
	
	
				if(inLight2.w > 0.0)
				{ 
					TmpLightDir = normalize(inLight2.xyz - position.xyz); //Light1.w > 0 tells us that Light1 is the lights position
				} else {
					TmpLightDir = inLight2.xyz;
				}
				//light.xyz is a vector representing the direction to the light
				Light2.x = dot(worldTangent, TmpLightDir);
				Light2.y = dot(worldBiNormal, TmpLightDir);
				Light2.z = dot(worldNormal, TmpLightDir);
				//Light2 is now in tangentspace
	
	
				gl_Position = ViewProjMatrix * position;
	
				// Pass through texcoords
				TexCoord = inTexCoord;
				mediump vec3  TmpOutNormal = inNormal;
				outNormal.x = dot(worldTangent, TmpOutNormal);
				outNormal.y = dot(worldBiNormal, TmpOutNormal);
				outNormal.z = dot(worldNormal, TmpOutNormal);
				//outNormal is now in tangentspace
	
				//outNormal = worldBiNormal;
	
				mediump vec3  TmpEyeDir = normalize(eyePos - position.xyz);
				eyeVector.x = dot(worldTangent, TmpEyeDir);
				eyeVector.y = dot(worldBiNormal, TmpEyeDir);
				eyeVector.z = dot(worldNormal, TmpEyeDir);
				//eyeVector is now in tangentspace
			}
	[/GLSL_CODE]
[/VERTEXSHADER]

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// fragment shaders
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

[FRAGMENTSHADER]
NAME textureMapNoNormalMapFragShader
[GLSL_CODE]
	const lowp float cf_maxPhong = 1024.0;

	uniform sampler2D sTexture;
	uniform mediump vec3 effectState;
	uniform highp vec3 specularMaterial;
	uniform highp float shininess;
	uniform mediump vec3 lightColor0;
	uniform mediump vec3 lightColor1;
	uniform mediump vec3 lightColor2;
    uniform mediump vec3 ambientMaterial;
	
	varying mediump vec3 outNormal;
	varying mediump vec2 TexCoord;
	varying mediump vec3 Light0;
	varying mediump vec3 Light1;
	varying mediump vec3 Light2;
	varying mediump vec3 eyeVector;
 
	void main()
	{
		highp vec3 N= normalize(outNormal);			
		highp vec3 L0 = normalize( Light0 );//in the pod this was the key light
		highp vec3 L1 = normalize( Light1 );
		highp vec3 L2 = normalize( Light2 );
		highp vec3 E = normalize(eyeVector);
		highp vec3 H0 = normalize(L0 + E);
		highp vec3 H1 = normalize(L1 + E);
		highp vec3 H2 = normalize(L2 + E);
		highp float sf0 = pow(max(0.0, dot(N, H0)), shininess * cf_maxPhong);
		highp float sf1 = pow(max(0.0, dot(N, H1)), shininess * cf_maxPhong);
	//	highp float sf2 = pow(max(0.0, dot(N, H2)), shininess * cf_maxPhong);
		highp vec4 color = vec4(0.0);

		highp vec4 combinedLightColor = vec4(lightColor0 * 0.2,1.0);
		combinedLightColor.xyz += (max(0.0, dot(N, L0)) * lightColor0) + (sf0 * specularMaterial);
	//	combinedLightColor.xyz += max(0.0, dot(N, L1)) * lightColor1;
		combinedLightColor.xyz += (max(0.0, dot(N, L1)) * lightColor1) + (sf1 * specularMaterial);
		combinedLightColor.xyz += max(0.0, dot(N, L2)) * lightColor2;

		highp vec4 sampleColor = texture2D(sTexture, TexCoord);
		color = combinedLightColor * sampleColor;
        color.xyz += ambientMaterial;
	//	color.xyz += sf * 0.5 *  specularMaterial;
		gl_FragColor = color;
	}
[/GLSL_CODE]
[/FRAGMENTSHADER]


[FRAGMENTSHADER]
NAME textureMapAndNormalMapFragShaderOLD
[GLSL_CODE]
	const int c_zero = 1;
	const int c_one = 1;
	const lowp float cf_highlightFactor = 2.0;
	const lowp float cf_maxPhong = 1024.0;
    const lowp float cf_dimFactor = 0.2;

	uniform sampler2D sTexture;
	uniform sampler2D sNormalMap;
	uniform mediump vec3 effectState;
	uniform highp vec3 specularMaterial;
	uniform highp float shininess;
	uniform mediump vec3 lightColor0;
	uniform mediump vec3 lightColor1;
	uniform mediump vec3 lightColor2;
    uniform mediump vec3 ambientMaterial;
	
	uniform lowp int isHighlighted;

	varying mediump vec2 TexCoord;
	varying mediump vec3 Light0;
	varying mediump vec3 Light1;
	varying mediump vec3 Light2;
	varying mediump vec3 eyeVector;

	void main()
	{
		highp vec3 N = texture2D(sNormalMap, TexCoord).xyz * 2.0 - 1.0;
		N = normalize(N);			
		highp vec3 L0 = normalize( Light0 );//in the pod this was the key light
		highp vec3 L1 = normalize( Light1 );
		highp vec3 L2 = normalize( Light2 );
		highp vec3 E = normalize(eyeVector);
		highp vec3 H0 = normalize(L0 + E);
		highp vec3 H1 = normalize(L1 + E);
		highp vec3 H2 = normalize(L2 + E);
		
        //shininess *= cf_dimFactor;
        
		highp float sf0 = pow(max(0.0, dot(N, H0)), shininess * cf_maxPhong);
		highp float sf1 = pow(max(0.0, dot(N, H1)), shininess * cf_maxPhong);
	//	highp float sf2 = pow(max(0.0, dot(N, H2)), shininess * cf_maxPhong);
		highp vec4 color = vec4(0.0);

		highp vec4 combinedLightColor = vec4(0,0,0,1);//vec4(lightColor0 * 0.2,1.0);//
		combinedLightColor.xyz += (max(0.0, dot(N, L0 )) * lightColor0) + (sf0 * specularMaterial * cf_dimFactor);
		combinedLightColor.xyz += (max(0.0, dot(N, L1)) * lightColor1) + (sf1 * specularMaterial * cf_dimFactor);
		combinedLightColor.xyz += (max(0.0, dot(N, L2 )) * lightColor2);// + (sf2 * specularMaterial);

		highp vec4 sampleColor = texture2D(sTexture, TexCoord);//using mip map
		color = combinedLightColor * sampleColor;
        color.xyz += ambientMaterial;
        
		if(isHighlighted == c_one){
			color *= cf_highlightFactor;
		}

		gl_FragColor = color;
	}
[/GLSL_CODE]
[/FRAGMENTSHADER]

[FRAGMENTSHADER]
NAME textureMapAndNormalMapFragShader
[GLSL_CODE]
	const int c_zero = 0;
	const int c_one = 1;
	const lowp float cf_highlightFactor = 2.0;
	//const lowp float cf_maxPhong = 1024.0;

	uniform sampler2D sTexture;
	uniform sampler2D sNormalMap;
	uniform mediump vec3 effectState;
	uniform highp vec3 specularMaterial;
	uniform highp float shininess;
	uniform mediump vec3 lightColor0;
	uniform mediump vec3 lightColor1;
	uniform mediump vec3 lightColor2;
    uniform highp vec3 diffuseMaterial;
	uniform mediump vec3 ambientMaterial;

	uniform lowp int isHighlighted;

	varying mediump vec2 TexCoord;
	varying mediump vec3 Light0;
	varying mediump vec3 Light1;
	varying mediump vec3 Light2;
	varying mediump vec3 eyeVector;

	void main()
	{
		highp vec3 N = texture2D(sNormalMap, TexCoord).xyz * 2.0 - 1.0;
		N = normalize(N);			
		highp vec3 L0 = normalize( Light0 );//in the pod this was the key light
		highp vec3 L1 = normalize( Light1 );
		highp vec3 L2 = normalize( Light2 );
		highp vec3 E = normalize(eyeVector);
		highp vec3 H0 = normalize(L0 + E);
		highp vec3 H1 = normalize(L1 + E);
		highp vec3 H2 = normalize(L2 + E);
		highp float sf0 = pow(max(0.0, dot(N, H0)), shininess);// * cf_maxPhong);
		highp float sf1 = pow(max(0.0, dot(N, H1)), shininess);// * cf_maxPhong);
	//	highp float sf2 = pow(max(0.0, dot(N, H2)), shininess);// * cf_maxPhong);
		highp vec4 color = vec4(0.0);

		highp vec4 combinedLightColor = vec4(0,0,0,1);//vec4(lightColor0 * 0.2,1.0);//
		combinedLightColor.xyz += (max(0.0, dot(N, L0 )) * lightColor0) + (sf0 * specularMaterial);
		combinedLightColor.xyz += (max(0.0, dot(N, L1)) * lightColor1) + (sf1 * specularMaterial);
		combinedLightColor.xyz += (max(0.0, dot(N, L2 )) * lightColor2);// + (sf2 * specularMaterial);

		highp vec4 sampleColor = texture2D(sTexture, TexCoord);//using mip map
		color = combinedLightColor * sampleColor;

        color.xyz += ambientMaterial;
    
		if(isHighlighted > c_zero){
			color *= cf_highlightFactor;
		}

		//color.xyz = vec3(0.,1.,0.);

		gl_FragColor = color;
	}
[/GLSL_CODE]
[/FRAGMENTSHADER]


[FRAGMENTSHADER]
NAME textureMapAndNormalMapAndHighlightMapFragShader
[GLSL_CODE]
	const int c_one = 1;
	const int c_two = 2;
	const lowp float cf_one = 1.0;
	const lowp float cf_maxPhong = 1024.0;

	uniform sampler2D sTexture;
	uniform sampler2D sNormalMap;
	uniform sampler2D sHighlightMap;
	uniform mediump vec3 effectState;
	uniform highp vec3 specularMaterial;
	uniform highp float shininess;
	uniform mediump vec3 lightColor0;
	uniform mediump vec3 lightColor1;
	uniform mediump vec3 lightColor2;
    uniform mediump vec3 ambientMaterial;
	
	uniform lowp int isHighlighted;

	varying mediump vec2 TexCoord;
	varying mediump vec3 Light0;
	varying mediump vec3 Light1;
	varying mediump vec3 Light2;
	varying mediump vec3 eyeVector;

	void main()
	{
		highp vec3 N = texture2D(sNormalMap, TexCoord).xyz * 2.0 - 1.0;
		N = normalize(N);			
		highp vec3 L0 = normalize( Light0 );//in the pod this was the key light
		highp vec3 L1 = normalize( Light1 );
		highp vec3 L2 = normalize( Light2 );
		highp vec3 E = normalize(eyeVector);
		highp vec3 H0 = normalize(L0 + E);
		highp vec3 H1 = normalize(L1 + E);
		highp vec3 H2 = normalize(L2 + E);
		highp float sf0 = pow(max(0.0, dot(N, H0)), shininess * cf_maxPhong);
		highp float sf1 = pow(max(0.0, dot(N, H1)), shininess * cf_maxPhong);
	//	highp float sf2 = pow(max(0.0, dot(N, H2)), shininess * cf_maxPhong);
		highp vec4 color = vec4(0.0);

		highp vec4 combinedLightColor = vec4(0,0,0,1);//vec4(lightColor0 * 0.2,1.0);
		combinedLightColor.xyz += (max(0.0, dot(N, L0)) * lightColor0) + (sf0 * specularMaterial) ;
		combinedLightColor.xyz += (max(0.0, dot(N, L1)) * lightColor1) + (sf1 * specularMaterial);
		combinedLightColor.xyz += max(0.0, dot(N, L2)) * lightColor2;

		highp vec4 sampleColor = texture2D(sTexture, TexCoord);//using mip map
		color = combinedLightColor * sampleColor;
        color.xyz += ambientMaterial;
        
		if(isHighlighted == c_one || isHighlighted == c_two){
			sampleColor = texture2D(sHighlightMap, TexCoord);//using mip map
			if(isHighlighted == c_two){
				sampleColor.x = cf_one - sampleColor.x;
			}

			color *= cf_one + sampleColor.x;
		}

		gl_FragColor = color;
	}
[/GLSL_CODE]
[/FRAGMENTSHADER]


[FRAGMENTSHADER]
NAME noTextureMapNormalMapFragShader
[GLSL_CODE]
	const lowp float cf_maxPhong = 1024.0;

	uniform sampler2D sTexture;
	uniform sampler2D sNormalMap;
	uniform mediump vec3 effectState;
	uniform highp vec3 diffuseMaterial;
	uniform highp vec3 specularMaterial;
	uniform highp float shininess;
	uniform mediump vec3 lightColor0;
	uniform mediump vec3 lightColor1;
	uniform mediump vec3 lightColor2;
    uniform mediump vec3 ambientMaterial;
	
	varying mediump vec2 TexCoord;
	varying mediump vec3 Light0;
	varying mediump vec3 Light1;
	varying mediump vec3 Light2;
	varying mediump vec3 eyeVector;

	void main()
	{
		highp vec3 N = texture2D(sNormalMap, TexCoord).xyz * 2.0 - 1.0;
		N = normalize(N);

		highp vec3 L0= normalize( Light0 );//in the pod this was the key light
		highp vec3 L1 = normalize( Light1 );
		highp vec3 L2 = normalize( Light2 );
		highp vec3 E = normalize(eyeVector);
		highp vec3 H0 = normalize(L0 + E);
		highp vec3 H1 = normalize(L1 + E);
		highp vec3 H2 = normalize(L2 + E);
		highp float sf0 = pow(max(0.0, dot(N, H0)), shininess * cf_maxPhong);
		highp float sf1 = pow(max(0.0, dot(N, H1)), shininess * cf_maxPhong);
	//	highp float sf2 = pow(max(0.0, dot(N, H2)), shininess * cf_maxPhong);
		highp vec4 color = vec4(0.0);

		highp vec4 combinedLightColor = vec4(0,0,0,1);//vec4(lightColor0 * 0.2,1.0);
		combinedLightColor.xyz += (max(0.0, dot(N, L0)) * lightColor0) + (sf0 * specularMaterial);
		combinedLightColor.xyz += (max(0.0, dot(N, L1)) * lightColor1) + (sf1 * specularMaterial);
		combinedLightColor.xyz += max(0.0, dot(N, L2)) * lightColor2;

		color.xyz = diffuseMaterial * combinedLightColor.xyz;
        color.xyz += ambientMaterial;
		color.w = 1.0;
        
		gl_FragColor = color;
	}
[/GLSL_CODE]
[/FRAGMENTSHADER]
 

[FRAGMENTSHADER]
NAME textureMapAndNormalMapAndSpecularMapFragShader
[GLSL_CODE]
	const int c_zero = 1;
	const int c_one = 1;
	const lowp float cf_highlightFactor = 0.25;
	const lowp float cf_maxPhong = 1024.0;

	uniform sampler2D sTexture;
	uniform sampler2D sNormalMap;
	uniform sampler2D sSpecularMap;																	// added this line
	uniform mediump vec3 effectState;
	uniform highp vec3 specularMaterial;
	uniform highp float shininess;
	uniform mediump vec3 lightColor0;
	uniform mediump vec3 lightColor1;
	uniform mediump vec3 lightColor2;
    uniform mediump vec3 ambientMaterial;
	
	varying mediump vec2 TexCoord;
	varying mediump vec3 Light0;
	varying mediump vec3 Light1;
	varying mediump vec3 Light2;
	varying mediump vec3 eyeVector;

	void main()
	{
		highp vec3 N = texture2D(sNormalMap, TexCoord).xyz * 2.0 - 1.0;
		
		N = normalize(N);			
		
		highp vec3 L0 = normalize( Light0 );//in the pod this was the key light
		highp vec3 L1 = normalize( Light1 );
		highp vec3 L2 = normalize( Light2 );		
		
		highp vec3 E = normalize(eyeVector);				
		highp vec3 H0 = normalize(L0 + E);
		highp vec3 H1 = normalize(L1 + E);
		highp vec3 H2 = normalize(L2 + E);
		
		highp vec4 specularMap = texture2D(sSpecularMap, TexCoord);									// added this line
		highp float shiny = specularMap.x;															// added this line
				
		highp float sf0 = pow(max(0.0, dot(N, H0)), shiny * cf_maxPhong);							// changed shininess to shiny
		highp float sf1 = pow(max(0.0, dot(N, H1)), shiny * cf_maxPhong);							// changed shininess to shiny 
		highp float sf2 = pow(max(0.0, dot(N, H2)), shiny * cf_maxPhong);							// changed shininess to shiny
		
		highp vec4 color = vec4(0.0);

		highp vec4 combinedLightColor = vec4(0,0,0,1);
		combinedLightColor.xyz += (max(0.0, dot(N, L0 )) * lightColor0) + (sf0 * specularMap.x);
		combinedLightColor.xyz += (max(0.0, dot(N, L1 )) * lightColor1) + (sf1 * specularMap.y);
		combinedLightColor.xyz += (max(0.0, dot(N, L2 )) * lightColor2) + (sf2 * specularMap.z);

		highp vec4 sampleColor = texture2D(sTexture, TexCoord);//using mip map
				
		color = combinedLightColor * sampleColor;
        color.xyz += ambientMaterial;
		
		gl_FragColor = color;
	}
[/GLSL_CODE]
[/FRAGMENTSHADER]


[FRAGMENTSHADER]
NAME noTexturesFragShader
[GLSL_CODE]
	const lowp float cf_maxPhong = 1024.0;

	uniform mediump vec3 effectState;
	uniform highp vec3 diffuseMaterial;
	uniform highp vec3 specularMaterial;
	uniform highp float shininess;
	uniform highp float opacity;
	uniform mediump vec3 lightColor0;
	uniform mediump vec3 lightColor1;
	uniform mediump vec3 lightColor2;	
	uniform mediump vec3 ambientMaterial;
	
	varying mediump vec3 outNormal;
	varying mediump vec3 Light0;
	varying mediump vec3 Light1;
	varying mediump vec3 Light2;
	varying mediump vec3 eyeVector;
	
	void main()
	{
		highp vec3 N= normalize(outNormal);			
		highp vec3 L0= normalize( Light0 );//in the pod this was the key light
		highp vec3 L1 = normalize( Light1 );
		highp vec3 L2 = normalize( Light2 );		
		highp vec3 E = normalize(eyeVector);
		highp vec3 H0 = normalize(L0 + E);
		highp vec3 H1 = normalize(L1 + E);
		highp vec3 H2 = normalize(L2 + E);
		highp float sf0 = pow(max(0.0, dot(N, H0)), shininess * cf_maxPhong);
		highp float sf1 = pow(max(0.0, dot(N, H1)), shininess * cf_maxPhong);
	//	highp float sf2 = pow(max(0.0, dot(N, H2)), shininess * cf_maxPhong);
		highp vec4 color = vec4(0.0);

		highp vec4 combinedLightColor = vec4(lightColor0 * 0.2,1.0);
		combinedLightColor.xyz += (max(0.0, dot(N, L0)) * lightColor0) + (sf0 * specularMaterial);
		combinedLightColor.xyz += (max(0.0, dot(N, L1)) * lightColor1) + (sf1 * specularMaterial);
		combinedLightColor.xyz += max(0.0, dot(N, L2)) * lightColor2;

		color.xyz = diffuseMaterial * combinedLightColor.xyz;
        color.xyz += ambientMaterial;
		color.w = opacity;

		gl_FragColor = color;
	}
[/GLSL_CODE]
[/FRAGMENTSHADER]


[FRAGMENTSHADER]
NAME annotationsFragShader
[GLSL_CODE]
	uniform sampler2D sTexture;
	varying mediump vec2 TexCoord;

	void main()
	{
		gl_FragColor = texture2D(sTexture, TexCoord) ;
	}
[/GLSL_CODE]
[/FRAGMENTSHADER]


// fragment shader, for use when picking/selecting a specific mesh.
// store the mesh nodeIndex as the color, then retrieve the color value and convert back to the nodeIndex
[FRAGMENTSHADER]
NAME pickingFragShader
[GLSL_CODE]
	uniform highp vec4 nodeIndex;
 
	void main()
	{
		gl_FragColor =  nodeIndex; 
	}
[/GLSL_CODE]
[/FRAGMENTSHADER]

[FRAGMENTSHADER]
NAME testFragShader
[GLSL_CODE]
    uniform highp vec4 nodeIndex;

    void main()
    {
        gl_FragColor =  vec4(0., 1.0, 0., 1.0);
    }
[/GLSL_CODE]
[/FRAGMENTSHADER]


[FRAGMENTSHADER]
NAME pinFragShader
[GLSL_CODE]
	const lowp float cf_maxPhong = 1024.0;
    const int c_one = 1;

	uniform mediump vec3 effectState;
	uniform highp vec3 diffuseMaterial;
	uniform highp vec3 ambientMaterial;
	uniform highp vec3 specularMaterial;
	uniform highp float shininess;
	uniform highp float opacity;
	uniform mediump vec3 lightColor0;
	uniform mediump vec3 lightColor1;
	uniform mediump vec3 lightColor2;	
    
    uniform lowp int isHighlighted;
	
	varying mediump vec3 outNormal;
	varying mediump vec3 Light0;
	varying mediump vec3 Light1;
	varying mediump vec3 Light2;
	varying mediump vec3 eyeVector;
	
	void main()
	{
		highp vec3 N= normalize(outNormal);			
		highp vec3 L0= normalize( Light0 );//in the pod this was the key light
		highp vec3 L1 = normalize( Light1 );
		highp vec3 L2 = normalize( Light2 );		
		highp vec3 E = normalize(eyeVector);
		highp vec3 H0 = normalize(L0 + E);
		highp vec3 H1 = normalize(L1 + E);
		highp vec3 H2 = normalize(L2 + E);
		highp float sf0 = pow(max(0.0, dot(N, H0)), shininess * cf_maxPhong);
		highp float sf1 = pow(max(0.0, dot(N, H1)), shininess * cf_maxPhong);
	//	highp float sf2 = pow(max(0.0, dot(N, H2)), shininess * cf_maxPhong);
		highp vec4 color = vec4(0.0);

        highp vec4 combinedLightColor = vec4(0,0,0,1);
		//highp vec4 combinedLightColor = vec4(lightColor1 * 0.2,1.0);
		combinedLightColor.xyz += (max(0.0, dot(N, L0)) * lightColor0) + (sf0 * specularMaterial);
		combinedLightColor.xyz += (max(0.0, dot(N, L1)) * lightColor1) + (sf1 * specularMaterial);
		combinedLightColor.xyz += max(0.0, dot(N, L2)) * lightColor2;

        highp vec3 pinColor = diffuseMaterial;
//        if(isHighlighted == c_one)
//        {
//			pinColor += vec3(0.1,0.1,0.1);
//		}

		color.xyz = pinColor * combinedLightColor.xyz;
        color.xyz += ambientMaterial;
		color.w = opacity;

		gl_FragColor = color;
	}
[/GLSL_CODE]
[/FRAGMENTSHADER]

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// effects
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

[EFFECT]
	NAME noTexturesEffect

	ATTRIBUTE	inVertex					POSITION
	ATTRIBUTE	inNormal					NORMAL
	ATTRIBUTE	inTexCoord					UV0
	
	UNIFORM 	ambientMaterial				MATERIALCOLORAMBIENT
	UNIFORM		diffuseMaterial				MATERIALCOLORDIFFUSE	
	UNIFORM		specularMaterial			MATERIALCOLORSPECULAR
	UNIFORM		shininess					MATERIALSHININESS
	UNIFORM		opacity						MATERIALOPACITY

	UNIFORM		eyePos						EYEPOSMODEL
	UNIFORM		ViewProjMatrix				WORLDVIEWPROJECTION
	
	UNIFORM		inLight0					LIGHTPOSMODEL0
	UNIFORM		inLight1					LIGHTPOSMODEL1
	UNIFORM		inLight2					LIGHTPOSMODEL2
	UNIFORM		lightColor0					LIGHTCOLOR0
	UNIFORM		lightColor1					LIGHTCOLOR1
	UNIFORM		lightColor2					LIGHTCOLOR2
	
	VERTEXSHADER 	noBoneObjectSpaceVertShader
	FRAGMENTSHADER	noTexturesFragShader
[/EFFECT]

[EFFECT]
	NAME pickingEffect
	
  	ATTRIBUTE		inVertex			POSITION
	ATTRIBUTE		inNormal			NORMAL
	ATTRIBUTE		inTexCoord			UV0

	UNIFORM			ViewProjMatrix		WORLDVIEWPROJECTION
	
	UNIFORM			nodeIndex			NODEINDEX
	
	VERTEXSHADER	noBoneObjectSpaceVertShader
 	FRAGMENTSHADER	pickingFragShader
[/EFFECT]

[EFFECT]
	NAME pin_annotation_importME:pinBase_annotation_Mat

	ATTRIBUTE	inVertex					POSITION
	ATTRIBUTE	inNormal					NORMAL
	ATTRIBUTE	inTexCoord					UV0
	ATTRIBUTE	inTangent					TANGENT
 	ATTRIBUTE	inBiNormal					BINORMAL
	
	UNIFORM 	ambientMaterial				MATERIALCOLORAMBIENT
	UNIFORM		diffuseMaterial				MATERIALCOLORDIFFUSE	
	UNIFORM		specularMaterial			MATERIALCOLORSPECULAR
	UNIFORM		shininess					MATERIALSHININESS
	UNIFORM		opacity						MATERIALOPACITY

	UNIFORM		eyePos						EYEPOSMODEL
	UNIFORM		ViewProjMatrix				WORLDVIEWPROJECTION
	
	UNIFORM		inLight0					LIGHTPOSMODEL0
	UNIFORM		inLight1					LIGHTPOSMODEL1
	UNIFORM		inLight2					LIGHTPOSMODEL2
	UNIFORM		lightColor0					LIGHTCOLOR0
	UNIFORM		lightColor1					LIGHTCOLOR1
	UNIFORM		lightColor2					LIGHTCOLOR2
    
    UNIFORM		isHighlighted				IS_HIGHLIGHTED
	
	VERTEXSHADER 	noBoneTangentSpaceVertShader
    FRAGMENTSHADER	pinFragShader
[/EFFECT]

[EFFECT]
	NAME pin_annotation_importME:pinHead_annotation_Mat

	ATTRIBUTE	inVertex					POSITION
	ATTRIBUTE	inNormal					NORMAL
	ATTRIBUTE	inTexCoord					UV0
	ATTRIBUTE	inTangent					TANGENT
 	ATTRIBUTE	inBiNormal					BINORMAL
	
	UNIFORM 	ambientMaterial				MATERIALCOLORAMBIENT
	UNIFORM		diffuseMaterial				MATERIALCOLORDIFFUSE	
	UNIFORM		specularMaterial			MATERIALCOLORSPECULAR
	UNIFORM		shininess					MATERIALSHININESS
	UNIFORM		opacity						MATERIALOPACITY

	UNIFORM		eyePos						EYEPOSMODEL
	UNIFORM		ViewProjMatrix				WORLDVIEWPROJECTION
	
	UNIFORM		inLight0					LIGHTPOSMODEL0
	UNIFORM		inLight1					LIGHTPOSMODEL1
	UNIFORM		inLight2					LIGHTPOSMODEL2
	UNIFORM		lightColor0					LIGHTCOLOR0
	UNIFORM		lightColor1					LIGHTCOLOR1
	UNIFORM		lightColor2					LIGHTCOLOR2
    
    UNIFORM		isHighlighted				IS_HIGHLIGHTED
	
	VERTEXSHADER 	noBoneTangentSpaceVertShader
    FRAGMENTSHADER	pinFragShader
[/EFFECT]

[EFFECT]
	NAME gum

	ATTRIBUTE	inVertex					POSITION
	ATTRIBUTE	inNormal					NORMAL
 	ATTRIBUTE	inTangent					TANGENT
 	ATTRIBUTE	inBiNormal					BINORMAL
	ATTRIBUTE	inTexCoord					UV0

	UNIFORM 	ambientMaterial				MATERIALCOLORAMBIENT
	UNIFORM		diffuseMaterial				MATERIALCOLORDIFFUSE	
	UNIFORM		specularMaterial			MATERIALCOLORSPECULAR
	UNIFORM		shininess					MATERIALSHININESS
		
	UNIFORM		eyePos						EYEPOSMODEL
	UNIFORM		ViewProjMatrix				WORLDVIEWPROJECTION
	
	UNIFORM		inLight0					LIGHTPOSMODEL0
	UNIFORM		inLight1					LIGHTPOSMODEL1
	UNIFORM		inLight2					LIGHTPOSMODEL2
	UNIFORM		lightColor0					LIGHTCOLOR0
	UNIFORM		lightColor1					LIGHTCOLOR1
	UNIFORM		lightColor2					LIGHTCOLOR2

	UNIFORM		isHighlighted				IS_HIGHLIGHTED
	
	UNIFORM		sTexture					TEXTURE0
	//UNIFORM		sNormalMap					TEXTURE1

	VERTEXSHADER	noBoneTangentSpaceVertShader
	FRAGMENTSHADER	textureMapNoNormalMapFragShader
	
	TEXTURE 0 gum
	//TEXTURE 1 gum_NM
[/EFFECT]

[EFFECT]
	NAME lowerJawcrossSectionDiffuse

	ATTRIBUTE	inVertex					POSITION
	ATTRIBUTE	inNormal					NORMAL
 	ATTRIBUTE	inTangent					TANGENT
 	ATTRIBUTE	inBiNormal					BINORMAL
	ATTRIBUTE	inTexCoord					UV0

	UNIFORM 	ambientMaterial				MATERIALCOLORAMBIENT
	UNIFORM		diffuseMaterial				MATERIALCOLORDIFFUSE	
	UNIFORM		specularMaterial			MATERIALCOLORSPECULAR
	UNIFORM		shininess					MATERIALSHININESS
		
	UNIFORM		eyePos						EYEPOSMODEL
	UNIFORM		ViewProjMatrix				WORLDVIEWPROJECTION
	
	UNIFORM		inLight0					LIGHTPOSMODEL0
	UNIFORM		inLight1					LIGHTPOSMODEL1
	UNIFORM		inLight2					LIGHTPOSMODEL2
	UNIFORM		lightColor0					LIGHTCOLOR0
	UNIFORM		lightColor1					LIGHTCOLOR1
	UNIFORM		lightColor2					LIGHTCOLOR2

	UNIFORM		isHighlighted				IS_HIGHLIGHTED
	
	UNIFORM		sTexture					TEXTURE0
	//UNIFORM		sNormalMap					TEXTURE1

	VERTEXSHADER	noBoneTangentSpaceVertShader
	FRAGMENTSHADER	textureMapNoNormalMapFragShader
	
	TEXTURE 0 lowerJawcrossSectionDiffuse
	//TEXTURE 1 lowerJawcrossSectionDiffuse_NM
[/EFFECT]

[EFFECT]
	NAME skull

	ATTRIBUTE	inVertex					POSITION
	ATTRIBUTE	inNormal					NORMAL
 	ATTRIBUTE	inTangent					TANGENT
 	ATTRIBUTE	inBiNormal					BINORMAL
	ATTRIBUTE	inTexCoord					UV0

	UNIFORM 	ambientMaterial				MATERIALCOLORAMBIENT
	UNIFORM		diffuseMaterial				MATERIALCOLORDIFFUSE	
	UNIFORM		specularMaterial			MATERIALCOLORSPECULAR
	UNIFORM		shininess					MATERIALSHININESS
		
	UNIFORM		eyePos						EYEPOSMODEL
	UNIFORM		ViewProjMatrix				WORLDVIEWPROJECTION
	
	UNIFORM		inLight0					LIGHTPOSMODEL0
	UNIFORM		inLight1					LIGHTPOSMODEL1
	UNIFORM		inLight2					LIGHTPOSMODEL2
	UNIFORM		lightColor0					LIGHTCOLOR0
	UNIFORM		lightColor1					LIGHTCOLOR1
	UNIFORM		lightColor2					LIGHTCOLOR2

	UNIFORM		isHighlighted				IS_HIGHLIGHTED
	
	UNIFORM		sTexture					TEXTURE0
	//UNIFORM		sNormalMap					TEXTURE1

	VERTEXSHADER	noBoneTangentSpaceVertShader
	FRAGMENTSHADER	textureMapNoNormalMapFragShader
	
	TEXTURE 0 skull
	//TEXTURE 1 skull_NM
[/EFFECT]

[EFFECT]
	NAME teethDiffuse

	ATTRIBUTE	inVertex					POSITION
	ATTRIBUTE	inNormal					NORMAL
 	ATTRIBUTE	inTangent					TANGENT
 	ATTRIBUTE	inBiNormal					BINORMAL
	ATTRIBUTE	inTexCoord					UV0

	UNIFORM 	ambientMaterial				MATERIALCOLORAMBIENT
	UNIFORM		diffuseMaterial				MATERIALCOLORDIFFUSE	
	UNIFORM		specularMaterial			MATERIALCOLORSPECULAR
	UNIFORM		shininess					MATERIALSHININESS
		
	UNIFORM		eyePos						EYEPOSMODEL
	UNIFORM		ViewProjMatrix				WORLDVIEWPROJECTION
	
	UNIFORM		inLight0					LIGHTPOSMODEL0
	UNIFORM		inLight1					LIGHTPOSMODEL1
	UNIFORM		inLight2					LIGHTPOSMODEL2
	UNIFORM		lightColor0					LIGHTCOLOR0
	UNIFORM		lightColor1					LIGHTCOLOR1
	UNIFORM		lightColor2					LIGHTCOLOR2

	UNIFORM		isHighlighted				IS_HIGHLIGHTED
	
	UNIFORM		sTexture					TEXTURE0
	//UNIFORM		sNormalMap					TEXTURE1

	VERTEXSHADER	noBoneTangentSpaceVertShader
	FRAGMENTSHADER	textureMapNoNormalMapFragShader
	
	TEXTURE 0 teethDiffuse
	//TEXTURE 1 teethDiffuse_NM
[/EFFECT]

[EFFECT]
	NAME teethMolarDiffuse

	ATTRIBUTE	inVertex					POSITION
	ATTRIBUTE	inNormal					NORMAL
 	ATTRIBUTE	inTangent					TANGENT
 	ATTRIBUTE	inBiNormal					BINORMAL
	ATTRIBUTE	inTexCoord					UV0

	UNIFORM 	ambientMaterial				MATERIALCOLORAMBIENT
	UNIFORM		diffuseMaterial				MATERIALCOLORDIFFUSE	
	UNIFORM		specularMaterial			MATERIALCOLORSPECULAR
	UNIFORM		shininess					MATERIALSHININESS
		
	UNIFORM		eyePos						EYEPOSMODEL
	UNIFORM		ViewProjMatrix				WORLDVIEWPROJECTION
	
	UNIFORM		inLight0					LIGHTPOSMODEL0
	UNIFORM		inLight1					LIGHTPOSMODEL1
	UNIFORM		inLight2					LIGHTPOSMODEL2
	UNIFORM		lightColor0					LIGHTCOLOR0
	UNIFORM		lightColor1					LIGHTCOLOR1
	UNIFORM		lightColor2					LIGHTCOLOR2

	UNIFORM		isHighlighted				IS_HIGHLIGHTED
	
	UNIFORM		sTexture					TEXTURE0
	//UNIFORM		sNormalMap					TEXTURE1

	VERTEXSHADER	noBoneTangentSpaceVertShader
	FRAGMENTSHADER	textureMapNoNormalMapFragShader
	
	TEXTURE 0 teethMolarDiffuse
	//TEXTURE 1 teethMolarDiffuse_NM
[/EFFECT]

[EFFECT]
	NAME upperJawCrossSectionDiffuse

	ATTRIBUTE	inVertex					POSITION
	ATTRIBUTE	inNormal					NORMAL
 	ATTRIBUTE	inTangent					TANGENT
 	ATTRIBUTE	inBiNormal					BINORMAL
	ATTRIBUTE	inTexCoord					UV0

	UNIFORM 	ambientMaterial				MATERIALCOLORAMBIENT
	UNIFORM		diffuseMaterial				MATERIALCOLORDIFFUSE	
	UNIFORM		specularMaterial			MATERIALCOLORSPECULAR
	UNIFORM		shininess					MATERIALSHININESS
		
	UNIFORM		eyePos						EYEPOSMODEL
	UNIFORM		ViewProjMatrix				WORLDVIEWPROJECTION
	
	UNIFORM		inLight0					LIGHTPOSMODEL0
	UNIFORM		inLight1					LIGHTPOSMODEL1
	UNIFORM		inLight2					LIGHTPOSMODEL2
	UNIFORM		lightColor0					LIGHTCOLOR0
	UNIFORM		lightColor1					LIGHTCOLOR1
	UNIFORM		lightColor2					LIGHTCOLOR2

	UNIFORM		isHighlighted				IS_HIGHLIGHTED
	
	UNIFORM		sTexture					TEXTURE0
	//UNIFORM		sNormalMap					TEXTURE1

	VERTEXSHADER	noBoneTangentSpaceVertShader
	FRAGMENTSHADER	textureMapNoNormalMapFragShader
	
	TEXTURE 0 upperJawCrossSectionDiffuse
	//TEXTURE 1 upperJawCrossSectionDiffuse_NM
[/EFFECT]






