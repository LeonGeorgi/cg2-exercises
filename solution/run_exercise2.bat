@ECHO OFF
cd ..
.\solution\bin\windows\cgv_viewer64_143.exe plugin:cg_fltk "type(shader_config):shader_path='%cd%\solution\glsl'" plugin:cg_icons plugin:cg_ext plugin:cg_meta plugin:cmi_io plugin:crg_stereo_view plugin:CG2_exercise2 "config:'%cd%\solution\config_ex2.cfg'"
