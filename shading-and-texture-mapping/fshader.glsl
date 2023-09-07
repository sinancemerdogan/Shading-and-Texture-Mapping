#version 410

in  vec3 fN;
in  vec3 fL;
in  vec3 fV;
in vec4 color;
out vec4 fcolor;
in  vec2 texCoord;
in  float texCoord1D;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform float Shininess;
//Uniform variable for controlling options
uniform int DisplayMode;
uniform int ShadingOption;
uniform int TextureOption;
uniform sampler2D tex;
uniform sampler1D tex1D;

void main()
{
    
    //DisplayMode = Shading
    if(DisplayMode == 0 || DisplayMode == 1) {
        //ShadingOption = Gouraud
        if(ShadingOption == 0) {
            fcolor = color;
        }
        //ShadingOption = Phong
        else if(ShadingOption == 1 ) {
            
            
            // Normalize the input lighting vectors
            vec3 N = normalize(fN);
            vec3 V = normalize(fV);
            vec3 L = normalize(fL);
            
            // Normalize the input lighting vectors
            vec3 R = normalize( (2.0 * dot(L, N) * N) - L) ; // other phong
            vec3 H = normalize( L + V );
                        
            vec4 ambient = AmbientProduct;
                        
            float Kd = max(dot(L, N), 0.0);
            vec4 diffuse = Kd*DiffuseProduct;
                        
            float Ks = pow(max(dot(V, R), 0.0), Shininess); // other phong
            vec4 specular = Ks*SpecularProduct;
            
            // discard the specular highlight if the light's behind the vertex
            if( dot(L, N) < 0.0 ) {
                specular = vec4(0.0, 0.0, 0.0, 1.0);
            }
                        
            fcolor = ambient + diffuse + specular;
            fcolor.a = 1.0;
            
        }
    }
    //DisplayMode = Texture
    else if (DisplayMode == 2) {
        //sample a texture color from texture object
       if(TextureOption == 0 || TextureOption == 1){
           fcolor = texture(tex, texCoord);
            
       } else {
           fcolor = texture(tex, vec2(texCoord1D, 0.0));
       }
    }
}

