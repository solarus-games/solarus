video_driver_name = ...
shading_language_version = ...
sampler_type = ...

shader {
	name = "normal",
	logical_scale = 2,
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
		uniform sampler2DRect solarus_sampler;

		void main()
		{
    		gl_FragColor = texture2DRect(solarus_sampler, v_texCoord) * v_color;
		}
	]]
}
