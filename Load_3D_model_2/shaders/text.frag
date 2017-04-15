 #version 430
 in vec2 text_coords;
 
 out vec4 color;
 
 uniform sampler2D texture_sampler;
 uniform vec3 text_color;
 
 void main()
 {
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(texture_sampler, text_coords).r);
	if(sampled.w <= 0.2) discard; // discard ALL full transparent pixel !!!
	color = vec4(text_color, 1.0) * sampled;
 }