-- This is a scale2x glsl shader implementation from the GlBoy project.
-- http://www.codesrc.com/src/glBoy/

video_driver_name = select(1, ...) -- Get the rendering driver name.
shading_language_version = select(2, ...) -- Get the shading language version.
sampler_type = select(3, ...) -- Get the type of samplers to use.
quest_width = select(4, ...) .. ".0"-- Get the quest width.
quest_height = select(5, ...) .. ".0" -- Get the quest height.

-- Get the type of texture2D function to use.
texture_type = "texture2DProj"
if (sampler_type == "sampler2DRect") then
	texture_type = "texture2DRectProj"	
end

 

shader {
	-- Name of the shader.
	name = "scale2x",
	-- Scale of the window compared to the quest_size.
	logical_scale = 2,
	-- Source of the vertex shader.
	vertex_source = [[
		varying vec4 vTexCoord;
		varying vec4 B;
		varying vec4 D;
		varying vec4 F;
		varying vec4 H;


		void main(void)
		{
			vec4 offsetx;
			vec4 offsety;

			offsetx = vec4(0.5, 0, 0, 0);
			offsety = vec4(0, 0.5, 0, 0);

			B = gl_MultiTexCoord0 - offsety;
			D = gl_MultiTexCoord0 - offsetx;
			F = gl_MultiTexCoord0 + offsetx;
			H = gl_MultiTexCoord0 + offsety;

			vTexCoord = gl_MultiTexCoord0;
			gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
		}
	]],
	-- Source of the fragment shader.
	fragment_source = [[
		uniform ]] .. sampler_type .. [[ solarus_sampler;

		varying vec4 vTexCoord; // supplied from vertex processor
		varying vec4 B;
		varying vec4 D;
		varying vec4 F;
		varying vec4 H;

		void main(void)
		{
			vec4 fragmentLocation;

			vec4 eColour;
			vec4 bColour;
			vec4 dColour;
			vec4 fColour;
			vec4 hColour;

			vec4 E0;
			vec4 E1;
			vec4 E2;
			vec4 E3;

			vec4 outRow1;
			vec4 outRow2;
			vec4 samplerSize;
		
			samplerSize = vec4(0, 0, ]] .. quest_width .. [[, ]] .. quest_height .. [[);

			eColour = ]] .. texture_type .. [[(solarus_sampler, vTexCoord);
			bColour = ]] .. texture_type .. [[(solarus_sampler, B);
			dColour = ]] .. texture_type .. [[(solarus_sampler, D);
			fColour = ]] .. texture_type .. [[(solarus_sampler, F);
			hColour = ]] .. texture_type .. [[(solarus_sampler, H);

			if (bColour != hColour && dColour != fColour) {
				E0 = dColour == bColour ? dColour : eColour;
				E1 = bColour == fColour ? fColour : eColour;
				E2 = dColour == hColour ? dColour : eColour;
				E3 = hColour == fColour ? fColour : eColour;
			} else {
				E0 = eColour;
				E1 = eColour;
				E2 = eColour;
				E3 = eColour;
			}

			// Is this pixel E0,E1,E2 or E3, or somewhere in between ?
			// Bi-linear interpolate (may not be exact x2 scale)
			fragmentLocation = fract(vTexCoord * samplerSize);

			outRow1 = mix(E0, E1, fragmentLocation.x);
			outRow2 = mix(E2, E3, fragmentLocation.x);
			gl_FragColor = mix(outRow1, outRow2, fragmentLocation.y);
		}
	]]
}
