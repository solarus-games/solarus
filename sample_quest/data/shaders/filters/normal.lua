video_driver_name = ...
shading_language_version = ...
sampler2D_type = ...

shader {
	name = "normal",
	logical_scale = 1,
	vertex_source = [[
		varying vec4 v_color;
		varying vec2 v_texCoord;

		void main()
		{
   			gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    		v_color = gl_Color;
    		v_texCoord = vec2(gl_MultiTexCoord0);
		}
	]],
	fragment_source = [[
		varying vec4 v_color;
		varying vec2 v_texCoord;
		uniform sampler2D]] .. sampler2D_type .. [[ solarus_sampler;

		void main()
		{
    		gl_FragColor = texture2D]] .. sampler2D_type .. [[(solarus_sampler, v_texCoord) * v_color;
		}
	]]
}
