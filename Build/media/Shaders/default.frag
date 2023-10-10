#version 130
uniform sampler2D texture;
uniform float timeNormal;

void main()
{
	// lookup the pixel in the texture
	vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
	
	if(pixel == vec4(0.0, 0.0, 0.0, 1.0))
		pixel = vec4(1.0, 1.0, 1.0, 1.0);
	
	// multiply it by the color
	gl_FragColor = gl_Color * pixel;
	gl_FragColor[0] -= timeNormal;
	gl_FragColor[1] -= timeNormal;
	gl_FragColor[2] -= timeNormal;
	gl_FragColor[2] += 0.2;
}