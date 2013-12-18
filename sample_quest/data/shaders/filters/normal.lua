-- This is a normal shader. It will display the exact same display than
-- the default non-shaded video mode, but with a custom shaded video mode.

video_driver_name = ... -- Get the rendering driver name.
shading_language_version = ... -- Get the shading language version.
sampler_type = ... -- Get the type of samplers to use.

-- Get the suffix of the sampler type to use.
sampler_suffix = ""
if (sampler_type == "sampler2DRect") then
	sampler_suffix = "Rect"	
end

shader {
	-- Name of the shader
	name = "normal",
	-- Scale of the window compared to the quest_size
	logical_scale = 1,
	-- Source of the vertex shader
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
	-- Source of the fragment shader
	fragment_source = [[
		varying vec4 v_color;
		varying vec2 v_texCoord;
		uniform ]] .. sampler_type .. [[ solarus_sampler;

		void main()
		{
    		gl_FragColor = texture2D]] .. sampler_suffix .. [[(solarus_sampler, v_texCoord) * v_color;
		}
	]]
}
