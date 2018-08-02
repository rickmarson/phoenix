begin vertex

    vec3 vVertex;
    vec3 vNormal;
    vec2 vTexCoord0;
    
end

begin uniforms

    //transform matrices
    mat4 mMV;
    mat4 mProj;
    //light position
    vec4 vLightPos;
    //camera look-at vector, used in the phong specular model
    vec4 vEye;
    //material properties
    vec4 vDiffuseColor;
    vec4 vAmbientColor;
    vec4 vSpecularColor;
    float sShininess;
    //texture
    sampler2D diffuseTexture0;
    //light properties
    vec4 vLightDiffuse;
    vec4 vLightAmbient;
    vec4 vLightSpecular;
    
end

begin vertex_outputs

    vec3 vLightDir;
    vec3 vNormalOut;
    vec3 vEyeDir;
    vec2 vTex;
    
end

begin vertex_shader
 
	vec4 Vertex = vec4(vVertex, 1.0);

	mat3 mNormalMatrix;
	mNormalMatrix[0] = normalize(mMV[0].xyz);
	mNormalMatrix[1] = normalize(mMV[1].xyz);
	mNormalMatrix[2] = normalize(mMV[2].xyz);
	vNormalOut = normalize(mNormalMatrix * vNormal);
	
	vec3 ecPosition3;
	vec4 temp;

	ecPosition3 = vec3(mMV * Vertex);
	temp = mMV * vLightPos;
	vLightDir = normalize(vec3(temp) - ecPosition3);
	temp = mMV * vEye;
	vEyeDir = normalize(vec3(temp));
	
	mat4 mMVP;
	mMVP = mProj * mMV;
	VertexOut = mMVP * Vertex; 
	vTex = vTexCoord0;

end

begin fragment_shader

    vec4 LightColor;
    
    //diffuse component
    float fDot = max(0.0, dot(vNormalOut, vLightDir));
    vec3 vTotalLightDiffuse = vLightDiffuse.rgb * fDot;
    LightColor.rgb = vDiffuseColor.rgb * vTotalLightDiffuse;
    
    //add ambient component
    LightColor.rgb += vAmbientColor.rgb * vLightAmbient.rgb;
    
    //add specular component
    vec3 reflect = normalize(2.0*vTotalLightDiffuse*vNormalOut - vLightDir);
    vec3 vTotalLightSpec = vLightSpecular.rgb * pow( max(0.0, dot(reflect, vEyeDir) ), sShininess );
    LightColor.rgb += vSpecularColor.rgb * vTotalLightSpec;
    
    //copy alpha value
    LightColor.a = vDiffuseColor.a;
    
    //modulate the texture color with the total light color computed in the VS
    //LighColor contains both light intensity and color and the base mesh color.
    ColorOut = LightColor * texture(diffuseTexture0, vTex);

end

