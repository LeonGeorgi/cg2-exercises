@ECHO OFF
cd ..
.\solution\bin\windows\cgv_viewer64_143.exe plugin:cg_fltk "type(shader_config):shader_path='%cd%\solution\glsl'" plugin:cmf_tt_gl_font plugin:cg_ext plugin:cg_meta plugin:cmi_io plugin:crg_stereo_view plugin:CG2_exercise1 "config:'%cd%\solution\config_ex1.cfg'"
