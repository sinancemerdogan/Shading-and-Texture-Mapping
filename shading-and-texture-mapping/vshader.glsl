#version 410
in  vec4 vPosition;
in  vec3 vNormal;
in  vec4 vColor;
in  vec2 vTexCoord;
in  float vTexCoord1D;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 LightPosition;
uniform mat4 LightSpaceMatrix;
uniform float Shininess;

out  vec3 fN;
out  vec3 fV;
out  vec3 fL;
out vec4 color;
out vec2 texCoord;
out float texCoord1D;

//Uniform variable for controlling options
uniform int DisplayMode;
uniform int ShadingOption;
uniform int LightSourcePosition;
uniform int ComponentOption;
uniform int TextureOption;


void main()
{
    //DisplayMode = Shading
    if(DisplayMode == 0 || DisplayMode == 1) {
        
        //ShadingOption = Gouraud
        if(ShadingOption == 0) {
            
            vec3 L, V, H, N;
            // Transform vertex position into camera (eye) coordinates
            vec3 pos = (ModelView * vPosition).xyz;

            //LightSourcePosition == Fixed
            if (LightSourcePosition == 0) {

                L = LightPosition.xyz; // light direction if directional light source
                
    
                L = normalize(L);
                
                V = normalize( -pos ); // viewer direction
                H = normalize( L + V ); // halfway vector

                // Transform vertex normal into camera coordinates
                N = normalize( ModelView * vec4(vNormal, 0.0) ).xyz;
            }
            //LightSourcePosition == Moving
            else {
                
                L = (ModelView * LightPosition).xyz; // light direction if directional light source
    
                L = normalize(L);
                
                V = normalize( -pos ); // viewer direction
                H = normalize( L + V ); // halfway vector

                // Transform vertex normal into camera coordinates
                N = normalize( ModelView * vec4(vNormal, 0.0) ).xyz;

            }

            // Compute terms in the illumination equation
            vec4 ambient = AmbientProduct;

            float Kd = max( dot(L, N), 0.0 ); //set diffuse to 0 if light is behind the surface point
            vec4  diffuse = Kd * DiffuseProduct;

            float Ks = pow( max(dot(N, H), 0.0), Shininess );
            vec4  specular = Ks * SpecularProduct;
            
            //ignore also specular component if light is behind the surface point
            if( dot(L, N) < 0.0 ) {
                specular = vec4(0.0, 0.0, 0.0, 1.0);
            }

            gl_Position = Projection * ModelView * vPosition;
            
            color = ambient + diffuse + specular;
            color.a = 1.0;
        }
        
        //ShadingOption = Phong
        else if(ShadingOption == 1) {
            
            // Transform vertex position into camera (eye) coordinates
            vec3 pos = (ModelView * vPosition).xyz;
            
            //LightSourcePosition == Fixed
            if (LightSourcePosition == 0) {
                
                fN = (ModelView * vec4(vNormal, 0.0)).xyz; // normal direction in camera coordinates

                fV = -pos; //viewer direction in camera coordinates

                fL = LightPosition.xyz; // light direction
                
                gl_Position =  Projection * ModelView * vPosition;
            }
            //LightSourcePosition == Moving
            else {
                
                fN = (ModelView * vec4(vNormal, 0.0)).xyz; // normal direction in camera coordinates

                fV = -pos; //viewer direction in camera coordinates

                fL = (ModelView * LightPosition).xyz; // light direction
                
                gl_Position = Projection * ModelView * vPosition;
                
            }
        }
    }
    
    //DisplayMode = Texture
    if (DisplayMode == 2) {
            color       = vColor;
            texCoord    = vTexCoord;
            texCoord1D    = vTexCoord1D;
            gl_Position = Projection * ModelView * vPosition;
    }
}
